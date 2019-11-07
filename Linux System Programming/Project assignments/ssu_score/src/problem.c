#include "problem.h"

ENUM_PROBLEM_TYPE get_problem_type(const char *problem_name)
{
  char answer_code[32], answer_text[32];
  sprintf(answer_code, "%s/%s.c", problem_name, problem_name);
  sprintf(answer_text, "%s/%s.txt", problem_name, problem_name);
  if (file_exists(answer_code)) {
    return PROBLEM_CODE;
  } else if (file_exists(answer_text)) {
    return PROBLEM_TEXT;
  } else {
    fatal_error_print("answerset not valid");
  }
  return PROBLEM_TEXT;
}

problem_set_type *get_problem_set(const char *ans_dir_name)
{
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  if (chdir(ans_dir_name) == -1) {
    fatal_error_print("cannot open TRUEDIR(%s)", ans_dir_name);
  }

  problem_set_type *problem_set =
      (problem_set_type *)malloc(sizeof(problem_set_type));

  problem_set->size = 0;

  // 디렉토리를 순회하며 디렉토리 존재 여부를 통해 문제 정보를 가져온다.
  for (int i = 1; i <= 100; ++i) {
    char pathname[16];
    sprintf(pathname, "%d", i);
    if (file_exists(pathname)) {
      problem_set->size += 1;
      problem_set->problem[problem_set->size - 1].type =
          get_problem_type(pathname);
      strcpy(problem_set->problem[problem_set->size - 1].name, pathname);
    } else {
      bool dir_exists = false;
      for (int j = 1; j <= 100; ++j) {
        sprintf(pathname, "%d-%d", i, j);
        if (file_exists(pathname)) {
          problem_set->size += 1;
          problem_set->problem[problem_set->size - 1].type =
              get_problem_type(pathname);
          strcpy(problem_set->problem[problem_set->size - 1].name, pathname);
          dir_exists = true;
        }
      }
      if (!dir_exists)
        break;
    }
  }

  chdir(cwd);

  return problem_set;
}
