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
#include <memory>
#include <thread>
#include <signal.h>

#include "util/logger.cpp"
#include "util/utils.h"
#include "config/environmentVars.h"
#include "routes/routes.h"

static volatile sig_atomic_t keep_running = 1;

// Signal (Control+C)
void signal_handler(int sig) {
    if (sig == SIGINT) {
        keep_running = 0;
    }
}

void set_signal_handler(const std::shared_ptr<util::Logger>& log){
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0; // No special flags
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        log->error("Failed to set up signal handler");
        exit(1);
    }
}

struct SSLDeleter {
    void operator()(SSL* ptr) const { if (ptr) SSL_free(ptr); }
};
using UniqueSSL = std::unique_ptr<SSL, SSLDeleter>;

int runningAsRoot();
void apiServe(const std::shared_ptr<util::Logger>& log);


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

    std::shared_ptr<util::Logger> log = std::make_shared<util::Logger>(logging);
    if (!runningAsRoot())
    {
        log->error("This program must be run as root/sudo user!");
        exit(0);
    }

    log->info("LOGGING IS ENABLED");

    set_signal_handler(log);
    
    while (keep_running)
    {
        try {
            apiServe(log);
        }
        catch(...){
            log->info("An Unknown error occurred...\n\nRestarting process...\n");

        }
    }
    log->info("Exititing main ...");
    return 0;
}

void serve_connected_client(SSL* ssl_raw, int sd2, const std::shared_ptr<util::Logger>& log) {
        UniqueSSL ssl(ssl_raw);
 
        std::vector<char> buffer(BUFFER_LENGTH);
        std::unique_ptr<routes::Router> router = std::make_unique<routes::Router>();

        int rc = SSL_read(ssl.get(), buffer.data(), buffer.size());
        if (rc < 0)
        {
            perror("Recv() failed()\n");
            return;
        }
        else if (rc <= 0)
        {
            printf("Received no bytes\n");
            return;
        }

        //printf("server received %d bytes\n", rc);
    
        util::Request received_request (std::string(buffer.begin(), buffer.end()));
        
        //received_request.printRequest();
        log->info(received_request.getRequestWithoutHeadersString());

        std::string msg = router->process(received_request);
        rc = SSL_write(ssl.get(), msg.data(), msg.size());
        if (rc < 0)
        {
            perror("Send() failed\n");
        }
        printf("server returned %d bytes\n", rc);

        close(sd2);
}

void apiServe(const std::shared_ptr<util::Logger>& log)
{
    int sd , sd2 , ready = -1;
    int rc = 1;
    
    fd_set read_fd;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set read_fds; // Set to monitor readability
    struct sockaddr_in serveraddr;
    SSL_CTX *ctx;
    SSL *ssl;

    SSL_library_init();
    ctx = util::InitServerCTX();
    util::LoadCertificates(ctx, "cert.pem", "key.pem", log);
    if (util::socketCreateBindListen(serveraddr, sd, rc) != 0)
    {
        log->error("Failed to create socket, bind, and listen");
        return;
    }

    

    while (keep_running)
    {
        FD_ZERO(&read_fds);
        FD_SET(sd, &read_fds);
        // Check if sd is readable
        // To avoid accept hanging
        ready = select(sd + 1, &read_fds, nullptr, nullptr, &timeout); 
        if (ready < 0) {
            if (errno != EINTR) {
                perror("select() failed");
            }
            continue;
        }
        if (ready == 0) {
            continue;
        }

        sd2 = accept(sd, NULL, NULL);
        if (sd2 < 0)
        {
            perror("Accept() failed");
            continue;
        }

        UniqueSSL ssl(SSL_new(ctx));
        if (!ssl) {
            log->error("SSL_new failed");
            close(sd2);
            continue;
        }
        SSL_set_fd(ssl.get(), sd2);

        if (SSL_accept(ssl.get()) == -1)
        {
            log->error("Failed to accept connection");
            ERR_print_errors_fp(stderr);
            continue;
        }

        std::thread connected_client (serve_connected_client, ssl.get(), sd2, log);
        connected_client.detach();
        ssl.release();

    }

    log->info("Shutting down apiServe...");
    if (sd != -1) close(sd);
    if (ctx != nullptr) SSL_CTX_free(ctx);
  }

int runningAsRoot() {
    return (getuid() == 0) ? 1 : 0;
}
