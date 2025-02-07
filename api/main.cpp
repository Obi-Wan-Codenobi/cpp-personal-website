#include "cstdlib"
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <cstdio>
#include "util/logger.cpp"

#define BUFFER_LENGTH 1024
#define SERVER_PORT 3005
#define FALSE 0

void LoadCertificates(SSL_CTX *ctx, char *CertFile, char *KeyFile);
SSL_CTX *InitServerCTX(void);
int isRoot();
void apiServe(Logger log);
int socketCreateBindListen(struct sockaddr_in &serveraddr, int &sd, int &rc);

int main(int argc, char *argv[]) {
    int logging = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') { 
            switch (argv[i][1]) {
                case 'L': 
                    if (i + 1 < argc) {
                        logging = std::atoi(argv[i + 1]);
                        i++; 
                    }
                    break;
                default:
                    printf("Unknown flag: %s\n",argv[i]);
            }
        }
    }

    Logger log = new Logger(logging);
    if (!isRoot())
    {
        log.error("This program must be run as root/sudo user!");
        exit(0);
    }

    log.info("LOGGING IS ENABLED");


    apiServe(log);
    return 0;
}

void apiServe(Logger log){
    int sd = -1, sd2 = -1;
    int rc, length, on = 1;
    char buffer[BUFFER_LENGTH];
    fd_set read_fd;
    struct timeval timeout;
    struct sockaddr_in serveraddr;
    SSL_CTX *ctx;

    SSL_library_init();
    ctx = InitServerCTX();
    LoadCertificates(ctx, "cert.pem", "key.pem");
    if (socketCreateBindListen(serveraddr, sd, rc)!=0){
        log.error("Failed to create socket, bind, and listen");
    }

    SSL *ssl;
    while (1)
    {
        
        sd2 = accept(sd, NULL, NULL);
        if (sd2 < 0)
        {
            perror("Accept() failed");
            continue;
        }
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sd2);

        if (SSL_accept(ssl) == -1)
        {
            ERR_print_errors_fp(stderr);
            break;
        }

        length = BUFFER_LENGTH;

        rc = SSL_read(ssl, buffer, sizeof(buffer));
        // test error rc < 0 or rc == 0 or   rc < sizeof(buffer
        if (rc < 0)
        {
            perror("Recv() failed()\n");
            break;
        }
        else if (rc <= 0)
        {
            printf("Received no bytes\n");
            continue;
        }

        printf("server received %d bytes\n", rc);



        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", 420);

        rc = SSL_write(ssl, buffer, strlen(buffer) + 1);
        if (rc < 0)
        {
            perror("Send() failed\n");
            break;
        }
        printf("server returned %d bytes\n", rc);

       
        if (ssl != NULL)
        {
            SSL_free(ssl);
        }
        close(sd2);
    }

    if (sd != -1)
        close(sd);
    if (sd2 != -1)
        close(sd2);

    if (ctx != NULL)
    {
        SSL_CTX_free(ctx);
    }
    if (ssl != NULL)
    {
        SSL_free(ssl);
    }

}

int socketCreateBindListen(struct sockaddr_in &serveraddr, int &sd, int &rc){
    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd < 0){
        perror("Socket() failed");
        return -1;
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (rc < 0) {
        perror("Bind() failed");
        return -1;
    }
    rc = listen(sd, 10);
    if (rc < 0) {
        perror("Listen() failed");
        return -1;
    }

    printf("Ready for client connect().\n");
    return 0;

}

int IsRoot()
{
    if (getuid() != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
SSL_CTX *InitServerCTX(void)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms(); /* load & register all cryptos, etc. */
    SSL_load_error_strings();     /* load all error messages */
    method = TLS_server_method(); /* create new server-method instance */
    ctx = SSL_CTX_new(method);    /* create new context from method */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
void LoadCertificates(SSL_CTX *ctx, char *CertFile, char *KeyFile)
{
    /* set the local certificate from CertFile */
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if (!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}
