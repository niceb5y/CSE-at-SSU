#include "score.h"

score_table_type *read_score_table(const char *score_table_path)
{
  score_table_type *score_table =
      (score_table_type *)calloc(1, sizeof(score_table_type));
  score_table->size = 0;
  FILE *fp;
  if ((fp = fopen(score_table_path, "r")) == NULL) {
    fatal_error_print("fp is null");
  }

  while (!feof(fp)) {
    char filename[16];
    int i = score_table->size;
    fscanf(fp, "%[^,],%lf\n", filename, &(score_table->row[i].score));
    if (string_ends_with(filename, ".c")) {
      score_table->row[i].type = PROBLEM_CODE;
    } else if (string_ends_with(filename, ".txt")) {
      score_table->row[i].type = PROBLEM_TEXT;
    } else {
      fatal_error_print("\"score_table.csv\" not valid.");
    }
    strcpy(score_table->row[i].name, strtok(filename, "."));
    score_table->size += 1;
  }

  fclose(fp);

  return score_table;
};

void write_score_table(const char *score_table_path,
                       const score_table_type *score_table)
{
  FILE *fp = fopen(score_table_path, "a");
  if (fp == NULL) {
    fatal_error_print("fp is NULL");
  }
  for (int i = 0; i < score_table->size; ++i) {
    char filename[16];
    strcpy(filename, score_table->row[i].name);
    strcat(filename, score_table->row[i].type == PROBLEM_TEXT ? ".txt" : ".c");
    fprintf(fp, "%s,%.2lf\n", filename, score_table->row[i].score);
  }
  fclose(fp);
}

score_table_type *get_score_table(const char *true_dir,
                                  const problem_set_type *problem_set)
{
  score_table_type *score_table = NULL;
  char score_table_path[PATH_MAX];
  strcpy(score_table_path, true_dir);
  strcat(score_table_path, "/score_table.csv");
  // 파일이 존재하면 불러온다.
  if (file_exists(score_table_path)) {
    score_table = read_score_table(score_table_path);
    // 문제 수나 항목이 다르면 올바르지 않은 스코어 테이블이다.
    if (score_table->size != problem_set->size) {
      fatal_error_print("score_table.csv is not valid.");
    }
    for (int i = 0; i < score_table->size; ++i) {
      if (strcmp(score_table->row[i].name, problem_set->problem[i].name) != 0) {
        fatal_error_print("score_table.csv is not valid.");
      }
    }
  } else {
    score_table = (score_table_type *)malloc(sizeof(score_table_type));
    score_table->size = problem_set->size;
    printf("score_table.csv file does't exist in \"%s\"!\n", true_dir);
    while (true) {
      // csv파일이 없으면 만든다.
      printf(
          "1. input blank question and program question's score ex) 0.5 1\n");
      printf("2. input all question's score ex) Onput value of 1-1: 0.1\n");
      int choice;
      scanf("%d", &choice);
      if (choice == 1) {
        // 유형당 점수 부여
        double blank, program;
        printf("Input value of blank question: ");
        scanf("%lf", &blank);
        printf("Input value of program question: ");
        scanf("%lf", &program);
        for (int i = 0; i < score_table->size; ++i) {
          score_table->row[i].score =
              problem_set->problem[i].type == PROBLEM_TEXT ? blank : program;
          score_table->row[i].type = problem_set->problem[i].type;
          strcpy(score_table->row[i].name, problem_set->problem[i].name);
        }
        write_score_table(score_table_path, score_table);
        break;
      } else if (choice == 2) {
        // 개별 점수 부여
        for (int i = 0; i < score_table->size; ++i) {
          score_table->row[i].type = problem_set->problem[i].type;
          strcpy(score_table->row[i].name, problem_set->problem[i].name);
          printf("Input of %s.%s:", score_table->row[i].name,
                 score_table->row[i].type == PROBLEM_TEXT ? "txt" : "c");
          scanf("%lf", &score_table->row[i].score);
        }
        write_score_table(score_table_path, score_table);
        break;
      } else {
        printf("error: input is not valid.\n");
      }
    }
  }
  return score_table;
}
