#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  if (fork() == 0)
    execl("/bin/echo", "echo", "this is", "message one", (char *)0);

  if (fork() == 0)
    execl("/bin/echo", "echo", "this is", "message Two", (char *)0);

  printf("parent:  waiting for children\n");

  while (wait((int *)0) != -1)
    ;

  printf("parent: all children terminated\n");

  gettimeofday(&end_t, NULL);
  ssu_runtime(&begin_t, &end_t);

  exit(0);
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
  end_t->tv_sec -= begin_t->tv_sec;

  if (end_t->tv_usec < begin_t->tv_usec) {
    end_t->tv_sec--;
    end_t->tv_usec += SECOND_TO_MICRO;
  }

  end_t->tv_usec -= begin_t->tv_usec;
  printf("Runtime: %ld:%06d(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}
