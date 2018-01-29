/* $Id: private.h,v 1.17 2001/05/07 02:41:36 kilobug Exp $ */

#ifndef __PROLO_PRIVATE_H__
#define __PROLO_PRIVATE_H__

#include "action.h"

#define MIN_ANGLE    0.01F
#define MAX_ANGLE    ((float)(M_PI * 2.0F))

#define DIST_MIN_TAKE    1.0    /* distance minimum pour prendre un r4d2 ou un anakronox (au carre) */

typedef enum _obj_type_t
{
    obj_r4d2 = 0, obj_akx = 1
} obj_type_t;

/* gestion des objets dans la carte */

typedef struct _obj_t
{
    boolean_t change;
    obj_type_t type;
    int id;
    float x, y;
    int team_id;
    int error;
} obj_t;

typedef struct _akx_t
{
    boolean_t change;
    obj_type_t type;
    int id;
    float x, y;
    int team_id;
    int error;

    akx_act_t action;
    float energy;
} akx_t;

typedef struct _r4d2_t
{
    boolean_t change;
    obj_type_t type;
    int id;
    float x, y;
    int team_id;
    int error;

    r4d2_act_t action;
} r4d2_t;

typedef union _item_t
{
    obj_t obj;
    akx_t akx;
    r4d2_t r4d2;
} item_t;

extern item_t *glbObjects;

void new_akx(float x, float y, int id);
void new_r4d2(float x, float y, int id);

#endif
