

#ifndef __INTERFACE_CPP_H__
#define __INTERFACE_CPP_H__

/*
** General functions
*/

/* Yours functions */
extern "C" void		player_init(int team_id);
extern "C" void		player_new_turn(int turn_number);
extern "C" void		player_akx_turn(int akx_id);
extern "C" void		player_r4d2_turn(int r4d2_id);

/* Turn function */
extern "C" int		turn_number(void); 
extern "C" int		turn_counter(void); 

/* Time function */
extern "C" int		time_get_left(void);

/* Score function */
extern "C" int		score_get(void);

/* Error function */
extern "C" int		error_get(void);

/*
** Specific functions
*/
/*
  team_id < 0	=> tous sauf (-team_id)
  team_id = 0	=> sauvage
  team_id > 0	=> uniquement team_id
*/
 
/* R4D2 */
extern "C" int		r4d2_get_team(int r4d2_id);
extern "C" float	r4d2_get_pos_x(int r4d2_id);
extern "C" float	r4d2_get_pos_y(int r4d2_id);
extern "C" int		r4d2_get_status(int r4d2_id);
extern "C" float	r4d2_get_speed(void);
extern "C" float	r4d2_get_destroy_speed(void);

extern "C" int		r4d2_turn_take_r4d2(void);
extern "C" int		r4d2_turn_untake_r4d2(void);
extern "C" int		r4d2_turn_take_akx(void);
extern "C" int		r4d2_turn_untake_akx(void);

extern "C" int		r4d2_move(int r4d2_id, float dx, float dy);
extern "C" int		r4d2_take_akx(int r4d2_id, int akx_id);
extern "C" int		r4d2_take_r4d2(int r4d2_id, int target_id);

/* Anakronox */
extern "C" int		akx_get_team(int akx_id);
extern "C" float	akx_get_pos_x(int akx_id);
extern "C" float	akx_get_pos_y(int akx_id);
extern "C" int		akx_get_status(int akx_id);

extern "C" float	akx_pulse_angle(int akx_id);
extern "C" float	akx_pulse_destx(int akx_id);
extern "C" float	akx_pulse_desty(int akx_id);

extern "C" float	akx_get_speed(void);
extern "C" float	akx_get_power(void);
extern "C" float	akx_get_pulse_coef(void);
extern "C" float	akx_get_see_power(void);

extern "C" int		akx_move(int akx_id, float dx, float dy);
extern "C" int		akx_pulse(int akx_id, float dx, float dy, float angle);
extern "C" int		akx_link(int akx_id, int target_id);

/* Map */
extern "C" float	map_get_size_x(void);
extern "C" float	map_get_size_y(void);
extern "C" float	map_get_pulse(int team_id, float x, float y);
extern "C" float	map_get_pulse_id(int akx_id, float x, float y);
extern "C" int		map_count_akx();
extern "C" int		map_count_r4d2();
extern "C" int		map_count_my_akx();
extern "C" int		map_count_my_r4d2();
extern "C" int		map_get_nearest_akx_plot(float x, float y, int tm_id);
extern "C" int		map_get_nearest_r4d2_plot(float x, float y, int tm_id);
extern "C" int		map_get_nearest_akx(int akx_id, int team_id);
extern "C" int		map_get_nearest_r4d2(int r4d2_id, int team_id);

#endif
