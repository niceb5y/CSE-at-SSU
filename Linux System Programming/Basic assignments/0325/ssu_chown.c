#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  struct stat statbuf;
  char *fname = "ssu_myfile";
  int fd;

  if ((fd = open(fname, O_RDWR | O_CREAT, 0600)) < 0) {
    fprintf(stderr, "open error for %s\n", fname);
    exit(1);
  }

  close(fd);
  stat(fname, &statbuf);
  printf("# 1st stat call # UID:%d    GID:%d\n", statbuf.st_uid,
         statbuf.st_gid);

  if (chown(fname, 501, 300) < 0) {
    fprintf(stderr, "chown error for %s\n", fname);
    exit(1);
  }

  stat(fname, &statbuf);
  printf("# 2nd stat call # UID:%d    GID:%d\n", statbuf.st_uid,
         statbuf.st_gid);

  if (unlink(fname) < 0) {
    fprintf(stderr, "unlink error for %s\n", fname);
    exit(1);
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
