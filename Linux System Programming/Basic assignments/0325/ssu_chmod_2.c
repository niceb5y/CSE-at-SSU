#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MODE_EXEC (S_IXUSR | S_IXGRP | S_IXOTH)
#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  struct stat statbuf;
  int i;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
    exit(1);
  }

  for (i = 1; i < argc; i++) {
    if (stat(argv[i], &statbuf) < 0) {
      fprintf(stderr, "%s : stat error\n", argv[i]);
      continue;
    }

    statbuf.st_mode |= MODE_EXEC;
    statbuf.st_mode ^= (S_IXGRP | S_IXOTH);
    if (chmod(argv[i], statbuf.st_mode) < 0)
      fprintf(stderr, "%s : chmod error\n", argv[i]);
    else
      printf("%s : file permission was changed.\n", argv[i]);
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
