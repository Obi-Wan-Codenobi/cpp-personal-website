#ifndef SOCKETS_H 
#define SOCKETS_H
#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> // gethostbyname()
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include "logger.cpp"
#include "../config/environmentVars.h"

#endif
