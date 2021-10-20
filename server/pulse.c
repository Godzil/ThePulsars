#include <objects.h>
#include <server.h>
#include <map.h>

float get_pulse_team(float x, float y, int team_id)
{
    float result = 0, r, agl;
    akx_t *akx;
    akx_pulse_t *pulse;
    int i;

    for ( i = 0 ; i < gl_config->nb_objects ; i++ )
    {
        akx = &glbObjects[i].akx;
        if ((( akx->team_id == team_id ) ||
             (( akx->team_id != -team_id ) && ( team_id < 0 ) && ( akx->team_id ))) &&
            ( akx->type == obj_akx ) &&
            ( akx->action.type == act_akx_pulse ))
        {
            pulse = &akx->action.act.pulse;
            if (( x != akx->x ) || ( y != akx->y ))
            {
                r = map_get_dist(pulse->x, pulse->y, akx->x, akx->y);
                if ( r >= map_get_dist(x, y, akx->x, akx->y))
                {
                    agl = map_get_angle(x, y, akx->x, akx->y, pulse->x, pulse->y);
                    if ( agl <= ( pulse->angle / 2 ))
                    {
                        result += akx->energy / ( 1 + r * pulse->angle / 2 );
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

    akx = &glbObjects[akx_id].akx;
    if ( akx->action.type == act_akx_pulse )
    {
        pulse = &akx->action.act.pulse;
        if (( x != akx->x ) || ( y != akx->y ))
        {
            r = map_get_dist(pulse->x, pulse->y, akx->x, akx->y);
            if ( r >= map_get_dist(x, y, akx->x, akx->y))
            {
                agl = map_get_angle(x, y, akx->x, akx->y, pulse->x, pulse->y);
                if ( agl <= ( pulse->angle / 2 ))
                {
                    result += akx->energy / ( 1 + r * pulse->angle / 2 );
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
