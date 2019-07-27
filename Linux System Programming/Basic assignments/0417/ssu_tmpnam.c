#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000
#define MAX_LINE 4096

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  char buf[MAX_LINE];
  char name[L_tmpnam];
  FILE *fp;

  printf("temp file 1 : %s\n", tmpnam(NULL));
  tmpnam(name);
  printf("temp file 2 : %s\n", name);

  if ((fp = tmpfile()) == NULL) {
    fprintf(stderr, "tmpfile error\n");
    exit(1);
  }

  fputs("tmpfile created temporary file.\n", fp);
  fseek(fp, 0, SEEK_SET);

  if (fgets(buf, sizeof(buf), fp) == NULL) {
    fprintf(stderr, "fgets error\n");
    exit(1);
  }

  fputs(buf, stdout);

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
