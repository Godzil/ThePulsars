#include <iostream.h>
#include <math.h>
#include <stdlib.h>
#include "prolo.hh"

struct coords
        {
    float x,y;
    int target;
        };


double distance (float x1, float y1, float x2, float y2)
// Distance entre deux points, merci pythagore
{
    return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}


bool trouve(int id, coords tab[100])
// l'unite visee est elle deja convoitee par un r4d2 ?
{
    int a;
    for(a=0;a<100;a++)
    {
        if (id==tab[a].target)
            return true;
    }
    return false;
}


const float PI=3.1418; // et des poussieres
int myteam;


// angles de tir
float angles[100];
int dirsens[100];

// ID r4d2 vise
int targetone=-1;

// ou vont mes r4d2 ?
coords lemming[100];

//
// Player Init
//

void	player_init(int team_id)
{
    myteam=team_id;

    for(int a=0;a<100;a++)
    {
        lemming[a].x=-1;
        lemming[a].y=-1;

        angles[a]=PI/12;
        // pqs encore pointe
        dirsens[a]=1;
    }
    return;
}


//
// Player New Turn
//

void	player_new_turn(int turn_number)
{
    // et oui, rien
    return;
}


//
// Player Akx Turn
//

void	player_akx_turn(int akx_id)
{
    float xdanger;
    float ydanger;
    int r4d2_proche;


    //reperage r4d2 adverse
    r4d2_proche = map_get_nearest_r4d2(akx_id,0-myteam);
    xdanger = r4d2_get_pos_x(r4d2_proche);
    ydanger = r4d2_get_pos_y(r4d2_proche);

    // Si je detecte un r4d2 ennemi a moins de 30
    if (xdanger!=-1 && distance(akx_get_pos_x(akx_id),
                                akx_get_pos_y(akx_id),
                                r4d2_get_pos_x(r4d2_proche),
                                r4d2_get_pos_y(r4d2_proche))<50)
    {
        // il devient nouvelle cible principale
        targetone=r4d2_proche;
        akx_pulse(akx_id,r4d2_get_pos_x(targetone),
                  r4d2_get_pos_y(targetone),0.01);
        if (r4d2_get_pos_x(targetone)==-1)
        {
            cout << "Cible detruite!\n";
            targetone=-1;
        }
    }
    else
        // pqs d'ennemi proche
        {
        //tous les 4 tours - Big Pulse
        if (turn_counter()%4==0)
        {
            //DeepPulse
            akx_pulse(akx_id,akx_get_pos_x(akx_id)+20,
                      akx_get_pos_y(akx_id)+20,2*PI);
            if (turn_counter()%8==0) targetone=-1;
        }
        else
            //pqs de deepPulse
            {
            if (targetone!=-1)
                //une cible etait lockee
                {
                akx_pulse(akx_id,r4d2_get_pos_x(targetone),
                          r4d2_get_pos_y(targetone),0.01);
                if (r4d2_get_pos_x(targetone)==-1)
                    //Plus de cible
                    {
                    cout << "Cible detruite!\n";
                    targetone=-1;
                    }
                }
            // Pqs de cible lockee
            else
            {
                //PULSE Rotatif

                bool haut=false;
                bool bas=false;
                bool gauche=false;
                bool droite=false;

                if (akx_get_pos_x(akx_id)==0) gauche=true;
                if (akx_get_pos_y(akx_id)==0) haut=true;
                if (akx_get_pos_x(akx_id)==map_get_size_x()) droite=true;
                if (akx_get_pos_y(akx_id)==map_get_size_y()) bas=true;


                //coins
                if (haut && gauche &&
                (angles[akx_id]>PI/2-0.2 ) )
                    angles[akx_id]=PI/12;

                if (haut && droite &&
                (angles[akx_id]>PI-0.2 || angles[akx_id]<PI/2) )
                    angles[akx_id]=PI/2+PI/12;

                if (bas && droite &&
                (angles[akx_id]>3*PI/2-0.2 || angles[akx_id]<PI) )
                    angles[akx_id]=PI+PI/12;

                if (bas && gauche &&
                (angles[akx_id]>2*PI-0.2 || angles[akx_id]<3*PI/2) )
                    angles[akx_id]=3*PI/2+PI/12;

                //bords
                if (haut && !gauche && !droite &&
                angles[akx_id]>PI-0.2 )
                    angles[akx_id]=0+PI/12;

                if (bas && !droite && !gauche &&
                (angles[akx_id]>2*PI-0.2 || angles[akx_id]<PI) )
                    angles[akx_id]=PI+PI/12;

                if (droite && !bas && !haut &&
                (angles[akx_id]>3*PI/2-0.2 || angles[akx_id]<PI/2) )
                    angles[akx_id]=PI/2+PI/12;

                if (gauche && !bas && !haut &&
                (angles[akx_id]>2*PI-0.2 ) )
                    angles[akx_id]=0+PI/12;

                if (gauche && !bas && !haut &&
                angles[akx_id]>PI/2-0.2 && angles[akx_id]<3*PI/2 )
                    angles[akx_id]=3*PI/2+PI/12;


                akx_pulse(akx_id,
                          akx_get_pos_x(akx_id)+cos(angles[akx_id])*40,
                          akx_get_pos_y(akx_id)+sin(angles[akx_id])*40,PI/6+0.1);

                angles[akx_id]+=PI/6*dirsens[akx_id];
            }
            }
        }
    return;
}


//
// r4d2 Turn
//

void	player_r4d2_turn(int r4d2_id)
{
    int akx_proche;
    float xakx;
    float yakx;

    int r4d2_proche;
    float xr4,yr4;

    // locate hostile akx
    akx_proche = map_get_nearest_akx(r4d2_id,0-myteam);
    xakx = akx_get_pos_x(akx_proche);
    yakx = akx_get_pos_y(akx_proche);
    // locate hostile r4d2
    r4d2_proche = map_get_nearest_r4d2(r4d2_id,0-myteam);
    xr4 = r4d2_get_pos_x(r4d2_proche);
    yr4 = r4d2_get_pos_y(r4d2_proche);




    // je suis immobile et j'ai repere un r4d2 proche
    if (r4d2_get_status(r4d2_id)==0 && xr4!=-1 &&
    distance(r4d2_get_pos_x(r4d2_id),r4d2_get_pos_y(r4d2_id),
             xr4,yr4)<1)
        //cqpture r4d2 -rare -
        {
        cout<< "Tentative capture d'un R4D2!\n";
        r4d2_take_r4d2(r4d2_id,r4d2_proche);
        }

    else
    {

        // je suis immobile ou en mouvement et j'ai repere un akx
        if (r4d2_get_status(r4d2_id)==0 && xakx!=-1)
        {
            if (lemming[r4d2_id].x==-1)
                // je suis ne suis pas en mission
                {
                //qqn est deja sur le coup
                if (trouve(akx_proche,lemming))
                {
                    if(rand()%4==1)
                        //on force le deplacement une fois sur 4
                        {
                        lemming[r4d2_id].x=xakx;
                        lemming[r4d2_id].y=yakx;
                        lemming[r4d2_id].target= akx_proche;
                        }
                    else
                        //qqn est sur le coup, et je nai pas le droit
                        //donc j'attends calmement
                        {
                        // r4d2_boire_un_cafe(r4d2_id);
                        }
                }
                else
                    // personne sur le coup
                    {
                    lemming[r4d2_id].x=xakx;
                    lemming[r4d2_id].y=yakx;
                    lemming[r4d2_id].target= akx_proche;
                    r4d2_move (r4d2_id, lemming[r4d2_id].x, lemming[r4d2_id].y);
                    }

                }
            else
                // je suis deja en mission, je m'y tiens
                {
                r4d2_move (r4d2_id, lemming[r4d2_id].x, lemming[r4d2_id].y);
                }
        }
    }
    // akx adversqire tout proche
    if ( distance(r4d2_get_pos_x(r4d2_id),
                  r4d2_get_pos_y(r4d2_id),lemming[r4d2_id].x,
                  lemming[r4d2_id].y)<1 && r4d2_get_status(r4d2_id)!=2 )
    {
        // je verif que je ne veux pas capturer un truc a moi
        if(akx_get_team(lemming[r4d2_id].target)!=myteam)
        {
            r4d2_take_akx(r4d2_id, lemming[r4d2_id].target);
            cout << "Tentative de capture d'un akx!\n";
        }

        lemming[r4d2_id].x=-1;
    }


    return;
}



