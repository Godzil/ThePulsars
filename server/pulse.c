/* $Id: pulse.c,v 1.14 2001/05/06 07:23:34 kilobug Exp $ */

#include <private.h>
#include <server.h>
#include <action.h>
#include <game.h>

float get_pulse_team(float x, float y, int team_id)
{
   float result = 0, r, agl;
   akx_t *akx;
   akx_pulse_t *pulse;
   int i;

   for (i = 0 ; i < gl_config->nb_objects ; i++)
   {
      akx = &gl_objects[i].akx;
      if (((akx->team_id == team_id) ||
           ((akx->team_id != -team_id) && (team_id < 0) && (akx->team_id))) &&
          (akx->type == obj_akx) &&
          (akx->action.type == act_akx_pulse))
      {
         pulse = &akx->action.act.pulse;
         if ((x != akx->x) || (y != akx->y))
         {
            r = dist(pulse->x, pulse->y, akx->x, akx->y);
            if (r >= dist(x, y, akx->x, akx->y))
            {
               agl = angle(x, y, akx->x, akx->y, pulse->x, pulse->y);
               if (agl <= (pulse->angle / 2))
               {
                  result += akx->energy / (1 + r * pulse->angle / 2);
               }
            }
            /*	    else
                printf("AGL :  D(%f %f) A(%f %f) P(%f %f)%f\n", x, y, akx->x, akx->y, pulse->x, pulse->y, agl);*/
         }
      }
   }
   return result;
}

float get_pulse_id(float x, float y, int akx_id)
{
   float result = 0, r, agl;
   akx_t *akx;
   akx_pulse_t *pulse;

   akx = &gl_objects[akx_id].akx;
   if (akx->action.type == act_akx_pulse)
   {
      pulse = &akx->action.act.pulse;
      if ((x != akx->x) || (y != akx->y))
      {
         r = dist(pulse->x, pulse->y, akx->x, akx->y);
         if (r >= dist(x, y, akx->x, akx->y))
         {
            agl = angle(x, y, akx->x, akx->y, pulse->x, pulse->y);
            if (agl <= (pulse->angle / 2))
            {
               result += akx->energy / (1 + r * pulse->angle / 2);
            }
         }
      }
   }
   return result;
}

float get_pulse_foe(float x, float y, int team_id)
{
   return get_pulse_team(x, y, -team_id);
}

float get_pulse_total(float x, float y, int team_id)
{
   return get_pulse_team(x, y, team_id) - get_pulse_foe(x, y, team_id);
}
