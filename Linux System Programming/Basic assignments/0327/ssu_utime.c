#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  struct utimbuf time_buf;
  struct stat statbuf;
  int fd;
  int i;

  for (i = 1; i < argc; i++) {
    if (stat(argv[i], &statbuf) < 0) {
      fprintf(stderr, "stat error for %s\n", argv[i]);
      continue;
    }

    if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
      fprintf(stderr, "open error for %s\n", argv[i]);
      continue;
    }

    close(fd);
    time_buf.actime = statbuf.st_atime;
    time_buf.modtime = statbuf.st_mtime;

    if (utime(argv[i], &time_buf) < 0) {
      fprintf(stderr, "utime error for %s\n", argv[i]);
      continue;
    }
  }
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
