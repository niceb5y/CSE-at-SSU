#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000
#define BUFFER_SIZE 1024

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  char buf[BUFFER_SIZE];
  FILE *fp;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    exit(1);
  }

  if ((fp = fopen(argv[1], "w+")) == NULL) {
    fprintf(stderr, "fopen error for %s\n", argv[1]);
    exit(1);
  }

  fputs("Input String >> ", stdout);
  gets(buf);
  fputs(buf, fp);
  rewind(fp);
  fgets(buf, sizeof(buf), fp);
  puts(buf);
  fclose(fp);

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
