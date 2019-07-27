#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_echo_exit(int status);
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  pid_t pid;
  int status;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    exit(1);
  }
  else if (pid == 0)
    exit(7);

  if (wait(&status) != pid) {
    fprintf(stderr, "wait error\n");
    exit(1);
  }

  ssu_echo_exit(status);

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    exit(1);
  }
  else if (pid == 0)
    abort();

  if (wait(&status) != pid) {
    fprintf(stderr, "wait error \n");
    exit(1);
  }

  ssu_echo_exit(status);

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    exit(1);
  }
  else if (pid == 0)
    status /= 0;

  if (wait(&status) != pid) {
    fprintf(stderr, "wait error\n");
    exit(1);
  }

  ssu_echo_exit(status);

  gettimeofday(&end_t, NULL);
  ssu_runtime(&begin_t, &end_t);

  exit(0);
}

void ssu_echo_exit(status)
{
  if (WIFEXITED(status))
    printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
  else if (WIFSIGNALED(status))
    printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
#if WCOREDUMP
           WCOREDUMP(status) ? " (core file genearated)" : "");
#else
           "");
#endif
  else if (WIFSTOPPED(status))
    printf("child stopped, signal number =%d\n", WSTOPSIG(status));
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
