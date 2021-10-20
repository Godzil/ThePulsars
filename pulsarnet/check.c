#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "net_private.h"

bool net_chk_string(int fd, const char *s)
{
    char *st;
    bool res;

    if ( s == NULL )
    {
        return false;
    }
    st = net_get_string(fd);
    if ( st == NULL )
    {
        return false;
    }
    res = strcmp(s, st);
    free(st);
    return !res;
}

int net_chk_str_list(int fd, ...)
{
    va_list va;
    char *st;
    const char *s;
    int res = -1;
    int i;

    va_start(va, fd);
    st = net_get_string(fd);
    if ( st == NULL )
    {
        return res;
    }
    for ( i = 0 ;; i++ )
    {
        s = va_arg(va, const char *);
        if ( s == NULL )
        {
            break;
        }
        if ( !strcmp(s, st))
        {
            res = i;
            break;
        }
    }
    va_end(va);
    free(st);
    return res;
}

bool net_chk_int(int fd, int val)
{
    return (val == net_get_int(fd));
}

bool net_chk_flag(int fd, bool val)
{
    return (val == net_get_flag(fd));
}

bool net_chk_char(int fd, char c)
{
    return (c == net_get_char(c));
}

bool net_chk_float(int fd, float f)
{
    return (f == net_get_char(f));
}
