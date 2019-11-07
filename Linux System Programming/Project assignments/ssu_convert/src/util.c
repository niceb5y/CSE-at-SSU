#include "util.h"

/*
 * 출력 관련 함수
 */

// 에러 메시지를 출력하고 종료한다.
void print_fatal_error(const char *format, ...)
{
  fprintf(stderr, "\033[0;31m");
  fprintf(stderr, "error: ");
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(stderr, "\033[0m");
  exit(1);
}

// 코드를 출력한다.
void print_code(const char *title, const char *code, int line_limit)
{
  printf("--------\n%s\n--------\n", title);
  char *code_dup = strdup(code);
  char *line;
  char *save_ptr;
  int line_num = 0;

  line = strtok_r(code_dup, "\n", &save_ptr);

  while (line != NULL && (line_limit == 0 || line_num < line_limit)) {
    printf("%3d  %s\n", ++line_num, line);
    line = strtok_r(NULL, "\n", &save_ptr);
  }

  free(code_dup);
}

// 프로그램의 실행 시간을 출력한다.
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
  end_t->tv_sec -= begin_t->tv_sec;
  if (end_t->tv_usec < begin_t->tv_usec) {
    end_t->tv_sec--;
    end_t->tv_usec += 1000000;
  }
  end_t->tv_usec -= begin_t->tv_usec;
#ifdef __APPLE__
  printf("\nRuntime: %ld:%06d(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
#else
  printf("\nRuntime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
#endif
}

/*
 * 파일 관련 함수
 */

// 파일이 존재하는지 확인한다.
bool file_exists(const char *pathname) { return access(pathname, F_OK) == 0; }

// 파일을 오픈하여 읽어들인다.
char *file_get_data(const char *pathname)
{
  if (!file_exists(pathname)) {
    return NULL;
  }
  int fd;
  if ((fd = open(pathname, O_RDONLY)) < 0) {
    print_fatal_error("file open error for %s", pathname);
  }
  off_t fsize;
  if ((fsize = lseek(fd, 0, SEEK_END)) < 0) {
    print_fatal_error("lseek error");
  }
  char *buf = (char *)malloc(fsize + 1);
  if (lseek(fd, 0, SEEK_SET) < 0) {
    print_fatal_error("lseek error");
  }
  ssize_t len;
  if ((len = read(fd, buf, fsize)) < 0) {
    print_fatal_error("file read error for %s", pathname);
  }
  close(fd);
  buf[len] = 0;
  if (strlen(buf) == 0) {
    return NULL;
  }
  return buf;
}

// 파일의 라인 수를 가져온다.
int file_get_line(const char *pathname)
{
  char *file_src = file_get_data(pathname);
  if (file_src == NULL) {
    return -1;
  }
  int line = 0;
  for (unsigned long i = 0; i < strlen(file_src); ++i) {
    if (file_src[i] == '\n')
      ++line;
  }
  free(file_src);
  return line;
}

// 파일의 크기를 가져온다.
off_t file_get_size(const char *pathname)
{
  if (!file_exists(pathname)) {
    return -1;
  }
  int fd;
  if ((fd = open(pathname, O_RDONLY)) < 0) {
    print_fatal_error("file open error for %s", pathname);
  }
  off_t fsize;
  if ((fsize = lseek(fd, 0, SEEK_END)) < 0) {
    print_fatal_error("lseek error");
  }
  return fsize;
}

// 코드 파일을 읽어들여서 출력한다.
void file_print_code(const char *title, const char *pathname)
{
  char *file_src = file_get_data(pathname);
  if (file_src == NULL) {
    return;
  }

  print_code(title, file_src, 0);

  free(file_src);
}

/*
 * 문자열 관련 함수
 */

// 문자열 좌우의 공백을 제거하여 반환한다.
char *string_trim(const char *str)
{
  char *tmp = strdup(str);
  char *cur = tmp;
  while (*cur == ' ' || *cur == '\t')
    cur++;
  if (*tmp == 0) {
    free(tmp);
    return NULL;
  }
  char *end = cur + strlen(cur) - 1;
  while (end > str && (*end == ' ' || *end == '\t'))
    end--;
  end[1] = '\0';
  char *ret = strdup(cur);
  free(tmp);
  return ret;
}

// 문자열이 특정 문자열로 끝나는지 확인한다.
bool string_ends_with(const char *str, const char *suffix)
{
  const int len_diff = (int)strlen(str) - (int)strlen(suffix);
  return (len_diff >= 0) && (strcmp(str + len_diff, suffix) == 0);
}
