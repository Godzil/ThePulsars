#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include "net_private.h"

int net_connect_to(const char *host, int port, char *err, int errsz,
                   struct timeval *to)
{
    struct sockaddr_in sin;
    struct hostent *hp;
    int fd;

    if (( hp = gethostbyname(host)) == NULL )
    {
        if ( err != NULL )
        {
            snprintf(err, errsz, "%s: machine non trouv�e.", host);
        }
        return -1;
    }
    bcopy(hp->h_addr_list, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = htons(port);
    if (( fd = socket(hp->h_addrtype, SOCK_STREAM, 0)) == -1 )
    {
        if ( err != NULL )
        {
            snprintf(err, errsz, "socket: %s.", strerror(errno));
        }
        return -1;
    }
    if ( to != NULL )
    {
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, to, sizeof(*to));
    }
    if ( connect(fd, (struct sockaddr *) &sin, sizeof(sin)) == -1 )
    {
        if ( err != NULL )
        {
            snprintf(err, errsz, "connect: %s.", strerror(errno));
        }
        return -1;
    }
    return fd;
}

int net_listen_at(int port, int max_con, char *err, int errsz)
{
    int fd;
    struct sockaddr_in sin;
    int value = 1;

    if (( fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        if ( err != NULL )
        {
            snprintf(err, errsz, "socket: %s.", strerror(errno));
        }
        return -1;
    }
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    if ( bind(fd, (struct sockaddr *) &sin, sizeof(sin)) == -1 )
    {
        if ( err != NULL )
        {
            snprintf(err, errsz, "bind: %s.", strerror(errno));
        }
        return -1;
    }
    listen(fd, max_con);
    return ( fd );
}

