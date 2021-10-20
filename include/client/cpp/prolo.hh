

#ifndef THE_PULSAR_BITMAP_H
#define THE_PULSAR_BITMAP_H

#include "interface.hh"

/*
** The class you must complete
*/
class Player
{
public:
  void	Init(int team_id);
  void	NewTurn(int turn_number);
  void	AkxTurn(int akx_id);
  void	R4d2Turn(int r4d2_id);
private:
};

/*
** The classes you can use
*/
class General
{
public:
  static int TurnNumber(void)	{return turn_number();};
  static int TurnCounter(void)	{return turn_counter();};
  static int GetTimeLeft(void)	{return time_get_left();};
  static int GetScore(void)	{return score_get();};
  static int GetError(void)	{return error_get();};
};

class R4D2
{
public:
  static int	GetTeam(int r4d2_id)	{return r4d2_get_team(r4d2_id);};
  static float GetPosX(int r4d2_id)	{return r4d2_get_pos_x(r4d2_id);};
  static float GetPosY(int r4d2_id)	{return r4d2_get_pos_y(r4d2_id);};
  static int	GetStatus(int r4d2_id)	{return r4d2_get_status(r4d2_id);};
  
  static float	GetSpeed(void)		{return r4d2_get_speed();};
  static float	GetDestroySpeed(void)	{return r4d2_get_destroy_speed();};
  
  static int	TurnTakeR4d2(void)	{return r4d2_turn_take_r4d2();};
  static int	TurnUntakeR4d2(void)	{return r4d2_turn_untake_r4d2();};
  static int	TurnTakeAkx(void)	{return r4d2_turn_take_akx();};
  static int	TurnUntakeAkx(void)	{return r4d2_turn_untake_akx();};
  
  static int	Move(int r4d2_id, float dx, float dy)	 {return r4d2_move(r4d2_id, dx, dy);};
  static int	TakeAkx(int r4d2_id, int akx_id)	 {return r4d2_take_akx(r4d2_id, akx_id);};
  static int	TakeR4d2(int r4d2_id, int target_id)	 {return r4d2_take_r4d2(r4d2_id, target_id);};
};

class Anakronox
{
public:
  static int	GetTeam(int akx_id)	{return akx_get_team(akx_id);};
  static float	GetPosX(int akx_id)	{return akx_get_pos_x(akx_id);};
  static float	GetPosY(int akx_id)	{return akx_get_pos_y(akx_id);};
  static int	GetStatus(int akx_id)	{return akx_get_status(akx_id);};

  static float	GetPulseAngle(int akx_id){return akx_pulse_angle(akx_id);};
  static float	GetPulseDestX(int akx_id){return akx_pulse_destx(akx_id);};
  static float	GetPulseDestY(int akx_id){return akx_pulse_desty(akx_id);};
 
  static float	GetSpeed(void)		{return akx_get_speed();};
  static float	GetPower(void)		{return akx_get_power();};
  static float	GetPulseCoef(void)	{return akx_get_pulse_coef();};
  static float	GetSeePower(void)	{return akx_get_see_power();};
  
  static int	Move(int akx_id, float dx, float dy)
  {return akx_move(akx_id, dx, dy);};
  static int	Pulse(int akx_id, float dx, float dy, float angle) 
  {return akx_pulse(akx_id, dx, dy, angle);};
  static int	Link(int akx_id, int target_id)		
  {return akx_link(akx_id, target_id);};
};

class Map
{
public:
  static float	GetSizeX(void)		{return map_get_size_x();};
  static float	GetSizeY(void)		{return map_get_size_y();};
  static float	GetPulse(int team_id, float x, float y)	
  {return map_get_pulse(team_id, x, y);};
  static float	GetPulseId(int akx_id, float x, float y)	
  {return map_get_pulse_id(akx_id, x, y);};
  static int	CountAkx(void)		{return map_count_akx();};
  static int	CountR4d2(void)		{return map_count_r4d2();};
  static int	CountMyAkx(void)	{return map_count_my_akx();};
  static int	CountMyR4d2(void)	{return map_count_my_r4d2();};
  static int	GetNearestAkxPlot(float x, float y, int team_id)
  {return map_get_nearest_akx_plot(x, y, team_id);};
  static int	GetNearestR4d2Plot(float x, float y, int team_id)
  {return map_get_nearest_r4d2_plot(x, y, team_id);};
  static int	GetNearestAkx(int akx_id, int team_id)	
  {return map_get_nearest_akx(akx_id, team_id);};
  static int	GetNearestR4d2(int r4d2_id, int team_id)	
			      {return map_get_nearest_r4d2(r4d2_id, team_id);};
};

#endif
