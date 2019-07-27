#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  char *arg_directory = NULL;
  char *arg_prefix = NULL;

  if (argc != 3) {
    fprintf(stderr, "usage %s <directory> <prefix>\n", argv[0]);
    exit(1);
  }

  arg_directory = argv[1][0] != ' ' ? argv[1] : NULL;
  arg_prefix = argv[2][0] != ' ' ? argv[2] : NULL;
  printf("created : %s\n", tempnam(arg_directory, arg_prefix));

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
