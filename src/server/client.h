/* $Id: client.h,v 1.4 2001/04/06 17:15:50 glop Exp $ */

#ifndef __PROLO_CLIENT_H__
#define __PROLO_CLIENT_H__

void	set_default_action_r4d2(int r4d2_id);
void	set_default_action_akx(int akx_id);

int	count_akx(int team_id);
int	count_r4d2(int team_id);

#endif
