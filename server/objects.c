/* $Id: objects.c,v 1.5 2001/04/06 17:36:55 glop Exp $ */

#include <server.h>
#include <objects.h>
#include <client.h>

#include <general.h>

item_t *glbObjects;

void new_akx(float x, float y, int id)
{
   item_t *res;

   res = glbObjects + id;
   res->akx.change = False;
   res->akx.x = x;
   res->akx.y = y;
   res->akx.id = id;
   res->akx.type = obj_akx;
   res->akx.energy = gl_config->pulse_power;
   res->akx.team_id = 0;
   set_default_action_akx(id);
}

void new_r4d2(float x, float y, int id)
{
   item_t *res;

   res = glbObjects + id;
   res->r4d2.change = False;
   res->r4d2.x = x;
   res->r4d2.y = y;
   res->r4d2.id = id;
   res->r4d2.type = obj_r4d2;
   res->r4d2.team_id = 0;
   set_default_action_r4d2(id);
}
