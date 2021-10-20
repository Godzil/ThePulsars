/*
 * _________          _______    _______           _       _______  _______  _______  _______
 * \__   __/|\     /|(  ____ \  (  ____ )|\     /|( \     (  ____ \(  ___  )(  ____ )(  ____ \
 *    ) (   | )   ( || (    \/  | (    )|| )   ( || (     | (    \/| (   ) || (    )|| (    \/
 *    | |   | (___) || (__      | (____)|| |   | || |     | (_____ | (___) || (____)|| (_____
 *    | |   |  ___  ||  __)     |  _____)| |   | || |     (_____  )|  ___  ||     __)(_____  )
 *    | |   | (   ) || (        | (      | |   | || |           ) || (   ) || (\ (         ) |
 *    | |   | )   ( || (____/\  | )      | (___) || (____/Y\____) || )   ( || ) \ \__/\____) |
 *    )_(   |/     \|(_______/  |/       (_______)(_______|_______)|/     \||/   \__/\_______)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#include <server.h>
#include <network.h>
#include <conf.h>
#include <player.h>
#include <game.h>
#include <libgen.h>
#include <ipc.h>
#include <map.h>

#ifdef ENABLE_GPC
void init_Gpc(int argc, char **argv, char **arge);
void init_Init();
#endif

static void cmdline_add_player(const char *str, conf_t *conf)
{
    char *name, *filename;
    char *p;
    static int id = 1;

    name = strdup(str);
    p = strchr(name, ':');
    if ( p == NULL)
    {
        filename = name;
        name = strdup(basename(filename));
    }
    else
    {
        *p = 0;
        filename = strdup(p + 1);
    }
    conf->players = g_slist_prepend(conf->players, player_new(filename, name, id++, conf));
}

static void help(char **argv)
{
    printf("Serveur Prologin version %d.%d.%d%s\n", V_MAJOR, V_MINOR, V_MICRO, V_SHA);
    printf("\n");
    printf("Syntaxe: %s [options] [-m carte] nom:fichier ...\n", argv[0]);
    printf("\nOptions:\n");
    printf("  -v     \t Mode verbose.\n");
    printf("  -q     \t Supprime les messages des clients.\n");
    printf("  -n val \t Nombre de tours que dure la partie.\n");
    printf("  -t val \t Nombre de milliseconde pas tour.\n");
    printf("  -m     \t Fichier carte.\n");
    printf("\nOptions graphiques:\n");
    printf("  -g     \t Accepte les clients graphiques.\n");
    printf("  -P     \t Mode prive.\n");
    printf("  -p val \t Port pour les clients graphiques (4242).\n");
}

static conf_t *parse_cmdline(int argc, char **argv)
{
    int i;
    conf_t *res;

    res = shm_alloc(sizeof(*res), get_shm_conf_id());

    res = conf_set_default(res);

    for ( i = 1 ; i < argc ; i++ )
    {
        if ( argv[i][0] == '-' )
        {
            switch ( argv[i][1] )
            {
            default:
            case 'h':
                help(argv);
                exit(0);
            case 'P':
                res->pub = False;
                break;
            case 'v':
                res->verbose = True;
                break;
            case 'g':
                res->gfx = True;
                break;
            case 'q':
                res->quiet = True;
                break;

            case 'n':
                i++;
                if ( i == argc )
                {
                    printf("-n requiert un argument.\n");
                    return NULL;
                }
                res->nb_turns = atoi(argv[i]);
                break;

            case 't':
                i++;
                if ( i == argc )
                {
                    printf("-t requiert un argument.\n");
                    return NULL;
                }
                res->turn_msec = atoi(argv[i]);
                break;

            case 'p':
                i++;
                if ( i == argc )
                {
                    printf("-p requiert un argument.\n");
                    return NULL;
                }
                res->port = atoi(argv[i]);
                break;

            case 'm':
                i++;
                if ( i == argc )
                {
                    printf("-m requiert un argument.\n");
                    return NULL;
                }
                res->map_name = argv[i];
                break;
            }
        }
        else
        {
            cmdline_add_player(argv[i], res);
        }
    }
    res->nb_players = g_slist_length(res->players);
    return res;
}

int main(int argc, char **argv, char **arge)
{
    conf_t *conf = NULL;
    GSList *l;
    player_t *player;

#ifdef ENABLE_GPC
    init_Gpc(argc, argv, arge);
    init_Init();
#endif


    sigset_t mask2;

    if ( argc < 2 )
    {
        help(argv);
        return NULL;
    }
    gl_config = conf = parse_cmdline(argc, argv);
    if ( conf == NULL)
    {
        server_abort();
    }

    atexit(clean_shm);

    load_map(conf);
    gfx_init();

    sigprocmask(0, NULL, &mask2);
    sigaddset(&mask2, SIGUSR1);
    sigaddset(&mask2, SIGCHLD);
    conf->mask = malloc(sizeof(*conf->mask));
    sigprocmask(SIG_SETMASK, &mask2, conf->mask);
    signal(SIGINT, exit);

    game_launch(conf);

    for ( l = conf->players ; l != NULL ; l = l->next )
    {
        glbPlayer = l->data;
        fprintf(stdout, "Player %d : %f\n", glbPlayer->team_id,
                glbPlayer->score);
        fflush(stdout);
    }

    gfx_client_new_turn(1);

    kill(gl_config->messager_pid, SIGUSR1);
    gl_config->messager_pid = 0;

    for ( l = gl_config->players ; l != NULL ; l = l->next )
    {
        player = l->data;
        kill(player->pid, SIGKILL);
        player->pid = 0;
    }

    return 0;
}

void server_abort()
{
    fflush(stdout);
    fflush(stderr);
    fprintf(stdout, "---ABORT---\nBye!\n");
    fflush(stdout);
    exit(1);
}