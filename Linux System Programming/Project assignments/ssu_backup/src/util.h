#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <utime.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

// 표준 입력 함수
void getln(char *buf);

// 문자열이 특정 문자열로 시작하는지 검사
bool string_starts_with(const char *str, const char *prefix);

// 두 파일 비교
int file_compare(const char *file1, const char *file2);
// 파일 복사
int file_copy(const char *src, const char *dest);

// 파일의 절대 경로를 구함
void path_normalize(const char *path, char *normalized_path);
// 경로에서 파일 이름을 구함
char *path_get_filename(const char *path);
