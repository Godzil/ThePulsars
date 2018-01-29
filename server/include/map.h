//
// Created by mlt on 29/01/18.
//

#ifndef THEPULSARS_MAP_H
#define THEPULSARS_MAP_H

#include "conf.h"

/* Map parser functions */
void load_map(conf_t *conf);

float map_get_dist(float x1, float y1, float x2, float y2);
float map_get_angle(float x, float y, float x1, float y1, float x2, float y2);

#endif /* THEPULSARS_MAP_H */
