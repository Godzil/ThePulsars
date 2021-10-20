#ifndef __PROLO_PARSER_H__
#define __PROLO_PARSER_H__

#ifndef G_TOKEN_SEMICOLON
#define G_TOKEN_SEMICOLON ';'
#endif

void parser_check_type(GScanner *scan, GTokenType type, GTokenType wanted);

int parser_get_symbol_as_int(GScanner *scan);

double parser_get_symbol_as_float(GScanner *scan);

char *parser_get_symbol_as_string(GScanner *scan);

int parser_get_int(GScanner *scan);

double parser_get_float(GScanner *scan);

char *parser_get_string(GScanner *scan);

#endif
