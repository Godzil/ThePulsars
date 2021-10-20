

#include "server.h"

void parser_check_type(GScanner *scan, GTokenType type, GTokenType wanted)
{
    if ( type != wanted )
    {
        g_scanner_unexp_token(scan, wanted, NULL, NULL, NULL, NULL, TRUE);
        server_abort();
    }
}

int parser_get_symbol_as_int(GScanner *scan)
{
    return *((int *) g_scanner_cur_value(scan).v_symbol );
}

double parser_get_symbol_as_float(GScanner *scan)
{
    return *((double *) g_scanner_cur_value(scan).v_symbol );
}

char *parser_get_symbol_as_string(GScanner *scan)
{
    return g_strdup(g_scanner_cur_value(scan).v_symbol);
}

int parser_get_int(GScanner *scan)
{
    scan->config->int_2_float = FALSE;
    switch ( g_scanner_get_next_token(scan))
    {
    case G_TOKEN_INT:
        return g_scanner_cur_value(scan).v_int;
    case G_TOKEN_SYMBOL:
        return parser_get_symbol_as_int(scan);
    default:
        g_scanner_unexp_token(scan, G_TOKEN_INT, NULL, NULL, NULL,
                              NULL, TRUE);
        server_abort();
    }
}

double parser_get_float(GScanner *scan)
{
    scan->config->int_2_float = TRUE;
    switch ( g_scanner_get_next_token(scan))
    {
    case '-':
        return -parser_get_float(scan);
    case G_TOKEN_FLOAT:
        return g_scanner_cur_value(scan).v_float;
    case G_TOKEN_SYMBOL:
        return parser_get_symbol_as_float(scan);
    default:
        g_scanner_unexp_token(scan, G_TOKEN_FLOAT, NULL, NULL, NULL,
                              NULL, TRUE);
        server_abort();
    }
}

char *parser_get_string(GScanner *scan)
{
    switch ( g_scanner_get_next_token(scan))
    {
    case G_TOKEN_STRING:
        return g_strdup(g_scanner_cur_value(scan).v_string);
    case G_TOKEN_SYMBOL:
        return parser_get_symbol_as_string(scan);
    default:
        g_scanner_unexp_token(scan, G_TOKEN_STRING, NULL, NULL, NULL,
                              NULL, TRUE);
        server_abort();
    }
}

