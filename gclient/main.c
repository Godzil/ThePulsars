/* $Id: main.c,v 1.6 2001/04/10 17:16:22 kilobug Exp $ */

#include "client.h"

int main(int argc, char **argv)
{
  conf_t *conf;

  conf = init(argc, argv);
  create_gtk_stuff(conf);
  gtk_idle_add(idle_connect, conf);
  gtk_main();
  gtk_exit(0);
  return 0;
}
