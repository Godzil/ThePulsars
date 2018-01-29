/* $Id: gen_bigmap2.c,v 1.3 2001/04/24 08:31:29 glop Exp $ */

#include "client.h"

#define OFFSET2(SX, X, Y)	(((X) + (Y) * (SX)) * 4)
#define	IRD(SX, X,Y)		(OFFSET2(SX, X, Y) + 0)
#define	IGR(SX, X,Y)		(OFFSET2(SX, X, Y) + 1)
#define	IBL(SX, X,Y)		(OFFSET2(SX, X, Y) + 2)
#define	MASK(SX, X,Y)		(OFFSET2(SX, X, Y) + 3)

#define OFFSET(X, Y)    (OFFSET2(MAP_SIZE, X, Y))
#define	RED(X,Y)	(OFFSET(X, Y) + 0)
#define	GREEN(X,Y)	(OFFSET(X, Y) + 1)
#define	BLUE(X,Y)	(OFFSET(X, Y) + 2)
#define	ID(X,Y)		(OFFSET(X, Y) + 3)

#define	RED_COLOR(map, id)	((map)->players[(id)]->colors[2])
#define	GREEN_COLOR(map, id)	((map)->players[(id)]->colors[1])
#define	BLUE_COLOR(map, id)	((map)->players[(id)]->colors[0])

#define	CONV_X(map, X)	(((X) * MAP_SIZE) / ((map)->sizex + 0.1))
#define	CONV_Y(map, Y)	(((Y) * MAP_SIZE) / ((map)->sizey + 0.1))

static void bm2_plot_point_ex(guchar *bigmap, guchar id, int x, 
			     int y, guchar r, guchar g, guchar b)
{
  int	temp;  
  
  if (bigmap[ID(x, y)] == id)
    return;

  temp = bigmap[RED(x,y)] + r;
  if (temp > PULSE_SAT)
    temp = PULSE_SAT;
  bigmap[RED(x,y)] = temp;
  
  temp = bigmap[GREEN(x,y)] + g;
  if (temp > PULSE_SAT)
    temp = PULSE_SAT;
  bigmap[GREEN(x,y)] = temp;
  
  temp = bigmap[BLUE(x,y)] + b;
  if (temp > PULSE_SAT)
    temp = PULSE_SAT;
  bigmap[BLUE(x,y)] = temp;
  bigmap[ID(x, y)] = id;
}

static void bm2_plot_point(guchar *bigmap, int x, int y, 
			  guchar r, guchar g, guchar b)
{
  int	temp;  

  if ((x < 0) || (x >= MAP_SIZE))
    return;
  if ((y < 0) || (y >= MAP_SIZE))
    return;
  temp = bigmap[RED(x,y)] + r;
  if (temp > 0xFF)
    temp = 0xFF;
  bigmap[RED(x,y)] = temp;

  temp = bigmap[GREEN(x,y)] + g;
  if (temp > 0xFF)
    temp = 0xFF;
  bigmap[GREEN(x,y)] = temp;

  temp = bigmap[BLUE(x,y)] + b;
  if (temp > 0xFF)
    temp = 0xFF;
  bigmap[BLUE(x,y)] = temp;
}

static void bm2_plot_line(guchar *bigmap, int x1, int y1, int x2, int y2, 
		      guchar r, guchar g, guchar b)
{
  float	a;
  int	diffx, diffy;
  int	x, y;

  diffx = x1 - x2;
  diffy = y1 - y2;
  if (!diffx && !diffy)
    bm2_plot_point(bigmap, x1, y2, r, g, b);
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
	      bm2_plot_point(bigmap, x, y1 + (x - x1) * a, r, g, b);
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
	      bm2_plot_point(bigmap, x1 + (y - y1) * a, y, r, g, b);
	      y++;
	    }
	}
    }
}

static float bm2_rotate_x(float x, float y, float cs, float sn)
{
  return cs * x + sn * y;
}

static float bm2_rotate_y(float x, float y, float cs, float sn)
{
  return cs * y - sn * x;
}

static int bm2_distance(int x1, int y1, int x2, int y2)
{ 
  return (SQR(x2 - x1) + SQR(y2 - y1));
}

static void bm2_plot_triangle(guchar *bigmap, guchar id,
			     int x1, int y1, int x2, int y2, int x3, int y3,
			     int dist, guchar cr, guchar cg, guchar cb)
{
  int	swap;
  float	delta1, delta2, delta3;
  float	xa, xb, txa, txb;
  int	x, y, begy, endy;
  int	memx = x1, memy = y1;
  int	rac;

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
  
  /* Premiäre partie du triangle ( tracage des section horizontale) */

  xa = x1;
  xb = x1;

  if (y1 != y2)
    {
      if (y1 < 0)
	{
	  xa -= delta1 * y1;
	  xb -= delta2 * y1;
	  begy = 0;
	}
      else
	begy = y1;
      if (y2 > (MAP_SIZE-1))
	endy = MAP_SIZE-1;
      else
	endy = y2;
      if (delta1 < delta2)
	for (y = begy; y < endy; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta2;
	    txa = MAX(0, xa);
	    txb = MIN(MAP_SIZE-1, xb);
	    
	    rac = sqrt(dist - SQR(y - memy));
	    
	    txa = MAX(txa, memx - rac);
	    txb = MIN(txb, memx + rac);

	    for (x = txa; x < txb; x++)
	      bm2_plot_point_ex(bigmap, id, x, y, cr, cg, cb);
	  }
      else
	for (y = begy; y < endy; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta2;
	    txb = MAX(0, xb);
	    txa = MIN(MAP_SIZE-1, xa);

	    rac = sqrt(dist - SQR(y - memy));
	    
	    txb = MAX(txb, memx - rac);
	    txa = MIN(txa, memx + rac);

	    for (x = txb; x < txa; x++)
	      bm2_plot_point_ex(bigmap, id, x, y, cr, cg, cb);
	  }
      xa = x1 + delta1 * (y2 - y1);
      xb = x1 + delta2 * (y2 - y1);
    }
  else 
    xb = x2;

  /* Deuxieme partie du triangle ( tracage des section horizontale) */

  if (y2 != y3) 
    {
      if (y2 < 0)
	{
	  xa -= delta1 * y2;
	  xb -= delta3 * y2;
	  begy = 0;
	}
      else
	begy = y2;
      if (y3 > (MAP_SIZE-1))
	endy = MAP_SIZE-1;
      else
	endy = y3;
      if (delta3 < delta1) 
	for (y = begy; y < endy; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta3;
	    txa = MAX(0, xa);
	    txb = MIN(MAP_SIZE-1, xb);
	    
	    rac = sqrt(dist - SQR(y - memy));
	    
	    txa = MAX(txa, memx - rac);
	    txb = MIN(txb, memx + rac);

	    for (x = txa; x < txb; x++)
	      bm2_plot_point_ex(bigmap, id, x, y, cr, cg, cb);
	  }
      else
 	for (y = begy; y < endy; y++)
	  {
	    xa = xa + delta1;
	    xb = xb + delta3;
	    txb = MAX(0, xb);
	    txa = MIN(MAP_SIZE-1, xa);
	    
	    rac = sqrt(dist - SQR(y - memy));
	    
	    txb = MAX(txb, memx - rac);
	    txa = MIN(txa, memx + rac);

	    for (x = txb; x < txa; x++)
	      bm2_plot_point_ex(bigmap, id, x, y, cr, cg, cb);
	  }
    }
}


static void bm2_plot_cone(guchar *bigmap, guchar id,
			 float x1, float y1, float x2, float y2, 
			 float angle, guchar r, guchar g, guchar b)
{
  float cs, sn, csi;
  float x, y, xc, xd, yc, yd;
  gboolean call_sub = FALSE;

  x = x2 - x1;
  y = y2 - y1;
  if (angle > M_PI_2)
    {
      call_sub = TRUE;
      angle /= 2;
    }
  cs = cos(angle / 2);
  sn = sin(angle / 2);
  xc = bm2_rotate_x(x, y, cs, sn);
  xd = bm2_rotate_x(x, y, cs, -sn);
  yc = bm2_rotate_y(x, y, cs, sn);
  yd = bm2_rotate_y(x, y, cs, -sn);
  if (call_sub)
    {
      bm2_plot_cone(bigmap, id, x1, y1, xc + x1, yc + y1, angle, r, g, b);
      bm2_plot_cone(bigmap, id, x1, y1, xd + x1, yd + y1, angle, r, g, b);
      return;
    }
  csi = 1 / cs;
  xc = xc * csi + x1;
  xd = xd * csi + x1;
  yc = yc * csi + y1;
  yd = yd * csi + y1;
  bm2_plot_triangle(bigmap, id, x1, y1, xc, yc, xd, yd, 
		   bm2_distance(x1, y1, x2, y2), r, g, b);
}

static void bm2_plot_object(const conf_t *conf, const obj_t *obj, 
			   guchar *bigmap, const guchar *colors)
{
  int x, y;

  x = obj->posx * conf->big_zoom - conf->big_ofs_x;
  y = obj->posy * conf->big_zoom - conf->big_ofs_y;
  switch (obj->type)
    {
    case obj_akx :
      bm2_plot_point(bigmap, x+2, y, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x-2, y, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x, y+2, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x, y-2, colors[2], colors[1], 
		     colors[0]);

      bm2_plot_point(bigmap, x+1, y+1, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x-1, y+1, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x+1, y-1, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x-1, y-1, colors[2], colors[1], 
		     colors[0]);
    case obj_r4d2 :
      bm2_plot_point(bigmap, x+1, y, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x, y+1, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x, y, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x-1, y, colors[2], colors[1], 
		     colors[0]);
      bm2_plot_point(bigmap, x, y-1, colors[2], colors[1], 
		     colors[0]);
      break;
    };
}

static void bm2_plot_akx_r4d2(const conf_t *conf, const obj_t *obj, 
			      guchar *bigmap)
{
  int tid;

  tid = obj->team_id;
  if (tid > conf->map->nb_players)
    tid = 0;
  if ((tid == 0) && obj->type == obj_r4d2)
    return;
  bm2_plot_object(conf, obj, bigmap, conf->map->players[tid]->colors);
}

static void bm2_plot_pulse(guchar *bigmap, const conf_t *conf,
			  const obj_t *obj, guchar *colors)
{
  int	x, y, x2, y2, id;

  id = obj->team_id;
  x = obj->posx * conf->big_zoom - conf->big_ofs_x;
  y = obj->posy * conf->big_zoom - conf->big_ofs_y;
  x2 = obj->tx * conf->big_zoom - conf->big_ofs_x;
  y2 = obj->ty * conf->big_zoom - conf->big_ofs_y;
  
  bm2_plot_cone(bigmap, (guchar)obj->id+1, x, y, x2, y2, 
	       obj->angle,
	       conf->map->players[id]->colors[2] / DIVID_PULSE_MINI, 
	       conf->map->players[id]->colors[1] / DIVID_PULSE_MINI,
	       conf->map->players[id]->colors[0] / DIVID_PULSE_MINI);
}

static void bm2_plot_link(guchar *bigmap, const conf_t *conf, 
			 const obj_t *obj, guchar *colors)
{
  int x, y, x2, y2;

  x = obj->posx * conf->big_zoom - conf->big_ofs_x;
  y = obj->posy * conf->big_zoom - conf->big_ofs_y;
  x2 = conf->map->objects[obj->target]->posx * 
    conf->big_zoom - conf->big_ofs_x;
  y2 = conf->map->objects[obj->target]->posy * 
    conf->big_zoom - conf->big_ofs_y;

  bm2_plot_line(bigmap, x, y, x2, y2, colors[2], colors[1], colors[0]);
}

static void bm2_plot_pulse_link(const conf_t *conf, const obj_t *obj, 
			       guchar *bigmap)
{
  int tid;

  tid = obj->team_id;
  if (tid > conf->map->nb_players)
    tid = 0;
  /* affichage pulse */
  if (obj->action == act_akx_pulse)
    bm2_plot_pulse(bigmap, conf, obj, conf->map->players[tid]->colors);
  
  /* affichage link */
  if (obj->action == act_akx_link)
    bm2_plot_link(bigmap, conf, obj, conf->map->players[tid]->colors);
}

guchar *gen_bigmap2(conf_t *conf, gboolean force)
{
  int			cpt;
  obj_t			*obj;

  if (conf->screen == NULL)
    {
      conf->screen = g_malloc(4 * SQR(MAP_SIZE));
      bzero(conf->screen, 4 * SQR(MAP_SIZE));
    }
  if (!force)
    return conf->screen;
  if (conf->map == NULL)
    return conf->screen;
  bzero(conf->screen, 4 * SQR(MAP_SIZE));

  if (conf->big_pulse)
    for (cpt = 0; cpt < conf->map->nb_objects; cpt++)
      {
	obj = conf->map->objects[cpt];
	if (obj->type == obj_akx)
	  bm2_plot_pulse_link(conf, obj, conf->screen); 
      }

  for (cpt = 0; cpt < conf->map->nb_objects; cpt++)
    {
      obj = conf->map->objects[cpt];
      bm2_plot_akx_r4d2(conf, obj, conf->screen);
    }
  return conf->screen;
}
