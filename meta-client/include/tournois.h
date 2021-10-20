typedef struct _rt_t
{
    int mapid;
    int nbp, nbt, nbm, nbc;
    char *cmd;
    enum
    {
        met_comb = 0, met_rand = 1, met_pyr = 3, met_rot = 2
    } methode;
    int *champ, *chmp;
    GtkWidget *dialog, *textarea;
    int cur;
    void *data;
    int gdki, fd;
    char *buf;
    char *map;
    bool run, disp;
    float *scores;
    int mysql_id;
    bool timeout;
} rt_t;

void trn_init(rt_t *rt);

void trn_free(rt_t *rt);
