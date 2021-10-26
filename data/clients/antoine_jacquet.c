#include "prolo.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



/* Quelques constantes... */

#define pi M_PI
#define sqr(x) ((x)*(x))
#define MAX 300
#define RAYON_MIN 1
#define OUVERTURE_DEPART pi/4
#define DEFENSE_DISTANCE 5
#define DEFENSE_OUVERTURE 5
#define ATTAQUE_DISTANCE 1
#define ATTAQUE_OUVERTURE 0.01
#define GRATUIT_FREQ 10/10
#define GRATUIT_OUVERTURE 0.5
#define AIDE_TIMELIMIT 10000
#define AIDE_NBLIMIT 1
#define AIDE_OUVERTURE (2*r4d2_get_speed()+1)



/* Variables globales */

struct
{
    int team;
    float x,y;
    float angle,ouverture;
} akx[MAX];

struct
{
    float x,y,r;
} vise;

int capturing[MAX];

int nation;



/* Player_Init */

void player_init(int team_id, int nb_team)
{
    int i;

    nation=team_id;
    printf("version 2\n");
    printf("je suis %d\n",nation);
    printf("akx_see_power : %f\n",akx_get_see_power());

    for (i=0;i<MAX;i++)
    {
        akx[i].ouverture=OUVERTURE_DEPART;
        akx[i].team=-1;
        capturing[i]=-1;
    }

    return;
}



/* Player_New_Turn */

void player_new_turn(int turn_number)
{
    int i,j,best_akx,best_r4d2;
    float best,dist;

    /* Mise a jour de la table locale des AnaKronoX */
    for (i=0;i<MAX;i++) if (akx_get_team(i)!=-1)
    {
        if (akx[i].team==-1)
        {
            printf("akx %d trouve\n",i);
            akx[i].x=akx_get_pos_x(i);
            akx[i].y=akx_get_pos_y(i);
            akx[i].team=akx_get_team(i);
        }
        if (akx[i].team!=akx_get_team(i))
        {
            printf("akx %d a change de team\n",i);
            akx[i].team=akx_get_team(i);
        }
        if ((akx[i].x!=akx_get_pos_x(i)) || (akx[i].y!=akx_get_pos_y(i)))
        {
            printf("akx %d a bouge\n",i);
            akx[i].x=akx_get_pos_x(i);
            akx[i].y=akx_get_pos_y(i);
        }
    }

    /* S'il ne reste que peu de r4d2 ou qu'on approche de la fin de partie, on defend le plus possible un des r4d2 restants */
    if ((turn_counter()>=AIDE_TIMELIMIT) || (map_count_my_r4d2()<=AIDE_NBLIMIT))
        for (i=0;i<MAX;i++) if (r4d2_get_team(i)==nation)
        {
            printf("AIDE r4d2 %d\n",i);
            vise.x=r4d2_get_pos_x(i);
            vise.y=r4d2_get_pos_y(i);
            vise.r=AIDE_OUVERTURE;
            return;
        }

    /* Recherche du r4d2 ennemi le plus proche d'un de mes AnaKronoX */
    best=10000;
    for (i=0;i<MAX;i++) if (((akx_get_team(i)==nation)||(akx_get_team(i))==0) && ((j=map_get_nearest_r4d2_plot(akx_get_pos_x(i),akx_get_pos_y(i),-nation))!=-1))
    {
        dist=sqrt(sqr(akx_get_pos_x(i)-r4d2_get_pos_x(j))
                +sqr(akx_get_pos_y(i)-r4d2_get_pos_y(j)));
        if (dist<best)
        {
            best=dist;
            best_akx=i;
            best_r4d2=j;
        }
    }

    /* S'il y a un r4d2 ennemi tres proche d'un de mes AnaKronoX (surement en train de convertir donc surement imobile, on le vise tres serre */
    if (best<ATTAQUE_DISTANCE)
    {
        printf("ATTAQUE %d\n",best_r4d2);
        vise.x=r4d2_get_pos_x(best_r4d2);
        vise.y=r4d2_get_pos_y(best_r4d2);
        vise.r=ATTAQUE_OUVERTURE;
        return;
    }

    /* S'il y a un r4d2 ennemi assez proche d'un des mes AnaKronoX, on le gene faisant un defense de zone autour de l'AnaKronoX */
    if (best<DEFENSE_DISTANCE)
    {
        printf("DEFENSE %d\n",best_akx);
        vise.x=akx_get_pos_x(best_akx);
        vise.y=akx_get_pos_y(best_akx);
        vise.r=DEFENSE_OUVERTURE;
        return;
    }

    /* On tente une attaque "gratuite" sur un r4d2 ennemi de temps en temps */
    if (map_random()<GRATUIT_FREQ)
        for (i=0;i<MAX;i++) if ((r4d2_get_team(i)!=-1) && (r4d2_get_team(i)!=nation))
        {
            vise.x=r4d2_get_pos_x(i);
            vise.y=r4d2_get_pos_y(i);
            vise.r=GRATUIT_OUVERTURE;
            return;
        }

    /* Sinon on ne vise rien de special (on fera du scanning) */
    vise.r=-1;
    return;
}



/* Player_AKX_Turn */

void player_akx_turn(int akx_id)
{
    float rayon,ouverture,tcos,tsin;

    /* Si on n'a plus de r4d2, on fait une defense de zone tres concentree, en bougeant une fois sur 3 */
    if (map_count_my_r4d2()==0)
    {
        /* J'utilise l'angle comme boolean pour alterner deplacement/defense */
        if (akx[akx_id].angle<2)
        {
            akx[akx_id].angle++;
            vise.x=akx_get_pos_x(akx_id);
            vise.y=akx_get_pos_y(akx_id);
            vise.r=RAYON_MIN;
        }
        else
        {
            akx[akx_id].angle=0;
            akx_move(akx_id,1,1);
            return;
        }
    }

    /* Si on a une directive de visee, on l'execute */
    if (vise.r!=-1)
    {
        rayon=sqrt(sqr(akx_get_pos_x(akx_id)-vise.x)
                +sqr(akx_get_pos_y(akx_id)-vise.y));
        /* Si la distance entre l'AnaKronoX et sa cible est trop faible, on fait une defense circulaire */
        if (rayon<RAYON_MIN)
            akx_pulse(akx_id,akx_get_pos_x(akx_id)+RAYON_MIN,akx_get_pos_y(akx_id),2*pi);
        /* Sinon on vise la cible avec l'ouverture demandee, en centrant au maximum */
        else
        {
            tcos=(vise.x-akx_get_pos_x(akx_id))/rayon;
            tsin=(vise.y-akx_get_pos_y(akx_id))/rayon;
            ouverture=2*atan(vise.r/rayon);
            akx_pulse(akx_id,vise.x+tcos*vise.r,vise.y+tsin*vise.r,ouverture);
        }
        return;
    }

    /* Sinon on scanne la carte pour trouver de nouveaux objectifs */
    rayon=sqrt(2*akx_get_power()/(akx_get_see_power()*akx[akx_id].ouverture));
    rayon--;
    akx_pulse(akx_id,akx_get_pos_x(akx_id)+cos(akx[akx_id].angle)*rayon,akx_get_pos_y(akx_id)+sin(akx[akx_id].angle)*rayon,akx[akx_id].ouverture);

    /* Calcul du prochain scan : on evite de scanner en dehors de la carte */
    do
    {
        akx[akx_id].angle+=akx[akx_id].ouverture;
        while (akx[akx_id].angle>=2*pi) akx[akx_id].angle-=2*pi;
        /* Si on a fait un tour complet, on scanne plus serre et plus loin */
        if ((akx[akx_id].angle>=0) && (akx[akx_id].angle<akx[akx_id].ouverture))
        {
            akx[akx_id].angle=0;
            /* Si on scanne deja un rayon plus grand que la map, on repart sur un petit scan */
            if ((rayon>=map_get_size_x()) && (rayon>=map_get_size_y())) akx[akx_id].ouverture=OUVERTURE_DEPART;
            else akx[akx_id].ouverture=akx[akx_id].ouverture/2;
        }
        rayon=sqrt(2*akx_get_power()/(akx_get_see_power()*akx[akx_id].ouverture));
    }
    while ((akx_get_pos_x(akx_id)+cos(akx[akx_id].angle)*rayon<-1) ||
    (akx_get_pos_x(akx_id)+cos(akx[akx_id].angle)*rayon>map_get_size_x()+1) ||
    (akx_get_pos_y(akx_id)+sin(akx[akx_id].angle)*rayon<-1) ||
    (akx_get_pos_y(akx_id)+sin(akx[akx_id].angle)*rayon>map_get_size_y()+1));

    return;
}



int being_captured(int akx_id)
/* Retourne 1 si l'AnaKronoX passe en parametre est deja en cours de capture */
{
    int i;
    for (i=0;i<MAX;i++) if (capturing[i]==akx_id) return(1);
    return(0);
}

int choose_nearest_akx_plot(int x, int y)
/* Retourne l'AnaKronoX le plus proche du point (x,y) et qui n'est pas deja en cours de capture */
{
    float dist,best=10000;
    int id=-1,i;

    for (i=0;i<MAX;i++) if ((akx[i].team!=-1) && (akx[i].team!=nation) && (!being_captured(i)))
    {
        dist=sqrt(sqr(akx_get_pos_x(i)-x)+sqr(akx_get_pos_y(i)-y));
        if (dist<best)
        {
            best=dist;
            id=i;
        }
    }
    return(id);
}


/* Player_R4D2_Turn */

void player_r4d2_turn(int r4d2_id)
{
    int i;

    if (akx_get_team(capturing[r4d2_id])==nation)
    {
        printf("%d -/-> %d\n",r4d2_id,capturing[r4d2_id]);
        capturing[r4d2_id]=-1;
    }

    if (capturing[r4d2_id]==-1)
    {
        if ((i=choose_nearest_akx_plot(r4d2_get_pos_x(r4d2_id),r4d2_get_pos_y(r4d2_id)))==-1)
            r4d2_move(r4d2_id,map_get_size_x()/2,map_get_size_y()/2);
        else
        {
            r4d2_move(r4d2_id,akx[i].x,akx[i].y);
            capturing[r4d2_id]=i;
            printf("%d ---> %d\n",r4d2_id,i);
        }
    }
    else
        if (sqrt(sqr(r4d2_get_pos_x(r4d2_id)-akx[capturing[r4d2_id]].x)+
        sqr(r4d2_get_pos_y(r4d2_id)-akx[capturing[r4d2_id]].y))<1)
            r4d2_take_akx(r4d2_id,capturing[r4d2_id]);
        else
            r4d2_move(r4d2_id,akx[capturing[r4d2_id]].x,akx[capturing[r4d2_id]].y);
        return;
}


