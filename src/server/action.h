/* $Id: action.h,v 1.7 2001/04/26 17:57:08 glop Exp $ */

#ifndef __PROLO_ACTION_H__
#define __PROLO_ACTION_H__

/* gestion des actions */
typedef enum _r4d2_action_type_e_ { act_r4d2_move = 0, act_r4d2_take_r4d2 = 1, 
				    act_r4d2_take_akx = 2} r4d2_action_type_t;  
typedef enum _akx_action_type_e_ { act_akx_move = 0, act_akx_pulse = 1, 
				   act_akx_link = 2 } akx_action_type_t;  

/* actions R4D2 */
typedef struct _r4d2_move_s_
{
  float	x, y;
} r4d2_move_t;

typedef struct _r4d2_take_r4d2_s_
{
  int	target_id;
  int	turn_need;
} r4d2_take_r4d2_t;

typedef struct _r4d2_take_akx_s_
{
  int	target_id;
  int	turn_need;
} r4d2_take_akx_t;

typedef struct _r4d2_act_s_
{
  r4d2_action_type_t	type;
  union 
  {
    r4d2_move_t		move;
    r4d2_take_r4d2_t	take_r4d2;
    r4d2_take_akx_t	take_akx;
  } act;
} r4d2_act_t;

/* actions ANAKRONOX */
typedef struct _akx_move_s_
{
  float	x, y;
} akx_move_t;

typedef struct _akx_pulse_s_
{
  float	x, y;
  float	angle;              /* Radian */
  float	new_x, new_y;
  float	new_angle;              /* Radian */
} akx_pulse_t;

typedef struct _akx_link_s_
{
  int	target_id;
} akx_link_t;

typedef struct _akx_act_s_
{
  akx_action_type_t	type;
  union
  {
    akx_move_t		move;
    akx_pulse_t		pulse;
    akx_link_t		link;
  } act;
} akx_act_t;

#endif
