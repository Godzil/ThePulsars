/* $Id: map_parser.c,v 1.15 2001/04/23 16:57:55 kilobug Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <server.h>
#include <objects.h>
#include <ipc.h>

/* TODO: This is going to be painful to remove GLIB */

/*
** Map parser
*/

#define MAP_SCOPE_NONE 0
#define MAP_SCOPE_SPEED 1
#define MAP_SCOPE_TURN 2
#define MAP_SCOPE_SEE 3
#define MAP_SCOPE_SAND 4
#define MAP_SCOPE_ITEM 10
#define MAP_SCOPE_TEAM 11

#define CONF_OFFS(ID) G_STRUCT_OFFSET(conf_t, ID)

/* TODO: Change that */
typedef int GScanner;

#if 0

typedef struct _map_hdr_t
{
    const char *name;
    GTokenType type;
    int scope;
    int offs;
} map_hdr_t;

static map_hdr_t map_hdr[] =
        {{ "take_akx",      G_TOKEN_INT,    MAP_SCOPE_TURN,  CONF_OFFS(turn_take_akx) },
         { "take_r4d2",     G_TOKEN_INT,    MAP_SCOPE_TURN,  CONF_OFFS(turn_take_r4d2) },
         { "untake_akx",    G_TOKEN_INT,    MAP_SCOPE_TURN,  CONF_OFFS(turn_untake_akx) },
         { "untake_r4d2",   G_TOKEN_INT,    MAP_SCOPE_TURN,  CONF_OFFS(turn_untake_r4d2) },
         { "size_x",        G_TOKEN_FLOAT,  MAP_SCOPE_NONE,  CONF_OFFS(size_x) },
         { "size_y",        G_TOKEN_FLOAT,  MAP_SCOPE_NONE,  CONF_OFFS(size_y) },
         { "akx_speed",     G_TOKEN_FLOAT,  MAP_SCOPE_SPEED, CONF_OFFS(akx_speed) },
         { "r4d2_speed",    G_TOKEN_FLOAT,  MAP_SCOPE_SPEED, CONF_OFFS(r4d2_speed) },
         { "pulse_power",   G_TOKEN_FLOAT,  MAP_SCOPE_NONE,  CONF_OFFS(pulse_power) },
         { "nb_objects",    G_TOKEN_INT,    MAP_SCOPE_NONE,  CONF_OFFS(nb_objects) },
         { "see",           G_TOKEN_FLOAT,  MAP_SCOPE_SEE,   CONF_OFFS(see_power) },
         { "see_power",     G_TOKEN_FLOAT,  MAP_SCOPE_SEE,   CONF_OFFS(see_power) },
         { "destroy",       G_TOKEN_FLOAT,  MAP_SCOPE_SPEED, CONF_OFFS(destroy_speed) },
         { "destroy_speed", G_TOKEN_FLOAT,  MAP_SCOPE_SPEED,
                                                             CONF_OFFS(destroy_speed) },
         { "pulse_coef",    G_TOKEN_FLOAT,  MAP_SCOPE_SPEED, CONF_OFFS(pulse_coef) },
         { "sand",          G_TOKEN_STRING, MAP_SCOPE_SAND,  CONF_OFFS(sand_file) },
         { "sand_file",     G_TOKEN_STRING, MAP_SCOPE_SAND,  CONF_OFFS(sand_file) },
         { NULL,            0, 0, 0 }};
#endif
/* Load a value: (x, y) keyword [param]; */
static void map_load_value(GScanner *scan, conf_t *conf, int id)
{
#if 0
   GTokenType type;
   float x, y;

   if (id >= conf->nb_objects)
   {
      g_scanner_error(scan,
                      "Le champ nb_objects ne correspond pas au nombre d'objets.");
      abort();
   }

   scan->config->int_2_float = TRUE;
   x = parser_get_float(scan);
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_COMMA);
   y = parser_get_float(scan);
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_RIGHT_PAREN);
   scan->config->int_2_float = FALSE;
   if (x > conf->size_x)
   {
      conf->size_x = x;
   }
   if (y > conf->size_y)
   {
      conf->size_y = y;
   }
   g_scanner_set_scope(scan, MAP_SCOPE_ITEM);
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_SYMBOL);
   switch (parser_get_symbol_as_int(scan))
   {
   case obj_akx:
      conf->nb_akx++;
      new_akx(x, y, id);
      break;
   case obj_r4d2:
      conf->nb_r4d2++;
      new_r4d2(x, y, id);
      break;
   }

   g_scanner_set_scope(scan, MAP_SCOPE_TEAM);
   type = g_scanner_get_next_token(scan);
   g_scanner_set_scope(scan, MAP_SCOPE_NONE);

   switch (type)
   {
   case G_TOKEN_SYMBOL:
      glbObjects[id].obj.team_id = parser_get_symbol_as_int(scan);
      parser_check_type(scan, g_scanner_get_next_token(scan),
                        G_TOKEN_SEMICOLON);
      break;
   case G_TOKEN_INT:
      glbObjects[id].obj.team_id = g_scanner_cur_value(scan).v_int;
      parser_check_type(scan, g_scanner_get_next_token(scan),
                        G_TOKEN_SEMICOLON);
      break;
   case G_TOKEN_SEMICOLON:
      break;
   default:
      g_scanner_unexp_token(scan, G_TOKEN_INT, NULL, NULL, NULL, NULL,
                            TRUE);
      abort();
   }
   if (glbObjects[id].obj.team_id > conf->nb_players)
   {
      glbObjects[id].obj.team_id = 0;
   }
#endif
}

static void map_load_data(GScanner *scan, conf_t *conf)
{
#if 0
   GTokenType type;
   int id = 0;

   while ((type = g_scanner_get_next_token(scan)) != G_TOKEN_EOF)
   {
      if (type == G_TOKEN_LEFT_PAREN)
      {
         map_load_value(scan, conf, id++);
      }
      else
      {
         g_scanner_unexp_token(scan, G_TOKEN_LEFT_PAREN, NULL, NULL, NULL,
                               NULL, TRUE);
         abort();
      }
   }
   if (id != conf->nb_objects)
   {
      g_scanner_unexp_token(scan, G_TOKEN_LEFT_PAREN, NULL, NULL, NULL,
                            NULL, TRUE);
      abort();
   }
#endif
}

static void map_load_header_value(GScanner *scan, conf_t *conf)
{
#if 0
   int i, scope;
   char *s;

   parser_check_type(scan, g_scanner_cur_token(scan), G_TOKEN_IDENTIFIER);
   s = g_scanner_cur_value(scan).v_identifier;
   for (i = 0 ; map_hdr[i].name != NULL ; i++)
   {
      if (!g_strcasecmp(map_hdr[i].name, s))
      {
         break;
      }
   }
   if (map_hdr[i].name == NULL)
   {
      g_scanner_error(scan, "Mot clef invalide: %s.\n", s);
      abort();
   }
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_EQUAL_SIGN);
   scope = scan->scope_id;
   g_scanner_set_scope(scan, map_hdr[i].scope);
   switch (map_hdr[i].type)
   {
   case G_TOKEN_INT:
      G_STRUCT_MEMBER(
      int, conf, map_hdr[i].offs) = parser_get_int(scan);
      break;
   case G_TOKEN_FLOAT:
      G_STRUCT_MEMBER(
      float, conf, map_hdr[i].offs) = parser_get_float(scan);
      break;
   case G_TOKEN_STRING:
      G_STRUCT_MEMBER(
      char *, conf, map_hdr[i].offs) = parser_get_string(scan);
      break;
   default:
      g_scanner_error(scan, "Fonction non implement�e.\n");
      abort();
   };
   g_scanner_set_scope(scan, scope);
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_SEMICOLON);
#endif
}

static void map_load_header(GScanner *scan, conf_t *conf)
{
#if 0
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_LEFT_BRACE);
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_IDENTIFIER);
   if (strcmp(g_scanner_cur_value(scan).v_identifier, "header"))
   {
      g_scanner_error(scan, "Section [header] non trouv�e.\n");
      abort();
   }
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_RIGHT_BRACE);

   while (g_scanner_get_next_token(scan) != G_TOKEN_LEFT_BRACE)
   {
      map_load_header_value(scan, conf);
   }

   if (conf->nb_objects <= 0)
   {
      g_scanner_error(scan, "Nombre d'objets invalide.\n");
      abort();
   }

   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_IDENTIFIER);
   if (strcmp(g_scanner_cur_value(scan).v_identifier, "objects"))
   {
      g_scanner_error(scan, "Section [objects] non trouv�e.\n");
      abort();
   }
   parser_check_type(scan, g_scanner_get_next_token(scan), G_TOKEN_RIGHT_BRACE);
#endif
}

static void map_init_scopes(GScanner *scan)
{
#if 0
   static obj_type_t akx = obj_akx;
   static obj_type_t r4d2 = obj_r4d2;
   static double fnone = 0.0;
   static int inone = 0;
   static char *sand = NULL;

   g_scanner_scope_add_symbol(scan, MAP_SCOPE_ITEM, "r4d2", &r4d2);
   g_scanner_scope_add_symbol(scan, MAP_SCOPE_ITEM, "akx", &akx);
   g_scanner_scope_add_symbol(scan, MAP_SCOPE_ITEM, "anakronox", &akx);
   g_scanner_scope_add_symbol(scan, MAP_SCOPE_TEAM, "none", &inone);
   g_scanner_scope_add_symbol(scan, MAP_SCOPE_TURN, "none", &inone);
   g_scanner_scope_add_symbol(scan, MAP_SCOPE_SPEED, "none", &fnone);
   g_scanner_scope_add_symbol(scan, MAP_SCOPE_SEE, "all", &fnone);
   g_scanner_scope_add_symbol(scan, MAP_SCOPE_SAND, "default", &sand);
#endif
}

void load_map(conf_t *conf)
{
   GScanner *scan;
   int fd;

   fd = open(conf->map_name, O_RDONLY);
   if (fd == -1)
   {
      fprintf(stderr, "Erreur: impossible d'ouvrir la carte (%s): %s\n",
              conf->map_name, strerror(errno));
      server_abort();
   }
#if 0
   scan = g_scanner_new(NULL);
   scan->config->int_2_float = FALSE;
   scan->config->case_sensitive = FALSE;
   scan->input_name = conf->map_name;
#else
   scan = NULL;
#endif

   map_init_scopes(scan);

#if 0
   g_scanner_set_scope(scan, MAP_SCOPE_NONE);
   g_scanner_input_file(scan, fd);
#endif

   map_load_header(scan, conf);

   glbObjects = shm_alloc(sizeof(*glbObjects) * conf->nb_objects,
                          get_shm_world_id());
   map_load_data(scan, conf);

#if 0
   g_scanner_destroy(scan);
#endif

   close(fd);
}
