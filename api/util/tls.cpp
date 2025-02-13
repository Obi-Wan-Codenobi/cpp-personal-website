#include "sockets.h"

namespace util
{
    int socketCreateBindListen(struct sockaddr_in &serveraddr, int &sd, int &rc)
    {
        sd = socket(PF_INET, SOCK_STREAM, 0);
        if (sd < 0)
        {
            perror("Socket() failed");
            return -1;
        }

        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(SERVER_PORT);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
        if (rc < 0)
        {
            perror("Bind() failed");
            return -1;
        }
        rc = listen(sd, 10);
        if (rc < 0)
        {
            perror("Listen() failed");
            return -1;
        }

        printf("Ready for client connect().\n");
        return 0;
    }

    int password_callback(char *buf, int size, int rwflag, void *userdata)
    {
        strncpy(buf, PEM_KEY_PASSWORD, size);
        return strlen(PEM_KEY_PASSWORD);
    }

    void LoadCertificates(SSL_CTX *ctx, const char *CertFile, const char *KeyFile, Logger log)
    {
        /* set the local certificate from CertFile */
        if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
        {
            log.error("Failed to set local cert");
            ERR_print_errors_fp(stderr);
            abort();
        }

        SSL_CTX_set_default_passwd_cb(ctx, password_callback);
        /* set the private key from KeyFile (may be the same as CertFile) */
        if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
        {
            log.error("Failed to set private key");
            ERR_print_errors_fp(stderr);
            abort();
        }
        /* verify private key */
        if (!SSL_CTX_check_private_key(ctx))
        {
            log.error("Failed to verify private key");
            fprintf(stderr, "Private key does not match the public certificate\n");
            abort();
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
}