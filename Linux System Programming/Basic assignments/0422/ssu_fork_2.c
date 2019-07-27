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

  pid_t pid;
  char character, first, last;
  long i;

  if ((pid = fork()) > 0) {
    first = 'A';
    last = 'Z';
  }
  else if (pid == 0) {
    first = 'a';
    last = 'z';
  }
  else {
    fprintf(stderr, "%s\n", argv[0]);
    exit(1);
  }

  for (character = first; character <= last; character++) {
    for (i = 0; i <= 100000; i++)
      ;

    write(1, &character, 1);
  }

  printf("\n");

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
