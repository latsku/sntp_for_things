/**
 * Copyright (C) 2015 Lari Lehtomäki
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief Tiny SNTP client for UNIX systems.
 *
 * @author Lari Lehtomäki <lari@lehtomaki.fi>
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* open, close */
#include <netdb.h>  /* addrinfo, ... */
#include <errno.h>  /* errno */
#include <string.h> /* memcpy, strerror */
#include <time.h>   /* strftime, ... */

#include "sntp.h"


/* Private variables ---------------------------------------------------------*/
static char buffer[48];
const char * defaultNetworkAddress = "2.pool.ntp.org";
const char * defaultNetworkPort    = "123";
static const int ITERATIONS = 2;


/* Private functions ---------------------------------------------------------*/
int initTCPConnection(const char * networkAddress, const char * networkPort) {
    int sockfd, r;
    struct     addrinfo base, *result;

    memset(&base,  0, sizeof(base));
    // IPv4 & TCP
    base.ai_family   = AF_INET;
    base.ai_socktype = SOCK_DGRAM;

    // DNS resolve
    r = getaddrinfo(networkAddress, networkPort, &base, &result);
    if ( r != 0 ) {
        fprintf(stderr, "Address resolving failed, code %d\n: ", r);
        exit(r);
    }

    /* Open socket */
    sockfd = socket(base.ai_family, base.ai_socktype, 0);
    if ( sockfd < 0 ) {
        fprintf(stderr, "Couldn't open a new socket... %s\n: ", strerror(errno));
        close(sockfd);
        exit(sockfd);
    }

    /* Connect */
    r = connect(sockfd, result->ai_addr, result->ai_addrlen);
    if ( r < 0 ) {
        fprintf(stderr, "Couldn't open the connection. %s\n: ", strerror(errno));
        close(sockfd);
        exit(sockfd);
    }

    return sockfd;
}


ssize_t sendBuffer(int handle, void * buffer, ssize_t bufferSize) {
    ssize_t r;

    r = write(handle, buffer, bufferSize);
    if ( r == -1 ) {
        fprintf(stderr, "Error on sending... %s\n", strerror(errno));
        return (-1)*errno;
    }

    return r;
}

ssize_t receiveBuffer(int handle, void * buffer, ssize_t bufferSize) {
    ssize_t r;

    r = read(handle, buffer, bufferSize);
    if ( r == -1 ) {
        fprintf(stderr, "Error on receiving... %s\n", strerror(errno));
        return (-1)*errno;
    }
    bufferSize = r;
    return r;
}


int main(int argc, char** argv) {

    int handle;
    time_t t;
    struct tm *tmp;
    char buf[32];

    sntpCreatePacket((void *)buffer);

    if ( argc > 2 ) {
        handle = initTCPConnection(argv[1], argv[2]);
    } else {
        handle = initTCPConnection(defaultNetworkAddress, defaultNetworkPort);
    }

    for (int i = 0; i < ITERATIONS; i++ ) {
        sendBuffer(handle, buffer, sizeof(buffer));
        receiveBuffer(handle, buffer, sizeof(buffer));

        t = sntpReadTimestamp((void *)buffer);
        usleep(2 * 1000 * 1000);
        sntpUpdatePacket((void *)buffer, t);
    }
    tmp = localtime(&t);

    strftime(buf, 32, "%Y-%m-%d %H:%M:%S", tmp);
    printf("Time is %s\n", buf);

    return 0;
}

