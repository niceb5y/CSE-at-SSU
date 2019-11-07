#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "util.h"
#include "problem.h"
#include "score.h"
#include "student.h"
#include "mark.h"

// 명령행 인자  관련 변수
bool flag_e = false, flag_t = false, flag_p = false, flag_c = false;
int qname_count = 0, id_count = 0;
char *student_dir = NULL, *true_dir = NULL, *error_dir = NULL;
char qnames[5][16], ids[5][16];

void parse_args(int argc, char *const argv[]); //명령행 인수 파싱
void print_score(void);                        //점수 표시(-c 옵션)
void show_help(void);                          //도움말 표시

int main(int argc, char *const argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  parse_args(argc, argv);

  if (error_dir != NULL) {
    if (strcmp(error_dir, student_dir) == 0 ||
        strcmp(error_dir, true_dir) == 0) {
      fatal_error_print("error directory is not valid!");
    }
  }

  // 디렉토리 정보가 온전한 경우만 채점을 실시한다.
  if (student_dir != NULL && true_dir != NULL) {
    // 문제, 점수, 학생 정보 가져오기
    problem_set_type *problem_set = get_problem_set(true_dir);
    score_table_type *score_table = get_score_table(true_dir, problem_set);
    student_set_type *student_set = get_student_set(student_dir, problem_set);
    // 채점
    mark(student_dir, true_dir, error_dir, problem_set, score_table,
         student_set, qnames, qname_count, flag_p);
    // csv출력
    write_student_score(problem_set, student_set);

    free(problem_set);
    free(score_table);
    free(student_set);
  }

  // -c 옵션을 받은 경우
  if (id_count > 0) {
    print_score();
  }

  gettimeofday(&end_t, NULL);
  ssu_runtime(&begin_t, &end_t);

  exit(0);
  return 0;
}

void parse_args(int argc, char *const argv[])
{
  // -h 옵션을 받았으면 도움말을 출력하고 바로 종료
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      show_help();
      exit(0);
    }
  }

  extern int optind, opterr;
  extern char *optarg;
  // -c 옵션이 처음 올 때
  if (argc > 2 && (strcmp(argv[1], "-c") == 0)) {
    int c;
    while ((c = getopt(argc, argv, "c:")) != -1) {
      switch (c) {
      case 'c':
        flag_c = true;
        optind -= 1;
        for (; optind < argc && *argv[optind] != '-'; ++optind) {
          if (*argv[optind] == '-')
            break;
          if (id_count >= 5)
            continue;
          strcpy(ids[id_count], argv[optind]);
          id_count += 1;
        }
        break;
      case '?':
        fatal_error_print("invalid option: %s", argv[optind]);
        break;
      }
    }
    return;
  }

  // 디렉토리가 먼저 주어지는 경우
  if (argc < 3) {
    fprintf(stderr, "Usage: ssu_score <STUDNETDIR> <TRUEDIR> [OPTION]\n");
    exit(1);
  }

  optind += 2;
  opterr = 0;
  int c;
  // 명령행 인자를 받아온다.
  while ((c = getopt(argc, argv, "e:t:pc:")) != -1) {
    switch (c) {
    case 'e':
      flag_e = true;
      error_dir = optarg;
      break;
    case 't':
      flag_t = true;
      optind -= 1;
      for (; optind < argc && *argv[optind] != '-'; ++optind) {
        if (qname_count >= 5)
          continue;
        strcpy(qnames[qname_count], argv[optind]);
        qname_count += 1;
      }
      break;
    case 'p':
      flag_p = true;
      break;
    case 'c':
      flag_c = true;
      optind -= 1;
      for (; optind < argc && *argv[optind] != '-'; ++optind) {
        if (*argv[optind] == '-')
          break;
        if (id_count >= 5)
          continue;
        strcpy(ids[id_count], argv[optind]);
        id_count += 1;
      }
      break;
    case '?':
      fatal_error_print("invalid option: %s", argv[optind]);
      break;
    }
  };
  student_dir = argv[1];
  true_dir = argv[2];
}

void print_score(void)
{
  printf("\n");
  char *fname = "score.csv";
  if (!file_exists(fname)) {
    fatal_error_print("score.csv file not exist.");
  }
  // csv에서 점수를 가져온다.
  FILE *score_csv;
  if ((score_csv = fopen(fname, "r")) == NULL) {
    fatal_error_print("file open error for score.csv");
  }
  // 학번을 순회하며 점수를 출력한다.
  for (int i = 0; i < id_count; ++i) {
    const char *id = ids[i];
    char buf[1024];
    bool matched = false;
    while (!feof(score_csv)) {
      fgets(buf, 1024, score_csv);

      if (strncmp(id, buf, strlen(id)) == 0 && buf[strlen(id)] == ',') {
        matched = true;
        char *score = strrchr(buf, ',') + 1;
        printf("%s's score : %s", id, score);
        fseek(score_csv, 0, SEEK_SET);
        break;
      }
    }
    if (!matched) {
      printf("%s's score : not exist.\n", id);
    }
  }
  printf("\n");
}

void show_help(void)
{
  const char *help_msg =
      "Usage: ssu_score <STUDNETDIR> <TRUEDIR> [OPTION]\n"
      "Option:\n"
      " -e <DIRNAME>      print error on 'DIRNAME/ID/qname_error.txt' file\n"
      " -t <qnames>       compile QNAME.c with -lpthread option\n"
      " -h                print usage\n"
      " -p                print student's score and total average\n"
      " -c <IDS>          print ID's score\n";
  printf("%s", help_msg);
}
