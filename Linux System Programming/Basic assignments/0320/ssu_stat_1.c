#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  struct stat statbuf;

  if (argc != 2) {
    fprintf(stderr, "usage %s <file>\n", argv[0]);
    exit(1);
  }

  if ((stat(argv[1], &statbuf)) < 0) {
    fprintf(stderr, "stat error\n");
    exit(1);
  }

  printf("%s is %lld bytes\n", argv[1], statbuf.st_size);

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
