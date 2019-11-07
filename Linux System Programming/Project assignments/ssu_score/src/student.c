#include "student.h"

int student_compare(const void *s1, const void *s2)
{
  return strcmp(((const student_type *)s1)->id, ((const student_type *)s2)->id);
}

student_set_type *get_student_set(const char *student_dir_name,
                                  const problem_set_type *problem_set)
{
  student_set_type *student_set =
      (student_set_type *)malloc(sizeof(student_set_type));
  student_set->size = 0;

  struct dirent *dentry;
  DIR *dirp;
  char cwd[1024];
  getcwd(cwd, 1024);

  // 디렉토리를 순회하며 학생 정보를 가져온다.
  if ((dirp = opendir(student_dir_name)) == NULL ||
      chdir(student_dir_name) == -1) {
    fatal_error_print("opendir, chdir error for %s", student_dir_name);
  }
  while ((dentry = readdir(dirp)) != NULL) {
    if (dentry->d_ino == 0)
      continue;
    if (!is_directory(dentry->d_name))
      continue;
    if (strcmp(".", dentry->d_name) == 0 || strcmp("..", dentry->d_name) == 0)
      continue;
    student_type *const student = &(student_set->students[student_set->size]);
    strcpy(student->id, dentry->d_name);
    student->score.size = problem_set->size;
    for (int i = 0; i < problem_set->size; ++i) {
      strcpy(student->score.row[i].name, problem_set->problem[i].name);
      student->score.row[i].score = 0;
    }
    student_set->size += 1;
  }
  qsort(student_set->students, student_set->size, sizeof(student_type),
        student_compare);

  chdir(cwd);
  closedir(dirp);

  return student_set;
}

void write_student_score(const problem_set_type *problem_set,
                         const student_set_type *student_set)
{
  // answer.csv 출력
  char *fname = "score.csv";
  FILE *score_csv;
  if ((score_csv = fopen(fname, "w+")) == NULL) {
    fatal_error_print("file open error for score.csv");
  };
  // 헤더 출력
  fprintf(score_csv, ",");
  for (int i = 0; i < problem_set->size; ++i) {
    fprintf(score_csv, "%s%s,", problem_set->problem[i].name,
            (problem_set->problem[i].type == PROBLEM_CODE) ? ".c" : ".txt");
  }
  fprintf(score_csv, "sum\n");
  // 내용 출력
  for (int i = 0; i < student_set->size; ++i) {
    fprintf(score_csv, "%s,", student_set->students[i].id);
    double score_sum = 0;
    for (int j = 0; j < problem_set->size; ++j) {
      const double score = student_set->students[i].score.row[j].score;
      score_sum += score;
      fprintf(score_csv, "%.2lf,", score);
    }
    fprintf(score_csv, "%.2lf\n", score_sum);
  }
  fclose(score_csv);
}
