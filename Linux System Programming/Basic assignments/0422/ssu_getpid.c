#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  printf("Process ID          =%d\n", getpid());
  printf("Parent Process ID   =%d\n", getppid());
  printf("Real User ID        =%d\n", getuid());
  printf("Effective User ID   =%d\n", geteuid());
  printf("Real group ID       =%d\n", getgid());
  printf("Effective group ID  =%d\n", getegid());

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
