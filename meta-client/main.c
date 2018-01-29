/* $Id: main.c,v 1.2 2001/04/11 14:42:23 kilobug Exp $ */

#include "mclient.h"

conf_t *gl_config = NULL;

int main(int argc, char **argv)
{
  init(argc, argv);
  create_gtk_stuff();
  gtk_main();
  return 0;
}
