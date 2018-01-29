/* $Id: time.c,v 1.7 2001/05/06 07:23:34 kilobug Exp $ */

#include <signal.h>
#include <sys/time.h>
#include "server.h"

void timer_timeout(int foo)
{
  printf("%d: timeout.\n", gl_player->team_id);
  fflush(stdout);
  kill(getpid(), SIGKILL);
}

int time_left()
{
  struct itimerval it;

  getitimer(ITIMER_PROF, &it);
  return it.it_value.tv_sec * 1000 + it.it_value.tv_usec / 1000;
}

void start_timer()
{
  struct itimerval it;

  it.it_value.tv_sec = gl_config->turn_msec / 1000;
  it.it_value.tv_usec = (gl_config->turn_msec % 1000) * 1000;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 0;
  signal(SIGPROF, timer_timeout);
  setitimer(ITIMER_PROF, &it, NULL);
}

void stop_timer()
{
  struct itimerval it;

  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 0;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 0;
  setitimer(ITIMER_PROF, &it, NULL);
  signal(SIGPROF, SIG_IGN);
}
