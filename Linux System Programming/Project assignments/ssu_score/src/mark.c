#include "mark.h"

void mark(const char *student_dir, const char *true_dir, const char *error_dir,
          const problem_set_type *problem_set,
          const score_table_type *score_table, student_set_type *student_set,
          const char qnames[5][16], const int qname_count, bool print_score)
{
  double avg_score = 0;
  printf("initiating...");
  fflush(stdout);

  answer_set_type answer_set;
  answer_set.size = problem_set->size;

  if (error_dir != NULL) {
    if (!file_exists(error_dir)) {
      if (mkdir(error_dir, 0755) < 0) {
        fatal_error_print("mkdir error for %s", error_dir);
      }
    }
  }

  // 문제 순회
  for (int i = 0; i < problem_set->size; ++i) {
    const problem_type *problem = &(problem_set->problem[i]);
    if (problem->type == PROBLEM_CODE) {
      bool use_lpthread = false;
      for (int k = 0; k < qname_count; ++k) {
        if (strcmp(problem->name, qnames[k]) == 0) {
          use_lpthread = true;
          break;
        }
      }
      // 정답 컴파일
      answer_set.answer[i] = init_answer_code(true_dir, problem, use_lpthread);
    } else {
      char ans_path[PATH_MAX];
      sprintf(ans_path, "%s/%s/%s.txt", true_dir, problem->name, problem->name);
      answer_set.answer[i] = get_file_data(ans_path);
    }
  }

  printf(" finished..\n");
  printf("grading student's test papers..\n");

  // 학생 채점
  for (int i = 0; i < student_set->size; ++i) {
    student_type *const student = &(student_set->students[i]);
    printf("%s", student->id);
    fflush(stdout);
    if (error_dir != NULL) {
      char student_error_dir[PATH_MAX];
      sprintf(student_error_dir, "%s/%s", error_dir, student->id);
      if (!file_exists(student_error_dir)) {
        if (mkdir(student_error_dir, 0755) < 0) {
          fatal_error_print("mkdir error for %s", student_error_dir);
        }
      }
    }

    for (int j = 0; j < problem_set->size; ++j) {
      const char *answer = answer_set.answer[j];
      const problem_type *problem = &(problem_set->problem[j]);
      const double problem_score = score_table->row[j].score;
      double *const student_score = &(student->score.row[j].score);
      // 텍스트인 경우
      if (problem->type == PROBLEM_TEXT) {
        *student_score =
            mark_text(student_dir, student->id, problem, answer, problem_score);
      }
      // 코드인 경우
      else if (problem->type == PROBLEM_CODE) {
        bool use_lpthread = false;
        for (int k = 0; k < qname_count; ++k) {
          if (strcmp(problem->name, qnames[k]) == 0) {
            use_lpthread = true;
            break;
          }
        }
        *student_score = PROGRAM_ERROR_SCORE;
        run_program_result_type program_result = mark_code(
            student_dir, student->id, problem, error_dir, use_lpthread);
        if (program_result.status == PROGRAM_SUCCESS) {
          if (string_compare_program_output(answer, program_result.output)) {
            *student_score = problem_score;
          }
        }
        if (program_result.error != NULL) {
          *student_score -= string_count(program_result.error, "warning:") *
                            PROGRAM_WARNING_DEMERIT;
        }
        if (*student_score < 0) {
          *student_score = 0;
        }
        if (program_result.error != NULL) {
          free(program_result.error);
          program_result.error = NULL;
        }
        if (program_result.output != NULL) {
          free(program_result.output);
          program_result.output = NULL;
        }
      } else {
        fatal_error_print("data is not valid.");
      }
    }
    printf(" is finished..");
    if (print_score) {
      double score = 0;
      for (int j = 0; j < student->score.size; ++j) {
        score += student->score.row[j].score;
      }
      avg_score += score;
      printf(" score : %.2lf", score);
    }
    printf("\n");
  }
  avg_score /= student_set->size;
  printf("Total average : %.2lf\n", avg_score);
}

char *init_answer_code(const char *true_dir, const problem_type *problem,
                       const bool use_lpthread)
{
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  char ans_path[PATH_MAX];
  sprintf(ans_path, "%s/%s", true_dir, problem->name);
  if (chdir(ans_path) == -1) {
    fatal_error_print("chdir error for %s", ans_path);
  }

  char answer_path[PATH_MAX];

  sprintf(answer_path, "%s.stdout", problem->name);
  if (!file_exists(answer_path)) {
    int stderr_backup = dup(2);
    int fd_err = open("/dev/null", O_WRONLY, 0664);
    if (fd_err < 0) {
      fatal_error_print("file open error for /dev/null");
    }
    dup2(fd_err, 2);

    int stdout_backup = dup(1);
    int fd_out = open("/dev/null", O_WRONLY, 0664);
    if (fd_out < 0) {
      fatal_error_print("file open error for /dev/null");
    }
    dup2(fd_out, 1);

    char cmd[LINE_MAX];
    sprintf(cmd, "gcc %s.c -o %s.exe %s", problem->name, problem->name,
            use_lpthread ? "-lpthread" : "");
    system(cmd);

    dup2(stdout_backup, 1);
    close(fd_out);

    fd_out = open(answer_path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd_out < 0) {
      fatal_error_print("file open error for %s", answer_path);
    }
    dup2(fd_out, 1);

    sprintf(cmd, "./%s.exe", problem->name);
    system(cmd);

    dup2(stdout_backup, 1);
    close(stdout_backup);
    close(fd_out);

    dup2(stderr_backup, 2);
    close(stderr_backup);
    close(fd_err);
  }
  char *ret = get_file_data(answer_path);
  chdir(cwd);
  return ret;
}

double mark_text(const char *student_dir, const char *student_id,
                 const problem_type *problem, const char *ans,
                 const double problem_score)
{
  char student_ans_path[PATH_MAX];
  sprintf(student_ans_path, "%s/%s/%s.txt", student_dir, student_id,
          problem->name);

  char *student_ans = get_file_data(student_ans_path);
  if (student_ans == NULL) {
    return 0;
  }

  char *ans_dup = strdup(ans);
  char *lasts;
  char *token = strtok_r(ans_dup, ":", &lasts);
  char *student_answer = string_trim(student_ans);
  while (token != NULL) {
    char *answer = string_trim(token);
    if (strcmp(student_answer, answer) == 0) {
      return problem_score;
    }
    if (compare_code(student_answer, answer)) {
      free(ans_dup);
      ans_dup = NULL;
      return problem_score;
    }
    free(answer);
    answer = NULL;
    token = strtok_r(NULL, ":", &lasts);
  }
  free(student_answer);
  student_answer = NULL;
  free(ans_dup);
  ans_dup = NULL;
  return 0;
}

run_program_result_type mark_code(const char *student_dir,
                                  const char *student_id,
                                  const problem_type *problem,
                                  const char *error_dir,
                                  const bool use_lpthread)
{
  run_program_result_type ret = {PROGRAM_SUCCESS, NULL, NULL};

  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  char error_out_path[PATH_MAX] = "/dev/null";
  sprintf(error_out_path, "%s/%s/%s_error.txt", error_dir, student_id,
          problem->name);

  char problem_path[PATH_MAX];
  sprintf(problem_path, "%s/%s", student_dir, student_id);
  if (chdir(problem_path) == -1) {
    fatal_error_print("chdir error for %s", problem_path);
  }
  // 코드 경로
  char source_path[PATH_MAX];
  sprintf(source_path, "%s.c", problem->name);
  // 출력 경로
  char stdout_path[PATH_MAX];
  sprintf(stdout_path, "%s.stdout", problem->name);
  // 바이너리 경로
  char stdexe_path[PATH_MAX];
  sprintf(stdexe_path, "%s.stdexe", problem->name);
  // 에러 경로
  char stderr_path[PATH_MAX];
  sprintf(stderr_path, "%s.stderr", problem->name);

  if (!file_exists(source_path)) {
    chdir(cwd);
    ret.status = PROGRAM_NOT_EXIST;
    return ret;
  }

  int stderr_backup = dup(2);
  int fd = open(stderr_path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
  if (fd < 0) {
    fatal_error_print("file open error for &s", stderr_path);
  }
  dup2(fd, 2);

  char cmd[LINE_MAX];
  sprintf(cmd, "gcc %s.c -o %s.stdexe %s", problem->name, problem->name,
          use_lpthread ? "-lpthread" : "");
  system(cmd);

  dup2(stderr_backup, 2);
  close(stderr_backup);
  close(fd);

  if (file_exists(stdexe_path)) {
    int stdout_backup = dup(1);
    int fd = open(stdout_path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd < 0) {
      fatal_error_print("file open error for &s", stdout_path);
    }
    dup2(fd, 1);

    int stderr_backup = dup(2);
    int fd_err = open("/dev/null", O_WRONLY, 0664);
    if (fd_err < 0) {
      fatal_error_print("file open error for /dev/null");
    }
    dup2(fd_err, 2);

    // 실행 쓰레드
    pthread_t program_thread;
    run_program_arg_type run_arg;
    strcpy(run_arg.problem_name, problem->name);
    run_program_status *program_status = NULL;
    pthread_create(&program_thread, NULL, run_program, &run_arg);

    // 대기 쓰레드
    pthread_t wait_thread;
    wait_program_arg_type wait_arg;
    wait_arg.tid = &program_thread;
    pthread_create(&wait_thread, NULL, wait_program, &wait_arg);

    pthread_join(program_thread, (void **)&program_status);
    if (pthread_kill(wait_thread, 0) == 0) {
      pthread_cancel(wait_thread);
    }
    pthread_join(wait_thread, NULL);

    dup2(stderr_backup, 2);
    close(stderr_backup);
    close(fd_err);

    dup2(stdout_backup, 1);
    close(stdout_backup);
    close(fd);

    if (program_status != PTHREAD_CANCELED) {
      ret.status = *program_status;
      free(program_status);
    } else {
      // 제한 시간 경과
      ret.status = PROGRAM_TIMEOUT;
      system("pkill -9 stdexe");
    }
  } else {
    ret.status = PROGRAM_COMPILE_ERROR;
  }
  // 출력, 워닝 리턴
  ret.output = get_file_data(stdout_path);
  ret.error = get_file_data(stderr_path);
  remove(stderr_path);
  chdir(cwd);
  if (error_dir != NULL) {
    if (file_exists(error_out_path)) {
      remove(error_out_path);
    }
    if (ret.error != NULL) {
      int fd_err;
      if ((fd_err = open(error_out_path, O_WRONLY | O_CREAT | O_TRUNC, 0666)) <
          0) {
        fatal_error_print("open error for %s", error_out_path);
      }
      write(fd_err, ret.error, strlen(ret.error));
      close(fd_err);
    }
  }
  return ret;
}

void *run_program(void *arg)
{
  run_program_status *ret = malloc(sizeof(run_program_status));
  *ret = PROGRAM_SUCCESS;
  const char *problem_name = ((run_program_arg_type *)arg)->problem_name;
  char cmd[LINE_MAX];
  sprintf(cmd, "./%s.stdexe", problem_name);
  if (system(cmd) != 0) {
    *ret = PROGRAM_RUNTIME_ERROR;
  }
  return ret;
}

void *wait_program(void *arg)
{
  pthread_t tid = *(((wait_program_arg_type *)arg)->tid);
  sleep(PROGRAM_TIMEOUT_SEC);
  pthread_cancel(tid);

  return NULL;
}
