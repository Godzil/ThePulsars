#include <stdio.h>
#include <unistd.h>

static int to_philix()
{
   printf("Bravo, tu as gagne un tour de patio!\n");
   fflush(stdout);
   _exit(1);
}

int system()  { to_philix(); return 42; }
int execl()   { to_philix(); return 42; }
int execlp()  { to_philix(); return 42; }
int execv()   { to_philix(); return 42; }
int execve()  { to_philix(); return 42; }
int execvp()  { to_philix(); return 42; }
int connect() { to_philix(); return 42; }
int vfork()   { to_philix(); return 42; }
int unlink()  { to_philix(); return 42; }
int pause()   { to_philix(); return 42; }
int alarm()   { to_philix(); return 42; }
int patio()   { to_philix(); return 42; }

