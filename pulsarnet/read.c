#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include "net_private.h"

static bool net_check_type(int fd, net_type_t wanted)
{
    unsigned char c = 42;

    if ( read(fd, &c, 1) != 1 )
    {
        c = net_type_none;
    }
#ifdef __DEBUG_NETLIB__
    fprintf(stderr, "Reading type: %#x (wanted: %#x)\n", c, wanted);
#endif
    if ( c != wanted )
    {
        net_error(c, wanted);
        return false;
    }
    return true;
}

static bool read_data(int fd, void *buf, int sz)
{
    int i;
    char *p = buf;

    while ( sz > 0 )
    {
        i = read(fd, p, sz);
        if ( i <= -1 )
        {
            return false;
        }
        p += i;
        sz -= i;
    }
    return true;
}

char *net_get_string(int fd)
{
    int sz;
    char *s;

    if ( !net_check_type(fd, net_type_str))
    {
        return NULL;
    }
    read_data(fd, &sz, 4);
    sz = ntohl(sz);
    if ( sz < 0 )
    {
        net_error(net_type_none, net_type_str);
        return NULL;
    }
    if ( sz == 0 )
    {
        return strdup("");
    }
    s = malloc(sz + 1);
    if ( !read_data(fd, s, sz))
    {
        free(s);
        net_error(net_type_none, net_type_str);
        return NULL;
    }
    s[sz] = 0;
#ifdef __DEBUG_NETLIB__
    fprintf(stderr, "Reading string: %s\n", s);
#endif
    return s;
}

int net_get_int(int fd)
{
    int l;

    if ( !net_check_type(fd, net_type_int))
    {
        return -1;
    }
    if ( !read_data(fd, &l, 4))
    {
        net_error(net_type_none, net_type_int);
        return -1;
    }
#ifdef __DEBUG_NETLIB__
    fprintf(stderr, "Reading int: %d\n", ntohl(l));
#endif
    return ntohl(l);
}

bool net_get_flag(int fd)
{
    return net_get_char(fd);
}

char net_get_char(int fd)
{
    char c;

    if ( !net_check_type(fd, net_type_char))
    {
        return false;
    }
    if ( read(fd, &c, 1) != 1 )
    {
        net_error(net_type_none, net_type_char);
        return 0;
    }
#ifdef __DEBUG_NETLIB__
    fprintf(stderr, "Reading char: %d\n", c);
#endif
    return c;
}

float net_get_float(int fd)
{
    float f;

    if ( !net_check_type(fd, net_type_float))
    {
        return 1;
    }
    if ( !read_data(fd, &f, 4))
    {
        net_error(net_type_none, net_type_float);
        return 1;
    }
#ifdef __DEBUG_NETLIB__
    fprintf(stderr, "Reading float: %f\n", f);
#endif
    return f;
}

