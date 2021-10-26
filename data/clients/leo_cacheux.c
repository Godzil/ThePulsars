#include <math.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include "prolo.h"

/* Structure pour stocker des coordonnees */
typedef struct {
    float x;
    float y;
} coord;


/* Variables globales */
coord map_size;
int nb_akx, nb_r4d2;
int my_akx, my_r4d2;
int my_team;
int change;
int *dest;
coord *dest_coord;
int *units;


/* Obtenir les coordonnees d'un r4d2 */
coord get_r4d2_coord (int id)
{
    coord foo;
    foo.x = r4d2_get_pos_x(id);
    foo.y = r4d2_get_pos_y(id);
    return foo;
}


/* Obtenir les coordonnees d'un Anakronox */
coord get_akx_coord (int id)
{
    coord foo;
    foo.x = akx_get_pos_x(id);
    foo.y = akx_get_pos_y(id);
    return foo;
}


/* Calcul de la distance entre deux points */
float distance (coord a, coord b)
{
    return ( sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) ) );
}


void player_init (int team_id)
{
    int i;

    map_size.x = map_get_size_x();
    map_size.y = map_get_size_y();

    nb_akx = map_count_akx();
    nb_r4d2 = map_count_r4d2();

    my_akx = map_count_my_akx();
    my_r4d2 = map_count_my_r4d2();

    change=1;

    dest = (int*)malloc(sizeof(int)*nb_r4d2);
    dest_coord = (coord*)malloc(sizeof(coord)*nb_r4d2);
    units = (int*)malloc(sizeof(int)*(nb_akx+nb_r4d2));

    for (i=0; i<nb_r4d2; i++)
    {
        dest[i] = -1;
    }

    for (i=0; i<nb_akx+nb_r4d2; i++)
    {
        units[i] = 0;
    }
}


void player_new_turn (int turn_number)
{

}


void player_akx_turn (int akx_id)
{
    int r4d2, akx;
    coord r4d2_coord, akx_coord, my_coord;

    my_team = akx_get_team(akx_id);
    my_coord = get_akx_coord (akx_id);

    printf ("%i\n",map_get_nearest_r4d2 (akx_id, -my_team));
    printf ("%i\n",map_get_nearest_akx (akx_id, -my_team));

    if ((r4d2 = map_get_nearest_r4d2 (akx_id, -my_team)) != -1)
    {
        change=0;
        r4d2_coord = get_r4d2_coord (r4d2);
        akx_pulse (akx_id, r4d2_coord.x, r4d2_coord.y, 0.2);
        return;
    }

    if ((akx = map_get_nearest_akx (akx_id, -my_team)) != -1)
    {
        change=0;
        akx_coord = get_akx_coord (akx);
        akx_pulse (akx_id, akx_coord.x, akx_coord.y, 0.5);
        return;
    }

    akx_pulse (akx_id, rand()%(int)map_size.x, rand()%(int)map_size.y, 0.2);
    return;

}


void player_r4d2_turn (int r4d2_id)
{
    int savage_r4d2, savage_akx;
    coord savage_r4d2_coord, savage_akx_coord;

    /*
    for (i=0; i<nb_r4d2; i++)
      {
        if (r4d2_get_team(i)==my_team && dest_coord[i].x==dest_coord[r4d2_id].x &&
   dest_coord[i].y==dest_coord[r4d2_id].y && dest[i]!=-1 && i!=r4d2_id)
          {
            r4d2_move (r4d2_id, rand()%(int)map_size.x, rand()%(int)map_size.y);
            return;
          }
      }
    */

    if ( (savage_r4d2 = map_get_nearest_r4d2 (r4d2_id, -my_team)) != -1)
    {
        units[savage_r4d2] = 1;
        dest[r4d2_id] = savage_r4d2;
        savage_r4d2_coord = get_r4d2_coord (savage_r4d2);
        dest_coord[r4d2_id] = savage_r4d2_coord;
        if (distance(get_r4d2_coord (r4d2_id), get_r4d2_coord (savage_r4d2)) <= 1)
        {
            dest[r4d2_id]=-1;
            units[savage_r4d2] = 0;
            change=1;
            r4d2_take_r4d2 (r4d2_id, savage_r4d2);
            return;
        }
    }

    if ( (savage_akx = map_get_nearest_akx (r4d2_id, 0)) != -1)
    {
        units[savage_akx] = 1;
        dest[r4d2_id] = savage_akx;
        savage_akx_coord = get_akx_coord (savage_akx);
        dest_coord[r4d2_id] = savage_akx_coord;
        if (distance(savage_akx_coord, get_r4d2_coord (r4d2_id) ) <= 1)
        {
            units[savage_akx]=0;
            dest[r4d2_id]=-1;
            change=1;
            r4d2_take_akx (r4d2_id, savage_akx);
            return;
        }
    }

    if (r4d2_get_status(r4d2_id) == 0 && dest[r4d2_id]!=-1)
    {
        r4d2_move (r4d2_id, dest_coord[r4d2_id].x, dest_coord[r4d2_id].y);
        return;
    }

    r4d2_move (r4d2_id, rand()%(int)map_size.x, rand()%(int)map_size.y);
}
