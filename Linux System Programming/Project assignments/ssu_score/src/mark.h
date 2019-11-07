#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "util.h"
#include "problem.h"
#include "score.h"
#include "student.h"
#include "evaluate.h"

// 프로그램 타임아웃 초
#define PROGRAM_TIMEOUT_SEC 5
// 워닝 감점 점수
#define PROGRAM_WARNING_DEMERIT 0.1
// 에러 발생했을 시 점수
#define PROGRAM_ERROR_SCORE 0

// 정답 셋
typedef struct _answer_set_type {
  int size;
  char *answer[100];
} answer_set_type;

// 프로그램 실행 타입
typedef enum _run_program_status {
  PROGRAM_SUCCESS,
  PROGRAM_NOT_EXIST,
  PROGRAM_COMPILE_ERROR,
  PROGRAM_RUNTIME_ERROR,
  PROGRAM_TIMEOUT
} run_program_status;

// 프로그램 출력
typedef struct _run_program_result_type {
  run_program_status status;
  char *output;
  char *error;
} run_program_result_type;

// 프로그램 실행 쓰레드에 전달되는  인자
typedef struct _run_program_arg_type {
  char problem_name[16];
} run_program_arg_type;

// 대기 쓰레드에 전달되는 인자
typedef struct _wait_program_arg_type {
  pthread_t *tid;
} wait_program_arg_type;

void mark(const char *student_dir, const char *true_dir, const char *error_dir,
          const problem_set_type *problem_set,
          const score_table_type *score_table, student_set_type *student_set,
          const char qnames[5][16], const int qname_count,
          bool print_score); // 문제 채점

char *init_answer_code(const char *true_dir, const problem_type *problem,
                       const bool use_lpthread); // 정답 코드 초기화

double mark_text(const char *student_dir, const char *student_id,
                 const problem_type *problem, const char *ans,
                 const double problem_score); // 텍스트 채점

run_program_result_type mark_code(const char *student_dir,
                                  const char *student_id,
                                  const problem_type *problem,
                                  const char *error_dir,
                                  const bool use_lpthread); // 코드 채점

void *run_program(void *arg);  // 프로그램 실행
void *wait_program(void *arg); // 프로그램 5초 대기후 run_program 캔슬
