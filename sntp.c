/**
 * Copyright (C) 2015 Lari Lehtomäki
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief Tiny SNTP client library.
 *
 * @author Lari Lehtomäki <lari@lehtomaki.fi>
 */


/* Includes ------------------------------------------------------------------*/
#include <string.h>     /* memcpy */
#include <arpa/inet.h>  /* htonl */
#include "sntp.h"


/* Exported functions ------------------------------------------------------- */
void sntpCreatePacket(void * buffer) {
    ntp_t * x;
    x = (ntp_t *)buffer;

    x->flags        = (NOSYNC << 6) | (VERSION << 3) | (M_CLNT << 0);
    x->stratum      = MAXSTRAT;
    x->poll         = MINPOLL;
    x->precision    = PRECISION;
    x->rootdelay    = htonl(1);
    x->rootdisp     = htonl(1);
    memcpy(x->refid, "INIT", 4);
    x->reftime      = 0;
    x->org          = 0;
    x->rec          = 0;
    x->xmt          = 0;

}

void sntpUpdatePacket(void * buffer, uint32_t timestamp) {
    ntp_t * x;
    x = (ntp_t *)buffer;

    x->flags        = (NOSYNC << 6) | (VERSION << 3) | (M_CLNT << 0);
    x->org          = x->xmt;
    x->rec          = D2LFP(timestamp);
    x->xmt          = D2LFP(timestamp);
}

int sntpReadTimestamp(void * buffer) {
    ntp_t * x;
    x = (ntp_t *)buffer;

    return (ntohl(x->xmt) - JAN_1970);
}
