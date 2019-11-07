#ifndef SSU_CONVERT_UTIL_H
#define SSU_CONVERT_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

/*
 * 출력 관련 함수
 */
// 에러 메시지를 출력하고 종료한다.
void print_fatal_error(const char *format, ...);
// 코드를 출력한다.
void print_code(const char *title, const char *code, int line_limit);
// 프로그램의 실행 시간을 출력한다.
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

/*
 * 파일 관련 함수
 */
// 파일이 존재하는지 확인한다.
bool file_exists(const char *pathname);
// 파일을 오픈하여 읽어들인다.
char *file_get_data(const char *pathname);
// 파일의 라인 수를 가져온다.
int file_get_line(const char *pathname);
// 파일의 크기를 가져온다.
off_t file_get_size(const char *pathname);
// 코드 파일을 읽어들여서 출력한다.
void file_print_code(const char *title, const char *pathname);

/*
 * 문자열 관련 함수
 */
// 문자열 좌우의 공백을 제거하여 반환한다.
char *string_trim(const char *str);
// 문자열이 특정 문자열로 끝나는지 확인한다.
bool string_ends_with(const char *str, const char *suffix);

#endif
