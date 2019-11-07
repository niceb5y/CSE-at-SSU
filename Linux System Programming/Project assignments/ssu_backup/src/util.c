#include "util.h"

// 표준 입력 함수
void getln(char *buf)
{
  // fgets 사용 후, 마지막 줄바꿈 문자를 제거한다.
  fgets(buf, LINE_MAX, stdin);
  buf[strlen(buf) - 1] = '\0';
}

// 문자열이 특정 문자열로 시작하는지 검사
bool string_starts_with(const char *str, const char *prefix)
{
  // str이 prefix보다 길이가 더 짧은 경우 항상 거짓
  if (strlen(str) < strlen(prefix))
    return false;
  // 이외의 경우 prefix길이만큼 비교
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

// 두 파일 비교
int file_compare(const char *file1, const char *file2)
{
  // 두 파일의 정보를 가져온다.
  struct stat statbuf1, statbuf2;
  // stat 에러 발생 시 -1 반환
  if (lstat(file1, &statbuf1) != 0 || lstat(file2, &statbuf2) != 0) {
    return -1;
  }
  // 두 파일의 크기 및 수정 시간 비교
  if (statbuf1.st_size == statbuf2.st_size &&
      statbuf1.st_mtime == statbuf2.st_mtime) {
    return 0;
  } else {
    return 1;
  }
}

// 파일 복사
int file_copy(const char *src, const char *dest)
{
  // 파일이 존재하지 않거나 권한이 없는 경우 -1 반환
  if (access(src, F_OK | R_OK) != 0)
    return -1;

  // 파일 정보를 가져온다.
  struct stat statbuf;
  lstat(src, &statbuf);

  int fd_src, fd_dest;
  char buf[4096];

  // 파일 오픈 후 에러 발생시 -1 반환
  if ((fd_src = open(src, O_RDONLY)) < 0) {
    return -1;
  }
  if ((fd_dest = open(dest, O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode)) <
      0) {
    return -1;
  }

  // 내용을 버퍼 크기만큼 읽어서 복사한다.
  ssize_t len;
  while ((len = read(fd_src, buf, sizeof(buf))) > 0) {
    write(fd_dest, buf, len);
  }

  // 파일을 닫는다.
  close(fd_src);
  close(fd_dest);

  // 시간 정보 변경
  struct utimbuf time_buf;
  time_buf.actime = statbuf.st_atime;
  time_buf.modtime = statbuf.st_mtime;
  utime(dest, &time_buf);

  // 소유권자 변경
  chown(dest, statbuf.st_uid, statbuf.st_gid);

  return 0;
}

// 파일의 절대 경로를 구함
void path_normalize(const char *path, char *normalized_path)
{
  // "~"를 경로에 포함하는 경우
  if (path[0] == '~') {
    strcpy(normalized_path, getpwuid(getuid())->pw_dir);
    strcat(normalized_path, path + 1);
  }
  // 이외의 경우 realpath를 사용하여 절대 경로를 구한다.
  else {
    realpath(path, normalized_path);
  }
}

// 경로에서 파일 이름을 구함
char *path_get_filename(const char *path)
{
  // 마지막 '/'위치를 구한다.
  char *ptr = strrchr(path, '/');
  if (ptr)
    // '/' 문자가 존재하는 경우 마지막 '/' 문자의 다음 위치 반환
    return ptr + 1;
  else
    // '/' 문자가 존재하지 않는 경우 경로 전체 반환
    return (char *)path;
}
