#ifndef THE_PULSAR_IPC_H
#define THE_PULSAR_IPC_H

int get_shm_conf_id();

int get_shm_world_id();

int get_shm_plr_id();

void *shm_alloc(int size, int id);

void clean_shm();

#endif
