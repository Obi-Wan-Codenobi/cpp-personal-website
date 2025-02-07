#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> // gethostbyname()
#include <arpa/inet.h>
#include <stdlib.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <resolv.h>

#include <arpa/inet.h>
#include <errno.h>
#include "requests.h"


#define FALSE 0
#define MAX_HOST_NAME_LENGTH 20


SSL_CTX *InitCTX(void);

int httpsRequest(const char *ip, const int port, const char *request, char* buffer /* buffer size is MAXBUF*/)
{
    info("HTTPS connection open");
    int sd = -1, rc, bytesReceived;
    char server[MAX_HOST_NAME_LENGTH];
    struct sockaddr_in serveraddr;
    struct hostent *hostp;

    SSL_CTX *ctx;
    SSL *ssl;

    // Initialize the SSL lib
    SSL_library_init();
    ctx = InitCTX();

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        error("Socket() failed", 1);
        return -1;
    }
    strcpy(server, ip);

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(server);

    //NOTE: Not sure if needed since I am searching for ip beforehand
    if (serveraddr.sin_addr.s_addr == (unsigned long)INADDR_NONE)
    {
        hostp = gethostbyname(server);
        if (hostp == (struct hostent *)NULL)
        {
            error("Host not found", 1);
            return -1;
        }

        memcpy(&serveraddr.sin_addr,
               hostp->h_addr,
               sizeof(serveraddr.sin_addr));
    }

    rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (rc < 0)
    {
        error("Connect() failed", 1);
        return -1;
    }

    // Create new SSL connection state
    ssl = SSL_new(ctx);

    // Attach the socket descriptor
    SSL_set_fd(ssl, sd);

    if (SSL_connect(ssl) == -1)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    if (sizeof(request) > MAXBUF)
    {
        error("Request larger than buffer", 1);
        return -1;
    }
    
    info("Connected to server");

    // Send request
    int request_len = strlen(request);
    SSL_write(ssl, request, request_len);


    // Receive the response from the server
    bytesReceived = SSL_read(ssl, buffer, MAXBUF);
    //printf("%s\n", buffer);
    
    if (bytesReceived < 0)
    {
        error("Recv() failed", 1);
    }
    else if (bytesReceived == 0)
    {
        error("Nothing received in Recv()", 1);
    }

    buffer[bytesReceived] = '\0';

    close(sd);

    if (ctx != NULL)
    {
        SSL_CTX_free(ctx);
    }
    if (ssl != NULL)
    {
        SSL_free(ssl);
    }

    ERR_print_errors_fp(stderr);
    info("HTTPS connection closed");
    return 0;
}

// Helper fucntions from example:

SSL_CTX *InitCTX(void)
{

    SSL_METHOD const *method;
    SSL_CTX *ctx;

    // Load cryptos, et.al.
    OpenSSL_add_all_algorithms();

    // Bring in and register error messages
    SSL_load_error_strings();

    // Create new client-method instance
    method = SSLv23_client_method();

    // Create new context
    ctx = SSL_CTX_new(method);

    if (ctx == NULL)
    {

        ERR_print_errors_fp(stderr);
        abort();

    } // if

    return ctx;

} // InitCTX
