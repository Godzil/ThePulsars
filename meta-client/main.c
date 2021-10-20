#include "mclient.h"

conf_t *gl_config = NULL;

int main(int argc, char **argv)
{
    init(argc, argv);
    create_gtk_stuff();
    gtk_main();
    return 0;
}
