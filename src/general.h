/* $Id: general.h,v 1.11 2001/04/30 16:02:05 kilobug Exp $ */

#ifndef __PROLO_GENERAL_H__
#define __PROLO_GENERAL_H__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_STDIO_H
# include <stdio.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_CTYPE_H
# include <ctype.h>
#endif

#ifdef HAVE_MATH_H
# include <math.h>
#endif

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifdef WANT_PROTOCOL_VERSION
# define PROTOCOL_VERSION 0x4207
#endif

#endif


