#include "util.h"

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
  end_t->tv_sec -= begin_t->tv_sec;
  if (end_t->tv_usec < begin_t->tv_usec) {
    end_t->tv_sec--;
    end_t->tv_usec += SECOND_TO_MICRO;
  }
  end_t->tv_usec -= begin_t->tv_usec;
  printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}

char *get_file_data(const char *pathname)
{
  // 파일 크기만큼 메모리를 동적 할당해 파일을 전부 가져온다.
  if (!file_exists(pathname)) {
    return NULL;
  }

  int fd;
  if ((fd = open(pathname, O_RDONLY)) < 0) {
    fatal_error_print("file open error for %s", pathname);
  }

  off_t fsize;
  if ((fsize = lseek(fd, 0, SEEK_END)) < 0) {
    fatal_error_print("lseek error");
  }

  char *buf = (char *)malloc(fsize + 1);

  if (lseek(fd, 0, SEEK_SET) < 0) {
    fatal_error_print("lseek error");
  }

  ssize_t len;

  if ((len = read(fd, buf, fsize)) < 0) {
    fatal_error_print("file read error for %s", pathname);
  }

  close(fd);

  buf[len] = 0;

  if (strlen(buf) == 0) {
    return NULL;
  }

  return buf;
}

bool file_exists(const char *pathname) { return access(pathname, F_OK) == 0; }

bool is_directory(const char *pathname)
{
  struct stat statbuf;
  if (stat(pathname, &statbuf) != 0)
    return false;
  return S_ISDIR(statbuf.st_mode);
}

bool string_ends_with(const char *str, const char *suffix)
{
  const int len_diff = (int)strlen(str) - (int)strlen(suffix);
  return (len_diff >= 0) && (strcmp(str + len_diff, suffix) == 0);
}

int string_count(const char *str, const char *substr)
{
  int count = 0;
  const char *tmp = str;
  while ((tmp = strstr(tmp, substr)) != NULL) {
    count++;
    tmp++;
  }
  return count;
}

char *string_trim(const char *str)
{
  // 문자열을 복제하여 양 쪽의 공백을 제거하여 반환한다.
  char *tmp = strdup(str);
  char *cur = tmp;
  while (*cur == ' ')
    cur++;
  if (*tmp == 0) {
    free(tmp);
    tmp = NULL;
    return NULL;
  }
  char *end = cur + strlen(cur) - 1;
  while (end > str && *end == ' ')
    end--;
  end[1] = '\0';
  char *ret = strdup(cur);
  free(tmp);
  tmp = NULL;
  return ret;
}

char *string_normalize_program_output(const char *str)
{
  char *ret = (char *)malloc(strlen(str) + 1);
  size_t i, j;
  for (i = 0, j = 0; i < strlen(str); i++, j++) {
    if (str[i] != ' ')
      ret[j] = str[i];
    else
      j--;
  }
  ret[j] = '\0';
  return ret;
}

bool string_compare_program_output(const char *str1, const char *str2)
{
  if (str1 == NULL || str2 == NULL)
    return 0;
  char *s1 = string_normalize_program_output(str1),
       *s2 = string_normalize_program_output(str2);
  bool isEqual = (strcasecmp(s1, s2) == 0);
  free(s1);
  free(s2);
  return isEqual;
}

void fatal_error_print(const char *format, ...)
{
  fprintf(stderr, "error: ");
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(1);
}
