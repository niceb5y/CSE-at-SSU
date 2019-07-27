#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

struct ssu_pirate {
  unsigned long booty;
  unsigned int beard_length;
  char name[128];
};

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  struct ssu_pirate blackbeard = {950, 48, "Edward Teach"}, pirate;
  char *fname = "ssu_data";
  FILE *fp1, *fp2;

  if ((fp2 = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "fopen error for %s\n", fname);
    exit(1);
  }

  if (fwrite(&blackbeard, sizeof(struct ssu_pirate), 1, fp2) != 1) {
    fprintf(stderr, "fwrite error\n");
    exit(1);
  }

  if (fclose(fp2)) {
    fprintf(stderr, "fclose error\n");
    exit(1);
  }

  if ((fp1 = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "fopen error\n");
    exit(1);
  }

  if (fread(&pirate, sizeof(struct ssu_pirate), 1, fp1) != 1) {
    fprintf(stderr, "fread error\n");
    exit(1);
  }

  if (fclose(fp1)) {
    fprintf(stderr, "fclose error\n");
    exit(1);
  }

  printf("name=\"%s\" booty=%lu beard_length=%u\n", pirate.name, pirate.booty,
         pirate.beard_length);

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
