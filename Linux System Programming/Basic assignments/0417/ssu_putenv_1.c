#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_addone(void);
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

extern char **environ;
char glob_var[] = "HOBBY=swimming";

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  int i;
  for (i = 0; environ[i] != NULL; i++) {
    printf("environ[%d] : %s\n", i, environ[i]);
  }

  putenv(glob_var);
  ssu_addone();
  printf("My hobby is %s\n", getenv("HOBBY"));
  printf("My lover is %s\n", getenv("LOVER"));
  strcpy(glob_var + 6, "fishing");

  for (i = 0; environ[i] != NULL; i++) {
    printf("environ[%d] : %s\n", i, environ[i]);
  }

  gettimeofday(&end_t, NULL);
  ssu_runtime(&begin_t, &end_t);

  exit(0);
}

void ssu_addone(void)
{
  char auto_var[10];

  strcpy(auto_var, "LOVER=js");
  putenv(auto_var);
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
