#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net_private.h"

static net_error_hdlr_t *gl_net_error_handler = net_error_abort;
static void *gl_net_error_data = NULL;

const char *net_type_name(net_type_t t)
{
    switch ( t )
    {
    case net_type_str:
        return "string";
    case net_type_int:
        return "integer";
    case net_type_float:
        return "float";
    case net_type_char:
        return "char";
    case net_type_none:
        return "(none)";
    default:
        return "(unkown)";
    }
}

void net_set_error_handler(net_error_hdlr_t *hdlr, void *data)
{
    if ( hdlr != NULL )
    {
        gl_net_error_handler = hdlr;
    }
    else
    {
        gl_net_error_handler = net_error_ignore;
    }
    gl_net_error_data = data;
}

void net_error_abort(net_type_t get, net_type_t wanted, void *data)
{
    fprintf(stderr,
            "Fatal protocol error (was %s, excepted %s). Aborting.\n",
            net_type_name(get), net_type_name(wanted));
    exit(2);
}

void net_error_ignore(net_type_t get, net_type_t wanted, void *data)
{
}

void net_error(net_type_t get, net_type_t wanted)
{
#ifdef __DEBUG_NETLIB__
    fprintf(stderr, "Type error: got %#.2x (%s), wanted %#.2x (%s)\n",
        get, net_type_name(get), wanted, net_type_name(wanted));
#endif
    gl_net_error_handler(get, wanted, gl_net_error_data);
}
