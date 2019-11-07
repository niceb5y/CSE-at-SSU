#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t,
                 struct timeval *end_t);   // 실행 시간 체크
bool file_exists(const char *pathname);    // 파일이 존재하는지 확인
char *get_file_data(const char *pathname); // 파일을 읽어서 가져온다.
bool is_directory(const char *pathname);   // 디렉토리인지 확인
bool string_ends_with(
    const char *str,
    const char *suffix); // 문자열이 다른 문자열로 끝나는지 확인
int string_count(const char *str,
                 const char *substr); // 문자열이 몇 번 등장하는지 체크
char *string_trim(const char *str); // 양쪽 공백 제거
char *
string_normalize_program_output(const char *str); // 프로그램 출력 노멀라이즈
bool string_compare_program_output(const char *str1,
                                   const char *str2); // 두 프로그램 출력 비교
void fatal_error_print(const char *format,
                       ...); // 에러가 발생했을 때 strerr출력 + 비정상 종료
