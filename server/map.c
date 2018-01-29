/* $Id: map.c,v 1.6 2001/05/06 01:23:05 kilobug Exp $ */

#include <math.h>

/*
** General map functions 
*/

float dist(float x1, float y1, float x2, float y2)
{
   return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

/*  float a, b, c, res;

 a = sqrt(dist(x, y, x1, y1));
 b = sqrt(dist(x, y, x2, y2));
 c = fabs((x1 - x) * (x2 - x) + (y1 - y) * (y2 - y)) * b;
 if (a)
   res = acos(CLAMP(c / a, -1, 1));
 else
   res = M_PI;
   return res;
float ac2, ab2, bc2, ac, ab, cosin;


bc2 = SQR(x1 - x2) + SQR(y1 - y2);
ab2 = SQR(x - x1) + SQR(y - y1);
ac2 = SQR(x - x2) + SQR(y - y2);

ac = (float)sqrt(ac2);
ab = (float)sqrt(ab2);

printf("BC2:%f AB2:%f AC2:%f AC:%f AB:%f\n", bc2, ab2, ac2, ac, ab);

if (!ac | !ab)
  return 0;
cosin = (bc2 - ab2 - ac2) / (2 * ac * ab);
printf("NUM: %f DEN: %f\n", bc2 - ab2 - ac2, 2 * ac * ab);
printf("COS:%f\n", cosin);
if (ABS(cosin) <= 1)
  return (float)acos(cosin);
return 0;
*/


float angle(float x, float y, float x1, float y1, float x2, float y2)
{                  // a              // b                // c
   float f, d1, d2;

   d1 = sqrtf(dist(x1, y1, x, y));
   d2 = sqrtf(dist(x1, y1, x2, y2));

   if ((d1 == 0) || (d2 == 0))
   {
      return 0;
   }

   f = ((x1 - x) * (x1 - x2) + (y1 - y) * (y1 - y2)) / (d1 * d2);

   return acosf(f);
}
