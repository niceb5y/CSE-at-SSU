#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

char glob_str[] = "write to standard output\n";
int glob_val = 10;

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  pid_t pid;
  int loc_val;

  loc_val = 100;
  if (write(STDOUT_FILENO, glob_str, sizeof(glob_str) - 1) !=
      sizeof(glob_str) - 1) {
    fprintf(stderr, "write error\n");
    exit(1);
  }

  printf("before fork\n");

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    exit(1);
  }
  else if (pid == 0) {
    glob_val++;
    loc_val++;
  }
  else
    sleep(3);

  printf("pid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val,
         loc_val);

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
