
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
        // Set another bigger request
        std::string set_request_big = "set sjlG73d2h8PH9L973oBZ 0 0 4096\r\n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\r\n";
        std::cout<<"Request sent to server :"<<std::endl;
        std::cout<<set_request_big;
        send(sock , set_request_big.c_str(), set_request_big.size() , 0);
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

