
// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <string>
#include <iostream>
#include <poll.h>
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::seconds
#define PORT 11211

int main(int argc, char** argv)
{
    // Default server port
    uint16_t server_port = PORT;
    // Parse command line to see if there is optional port
    // number specified
    if (argc == 2) {
        server_port = atoi(argv[1]);
    }
    int total_tests_passed = 0;
    int total_tests = 8;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout<<"\n Socket creation error"<<std::endl;
        return -1; 
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        std::cout<<"Invalid address/ Address not supported"<<std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout<<"Connection Failed"<<std::endl;
        return -1;
    }

    {
      /*
       * Testcase 1: set a key-value pair into the cache-server
       * Expected Result: "STORED\r\n" is the response from the
       *                   cache-server
       */
      std::string set_request = "set xyzkey 0 0 6\r\nabcdef\r\n";
      std::string expected_response = "STORED\r\n";
      std::cout<<"===== Testcase 1 ====="<<std::endl;
      std::cout<<"Test set key-value pair"<<std::endl;
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<set_request;
      send(sock , set_request.c_str(), set_request.size() , 0);
      memset(buffer, 0, sizeof(buffer));
      valread = read( sock , buffer, 1024);
      std::string response = buffer;
      response = response.substr(0, valread);
      std::cout<<"Response received from the server :"<<std::endl;
      std::cout<<response;
      if (response.compare(expected_response) == 0) {
        std::cout<<"===== Testcase 1 Passed ====="<<std::endl;
        total_tests_passed++;
      } else {
        std::cout<<"===== Testcase 1 Failed ====="<<std::endl;
      }
    }

    {
      /*
       * Testcase 2: get a key-value pair already stored on server
       * Expected Result:"VALUE xyzkey 0 0 6\r\nabcdef\r\nEND\r\n"
       *                 This is continuing to work on the set of
       *                 Testcase 1.
       */
      std::string get_request = "get xyzkey\r\n";
      std::string expected_response = "VALUE xyzkey 0 0 6\r\nabcdef\r\nEND\r\n";
      std::cout<<"===== Testcase 2 ====="<<std::endl;
      std::cout<<"Test get key-value pair"<<std::endl;
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<get_request;
      send(sock , get_request.c_str(), get_request.size() , 0);
      memset(buffer, 0, sizeof(buffer));
      valread = read( sock , buffer, 1024);
      std::string response = buffer;
      response = response.substr(0, valread);
      std::cout<<"Response received from the server :"<<std::endl;
      std::cout<<response;

      if (response.compare(expected_response) == 0) {
        std::cout<<"===== Testcase 2 Passed ====="<<std::endl;
        total_tests_passed++;
      } else {
        std::cout<<"===== Testcase 2 Failed ====="<<std::endl;
      }
    }

    {
      /*
       * Testcase 3: handle no-reply after set key-value pair.
       * Expected Result: There should be no response from the server.
       */
      std::string set_request = "set noreplykey 0 0 8 noreply\r\nabcdefgh\r\n";
      std::string expected_response; // There is no response in this case
      std::cout<<"===== Testcase 3 ====="<<std::endl;
      std::cout<<"Test set key-value pair with noreply"<<std::endl;
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<set_request;
      send(sock , set_request.c_str(), set_request.size() , 0);
      memset(buffer, 0, sizeof(buffer));
      valread = 0;
      while(true)
      {
        struct pollfd fd;
        int ret;
        fd.fd = sock; // socket handler
        fd.events = POLLIN;
        ret = poll(&fd, 1, 1000); // 1 second for timeout
        switch(ret)
        {
            case 0:
              //Timeout
              std::cout<<"timeout"<<std::endl;
              break;
            case -1:
              //Error
              std::cout<<"error"<<std::endl;
              break;
            default:
              valread = read( sock , buffer, 1024);
              break;
        }
        if (ret == 0 ) {
          //Nothing read from the socket
          break;
        }
      }

      if (valread == 0) {
        std::cout<<"Set with noreply succeeded"<<std::endl;
      } else {
        std::string response = buffer;
        response = response.substr(0, valread);
        std::cout<<"Response received from the server :"<<std::endl;
        std::cout<<response;
        std::cout<<"Set with noreply failed"<<std::endl;
      }

      // Get the key that has just been set
      expected_response = "VALUE noreplykey 0 0 8\r\nabcdefgh\r\nEND\r\n";
      std::string get_request = "get noreplykey\r\n";
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<get_request;
      send(sock, get_request.c_str(), get_request.size(), 0);
      memset(buffer, 0, sizeof(buffer));
      valread = read( sock, buffer, 1024);
      std::string response = buffer;
      response = response.substr(0, valread);
      std::cout<<"Response received from the server :"<<std::endl;
      std::cout<<response;
      if (response.compare(expected_response) == 0) {
        std::cout<<"===== Testcase 3 Passed ====="<<std::endl;
        total_tests_passed++;
      } else {
        std::cout<<"===== Testcase 3 Failed ====="<<std::endl;
      }
    }

    {
      /*
       * Testcase 4: set 3 key-value pairs with noreply and invoke a single get
       *             for all 3 keys.
       * Expected Result: Should return "STORED\r\n" for all the set operations
       *                  and the get operation should display values followed
       *                  by "END\r\n"
       */
      std::cout<<"===== Testcase 4 ====="<<std::endl;
      std::cout<<"Test get with multiple keys after sets with noreply"<<std::endl;
      {
        std::string set_request1 = "set key1 0 0 6 noreply\r\nabcdef\r\n";
        std::cout<<"Request sent to server :"<<std::endl;
        std::cout<<set_request1;
        send(sock , set_request1.c_str(), set_request1.size() , 0);
        // Always poll the socket before next send, else the underlying
        // TCP will stitch the send requests, the input to the server will
        // be malformed and is not the intention of this testcase
        while(true)
        {
          struct pollfd fd;
          int ret;
          fd.fd = sock; // socket handler
          fd.events = POLLIN;
          ret = poll(&fd, 1, 1000); // 1 second for timeout
          switch(ret)
          {
              case 0:
                //Timeout
                std::cout<<"timeout"<<std::endl;
                break;
              case -1:
                //Error
                std::cout<<"error"<<std::endl;
                break;
              default:
                valread = read( sock , buffer, 1024);
                break;
          }
          if (ret == 0 ) {
            //Nothing read from the socket
            break;
          }
        }
      }

      {
        std::string set_request2 = "set key2 0 0 3 noreply\r\nghi\r\n";
        std::cout<<"Request sent to server :"<<std::endl;
        std::cout<<set_request2;
        send(sock , set_request2.c_str(), set_request2.size() , 0);
        // Always poll the socket before next send, else the underlying
        // TCP will stitch the send requests, the input to the server will
        // be malformed and is not the intention of this testcase
        while(true)
        {
          struct pollfd fd;
          int ret;
          fd.fd = sock; // socket handler
          fd.events = POLLIN;
          ret = poll(&fd, 1, 1000); // 1 second for timeout
          switch(ret)
          {
              case 0:
                //Timeout
                std::cout<<"timeout"<<std::endl;
                break;
              case -1:
                //Error
                std::cout<<"error"<<std::endl;
                break;
              default:
                valread = read( sock , buffer, 1024);
                break;
          }
          if (ret == 0 ) {
            //Nothing read from the socket
            break;
          }
        }
      }

      {
        std::string set_request3 = "set key3 1 5 4 noreply\r\njklm\r\n";
        std::cout<<"Request sent to server :"<<std::endl;
        std::cout<<set_request3;
        send(sock , set_request3.c_str(), set_request3.size() , 0);
        // Always poll the socket before next send, else the underlying
        // TCP will stitch the send requests, the input to the server will
        // be malformed and is not the intention of this testcase
        while(true)
        {
          struct pollfd fd;
          int ret;
          fd.fd = sock; // socket handler
          fd.events = POLLIN;
          ret = poll(&fd, 1, 1000); // 1 second for timeout
          switch(ret)
          {
              case 0:
                //Timeout
                std::cout<<"timeout"<<std::endl;
                break;
              case -1:
                //Error
                std::cout<<"error"<<std::endl;
                break;
              default:
                valread = read( sock , buffer, 1024);
                break;
          }
          if (ret == 0 ) {
            //Nothing read from the socket
            break;
          }
        }
      }

      {
        // Send get request with multiple keys stored
        std::string get_request = "get key1 key2 key3\r\n";
        std::string expected_get_response =
          "VALUE key1 0 0 6\r\nabcdef\r\nVALUE key2 0 0 3\r\nghi\r\n"
          "VALUE key3 1 5 4\r\njklm\r\nEND\r\n";
        std::cout<<"Request sent to server :"<<std::endl;
        std::cout<<get_request;
        send(sock , get_request.c_str(), get_request.size() , 0);
        memset(buffer, 0, sizeof(buffer));
        valread = read( sock , buffer, 1024);
        std::string response = buffer;
        response = response.substr(0, valread);
        std::cout<<"Response received from the server :"<<std::endl;
        std::cout<<response;

        if (response.compare(expected_get_response) == 0) {
          std::cout<<"===== Testcase 4 Passed ====="<<std::endl;
          total_tests_passed++;
        } else {
          std::cout<<"===== Testcase 4 Failed ====="<<std::endl;
        }
      }
    }

    {
      /*
       * Testcase 5: invoke get on a non-existent key
       * Expected Result: "END\r\n" from server with no data fields
       */
      std::cout<<"===== Testcase 5 ====="<<std::endl;
      std::cout<<"Test get on non-existent key"<<std::endl;
      std::string get_request = "get non-existent-key\r\n";
      std::string expected_response = "END\r\n";
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<get_request;
      send(sock , get_request.c_str(), get_request.size() , 0);
      memset(buffer, 0, sizeof(buffer));
      valread = read( sock , buffer, 1024);
      std::string response = buffer;
      response = response.substr(0, valread);
      std::cout<<"Response received from the server :"<<std::endl;
      std::cout<<response;

      if (response.compare(expected_response) == 0) {
        std::cout<<"===== Testcase 5 Passed ====="<<std::endl;
        total_tests_passed++;
      } else {
        std::cout<<"===== Testcase 5 Failed ====="<<std::endl;
      }

    }

    {
      /*
       * Testcase 6: test malformed inputs to server
       * Expected Result: Server returns ERROR on submitting a command
       *                  other than 'get' or 'set'
       *                  Server returns CLIENT_ERROR on errors in malformed
       *                  input
       */
      std::cout<<"===== Testcase 6 ====="<<std::endl;
      std::cout<<"Test malformed command and input to server"<<std::endl;
      std::string bad_command = "put random\thello";
      std::string expected_response = "ERROR\r\n";
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<bad_command<<std::endl;
      send(sock, bad_command.c_str(), bad_command.size(), 0);
      memset(buffer, 0, sizeof(buffer));
      valread = read(sock, buffer, 1024);
      std::string response = buffer;
      response = response.substr(0, valread);
      std::cout<<"Response received from the server :"<<std::endl;
      std::cout<<response;

      if (response.compare(expected_response) == 0) {
        std::string bad_input = "set 10yhffrb";
        std::string bad_expected_response = "CLIENT_ERROR\r\n";
        std::cout<<"Request sent to server :"<<std::endl;
        std::cout<<bad_input<<std::endl;
        send(sock, bad_input.c_str(), bad_input.size(), 0);
        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, 1024);
        std::string bad_response = buffer;
        bad_response = bad_response.substr(0, valread);
        std::cout<<"Response received from the server :"<<std::endl;
        std::cout<<bad_response;
        if (bad_response.compare(bad_expected_response) == 0) {
          std::cout<<"===== Testcase 6 Passed ====="<<std::endl;
          total_tests_passed++;
        } else {
          std::cout<<"===== Testcase 6 Failed ====="<<std::endl;
        }
      } else {
        std::cout<<"===== Testcase 6 Failed ====="<<std::endl;
      }
    }

    {
      /*
       * Testcase 7: test key with control characters
       * Expected Result: Server returns CLIENT_ERROR on key with control
       *                  characters
       */
      std::cout<<"===== Testcase 7 ====="<<std::endl;
      std::cout<<"Test key with control characters"<<std::endl;
      std::string control_key_command = "set key1\tq 0 0 1\r\nd\r\n";
      std::string control_key_response = "CLIENT_ERROR\r\n";
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<control_key_command;
      send(sock, control_key_command.c_str(), control_key_command.size(), 0);
      memset(buffer, 0, sizeof(buffer));
      valread = read(sock, buffer, 1024);
      std::string bad_response = buffer;
      bad_response = bad_response.substr(0, valread);
      std::cout<<"Response received from the server :"<<std::endl;
      std::cout<<bad_response;
      if (bad_response.compare(control_key_response) == 0) {
        std::cout<<"===== Testcase 7 Passed ====="<<std::endl;
        total_tests_passed++;
      } else {
        std::cout<<"===== Testcase 7 Failed ====="<<std::endl;
      }
    }

    {
      /*
       * Testcase 8: test cache eviction
       * Expected Result: The current eviction time is 20 seconds.
       *                  Therefore, after sleeping for 20 seconds,
       *                  the first key 'xyzkey' added in Testcase 1
       *                  should not be found
       *
       */
      std::string get_request = "get xyzkey\r\n";
      std::string expected_response = "END\r\n";
      std::cout<<"===== Testcase 8 ====="<<std::endl;
      std::cout<<"Test get on evicted key-value pair"<<std::endl;
      std::cout<<"Sleep for 20 seconds which is when the eviction timer sets up"<<std::endl;
      std::this_thread::sleep_for (std::chrono::seconds(20));
      std::cout<<"Request sent to server :"<<std::endl;
      std::cout<<get_request;
      send(sock , get_request.c_str(), get_request.size() , 0);
      memset(buffer, 0, sizeof(buffer));
      valread = read( sock , buffer, 1024);
      std::string response = buffer;
      response = response.substr(0, valread);
      std::cout<<"Response received from the server :"<<std::endl;
      std::cout<<response;

      if (response.compare(expected_response) == 0) {
        std::cout<<"===== Testcase 8 Passed ====="<<std::endl;
        total_tests_passed++;
      } else {
        std::cout<<"===== Testcase 8 Failed ====="<<std::endl;
      }
    }

    std::cout<<"------------------------------------------"<<std::endl;
    std::cout<<total_tests_passed<<"/"<<total_tests<<" tests passed"<<std::endl;
    std::cout<<"------------------------------------------"<<std::endl;

    return 0; 
} 

