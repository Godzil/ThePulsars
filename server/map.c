/* $Id: map.c,v 1.6 2001/05/06 01:23:05 kilobug Exp $ */

#include <math.h>

/*
** General map functions 
*/

float map_get_dist(float x1, float y1, float x2, float y2)
{
   return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

float map_get_angle(float x, float y, float x1, float y1, float x2, float y2)
{                  // a              // b                // c
   float f, d1, d2;

   d1 = sqrtf(map_get_dist(x1, y1, x, y));
   d2 = sqrtf(map_get_dist(x1, y1, x2, y2));

   if ((d1 == 0) || (d2 == 0))
   {
      return 0;
   }

   f = ((x1 - x) * (x1 - x2) + (y1 - y) * (y1 - y2)) / (d1 * d2);

   return acosf(f);
}
