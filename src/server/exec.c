#include "server.h"

void	calc_score()
{
  GSList	*l;
  player_t	*player;

  for (l = gl_config->players; l != NULL; l = l->next)
    {
      player = l->data;
      player->score = count_akx(player->team_id) + 
	count_r4d2(player->team_id) / ((float) gl_config->nb_r4d2);
    }
}

int	get_max(int *nbr, int len)
{
  int	max = nbr[0], team = 1;
  int	cpt;

  for (cpt = 1; cpt < len; cpt++)
    {
      if (nbr[cpt] > max)
	{
	  max = nbr[cpt];
	  team = cpt+1;
	}
      else
	{
	  if (nbr[cpt] == max)
	    team = 0;
	}
    }
  return team;
}

void	check_conflict_akx(int akx_id)
{
  int		*nbr_r4d2;
  int		cpt, winner;
  item_t	*temp;

  nbr_r4d2 = g_malloc(gl_config->nb_players * sizeof(int));
  bzero(nbr_r4d2, gl_config->nb_players * sizeof(int));
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      temp = gl_objects + cpt;
      if (temp->obj.type == obj_r4d2)
	{
	  if (temp->r4d2.action.type == act_r4d2_take_akx)
	      if (temp->r4d2.action.act.take_akx.target_id == akx_id)
		nbr_r4d2[temp->r4d2.team_id - 1]++;
	} 
    }
  winner = get_max(nbr_r4d2, gl_config->nb_players);
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      temp = gl_objects + cpt;
      if (temp->obj.type == obj_r4d2)
	{
	  if (temp->r4d2.action.type == act_r4d2_take_akx)
	      if (temp->r4d2.action.act.take_akx.target_id == akx_id)
		if (temp->r4d2.team_id != winner)
		  set_default_action_r4d2(cpt);
	} 
    }
  g_free(nbr_r4d2);
}

void	check_conflict_r4d2(int r4d2_id)
{
  int		*nbr_r4d2;
  int		cpt, winner;
  item_t	*temp;

  nbr_r4d2 = g_malloc(gl_config->nb_players * sizeof(int));
  bzero(nbr_r4d2, gl_config->nb_players * sizeof(int));
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      temp = gl_objects + cpt;
      if (temp->obj.type == obj_r4d2)
	{
	  if (temp->r4d2.action.type == act_r4d2_take_r4d2)
	      if (temp->r4d2.action.act.take_r4d2.target_id == r4d2_id)
		nbr_r4d2[temp->r4d2.team_id - 1]++;
	} 
    }
  winner = get_max(nbr_r4d2, gl_config->nb_players);
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      temp = gl_objects + cpt;
      if (temp->obj.type == obj_r4d2)
	{
	  if (temp->r4d2.action.type == act_r4d2_take_r4d2)
	      if (temp->r4d2.action.act.take_r4d2.target_id == r4d2_id)
		if (temp->r4d2.team_id != winner)
		  set_default_action_r4d2(cpt);
	} 
    }
  g_free(nbr_r4d2);
}

void	check_conflict()
{
  int		cpt;
  item_t	*temp;

  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      temp = gl_objects + cpt;
      if (temp->obj.type == obj_r4d2)
	{
	  if (temp->r4d2.action.type == act_r4d2_take_akx)
	    check_conflict_akx(temp->r4d2.action.act.take_akx.target_id);
	  else
	    if (temp->r4d2.action.type == act_r4d2_take_r4d2)
	      check_conflict_r4d2(temp->r4d2.action.act.take_r4d2.target_id);
	} 
    }
}

void	exec_destroy(r4d2_t *r4d2)
{
  float	speed;

  speed = get_pulse_total(r4d2->x, r4d2->y, r4d2->team_id);
  speed = gl_config->r4d2_speed + gl_config->pulse_coef * speed;
    
  if (speed < 0)
    {
      if (speed <= gl_config->destroy_speed)
	{
	  r4d2->change = TRUE;
	  r4d2->team_id = 0;
	  set_default_action_r4d2(r4d2->id);
	} 
    }
}

void	check_take()
{
  int		cpt, tid;
  item_t	*it;

  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      it = gl_objects + cpt;
      if (it->obj.type == obj_r4d2)
	{
	  switch (it->r4d2.action.type)
	    {
	    case act_r4d2_take_akx:
	      tid = it->r4d2.action.act.take_akx.target_id;
	      if (dist(gl_objects[cpt].r4d2.x,
		       gl_objects[cpt].r4d2.y,
		       gl_objects[tid].akx.x,
		       gl_objects[tid].akx.y) 
		  > DIST_MIN_TAKE)
		set_default_action_r4d2(cpt);
	      break;
	    case act_r4d2_take_r4d2:
	      tid = it->r4d2.action.act.take_r4d2.target_id;
	      if (dist(gl_objects[cpt].r4d2.x,
		       gl_objects[cpt].r4d2.y,
		       gl_objects[tid].r4d2.x,
		       gl_objects[tid].r4d2.y) 
		  > DIST_MIN_TAKE)
		set_default_action_r4d2(cpt);
	      break;
	    default:
	      break;
	    }
	}
    }
}

void	exec_objects()
{
  int	cpt;

  check_take();
  check_conflict();
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      if (gl_objects[cpt].obj.type == obj_r4d2)
	exec_destroy(&(gl_objects[cpt].r4d2));
    }
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      if (gl_objects[cpt].obj.type == obj_akx)
	{
	  if (gl_objects[cpt].akx.action.type == act_akx_move)
	    exec_akx_move(&(gl_objects[cpt].akx));
	}
      else
	if ((gl_objects[cpt].obj.type == obj_r4d2) &&
	    (gl_objects[cpt].r4d2.action.type == act_r4d2_move))
	  exec_r4d2_move(&(gl_objects[cpt].r4d2));
    }
  /* AJOUT MATHIAS */
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      if (gl_objects[cpt].obj.type == obj_akx)
	exec_akx_reset_energy(&(gl_objects[cpt].akx));
    }


  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      if ((gl_objects[cpt].obj.type == obj_akx) &&
	  (gl_objects[cpt].akx.action.type == act_akx_link))
	exec_akx_link(&(gl_objects[cpt].akx));
    }
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      if (gl_objects[cpt].obj.type == obj_r4d2)
	{
	  if (gl_objects[cpt].akx.action.type == act_r4d2_take_akx)
	    exec_r4d2_take_akx(&gl_objects[cpt].r4d2);
	  else
	    if (gl_objects[cpt].r4d2.action.type == act_r4d2_take_r4d2)
	      exec_r4d2_take_r4d2(&gl_objects[cpt].r4d2);
	}
    }
  for (cpt = 0; cpt < gl_config->nb_objects; cpt++)
    {
      if ((gl_objects[cpt].obj.type == obj_akx) &&
	  (gl_objects[cpt].akx.action.type == act_akx_pulse))
	exec_akx_pulse(&(gl_objects[cpt].akx));
    }

  calc_score();
  gfx_client_new_turn(FALSE);
}

void	exec_r4d2_move(r4d2_t *r4d2)
{
  float	diff_x, diff_y;
  float	signe_x, signe_y;
  float	prop_x, prop_y;
  float	speed;

  speed = gl_config->r4d2_speed + gl_config->pulse_coef * 
    get_pulse_total(r4d2->x, r4d2->y, r4d2->team_id);
  if (speed < 0)
    speed = 0;
  if (speed > (gl_config->r4d2_speed * 2))
    speed = gl_config->r4d2_speed * 2;

  diff_x = r4d2->action.act.move.x - r4d2->x;
  diff_y = r4d2->action.act.move.y - r4d2->y;
  signe_x = SIGNE(diff_x);
  signe_y = SIGNE(diff_y);
  if (signe_x || signe_y)
    {
      r4d2->change = TRUE;
      if (!signe_x || !signe_y)
	{
	  r4d2->x += signe_x * speed;
	  r4d2->y += signe_y * speed;
	  diff_x = r4d2->action.act.move.x - r4d2->x;
	  diff_y = r4d2->action.act.move.y - r4d2->y;
	  if (signe_x != SIGNE(diff_x))
	    r4d2->x = r4d2->action.act.move.x; 
	  if (signe_y != SIGNE(diff_y))
	    r4d2->y = r4d2->action.act.move.y; 
	}
      else
	{
	  prop_x = diff_x / sqrt(SQR(diff_x) + SQR(diff_y));
	  prop_y = diff_y / sqrt(SQR(diff_x) + SQR(diff_y));
	  r4d2->x += speed * prop_x;
	  r4d2->y += speed * prop_y;
	  diff_x = r4d2->action.act.move.x - r4d2->x;
	  diff_y = r4d2->action.act.move.y - r4d2->y;
	  if (signe_x != SIGNE(diff_x))
	    r4d2->x = r4d2->action.act.move.x; 
	  if (signe_y != SIGNE(diff_y))
	    r4d2->y = r4d2->action.act.move.y; 
	}
      r4d2->x = CLAMP(r4d2->x, 0, gl_config->size_x);
      r4d2->y = CLAMP(r4d2->y, 0, gl_config->size_y);
    }
}

void	exec_akx_move(akx_t *akx)
{
  float	diff_x, diff_y;
  float	signe_x, signe_y;
  float	prop_x, prop_y;

  diff_x = akx->action.act.move.x - akx->x;
  diff_y = akx->action.act.move.y - akx->y;
  signe_x = SIGNE(diff_x);
  signe_y = SIGNE(diff_y);
  if (signe_x || signe_y)
    {
      akx->change = TRUE;
      if (!signe_x || !signe_y)
	{
	  akx->x += signe_x * gl_config->akx_speed;
	  akx->y += signe_y * gl_config->akx_speed;
	  diff_x = akx->action.act.move.x - akx->x;
	  diff_y = akx->action.act.move.y - akx->y;
	  if (signe_x != SIGNE(diff_x))
	    akx->x = akx->action.act.move.x; 
	  if (signe_y != SIGNE(diff_y))
	    akx->y = akx->action.act.move.y; 
	}
      else
	{
	  prop_x = diff_x / sqrt(SQR(diff_x) + SQR(diff_y));
	  prop_y = diff_y / sqrt(SQR(diff_x) + SQR(diff_y));
	  akx->x += gl_config->akx_speed * prop_x;
	  akx->y += gl_config->akx_speed * prop_y;
	  diff_x = akx->action.act.move.x - akx->x;
	  diff_y = akx->action.act.move.y - akx->y;
	  if (signe_x != SIGNE(diff_x))
	    akx->x = akx->action.act.move.x; 
	  if (signe_y != SIGNE(diff_y))
	    akx->y = akx->action.act.move.y; 
	}
      akx->x = CLAMP(akx->x, 0, gl_config->size_x);
      akx->y = CLAMP(akx->y, 0, gl_config->size_y);
    }
}

void	exec_r4d2_take_r4d2(r4d2_t *r4d2)
{
  r4d2->action.act.take_r4d2.turn_need--;
  if (r4d2->action.act.take_r4d2.turn_need <= 0)
    {
      gl_objects[r4d2->action.act.take_r4d2.target_id].obj.change = TRUE;
      gl_objects[r4d2->action.act.take_r4d2.target_id].obj.team_id = 
	r4d2->team_id;
      r4d2->action.type = act_r4d2_move;
      r4d2->action.act.move.x = r4d2->x;
      r4d2->action.act.move.y = r4d2->y;
    }
}

void	exec_r4d2_take_akx(r4d2_t *r4d2)
{
  r4d2->action.act.take_akx.turn_need--;
  if (r4d2->action.act.take_akx.turn_need <= 0)
    {
      gl_objects[r4d2->action.act.take_akx.target_id].obj.change = TRUE;
      gl_objects[r4d2->action.act.take_akx.target_id].obj.team_id = 
	r4d2->team_id;
      r4d2->action.type = act_r4d2_move;
      r4d2->action.act.move.x = r4d2->x;
      r4d2->action.act.move.y = r4d2->y;
    }
}

void	exec_akx_pulse(akx_t *akx)
{
  akx->action.act.pulse.x = akx->action.act.pulse.new_x;
  akx->action.act.pulse.y = akx->action.act.pulse.new_y;
  akx->action.act.pulse.angle = akx->action.act.pulse.new_angle;
  
  /* VIRE PAR MATHIAS akx->energy = gl_config->pulse_power; */
}

/* AJOUT MATHIAS */
void exec_akx_reset_energy(akx_t *akx)
{
  akx->energy = gl_config->pulse_power;
}

void	exec_akx_link(akx_t *akx)
{
  float	dst;

  if (akx->team_id != gl_objects[akx->action.act.link.target_id].obj.team_id)
    {
       gl_objects[akx->id].obj.change = TRUE;
       set_default_action_akx(akx->id);
      return;
    }

  dst = dist(akx->x, akx->y, 
	     gl_objects[akx->action.act.link.target_id].obj.x,
	     gl_objects[akx->action.act.link.target_id].obj.y);
  gl_objects[akx->action.act.link.target_id].akx.energy += 
    akx->energy / (1 + sqrt(dst));

  akx->energy = gl_config->pulse_power;
}

