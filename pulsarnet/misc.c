/* $Id: misc.c,v 1.4 2001/04/29 15:44:12 kilobug Exp $ */

#include "net_private.h"

#define NET_ACK		0x42034212
#define NET_ABORT	0x42004200

gboolean net_wait_ack(int fd)
{
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Waiting for ACK\n");
#endif
  return net_chk_int(fd, NET_ACK);
}

void net_send_ack(FILE *file)
{
  if (file == NULL)
    return;
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Sending ACK\n");
#endif
  net_wr_int(file, NET_ACK);
  fflush(file);
}

void net_send_abort(FILE *file)
{
  if (file == NULL)
    return;
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Sending ERROR\n");
#endif
  net_wr_int(file, NET_ABORT);  
  fflush(file);
}

