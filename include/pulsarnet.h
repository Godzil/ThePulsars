#ifndef __PROLO_NET_LIB_H__
#define __PROLO_NET_LIB_H__

#include <stdio.h>

#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <general.h>

#define PROTOCOL_VERSION 0x4207

typedef enum
{
    net_type_int = 0x03, net_type_float = 0x0B,
    net_type_char = 0x30, net_type_str = 0xB0,
    net_type_none = 0x33
} net_type_t;

typedef void (net_error_hdlr_t)(net_type_t get, net_type_t wanted, void *data);

/* Error handlers */
void net_set_error_handler(net_error_hdlr_t *hdlr, void *data);

net_error_hdlr_t net_error_abort;
net_error_hdlr_t net_error_ignore;

const char *net_type_name(net_type_t t);

/* Opening sockets */
int net_connect_to(const char *host, int port, char *err, int errsz,
                   struct timeval *to);

int net_listen_at(int port, int max_con, char *err, int errsz);

/* Writing data */
void net_wr_string(FILE *file, const char *s);

void net_wr_int(FILE *file, int val);

void net_wr_flag(FILE *file, bool val);

void net_wr_char(FILE *file, char c);

void net_wr_float(FILE *file, float f);

/* Reading data */
char *net_get_string(int fd);

int net_get_int(int fd);

bool net_get_flag(int fd);

char net_get_char(int fd);

float net_get_float(int fd);

/* Checking values */
bool net_chk_string(int fd, const char *s);

bool net_chk_int(int fd, int val);

bool net_chk_flag(int fd, bool val);

bool net_chk_char(int fd, char c);

bool net_chk_float(int fd, float f);

int net_chk_str_list(int fd, ...); /* -1 = not in list */

/* Ack */
bool net_wait_ack(int fd);

void net_send_ack(FILE *file);

void net_send_abort(FILE *file);

#endif
