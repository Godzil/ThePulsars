/* $Id: public.h,v 1.14 2001/05/04 10:29:52 glop Exp $ */

#ifndef __PROLO_PUBLIC_H__
#define __PROLO_PUBLIC_H__

/*
** General functions
*/

/* Turn function */
int	turn_number(void); /* nombre de tour total */
int	turn_counter(void); /* numero du tour en cour */

/* Time function */
int	time_get_left(void);

/* Score function */
int	score_get(void);

/* Error function */
int	error_get(void);

/*
** Specific functions
*/

/*
  team_id < 0	=> tous sauf (-team_id)
  team_id = 0	=> sauvage
  team_id > 0	=> uniquement team_id
*/
  

/* R4D2 */
int	r4d2_get_team(int r4d2_id);
float	r4d2_get_pos_x(int r4d2_id);
float	r4d2_get_pos_y(int r4d2_id);
int	r4d2_get_status(int r4d2_id);
float	r4d2_get_speed(void);
float	r4d2_get_destroy_speed(void);

int	r4d2_turn_take_r4d2(void);
int	r4d2_turn_untake_r4d2(void);
int	r4d2_turn_take_akx(void);
int	r4d2_turn_untake_akx(void);

int	r4d2_move(int r4d2_id, float dx, float dy);
int	r4d2_take_akx(int r4d2_id, int akx_id);
int	r4d2_take_r4d2(int r4d2_id, int target_id);

/* Anakronox */
int	akx_get_team(int akx_id);
float	akx_get_pos_x(int akx_id);
float	akx_get_pos_y(int akx_id);
int	akx_get_status(int akx_id);

float	akx_pulse_angle(int akx_id);
float	akx_pulse_destx(int akx_id);
float	akx_pulse_desty(int akx_id);

float	akx_get_speed(void);
float	akx_get_power(void);
float	akx_get_pulse_coef(void);
float	akx_get_see_power(void);

int	akx_move(int akx_id, float dx, float dy);
int	akx_pulse(int akx_id, float dx, float dy, float angle);
int	akx_link(int akx_id, int target_id);

/* Map */
float	map_get_size_x(void);
float	map_get_size_y(void);
float	map_get_pulse(int team_id, float x, float y);
float	map_get_pulse_id(int akx_id, float x, float y);
int	map_count_akx(void);
int	map_count_r4d2(void);
int	map_count_my_akx(void);
int	map_count_my_r4d2(void);
int	map_get_nearest_akx_plot(float x, float y, int team_id);
int	map_get_nearest_r4d2_plot(float x, float y, int team_id);
int	map_get_nearest_akx(int id, int team_id);
int	map_get_nearest_r4d2(int id, int team_id);

#endif

