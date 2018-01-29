/* $Id: ipc.h,v 1.6 2001/04/29 22:07:44 kilobug Exp $ */

#ifndef __PROLO_IPC_H__
#define __PROLO_IPC_H__

int get_shm_conf_id();
int get_shm_world_id();
int get_shm_plr_id();
void *shm_alloc(int size, int id);
void clean_shm();

#endif
