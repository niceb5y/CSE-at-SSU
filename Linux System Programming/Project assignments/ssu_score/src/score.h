#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "util.h"
#include "problem.h"

// 점수 타입의 열
typedef struct _score_row_type {
  char name[16];
  double score;
} score_row_type;

// 점수 타입
typedef struct _score_type {
  int size;
  score_row_type row[100];
} score_type;

// 스코어 테이블 타입의 열
typedef struct _score_table_row_type {
  ENUM_PROBLEM_TYPE type;
  char name[16];
  double score;
} score_table_row_type;

// 스코어 테이블 타입
typedef struct _score_table_type {
  int size;
  score_table_row_type row[100];
} score_table_type;

score_table_type *
read_score_table(const char *score_table_path); // 스코어 테이블을 읽는다.
void write_score_table(
    const char *score_table_path,
    const score_table_type *score_table); // 스코어 테이블을 기록한다.
score_table_type *get_score_table(
    const char *true_dir,
    const problem_set_type *problem_set); //스코어 테이블을 가져온다.
