
// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <string>
#include <iostream>
#define PORT 11211

int main()
{ 
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    //char *hello = "Hello from client";
    std::string response = "set xyzkey 0 0 6\r\nabcdef\r\n";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1; 
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(sock , response.c_str(), response.size() , 0 );
    printf("Set message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s",buffer );

    // Make a get request on the set key before
    std::string get_request = "get xyzkey\r\n";
    printf("Get message sent\n"); 
    send(sock, get_request.c_str(), get_request.size(), 0);
    valread = read( sock, buffer, 1024 );
    printf("%d", valread);
    printf("Get message rcvd\n");
    printf("%s", buffer);

    return 0; 
} 

