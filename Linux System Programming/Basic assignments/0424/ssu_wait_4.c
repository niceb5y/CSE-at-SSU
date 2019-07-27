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

  pid_t child1, child2;
  int pid, status;

  if ((child1 = fork()) == 0)
    execlp("date", "date", (char *)0);

  if ((child2 = fork()) == 0)
    execlp("who", "who", (char *)0);

  printf("parent: waiting for children\n");

  while ((pid = wait(&status)) != -1) {
    if (child1 == pid)
      printf("parent: first child: %d\n", (status >> 8));
    else if (child2 == pid)
      printf("parent: second child %d\n", (status >> 8));
  }

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
