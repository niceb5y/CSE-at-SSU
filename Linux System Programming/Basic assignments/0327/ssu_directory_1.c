#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000
#define DIRECTORY_SIZE MAXNAMLEN

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  struct dirent *dentry;
  struct stat statbuf;
  char filename[DIRECTORY_SIZE + 1];
  DIR *dirp;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <directory>\n", argv[0]);
    exit(1);
  }

  if ((dirp = opendir(argv[1])) == NULL || chdir(argv[1]) == -1) {
    fprintf(stderr, "opendir, chdir error for %s\n", argv[1]);
    exit(1);
  }

  while ((dentry = readdir(dirp)) != NULL) {
    if (dentry->d_ino == 0)
      continue;

    memcpy(filename, dentry->d_name, DIRECTORY_SIZE);

    if (stat(filename, &statbuf) == -1) {
      fprintf(stderr, "stat error for %s\n", filename);
      break;
    }

    if ((statbuf.st_mode & S_IFMT) == S_IFREG)
      printf("%-14s %lld\n", filename, statbuf.st_size);
    else
      printf("%-14s\n", filename);
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
