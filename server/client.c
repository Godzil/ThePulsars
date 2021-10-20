#include <math.h>

#include <general.h>

#include <map.h>
#include <pulse.h>
#include <server.h>
#include <conf.h>
#include <objects.h>
#include <action.h>
#include <erreur.h>
#include <bits/mathcalls.h>
#include <exec.h>

int turn_number(void)
{
    return gl_config->nb_turns;
}

int turn_counter(void)
{
    return gl_config->turn_num;
}

static boolean_t is_visible(int team_id, float x, float y)
{
    return ( get_pulse_total(x, y, team_id) >= gl_config->see_power );
    /* AVANT MODIF MATHIAS  return (get_pulse_team(x, y, team_id) >= gl_config->see_power); */
}

static boolean_t is_obj_visible(int team_id, int id)
{
    if ( glbObjects[id].obj.team_id == team_id )
    {
        return True;
    }
    return is_visible(team_id, glbObjects[id].obj.x, glbObjects[id].obj.y);
}

void set_default_action_r4d2(int r4d2_id)
{
    glbObjects[r4d2_id].r4d2.action.type = act_r4d2_move;
    glbObjects[r4d2_id].r4d2.action.act.move.x = glbObjects[r4d2_id].r4d2.x;
    glbObjects[r4d2_id].r4d2.action.act.move.y = glbObjects[r4d2_id].r4d2.y;
}

void set_default_action_akx(int akx_id)
{
    glbObjects[akx_id].akx.action.type = act_akx_move;
    glbObjects[akx_id].akx.action.act.move.x = glbObjects[akx_id].akx.x;
    glbObjects[akx_id].akx.action.act.move.y = glbObjects[akx_id].akx.y;
}

int time_get_left()
{
    return time_left();
}

int error_get()
{
    int foo;

    foo = glbPlayer->error;
    glbPlayer->error = NONE;
    return foo;
}

int score_get()
{
    return (int) glbPlayer->score;
}

static boolean_t validate_object(int obj_id, obj_type_t type)
{
    if (( obj_id >= gl_config->nb_objects ) || ( obj_id < 0 ))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return False;
    }
    if ( !is_obj_visible(glbPlayer->team_id, obj_id))
    {
        glbPlayer->error = NOT_VISIBLE;
        return False;
    }
    if ( glbObjects[obj_id].obj.type != type )
    {
        glbPlayer->error = TYPE_ERROR;
        return False;
    }
    return True;
}

static boolean_t validate_object_nosee(int obj_id, obj_type_t type)
{
    if (( obj_id >= gl_config->nb_objects ) || ( obj_id < 0 ))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return False;
    }
    if ( glbObjects[obj_id].obj.type != type )
    {
        glbPlayer->error = TYPE_ERROR;
        return False;
    }
    return True;
}

static boolean_t validate_object_owner(int obj_id, obj_type_t type)
{
    if (( obj_id >= gl_config->nb_objects ) || ( obj_id < 0 ))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return False;
    }
    if ( glbObjects[obj_id].obj.team_id != glbPlayer->team_id )
    {
        if (( glbObjects[obj_id].obj.team_id == 0 ) && ( type == obj_r4d2 ))
        {
            glbPlayer->error = DESTROYED;
        }
        else
        {
            glbPlayer->error = NOT_YOURS;
        }
        return False;
    }
    if ( glbObjects[obj_id].obj.type != type )
    {
        glbPlayer->error = TYPE_ERROR;
        return False;
    }
    return True;
}

int r4d2_get_team(int r4d2_id)
{
    if ( !validate_object(r4d2_id, obj_r4d2))
    {
        return -1;
    }
    if ( glbObjects[r4d2_id].obj.team_id == 0 )
    {
        glbPlayer->error = DESTROYED;
        return -1;
    }
    return glbObjects[r4d2_id].obj.team_id;
}

float r4d2_get_pos_x(int r4d2_id)
{
    if ( !validate_object(r4d2_id, obj_r4d2))
    {
        return -1;
    }
    return glbObjects[r4d2_id].obj.x;
}

float r4d2_get_pos_y(int r4d2_id)
{
    if ( !validate_object(r4d2_id, obj_r4d2))
    {
        return -1;
    }
    return glbObjects[r4d2_id].obj.y;
}

int r4d2_get_status(int r4d2_id)
{
    if ( !validate_object(r4d2_id, obj_r4d2))
    {
        return -1;
    }
    return glbObjects[r4d2_id].r4d2.action.type;
}

float r4d2_get_speed()
{
    return gl_config->r4d2_speed;
}

float r4d2_get_destroy_speed()
{
    return gl_config->destroy_speed;
}

int r4d2_turn_take_r4d2()
{
    return gl_config->turn_take_r4d2;
}

int r4d2_turn_untake_r4d2()
{
    return gl_config->turn_untake_r4d2;
}

int r4d2_turn_take_akx()
{
    return gl_config->turn_take_akx;
}

int r4d2_turn_untake_akx()
{
    return gl_config->turn_untake_akx;
}

int r4d2_move(int r4d2_id, float x, float y)
{
    if ( !finitef(x) || !finitef(y))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    if ( !validate_object_owner(r4d2_id, obj_r4d2))
    {
        return -1;
    }
    glbObjects[r4d2_id].r4d2.action.type = act_r4d2_move;
    glbObjects[r4d2_id].r4d2.action.act.move.x = x;
    glbObjects[r4d2_id].r4d2.action.act.move.y = y;

    return 0;
}

int r4d2_take_akx(int r4d2_id, int target_id)
{
    if ( !validate_object_owner(r4d2_id, obj_r4d2))
    {
        return -1;
    }
    if ( !validate_object_nosee(target_id, obj_akx))
    {
        return -1;
    }
    if ( map_get_dist(glbObjects[r4d2_id].r4d2.x,
                      glbObjects[r4d2_id].r4d2.y,
                      glbObjects[target_id].akx.x,
                      glbObjects[target_id].akx.y) > DIST_MIN_TAKE )
    {
        set_default_action_r4d2(r4d2_id);
        glbPlayer->error = POS_OUT_OF_RANGE;
        return -1;
    }
    if ( glbObjects[r4d2_id].r4d2.action.type == act_r4d2_take_akx )
    {
        return 0;
    }
    glbObjects[r4d2_id].r4d2.action.type = act_r4d2_take_akx;
    glbObjects[r4d2_id].r4d2.action.act.take_akx.target_id = target_id;
    glbObjects[r4d2_id].r4d2.action.act.take_akx.turn_need =
            gl_config->turn_take_akx;
    if ( glbObjects[target_id].akx.team_id != 0 )
    {
        glbObjects[r4d2_id].r4d2.action.act.take_akx.turn_need +=
                gl_config->turn_untake_akx;
    }
    return 0;
}

int r4d2_take_r4d2(int r4d2_id, int target_id)
{
    if ( !validate_object_owner(r4d2_id, obj_r4d2))
    {
        return -1;
    }
    if ( !validate_object_nosee(target_id, obj_akx))
    {
        return -1;
    }
    if ( target_id == r4d2_id )
    {
        glbPlayer->error = INVALID_TARGET;
        return -1;
    }
    if (( glbObjects[target_id].obj.team_id == 0 ) ||
        ( glbObjects[target_id].obj.team_id > gl_config->nb_players ))
    {
        glbPlayer->error = DESTROYED;
        return -1;
    }
    if ( map_get_dist(glbObjects[r4d2_id].r4d2.x,
                      glbObjects[r4d2_id].r4d2.y,
                      glbObjects[target_id].r4d2.x,
                      glbObjects[target_id].r4d2.y) > DIST_MIN_TAKE )
    {
        set_default_action_r4d2(r4d2_id);
        glbPlayer->error = POS_OUT_OF_RANGE;
        return -1;
    }
    if ( glbObjects[r4d2_id].r4d2.action.type == act_r4d2_take_r4d2 )
    {
        return 0;
    }
    glbObjects[r4d2_id].r4d2.action.type = act_r4d2_take_r4d2;
    glbObjects[r4d2_id].r4d2.action.act.take_r4d2.target_id = target_id;
    glbObjects[r4d2_id].r4d2.action.act.take_r4d2.turn_need =
            gl_config->turn_take_r4d2;
    if ( glbObjects[target_id].r4d2.team_id != 0 )
    {
        glbObjects[r4d2_id].r4d2.action.act.take_r4d2.turn_need +=
                gl_config->turn_untake_r4d2;
    }
    return 0;
}

static int get_nearest_not(float x, float y, int id, int team_id,
                           obj_type_t target_type)
{
    int nearest = id;
    float dist_nearest = MAXFLOAT;
    int cpt;
    float dist_temp;

    for ( cpt = 0 ; cpt < gl_config->nb_objects ; cpt++ )
    {
        if (( cpt != id ) &&
            ( glbObjects[cpt].obj.team_id != team_id ) &&
            ( glbObjects[cpt].obj.type == target_type ))
        {
            if (( target_type != obj_r4d2 ) || ( glbObjects[cpt].obj.team_id != 0 ))
            {
                if ( is_obj_visible(glbPlayer->team_id, cpt))
                {
                    dist_temp = map_get_dist(x, y,
                                             glbObjects[cpt].obj.x,
                                             glbObjects[cpt].obj.y);
                    if ( dist_temp < dist_nearest )
                    {
                        nearest = cpt;
                        dist_nearest = dist_temp;
                    }
                }
            }
        }
    }
    return nearest;
}

static int get_nearest_plot(float x, float y, int id, int team_id,
                            obj_type_t target_type)
{
    int nearest = id;
    float dist_nearest = MAXFLOAT;
    int cpt;
    float dist_temp;

    if ( team_id < 0 )
    {
        nearest = get_nearest_not(x, y, id, -team_id, target_type);
    }
    else
    {
        for ( cpt = 0 ; cpt < gl_config->nb_objects ; cpt++ )
        {
            if (( cpt != id ) &&
                ( glbObjects[cpt].obj.team_id == team_id ) &&
                ( glbObjects[cpt].obj.type == target_type ))
            {
                if (( target_type != obj_r4d2 ) || ( glbObjects[cpt].obj.team_id > 0 ))
                {
                    if ( is_obj_visible(glbPlayer->team_id, cpt))
                    {
                        dist_temp = map_get_dist(x, y,
                                                 glbObjects[cpt].obj.x,
                                                 glbObjects[cpt].obj.y);
                        if ( dist_temp < dist_nearest )
                        {
                            nearest = cpt;
                            dist_nearest = dist_temp;
                        }
                    }
                }
            }
        }
    }
    return nearest;
}

int get_nearest(int id, int team_id, obj_type_t target_type)
{
    return get_nearest_plot(glbObjects[id].obj.x,
                            glbObjects[id].obj.y,
                            id, team_id, target_type);
}

int akx_get_team(int akx_id)
{
    if ( !validate_object(akx_id, obj_akx))
    {
        return -1;
    }
    return glbObjects[akx_id].obj.team_id;
}

float akx_get_pos_x(int akx_id)
{
    if ( !validate_object(akx_id, obj_akx))
    {
        return -1;
    }
    return glbObjects[akx_id].obj.x;
}

float akx_get_pos_y(int akx_id)
{
    if ( !validate_object(akx_id, obj_akx))
    {
        return -1;
    }
    return glbObjects[akx_id].obj.y;
}

int akx_get_status(int akx_id)
{
    if ( !validate_object(akx_id, obj_akx))
    {
        return -1;
    }
    return glbObjects[akx_id].akx.action.type;
}


float akx_get_speed()
{
    return gl_config->akx_speed;
}

float akx_get_power()
{
    return gl_config->pulse_power;
}

float akx_get_see_power()
{
    return gl_config->see_power;
}

float akx_see_power()
{
    return gl_config->see_power;
}

float akx_get_pulse_coef()
{
    return gl_config->pulse_coef;
}

int akx_move(int akx_id, float x, float y)
{
    if ( !finitef(x) || !finitef(y))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    if ( !validate_object_owner(akx_id, obj_akx))
    {
        return -1;
    }
    glbObjects[akx_id].akx.action.type = act_akx_move;
    glbObjects[akx_id].akx.action.act.move.x = x;
    glbObjects[akx_id].akx.action.act.move.y = y;
    glbObjects[akx_id].akx.change = True;
    return 0;
}

int akx_pulse(int akx_id, float x, float y, float angle)
{
    if ( !finitef(x) || !finitef(y) || !finitef(angle))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    if ( !validate_object_owner(akx_id, obj_akx))
    {
        return -1;
    }
    if ( glbObjects[akx_id].akx.action.type != act_akx_pulse )
    {
        glbObjects[akx_id].akx.action.act.pulse.x = glbObjects[akx_id].obj.x;
        glbObjects[akx_id].akx.action.act.pulse.y = glbObjects[akx_id].obj.y;
        glbObjects[akx_id].akx.energy = 0;
    }
    glbObjects[akx_id].akx.action.type = act_akx_pulse;
    glbObjects[akx_id].akx.action.act.pulse.new_x = x;
    glbObjects[akx_id].akx.action.act.pulse.new_y = y;
    glbObjects[akx_id].akx.action.act.pulse.new_angle = CLAMP(angle, MIN_ANGLE, MAX_ANGLE);
    glbObjects[akx_id].akx.change = True;
    return 0;
}

float akx_pulse_angle(int akx_id)
{
    if ( !validate_object(akx_id, obj_akx))
    {
        return -1;
    }
    if ( glbObjects[akx_id].akx.action.type != act_akx_pulse )
    {
        glbPlayer->error = INVALID_TARGET;
        return -1;
    }
    return glbObjects[akx_id].akx.action.act.pulse.angle;
}

float akx_pulse_destx(int akx_id)
{
    if ( !validate_object(akx_id, obj_akx))
    {
        return -1;
    }
    if ( glbObjects[akx_id].akx.action.type != act_akx_pulse )
    {
        glbPlayer->error = INVALID_TARGET;
        return -1;
    }
    return glbObjects[akx_id].akx.action.act.pulse.x;
}

float akx_pulse_desty(int akx_id)
{
    if ( !validate_object(akx_id, obj_akx))
    {
        return -1;
    }
    if ( glbObjects[akx_id].akx.action.type != act_akx_pulse )
    {
        glbPlayer->error = INVALID_TARGET;
        return -1;
    }
    return glbObjects[akx_id].akx.action.act.pulse.y;
}

int akx_link(int akx_id, int target_id)
{
    if ( !validate_object_owner(akx_id, obj_akx))
    {
        return -1;
    }
    if ( !validate_object_owner(target_id, obj_akx))
    {
        return -1;
    }
    if ( target_id == akx_id )
    {
        glbPlayer->error = INVALID_TARGET;
        return -1;
    }
    glbObjects[akx_id].akx.change = True;
    set_default_action_akx(akx_id);
    glbObjects[akx_id].akx.action.type = act_akx_link;
    glbObjects[akx_id].akx.action.act.link.target_id = target_id;
    return 0;
}

int map_get_nearest_akx(int id, int team_id)
{
    int nearest;

    if ( id >= gl_config->nb_objects )
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    nearest = get_nearest(id, team_id, obj_akx);
    if ( nearest == id )
    {
        glbPlayer->error = NOT_VISIBLE;
        return -1;
    }
    return nearest;
}

int map_get_nearest_akx_plot(float x, float y, int team_id)
{
    int nearest;

    if ( !finitef(x) || !finitef(y))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    nearest = get_nearest_plot(x, y, -1, team_id, obj_akx);
    if ( nearest == -1 )
    {
        glbPlayer->error = NOT_VISIBLE;
        return -1;
    }
    return nearest;
}

int map_get_nearest_r4d2_plot(float x, float y, int team_id)
{
    int nearest;

    if ( !finitef(x) || !finitef(y))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    nearest = get_nearest_plot(x, y, -1, team_id, obj_r4d2);
    if ( nearest == -1 )
    {
        glbPlayer->error = NOT_VISIBLE;
        return -1;
    }
    return nearest;
}

int map_get_nearest_r4d2(int id, int team_id)
{
    int nearest;

    if ( id >= gl_config->nb_objects )
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    nearest = get_nearest(id, team_id, obj_r4d2);
    if ( nearest == id )
    {
        glbPlayer->error = NOT_VISIBLE;
        return -1;
    }
    return nearest;
}

int map_count_akx()
{
    return gl_config->nb_akx;
}

int map_count_r4d2()
{
    return gl_config->nb_r4d2;
}

int count_unit(int team_id, int obj_type)
{
    int cpt = 0, i;

    for ( i = 0 ; i < gl_config->nb_objects ; i++ )
    {
        if (( glbObjects[i].obj.team_id == team_id ) &&
            ( glbObjects[i].obj.type == obj_type ))
        {
            cpt++;
        }
    }
    return cpt;
}

int count_akx(int team_id)
{
    return count_unit(team_id, obj_akx);
}

int map_count_my_akx()
{
    return count_akx(glbPlayer->team_id);
}

int count_r4d2(int team_id)
{
    return count_unit(team_id, obj_r4d2);
}

float map_get_size_x()
{
    return gl_config->size_x;
}

float map_get_size_y()
{
    return gl_config->size_y;
}

int map_count_my_r4d2()
{
    return count_r4d2(glbPlayer->team_id);
}

float map_get_pulse(int team_id, float x, float y)
{
    if ( !finitef(x) || !finitef(y))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    if ( !is_visible(glbPlayer->team_id, x, y))
    {
        glbPlayer->error = NOT_VISIBLE;
        return -1;
    }
    return get_pulse_team(x, y, team_id);
}

float map_get_pulse_id(int akx_id, float x, float y)
{
    if ( !finitef(x) || !finitef(y))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    if (( akx_id >= gl_config->nb_objects ) || ( akx_id < 0 ))
    {
        glbPlayer->error = NBR_OUT_OF_RANGE;
        return -1;
    }
    if ( !is_visible(glbPlayer->team_id, x, y))
    {
        glbPlayer->error = NOT_VISIBLE;
        return -1;
    }
    if ( glbObjects[akx_id].obj.type != obj_akx )
    {
        glbPlayer->error = TYPE_ERROR;
        return -1;
    }
    return get_pulse_id(x, y, akx_id);
}

static float __map_random_seed = 0.23;

float map_random()
{
    __map_random_seed = 4 * __map_random_seed * ( 1 - __map_random_seed );
    return __map_random_seed;
}

void map_set_seed(float value)
{
    if ( value < 0 || value > 1 )
    {
        return;
    }
    __map_random_seed = value;
}
