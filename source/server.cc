#include <sys/socket.h> // For socket()
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <thread> // std::thread
#include <vector> // std::vector
#include <queue> // std::queue
#include <mutex> // std::mutex
#include <condition_variable> // std::condition_variable
#include "cache.h" //Cache::processRequest


// This class manages server thread pool that will process TCP requests
class ServerThreadPool : public NetflixCached::Cache {
 public:
  ServerThreadPool() : done(false) {
    // This returns the number of threads supported by the system. If the
    // function can't figure out this information, it returns 0, instead return
    // 1 thread
    auto number_of_threads = std::thread::hardware_concurrency();
    if (number_of_threads == 0) {
      number_of_threads = 1;
    }
    std::cout<<"total number of concurrent threads = "<<number_of_threads<<std::endl;

    for (unsigned i = 0; i < number_of_threads; ++i) {
      // The threads will execute the private member `doWork`. Note that we need
      // to pass a reference to the function (namespaced with the class name) as
      // the first argument, and the current object as second argument
      threads.push_back(std::thread(&ServerThreadPool::doWork, this));
    }
  }

  // The destructor joins all the threads so the program can exit gracefully.
  // This will be executed if there is any exception (e.g. creating the threads)
  ~ServerThreadPool() {
    // So threads know it's time to shut down
    done = true;

    // Wake up all the threads, so they can finish and be joined
    work_queue_condition_var.notify_all();
    for (auto& thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  // This function will be called by the server, every time there is a request
  // that needs to be processed by the thread pool
  void queueWork(int fd, std::string& request) {
    // Grab the mutex
    std::lock_guard<std::mutex> g(work_queueMutex);

    // Push the request to the queue
    work_queue.push(std::pair<int, std::string>(fd, request));

    // Notify one thread that there are requests to process
    work_queue_condition_var.notify_one();
  }

 private:
  // This condition variable is used for the threads to wait until there is work
  // to do
  std::condition_variable_any work_queue_condition_var;

  // We store the threads in a vector, so we can later stop them gracefully
  std::vector<std::thread> threads;

  // Mutex to protect work_queue
  std::mutex work_queueMutex;

  // Queue of requests waiting to be processed
  std::queue<std::pair<int, std::string>> work_queue;

  // This will be set to true when the thread pool is shutting down. This tells
  // the threads to stop looping and finish
  bool done;

  // Function used by the threads to grab work from the queue
  void doWork() {
    // Loop while the queue is not destructing
    while (!done) {
      std::pair<int, std::string> request;

      // Create a scope, so we don't lock the queue for longer than necessary
      {
        std::unique_lock<std::mutex> g(work_queueMutex);
        work_queue_condition_var.wait(g, [&]{
          // Only wake up if there are elements in the queue or the program is
          // shutting down
          return !work_queue.empty() || done;
        });

        request = work_queue.front();
        work_queue.pop();
      }

      processRequest(request);
    }
  }
};

int main() {
  // Create a socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == 0) {
    std::cout << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  // Listen to a port
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(11211);
  if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    std::cout << "Failed to bind to port 11211. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  // Start listening
  if (listen(sockfd, 10) < 0) {
    std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  ServerThreadPool tp;

  while (true) {
    // Grab a connection from the queue
    auto addrlen = sizeof(sockaddr);
    int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    if (connection < 0) {
      std::cout << "Failed to grab connection. errno: " << errno << std::endl;
      exit(EXIT_FAILURE);
    }

    // Read from the connection
    char buffer[100];
    auto bytes_read = read(connection, buffer, 100);
    std::string req = buffer;
    std::string request = req.substr(0, bytes_read);
    std::cout << "Bytes read "<<bytes_read<<std::endl;

    // Add some work to the queue
    tp.queueWork(connection, request);
  }

  close(sockfd);
}
