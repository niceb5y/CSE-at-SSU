#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define EXIT_CODE 1
#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  pid_t pid;
  int ret_val, status;

  if ((pid = fork()) == 0) {
    printf("child: pid = %d ppid = %d exit_code = %d\n", getpid(), getppid(),
           EXIT_CODE);
    exit(EXIT_CODE);
  }

  printf("parent: waiting for child = %d\n", pid);
  ret_val = wait(&status);
  printf("parent: return value = %d, ", ret_val);
  printf(" child's status = %x", status);
  printf(" and shifted = %x\n", (status >> 8));

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
