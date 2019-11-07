#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "util.h"
#include "transpile.h"

// flag_j: 변환할 java 파일을 출력할지 여부
// flag_c: 변환된 c 파일을 출력할지 여부
// flag_p: java 함수에 대응되는 c 함수를 출력할지 여부
// flag_f: 프로그램 파일의 크기를 출력할지 여부
// flag_l: 프로그램 파일의 라인 수를 출력할지 여부
// flag_r: 코드가 변환되는 과정을 출력할지 여부
bool flag_j = false, flag_c = false, flag_p = false, flag_f = false,
     flag_l = false, flag_r = false;

// 명령행 인수를 처리하는 함수
void process_args(int argc, char **argv);

int main(int argc, char *argv[])
{
  struct timeval begin_t, end_t;
  gettimeofday(&begin_t, NULL);

  // 명령행 인수 처리
  process_args(argc, argv);

  // pathname: 파일의 전체 경로
  const char *pathname = argv[1];
  // file_name: 경로와 확장자를 제외한 파일의 순수한 이름
  char file_name[NAME_MAX];
  // output_path: C 파일이 출력될 경로
  char output_path[LINE_MAX];

  // pathname에서 file_name과 output_path를 구한다.
  if (strrchr(argv[1], '/') != NULL) {
    // pathname이 경로를 포함하고 있는 경우
    strncpy(output_path, pathname, (strrchr(argv[1], '/') - pathname) + 1);
    output_path[(strrchr(argv[1], '/') - pathname) + 1] = '\0';
    strcpy(file_name, strrchr(argv[1], '/') + 1);
  } else {
    // pathname이 경로를 포함하고 있지 않은 경우
    strcpy(output_path, "");
    strcpy(file_name, pathname);
  }

  // file_name이 확장자를 포함하는 경우 제거한다.
  if (strrchr(file_name, '.') != NULL) {
    *strrchr(file_name, '.') = '\0';
  }

  // 자바 파일을 읽어들인다.
  char *file_src = file_get_data(pathname);

  // 자바 파일이 존재하지 않는 경우
  if (file_src == NULL) {
    // 에러 메시지를 출력하고 종료한다.
    print_fatal_error("file '%s' is not exist.", pathname);
  }
  // 원본 코드 출력에 사용하기 위한 자바 파일을 복제한다.
  char *file_src_dup = strdup(file_src);
  // 출력할 라인 수
  int line_num = 1;
  // 현재 pid를 구한다.
  pid_t pid = getpid();

  char *save_ptr;
  // strtok_r을 이용하여 line 변수에 자바 파일의 첫번째 줄을 가져온다.
  char *line = strtok_r(file_src, "\n", &save_ptr);

  // 코드 변환기를 초기화한다.
  transpile_init();

  // '-r'옵션이 지정되어 있는 경우
  if (flag_r) {
    // 자식 프로세스를 생성한다.
    pid = fork();
    if (pid < 0) {
      // 에러가 발생하면 에러 메시지를 출력하고 종료한다.
      print_fatal_error("fork error");
    }
  }

  // 줄의 끝까지 도달할 때까지 반복한다.
  while (line != NULL) {
    // line을 변환한다.
    source_code *transpiled_source_code =
        transpile(file_name, output_path, line, pid);
    if (pid == 0) {
      // '-r' 옵션을 받아 fork로 생성된 자식 프로세스의 경우 변환 과정을
      // 출력한다.
      // '변환중...' 메시지 출력
      char java_file[FILENAME_MAX];
      sprintf(java_file, "%s.java", file_name);
      printf("%s Converting...\n", java_file);
      // 변환될 자바 코드 출력
      print_code(java_file, file_src_dup, line_num++);
      // 변환된 C 코드 출력
      write_source_code(transpiled_source_code, stdout);
      // 변환 과정을 보여주기 위해 1초 대기
      sleep(1);
      // 화면 클리어
      system("clear");
    }
    // line에 다음 줄의 시작 위치를 저장한다.
    line = strtok_r(NULL, "\n", &save_ptr);
  }

  // 'Makefile'을 생성한다.
  write_makefile(output_path, file_name);

  // '-r' 옵션이 지정된 경우
  if (flag_r) {
    // 자식 프로세스인경우 종료한다.
    if (pid == 0) {
      exit(0);
    }

    int status;
    // 부모 프로세스는 자식 프로세스의 종료를 대기한다.
    if (wait(&status) != pid) {
      print_fatal_error("wait error");
    }
  }

  // 자바 파일을 메모리에서 해제한다.
  free(file_src);
  free(file_src_dup);

  // 코드 변환기를 종료하고, 필요에 따라 메시지를 출력한다.
  transpile_finalize(output_path, file_name, flag_j, flag_c, flag_f, flag_l,
                     flag_p);

  // 프로그램의 수행 시간을 출력한다.
  gettimeofday(&end_t, NULL);
  ssu_runtime(&begin_t, &end_t);

  exit(0);
}

// 명령행 인수를 처리하는 함수
void process_args(int argc, char **argv)
{
  // 인수의 수가 너무 적으면 에러를 출력하고 프로그램을 종료한다.
  if (argc < 2) {
    fprintf(stderr, "usage: %s <FILENAME> [option]\n", argv[0]);
    exit(1);
  }

  for (int i = 2; i < argc; ++i) {
    // 명령행 인수에 따라 플래그를 설정한다.
    if (strcmp(argv[i], "-j") == 0) {
      flag_j = true;
    } else if (strcmp(argv[i], "-c") == 0) {
      flag_c = true;
    } else if (strcmp(argv[i], "-p") == 0) {
      flag_p = true;
    } else if (strcmp(argv[i], "-f") == 0) {
      flag_f = true;
    } else if (strcmp(argv[i], "-l") == 0) {
      flag_l = true;
    } else if (strcmp(argv[i], "-r") == 0) {
      flag_r = true;
    } else {
      // 올바르지 않은 명령행 인수가 들어오면 오류를 출력한다.
      print_fatal_error("unknown option '%s'\n"
                        "usage: %s <FILENAME> [option]",
                        argv[i], argv[0]);
    }
  }
}
