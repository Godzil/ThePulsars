#include "client.h"

#define OFFSET(X, Y)    (((X) + (Y) * MINI_MAP_SIZE) * 4)
#define	RED(X,Y)	(OFFSET(X, Y) + 0)
#define	GREEN(X,Y)	(OFFSET(X, Y) + 1)
#define	BLUE(X,Y)	(OFFSET(X, Y) + 2)
#define	ID(X,Y)		(OFFSET(X, Y) + 3)

#define	RED_COLOR(map, id)	((map)->players[(id)]->colors[2])
#define	GREEN_COLOR(map, id)	((map)->players[(id)]->colors[1])
#define	BLUE_COLOR(map, id)	((map)->players[(id)]->colors[0])

#define	CONV_X(map, X)	(((X) * MINI_MAP_SIZE) / ((map)->sizex + 0.1))
#define	CONV_Y(map, Y)	(((Y) * MINI_MAP_SIZE) / ((map)->sizey + 0.1))

static void mm_plot_point_ex(guchar *minimap, guchar id, guchar sat, int x, int y, 
			  guchar r, guchar g, guchar b)
{
  int	temp;  

  if ((x < 0) || (x >= MINI_MAP_SIZE))
    return;
  if ((y < 0) || (y >= MINI_MAP_SIZE))
    return;
  if (minimap[ID(x, y)] == id)
    return;

  temp = minimap[RED(x,y)];
  if (temp < sat)
    {
      temp += r;
      if (temp > sat)
	temp = sat;
      minimap[RED(x,y)] = temp;
    }

  temp = minimap[GREEN(x,y)];
  if (temp < sat)
    {
      temp += g;
      if (temp > sat)
	temp = sat;
      minimap[GREEN(x,y)] = temp;
    }
  
  temp = minimap[BLUE(x,y)];
  if (temp < sat)
    {
      temp += b;
      if (temp > sat)
	temp = sat;
      minimap[BLUE(x,y)] = temp;
    }
  minimap[ID(x, y)] = id;
}

static void mm_plot_point(guchar *minimap, int x, int y, 
			  guchar r, guchar g, guchar b)
{
  int	temp;  

  if ((x < 0) || (x >= MINI_MAP_SIZE))
    return;
  if ((y < 0) || (y >= MINI_MAP_SIZE))
    return;
  temp = minimap[RED(x,y)] + r;
  if (temp > 0xFF)
    temp = 0xFF;
  minimap[RED(x,y)] = temp;

  temp = minimap[GREEN(x,y)] + g;
  if (temp > 0xFF)
    temp = 0xFF;
  minimap[GREEN(x,y)] = temp;

  temp = minimap[BLUE(x,y)] + b;
  if (temp > 0xFF)
    temp = 0xFF;
  minimap[BLUE(x,y)] = temp;
}

static void mm_plot_line(guchar *minimap, int x1, int y1, int x2, int y2, 
		      guchar r, guchar g, guchar b)
{
  float	a;
  int	diffx, diffy;
  int	x, y;

  diffx = x1 - x2;
  diffy = y1 - y2;
  if (!diffx && !diffy)
    mm_plot_point(minimap, x1, y2, r, g, b);
  else
    {
      if (ABS(diffx) > ABS(diffy))
	{
	  a = (float)diffy / diffx;
	  if (x1 > x2)
	    {
	      x = x2;
	      x2 = x1;
	      x1 = x;
	      y = y1;
	      y1 = y2;
	      y2 = y;
	    }
	  else
	    x = x1;
	  while (x != x2)
	    {
	      mm_plot_point(minimap, x, y1 + (x - x1) * a, r, g, b);
	      x++;
	    }
	}
      else
	{
	  a = (float)diffx / diffy;
	  if (y1 > y2)
	    {
	      y = y2;
	      y2 = y1;
	      y1 = y;
	      x = x1;
	      x1 = x2;
	      x2 = x;
	    }
	  else
	    y = y1;
	  while (y != y2)
	    {
	      mm_plot_point(minimap, x1 + (y - y1) * a, y, r, g, b);
	      y++;
	    }
	}
    }
}

static float mm_rotate_x(float x, float y, float cs, float sn)
{
  return cs * x + sn * y;
}

static float mm_rotate_y(float x, float y, float cs, float sn)
{
  return cs * y - sn * x;
}

static int mm_distance(int x1, int y1, int x2, int y2)
{
  return (SQR(x2 - x1) + SQR(y2 - y1));
}

static void mm_plot_triangle(guchar *minimap, guchar id, guchar sat,
			     int x1, int y1, int x2, int y2, int x3, int y3,
			     int dist, guchar cr, guchar cg, guchar cb)
{
  int	swap;
  float	delta1, delta2, delta3;
  float	xa, xb;
  int	x, y;
  int	memx = x1, memy = y1;

  if (y1 > y2) 
    {
      swap  = x1;
      x1    = x2;
      x2    = swap;

      swap  = y1;
      y1    = y2;
      y2    = swap;
    }
  if (y1 > y3)
    {
      swap  = x3;
      x3    = x1;
      x1    = swap;

      swap  = y1;
      y1    = y3;
      y3    = swap;
    }
  
  if (y2 > y3) 
    {
      swap  = x3;
      x3    = x2;
      x2    = swap;
  
      swap  = y3;
      y3    = y2;
      y2    = swap;
    }


  /* determination des coefficients directeurs */
  if (y1 != y3)
    delta1 = ((float)x1-x3)/(y1-y3);  
  else
    delta1 = 0;
  
  if (y1 != y2)
    delta2 = ((float)x2-x1)/(y2-y1);
  else
    delta2 = 0;
  
  if (y2 != y3)  
    delta3 = ((float)x3-x2)/(y3-y2);
  else
    delta3 = 0;
  
  /* Premi�re partie du triangle ( tracage des section horizontale) */

  xa = x1;
  xb = x1;

  if (y1 != y2)
    {
      if (delta1 < delta2)
	for (y = y1; y < y2; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta2;
	    for (x = xa; x < xb; x++)
	      if (mm_distance(memx, memy, x, y) <= dist)
		mm_plot_point_ex(minimap, id, sat, x, y, cr, cg, cb);
	  }
      else
	for (y = y1; y < y2; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta2;
	    for (x = xb; x < xa; x++)
	      if (mm_distance(memx, memy, x, y) <= dist)
		mm_plot_point_ex(minimap, id, sat, x, y, cr, cg, cb);
	  }
    }
  else 
    xb = x2;

  /* Deuxieme partie du triangle ( tracage des section horizontale) */

  if (y2 != y3) 
    {
      if (delta3 < delta1) 
	for (y = y2; y < y3; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta3;
            for (x = xa; x < xb; x++)
	      if (mm_distance(memx, memy, x, y) <= dist)
		mm_plot_point_ex(minimap, id, sat, x, y, cr, cg, cb);
	  }
      else
	for (y = y2; y < y3; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta3;
	    for (x = xb; x < xa; x++)
	      if (mm_distance(memx, memy, x, y) <= dist)
		mm_plot_point_ex(minimap, id, sat, x, y, cr, cg, cb);
	  }
    }
}


static void mm_plot_cone(guchar *minimap, guchar id, guchar sat, 
			 float x1, float y1, float x2, float y2, 
			 float angle, guchar r, guchar g, guchar b)
{
  float cs, sn, csi;
  float x, y, xc, xd, yc, yd;
  bool call_sub = FALSE;

  x = x2 - x1;
  y = y2 - y1;
  if (angle > M_PI_2)
    {
      call_sub = TRUE;
      angle /= 2;
    }
  cs = cos(angle / 2);
  sn = sin(angle / 2);
  xc = mm_rotate_x(x, y, cs, sn);
  xd = mm_rotate_x(x, y, cs, -sn);
  yc = mm_rotate_y(x, y, cs, sn);
  yd = mm_rotate_y(x, y, cs, -sn);
  if (call_sub)
    {
      mm_plot_cone(minimap, id, sat, x1, y1, xc + x1, yc + y1, angle, r, g, b);
      mm_plot_cone(minimap, id, sat, x1, y1, xd + x1, yd + y1, angle, r, g, b);
      return;
    }
  csi = 1 / cs;
  xc = xc * csi + x1;
  xd = xd * csi + x1;
  yc = yc * csi + y1;
  yd = yd * csi + y1;
  mm_plot_triangle(minimap, id, sat, x1, y1, xc, yc, xd, yd, 
		   mm_distance(x1, y1, x2, y2), r, g, b);
}

static void mm_plot_r4d2(const map_t *map, obj_t *obj, guchar *minimap)
{  
  int	x = CONV_X(map, obj->posx);
  int	y = CONV_Y(map, obj->posy);
  guchar	*couleurs;

  if ((obj->team_id == 0) || (obj->team_id > map->nb_players))
    return;
  couleurs = map->players[obj->team_id]->colors;

  mm_plot_point(minimap, x, y, 
		couleurs[2], 
		couleurs[1],
		couleurs[0]);
  mm_plot_point(minimap, x+1, y, 
		couleurs[2], 
		couleurs[1],
		couleurs[0]);
  mm_plot_point(minimap, x, y+1, 
		couleurs[2], 
		couleurs[1],
		couleurs[0]);
  mm_plot_point(minimap, x-1, y, 
		couleurs[2], 
		couleurs[1],
		couleurs[0]);
  mm_plot_point(minimap, x, y-1, 
		couleurs[2], 
		couleurs[1],
		couleurs[0]);
}

static void mm_plot_akx(const map_t *map, obj_t *obj, guchar *minimap)
{
  int	id = obj->team_id;
  int	x, y, x2, y2;
  int	dx, dy;

  x = CONV_X(map, obj->posx);
  y = CONV_Y(map, obj->posy);
  x2 = CONV_X(map, obj->tx);
  y2 = CONV_Y(map, obj->ty);
  if (id > map->nb_players)
    id = 0;
  mm_plot_point(minimap, x, y, 0xFF, 0xFF, 0xFF);
  for (dx = -2; dx <= 2; dx++)
    for (dy = -2; dy <= 2; dy++)
      mm_plot_point(minimap, x + dx, y + dy,
		map->players[id]->colors[2], 
		map->players[id]->colors[1],
		map->players[id]->colors[0]);

  /* affichage pulse */
  if (obj->action == act_akx_pulse)
    mm_plot_cone(minimap, 1+(guchar)obj->id, PULSE_SAT, x, y, x2, y2, 
		 obj->angle,
		 map->players[id]->colors[2] / DIVID_PULSE_MINI, 
		 map->players[id]->colors[1] / DIVID_PULSE_MINI,
		 map->players[id]->colors[0] / DIVID_PULSE_MINI);
  
  /* affichage link */
  if (obj->action == act_akx_link)
    mm_plot_line(minimap, x, y, 
		 CONV_X(map, map->objects[obj->target]->posx),
		 CONV_Y(map, map->objects[obj->target]->posy),
		 map->players[id]->colors[2], 
		 map->players[id]->colors[1],
		 map->players[id]->colors[0]);
}

/* Zone de la vue principale */
static void mm_plot_rectangle(guchar *minimap, const conf_t *conf)
{
  int x1, y1, x2, y2;
  float ratiox, ratioy;
  int y, l, *buf;

  ratiox = MINI_MAP_SIZE / (conf->map->sizex * conf->big_zoom);
  ratioy = MINI_MAP_SIZE / (conf->map->sizey * conf->big_zoom);
  x1 = conf->big_ofs_x * ratiox;
  y1 = conf->big_ofs_y * ratioy;
  x2 = x1 + MAP_SIZE * ratiox;
  y2 = y1 + MAP_SIZE * ratioy;
  x1 = CLAMP(x1, 0, MINI_MAP_SIZE - 1);
  x2 = CLAMP(x2, 0, MINI_MAP_SIZE - 1);
  y1 = CLAMP(y1, 0, MINI_MAP_SIZE - 1);
  y2 = CLAMP(y2, 0, MINI_MAP_SIZE - 1);
  buf = (void *) minimap;
  memset(buf + x1 + y1 * MINI_MAP_SIZE, 255, (x2 - x1) * 4);
  memset(buf + x1 + y2 * MINI_MAP_SIZE, 255, (x2 - x1) * 4);
  memset(&l, 255, 4);
  for (y = y1 + 1; y < y2; y++)
    {
      buf[x1 + y * MINI_MAP_SIZE] = l;
      buf[x2 + y * MINI_MAP_SIZE] = l;
    }
}

guchar *gen_minimap(const conf_t *conf, bool force)
{
  static guchar		*minimap = NULL;
  int			cpt;
  obj_t			*obj;

  if (minimap == NULL)
    {
      minimap = g_malloc(4 * SQR(MINI_MAP_SIZE));      
      bzero(minimap, SQR(MINI_MAP_SIZE) * 4);
    }
  if (!force)
    return minimap;
  if (conf->map == NULL)
    return minimap;
  bzero(minimap, SQR(MINI_MAP_SIZE) * 4);  
  for (cpt = 0; cpt < conf->map->nb_objects; cpt++)
    {
      obj = conf->map->objects[cpt];
      switch (obj->type)
	{
	case obj_r4d2 :
	  mm_plot_r4d2(conf->map, obj, minimap);
	  break;
	case obj_akx :
	  mm_plot_akx(conf->map, obj, minimap);
	  break;
	}
    }
  mm_plot_rectangle(minimap, conf);
  return minimap;
}

static guchar my_creneau(int val)
{
  if (val >= 0)
      return CLAMP((120 - val) * 0xFF / 120, 0, 0xFF);
  else
      return CLAMP((120 + val) * 0xFF / 120, 0, 0xFF);
}

void	gen_colors(int id, int nbr, player_t *play)
{
  int	angle;

  angle = (360 * id) / nbr; /* position => angle dans spectre */
  if (angle > 180)
    angle = angle - 360;
  play->colors[2] = my_creneau(angle);
  play->colors[1] = my_creneau(angle - 120);
  play->colors[0] = my_creneau(angle + 120);
  play->gdkcol = g_malloc(sizeof(*(play->gdkcol)));
  pl_make_color(play->gdkcol, 
		play->colors[2], play->colors[1], play->colors[0]);
}
