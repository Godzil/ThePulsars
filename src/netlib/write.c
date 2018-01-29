/* $Id: write.c,v 1.13 2001/04/30 16:02:07 kilobug Exp $ */

#include "net_private.h"

static void net_write_type(FILE *file, net_type_t type)
{
  unsigned char c;

  if (file == NULL)
    return;
  c = type;
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Writing type: %#x\n", c);
#endif
  fwrite(&c, 1, 1, file);
}

void net_wr_string(FILE *file, const char *s)
{
  int l;

  if (file == NULL)
    return;
  net_write_type(file, net_type_str);
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Sending string: %s\n", s);
#endif
  if (s == NULL)
    l = 0;
  else
    l = strlen(s);
  l = g_htonl(l);
  fwrite(&l, 4, 1, file);;
  l = g_ntohl(l);
  fwrite(s, 1, l, file);;
}

void net_wr_int(FILE *file, int val)
{
  int l;

  if (file == NULL)
    return;
  net_write_type(file, net_type_int);
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Sending int: %d\n", val);
#endif
  l = g_htonl(val);
  fwrite(&l, 4, 1, file);;
}

void net_wr_flag(FILE *file, gboolean val)
{
  if (file == NULL)
    return;
  net_wr_char(file, val);
}

void net_wr_char(FILE *file, char c)
{
  if (file == NULL)
    return;
  net_write_type(file, net_type_char);
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Sending char: %d\n", c);
#endif
  fwrite(&c, 1, 1, file);
}

void net_wr_float(FILE *file, float f)
{
  if (file == NULL)
    return;
  net_write_type(file, net_type_float);
#ifdef __DEBUG_NETLIB__
  fprintf(stderr, "Sending float: %f\n", f);
#endif
  fwrite(&f, 4, 1, file);
}
