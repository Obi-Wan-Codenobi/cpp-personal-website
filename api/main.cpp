#include "cstdlib"
#include <algorithm>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>
#include <resolv.h>
#include <cstdio>
#include <vector>
#include "util/utils.h"
#include "config/environmentVars.h"
#include "routes/routes.h"
#include <memory>

int runningAsRoot();
void apiServe(util::Logger log);

int main(int argc, char *argv[])
{
    int logging = 0;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            switch (argv[i][1])
            {
            case 'L':
                if (i + 1 < argc)
                {
                    logging = std::atoi(argv[i + 1]);
                    i++;
                }
                break;
            default:
                printf("Unknown flag: %s\n", argv[i]);
            }
        }
    }

    util::Logger log = new util::Logger(logging);
    if (!runningAsRoot())
    {
        log.error("This program must be run as root/sudo user!");
        exit(0);
    }

    log.info("LOGGING IS ENABLED");

    while (1)
    {
        try {
            apiServe(log);
        }
        catch(...){
            log.info("An Unknown error occurred...\n\nRestarting process...\n");

        }
    }
    return 0;
}

void apiServe(util::Logger log)
{
    int sd = -1, sd2 = -1;
    int rc, length, on = 1;
    std::vector<char> buffer(BUFFER_LENGTH);
    fd_set read_fd;
    struct timeval timeout;
    struct sockaddr_in serveraddr;
    SSL_CTX *ctx;

    SSL_library_init();
    ctx = util::InitServerCTX();
    util::LoadCertificates(ctx, "cert.pem", "key.pem", log);
    if (util::socketCreateBindListen(serveraddr, sd, rc) != 0)
    {
        log.error("Failed to create socket, bind, and listen");
    }

    SSL *ssl;

    std::unique_ptr<routes::Router> router = std::make_unique<routes::Router>();
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
            log.error("Failed to accept connection");
            ERR_print_errors_fp(stderr);
            continue;
        }

        length = BUFFER_LENGTH;

        rc = SSL_read(ssl, buffer.data(), buffer.size());
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
        
        util::Request received_request (std::string(buffer.begin(), buffer.end()));
        
        //received_request.printRequest();
        log.info(received_request.getRequestWithoutHeadersString());

        std::string msg = router->process(received_request);
        log.info(msg);
        rc = SSL_write(ssl, msg.data(), msg.size());
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

int runningAsRoot()
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
