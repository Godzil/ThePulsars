/* $Id: conf.h,v 1.26 2001/05/03 17:18:04 kilobug Exp $ */

#ifndef __PROLO_CONF_H__
#define __PROLO_CONF_H__

#include <sys/types.h>


#include "network.h"

typedef struct _conf_t
{
  int		nb_turns;
  boolean_t	verbose;
  boolean_t	quiet;
  boolean_t	gfx, pub;
  gfx_t		*gfx_clients;
  const char	*map_name;
  GSList	*players;
  int		nb_objects;
  int		turn_num;
  pid_t		messager_pid;
  int		port;

  int		nb_akx;
  int		nb_r4d2;
  int		turn_msec;
  int		nb_players;

  int		turn_take_akx;
  int		turn_untake_akx;
  int		turn_take_r4d2;
  int		turn_untake_r4d2;
  float		r4d2_speed;
  float		akx_speed;
  float		pulse_power;
  float		see_power;
  float		pulse_coef;
  float		destroy_speed;
  float		size_x;
  float		size_y;
  char		*sand_file;
  sigset_t	*mask;
} conf_t;

extern conf_t	*gl_config;

#define	CONF_DEFAULT_PORT	4242
#define CONF_DEFAULT_MAP	"./default.map"
#define CONF_DEFAULT_NBTURNS	420
#define CONF_DEFAULT_TURN_MSEC	200
#define	DEF_TURN_TAKE_AKX	5
#define	DEF_TURN_UNTAKE_AKX	5
#define	DEF_TURN_TAKE_R4D2	5
#define	DEF_TURN_UNTAKE_R4D2	5
#define	DEF_R4D2_SPEED		2.0
#define	DEF_AKX_SPEED		0.1
#define	DEF_PULSE_POWER		1000.0
#define	DEF_SEE_POWER		1.0
#define	DEF_PULSE_COEF		1.0
#define	DEF_SIZE_X		1
#define	DEF_SIZE_Y		1
#define	DEF_DESTROY_SPEED	-1.0

#endif
