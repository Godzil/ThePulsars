/* $Id: network.c,v 1.14 2001/04/29 22:24:37 kilobug Exp $ */

#include "client.h"

void send_goodbye(conf_t *conf)
{
  if ((conf->connected) && (conf->debug))
    {
      net_wr_string(conf->file, "END");
      fflush(conf->file);
      net_wait_ack(conf->socket);
      net_chk_string(conf->socket, "BYE");
      net_send_ack(conf->file);      
    }
  if (conf->connected)
    close_socket(conf);
}

void network_error(net_type_t get, net_type_t wanted, void *data)
{  
  char buf[100];
  conf_t *conf;

  conf = data;
  if ((conf == NULL) || (!conf->connected))
    return;
  conf->connected = FALSE;
  g_snprintf(buf, 100, 
	     "Erreur réseau: `wanted (%s), get (%s)'. Connexion perdue.\n", 
	     net_type_name(wanted), net_type_name(get));
  set_status(conf, buf);
}

void do_connect(conf_t *conf)
{
  char buf[420];
  
  send_goodbye(conf);
  conf->terminated = FALSE;
  snprintf(buf, 420, "Connexion en cours sur %s:%d...", conf->host, 
	   conf->port);
  set_status(conf, buf);

  conf->socket = net_connect_to(conf->host, conf->port, buf, 420, NULL);
  if (conf->socket == -1)
    {
      set_status(conf, buf);
      return;
    }
  snprintf(buf, 420, 
	   "Connexion établie avec %s:%d... authentification en cours.", 
	   conf->host, conf->port);
  set_status(conf, buf);
  conf->connected = TRUE;
  conf->file = fdopen(conf->socket, "w");

  net_wr_string(conf->file, "HELLO 42");
  fflush(conf->file);
  if (!net_chk_string(conf->socket, "HELLO 42"))
    {
      snprintf(buf, 420, "%s:%d n'est pas un serveur Prologin.", 
	       conf->host, conf->port);
      set_status(conf, buf);
      conf->socket = -1;
      return;
    }
  net_wr_int(conf->file, PROTOCOL_VERSION);
  fflush(conf->file);
  if (!net_wait_ack(conf->socket))
    {
      snprintf(buf, 420, "%s:%d connexion refusée (mauvaise version).", 
	       conf->host, conf->port);
      set_status(conf, buf);
      conf->socket = -1;
      return;
    }
  net_wr_flag(conf->file, conf->debug);
  fflush(conf->file);
  if (!net_wait_ack(conf->socket))
    {
      snprintf(buf, 420, "%s:%d connexion refusée (mauvais type de client).", 
	       conf->host, conf->port);
      set_status(conf, buf);
      conf->socket = -1;
      return;
    }
  net_send_ack(conf->file);
  load_map(conf);
  conf->gdk_io_id = gdk_input_add(conf->socket, GDK_INPUT_READ, 
				  update_map_callback, conf);
  snprintf(buf, 420, 
	   "Connexion établie avec %s:%d.", 
	   conf->host, conf->port);
  set_status(conf, buf);

  if (conf->conf != NULL)
    gtk_widget_hide(conf->conf);
}
  
void protocol_error(conf_t *conf, const char *msg)
{
  char buf[420];

  net_send_abort(conf->file);
  close_socket(conf);
  fprintf(stderr, "Erreur fatale de protocole avec %s:%d: %s.\n", 
	  conf->host, conf->port, msg);
  snprintf(buf, 420, "Erreur fatale de protocole avec %s:%d: %s.", 
	   conf->host, conf->port, msg);
  set_status(conf, buf);
  conf->ready = FALSE;
}

void close_socket(conf_t *conf)
{
  if (conf->socket > 0)
    {
      if (conf->gdk_io_id >= 0)
	gdk_input_remove(conf->gdk_io_id);
      conf->connected = FALSE;
      fclose(conf->file);
      conf->socket = -1;
      conf->file = NULL;
    }
}

void allow_new_turn(conf_t *conf)
{  
  if (!conf->connected)
    return;
  if (!conf->ready)
    return;
  conf->ready = FALSE;
  if (conf->gdk_io_id >= 0)
    gdk_input_remove(conf->gdk_io_id);
  if (conf->terminated)
    {
      send_goodbye(conf);
      return;
    }
  else
    net_wr_string(conf->file, "GO");
  fflush(conf->file);
  if (!net_wait_ack(conf->socket))
    protocol_error(conf, "expected ACK.");
  conf->gdk_io_id = gdk_input_add(conf->socket, GDK_INPUT_READ, 
				  update_map_callback, conf);
}

