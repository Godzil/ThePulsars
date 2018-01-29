/* $Id: check.c,v 1.5 2001/04/29 17:45:19 kilobug Exp $ */

#include "net_private.h"

gboolean net_chk_string(int fd, const char *s)
{
  char *st;
  gboolean res;

  if (s == NULL)
    return FALSE;
  st = net_get_string(fd);
  if (st == NULL)
    return FALSE;
  res = strcmp(s, st);
  g_free(st);
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
  if (st == NULL)
    return res;
  for (i = 0; ; i++)
    {
      s = va_arg(va, const char *);
      if (s == NULL)
	break;
      if (!strcmp(s, st))
	{
	  res = i;
	  break;
	}
    }
  va_end(va);
  g_free(st);
  return res;
}

gboolean net_chk_int(int fd, int val)
{
  return (val == net_get_int(fd));
}

gboolean net_chk_flag(int fd, gboolean val)
{
  return (val == net_get_flag(fd));
}

gboolean net_chk_char(int fd, char c)
{
  return (c == net_get_char(c));
}

gboolean net_chk_float(int fd, float f)
{
  return (f == net_get_char(f));
}
