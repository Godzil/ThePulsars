/* $Id: net.h,v 1.8 2001/04/28 15:29:25 kilobug Exp $ */

#ifndef __PROLO_NET_LIB_H__
#define __PROLO_NET_LIB_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "../general.h"
#include <glib.h>

typedef enum { net_type_int = 0x03, net_type_float = 0x0B, 
	       net_type_char = 0x30, net_type_str = 0xB0,
               net_type_none = 0x33 } net_type_t;
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
void net_wr_flag(FILE *file, gboolean val);
void net_wr_char(FILE *file, char c);
void net_wr_float(FILE *file, float f);

/* Reading data */
char *net_get_string(int fd);
int net_get_int(int fd);
gboolean net_get_flag(int fd);
char net_get_char(int fd);
float net_get_float(int fd);

/* Checking values */
gboolean net_chk_string(int fd, const char *s);
gboolean net_chk_int(int fd, int val);
gboolean net_chk_flag(int fd, gboolean val);
gboolean net_chk_char(int fd, char c);
gboolean net_chk_float(int fd, float f);
int net_chk_str_list(int fd, ...); /* -1 = not in list */

/* Ack */
gboolean net_wait_ack(int fd);
void net_send_ack(FILE *file);
void net_send_abort(FILE *file);

#endif
