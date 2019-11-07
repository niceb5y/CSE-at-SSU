#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include "util.h"

// 문제의 종류
typedef enum _ENUM_PROBLEM_TYPE {
  PROBLEM_TEXT,
  PROBLEM_CODE
} ENUM_PROBLEM_TYPE;

// 문제 타입
typedef struct _problem_type {
  ENUM_PROBLEM_TYPE type;
  char name[16];
} problem_type;

// 문제 셋 타입
typedef struct _problem_set_type {
  int size;
  problem_type problem[100];
} problem_set_type;

ENUM_PROBLEM_TYPE get_problem_type(
    const char *problem_name); // 디렉토리에서 문제의 타입을 알아낸다.
problem_set_type *
get_problem_set(const char *ans_dir_name); // 문제 세트를 가져온다.
