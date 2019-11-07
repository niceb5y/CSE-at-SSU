#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "score.h"
#include "score.h"

#define STUDENT_MAX 100

typedef struct _student_type {
  char id[16];
  score_type score;
} student_type;

typedef struct _student_set_type {
  int size;
  student_type students[STUDENT_MAX];
} student_set_type;

int student_compare(const void *s1,
                    const void *s2); //두 학생의 학번을 비교한다.
student_set_type *
get_student_set(const char *student_dir_name,
                const problem_set_type *problem_set); // 학생 정보를 가져온다.
void write_student_score(
    const problem_set_type *problem_set,
    const student_set_type *student_set); // score.csv를 기록한다.
