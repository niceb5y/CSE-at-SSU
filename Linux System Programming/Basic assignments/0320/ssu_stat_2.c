#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

struct stat statbuf;

void ssu_checkfile(char *fname, time_t *time);
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  time_t intertime;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    exit(1);
  }

  if (stat(argv[1], &statbuf) < 0) {
    fprintf(stderr, "stat error for %s\n", argv[1]);
    exit(1);
  }

  gettimeofday(&end_t, NULL);
  ssu_runtime(&begin_t, &end_t);

  intertime = statbuf.st_mtime;
  while (1) {
    ssu_checkfile(argv[1], &intertime);
    sleep(10);
  }
}

void ssu_checkfile(char *fname, time_t *time)
{
  if (stat(fname, &statbuf) < 0) {
    fprintf(stderr, "Warning : ssu_checkfile() error!\n");
    exit(1);
  } else if (statbuf.st_mtime != *time) {
    printf("Warning %s was modified!.\n", fname);
    *time = statbuf.st_mtime;
  }
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
