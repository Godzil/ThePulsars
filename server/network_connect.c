#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <server.h>
#include <network.h>
#include <objects.h>

#include <general.h>
#include <pulsarnet.h>


static void gfx_send_map(gfx_client_t *cl)
{
    player_t *play;
    GSList *l;
    int i;
    item_t *obj;

    net_wr_float(cl->wsock, gl_config->size_x);
    net_wr_float(cl->wsock, gl_config->size_y);
    net_wr_int(cl->wsock, gl_config->nb_turns);
    net_wr_int(cl->wsock, gl_config->turn_num);
    net_wr_int(cl->wsock, gl_config->nb_players);
    net_wr_int(cl->wsock, gl_config->nb_r4d2);
    net_wr_int(cl->wsock, gl_config->nb_akx);
    net_wr_string(cl->wsock, gl_config->sand_file);
    if ( !gfx_get_ack(cl))
    {
        return;
    }
    for ( l = gl_config->players ; l != NULL ; l = l->next )
    {
        play = l->data;
        net_wr_string(cl->wsock, "PLR");
        net_wr_int(cl->wsock, play->team_id);
        net_wr_string(cl->wsock, play->name);
        net_wr_float(cl->wsock, play->score);
    }
    if ( !gfx_get_ack(cl))
    {
        return;
    }
    for ( i = 0 ; i < gl_config->nb_objects ; i++ )
    {
        obj = glbObjects + i;
        net_wr_string(cl->wsock, "OBJ");
        net_wr_int(cl->wsock, obj->obj.id);
        net_wr_int(cl->wsock, obj->obj.type);
        gfx_write_object(cl->wsock, obj);
    }
    if ( !gfx_get_ack(cl))
    {
        return;
    }
}

boolean_t gfx_new_client(int socket, boolean_t master)
{
    gfx_client_t *cl;
    gfx_t *gfx;
    struct sockaddr_in sin;
    int sin_len;
    int fd;
    FILE *file;
    struct timeval tv;
    int value = 1;

    if ( !gl_config->gfx )
    {
        return True;
    }
    if (( !master ) && ( !gl_config->pub ))
    {
        return False;
    }
    sin_len = sizeof(sin);
    if ( !master )
    {
        fcntl(gl_config->gfx_clients->sock, F_SETFL, O_NONBLOCK);
    }
    if (( fd = accept(socket, (struct sockaddr *) &sin, &sin_len)) == -1 )
    {
        if ( master )
        {
            perror("accept");
        }
        return False;
    }
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &value, sizeof(value));
    if ( !master )
    {
        fcntl(fd, F_SETFL, 0);
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        net_set_error_handler(NULL, NULL);
    }
    else
    {
        net_set_error_handler(net_error_abort, NULL);
    }
    if ( !net_chk_string(fd, "HELLO 42"))
    {
        close(fd);
        return False;
    }
    file = fdopen(fd, "w");
    net_wr_string(file, "HELLO 42");
    fflush(file);
    if ( net_get_int(fd) != PROTOCOL_VERSION )
    {
        net_send_abort(file);
        fclose(file);
        return False;
    }
    net_send_ack(file);
    if ( net_get_flag(fd) != master )
    {
        net_send_abort(file);
        fclose(file);
        return False;
    }
    net_send_ack(file);
    if ( !net_wait_ack(fd))
    {
        net_send_abort(file);
        fclose(file);
        return False;
    }
    gfx = gl_config->gfx_clients;
    cl = malloc(sizeof(*cl));
    cl->sock = fd;
    cl->wsock = file;
    cl->alive = True;
    //gfx->clients = g_slist_prepend(gfx->clients, cl);
    if ( master )
    {
        gfx->master = cl->sock;
        gfx->wmaster = cl->wsock;
    }
    gfx_send_map(cl);
    return True;
}

