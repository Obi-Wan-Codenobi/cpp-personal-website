#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <netdb.h>
#include "requests.h"


int httpRequest(const char *ip, const int port, const char *request, char* buffer /* buffer size is MAXBUF*/)
{

    // tpc connection
    info("HTTP connection open");
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        error("socket creation failed", true);
        return -1;
    }

    // Create server address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("connect failed", true);
        return -1;
    }

    info("Connected to server");

    // Send http request
    int request_len = strlen(request);
    int nbytes_total = 0;
    int nbytes_last = 0;
    while (nbytes_total < request_len)
    {
        nbytes_last = write(sockfd, request + nbytes_total, request_len - nbytes_total);
        if (nbytes_last == -1)
        {
            perror("write");
            return -1;
        }
        nbytes_total += nbytes_last;
    }

    // Receive data from server

    // may not need this loop
    while (1)
    {
        int n = read(sockfd, buffer, MAXBUF);
        //printf("Read %d bytes from server: \n", n);
        if (n < 0)
        {
            error("read failed", true);
        }
        if (n == 0)
        {
            info("Server closed connection");
            break;
        }

        buffer[n] = '\0';
    }
    //printf("%s\n", buffer);

    // Close socket
    close(sockfd);
    info("HTTP connection closed");

    return 0;
}


int resolveDomainToIP(const char *domain, char *ipStr, size_t ipStrSize) {
    struct addrinfo hints, *res;
    int err;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    // Resolve domain name to IP address
    err = getaddrinfo(domain, NULL, &hints, &res);
    if (err != 0) {
        perror("getaddrinfo");
        return -1; 
    }

    struct sockaddr_in *addr_in = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &addr_in->sin_addr, ipStr, ipStrSize);

    freeaddrinfo(res);
    
    return 0;
}


