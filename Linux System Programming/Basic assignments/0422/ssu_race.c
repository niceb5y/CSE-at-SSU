#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

static void ssu_charactatime(char *str);
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  pid_t pid;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    exit(1);
  } else if (pid == 0)
    ssu_charactatime("output from child\n");
  else
    ssu_charactatime("output from parent\n");

  gettimeofday(&end_t, NULL);
  ssu_runtime(&begin_t, &end_t);

  exit(0);
}

static void ssu_charactatime(char *str)
{
  char *ptr;
  int print_char;

  setbuf(stdout, NULL);

  for (ptr = str; (print_char = *ptr++) != 0;) {
    putc(print_char, stdout);
    usleep(10);
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
