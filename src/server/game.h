#/* $Id: game.h,v 1.2 2001/05/02 11:32:08 glop Exp $ */

#ifndef __PROLO_GAME_H__
#define __PROLO_GAME_H__

float dist(float x1, float y1, float x2, float y2);
float angle(float x, float y, float x1, float y1, float x2, float y2);

float get_pulse_team(float x, float y, int team_id);
float get_pulse_id(float x, float y, int akx_id);
float get_pulse_foe(float x, float y, int team_id);
float get_pulse_total(float x, float y, int team_id);

#endif
