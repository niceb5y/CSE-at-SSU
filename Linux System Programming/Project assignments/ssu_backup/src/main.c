#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#include "backup.h"
#include "command.h"
#include "util.h"

void exit_with_print_usage(char *);

int main(int argc, char *argv[])
{
  char backup_path[PATH_MAX];
  struct stat statbuf;

  // 인자가 1개만 주어진 경우 현재 경로 밑에 디렉토리 생성
  if (argc == 1) {
    getcwd(backup_path, PATH_MAX);
  }
  // 인자가 2개 주어진 경우
  else if (argc == 2) {
    // 절대 경로 확인
    path_normalize(argv[1], backup_path);
    // 디렉토리 또는 디렉토리에 대한 권한이 없는 경우 프로그램 종료
    if (access(backup_path, F_OK | R_OK | W_OK) != 0) {
      exit_with_print_usage(argv[0]);
    }
    lstat(backup_path, &statbuf);
    // 주어진 경로가 디렉토리가 아닌 경우 프로그램 종료
    if (!S_ISDIR(statbuf.st_mode)) {
      exit_with_print_usage(argv[0]);
    }
  }
  // 3개 이상의 인자가 주어진 경우
  else {
    exit_with_print_usage(argv[0]);
  }

  // 하위 디렉토리 "backup"이 존재하지 않으면 생성
  strcat(backup_path, "/backup");
  if (access(backup_path, F_OK) != 0) {
    mkdir(backup_path, 0755);
  }

  system("clear");

  // 백업 초기 설정
  backup_init(backup_path);

  while (true) {
    char buf[LINE_MAX];
    memset(buf, 0, sizeof(0));

    // 프롬프트 출력
    printf("20142314>");
    // 입력을 받는다.
    getln(buf);
    // 빈 입력 시 프롬프트로 넘어간다.
    if (strcmp(buf, "") == 0)
      continue;
    // "exit" 입력 시 종료
    else if (strcmp(buf, "exit") == 0) {
      backup_finalize();
      break;
    }
    // "list" 입력 시 리스트 출력
    else if (strcmp(buf, "list") == 0) {
      backup_print_list();
    }
    // "vi" / "vim" / "ls" 입력 시 system()을 통해 명령어 실행
    else if (strcmp(buf, "vi") == 0 || strcmp(buf, "vim") == 0 ||
             strcmp(buf, "ls") == 0) {
      system(buf);
    }
    // 인자 받는 경우 포함
    else if (string_starts_with(buf, "vi ") ||
             string_starts_with(buf, "vim ") ||
             string_starts_with(buf, "ls ")) {
      system(buf);
    } else {
      // 공백에 따라 파라미터 분리
      char *ret_ptr, *save_ptr;
      ret_ptr = strtok_r(buf, " ", &save_ptr);
      int _argc = 0;
      char _argv[16][512];
      memset(&_argv[0][0], 0, 16 * 512);
      while (ret_ptr != NULL) {
        if (argc >= 16)
          break;
        strcpy(_argv[_argc++], ret_ptr);
        ret_ptr = strtok_r(NULL, " ", &save_ptr);
      }
      // add 명령어
      if (strcmp(_argv[0], "add") == 0) {
        // 인자가 3개 미만인 경우 에러
        if (_argc < 3) {
          fprintf(stderr, "Usage:\n"
                          "  add <FILENAME> <PERIOD> [OPTION]\n");
          continue;
        }

        // 경로를 절대 경로로 변환
        char path[PATH_MAX];
        path_normalize(_argv[1], path);

        // period 숫자 변환
        int backup_period;
        char *end_ptr = NULL;
        backup_period = strtol(_argv[2], &end_ptr, 10);
        // 정수를 입력받지 않은 경우 에러
        if (end_ptr == _argv[2] || end_ptr[0] != '\0') {
          fprintf(stderr, "Error: period must be an integer\n");
          continue;
        }
        // 올바른 범위가 아닌 경우 에러
        if (backup_period < 5 || backup_period > 10) {
          fprintf(stderr, "Error: period range error. "
                          "period must be between 5 and 10.\n");
          continue;
        }

        // 옵션 처리
        int backup_limit = -1, backup_time_to_live = -1;
        bool backup_watch_modify = false, backup_dir = false,
             error_occurred = false;

        for (int i = 3; i < _argc; ++i) {
          // -m 옵션 처리
          if (strcmp(_argv[i], "-m") == 0) {
            backup_watch_modify = true;
          }
          // -n 옵션 처리
          else if (strcmp(_argv[i], "-n") == 0) {
            // 파라미터를 입력받지 않은 경우 에러
            if (i == _argc - 1) {
              fprintf(stderr, "Error: -n option has no NUMBER parameter\n");
              error_occurred = true;
              break;
            }
            // 파라미터 숫자 변환
            end_ptr = NULL;
            backup_limit = strtol(_argv[i + 1], &end_ptr, 10);
            // 파라미터가 정수가 아닌 경우 에러
            if (end_ptr == _argv[i + 1] || end_ptr[0] != '\0') {
              fprintf(
                  stderr,
                  "Error: -n option's NUMBER parameter must be an integer\n");
              error_occurred = true;
              break;
            }
            // 파라미터가 올바른 범위가 아닌 경우 에러
            if (backup_limit < 1 || backup_limit > 100) {
              fprintf(
                  stderr,
                  "Error: -n option's NUMBER parameter range error. "
                  "-n option's NUMBER parameter must be between 1 and 100.\n");
              error_occurred = true;
              break;
            }
            ++i;
          }
          // -t 옵션 처리
          else if (strcmp(_argv[i], "-t") == 0) {
            // 파라미터를 입력받지 않은 경우 에러
            if (i == _argc - 1) {
              fprintf(stderr, "Error: -t has no TIME parameter\n");
              error_occurred = true;
              break;
            }
            // 파라미터 숫자 변환
            end_ptr = NULL;
            backup_time_to_live = strtol(_argv[i + 1], &end_ptr, 10);
            // 파라미터가 정수가 아닌 경우 에러
            if (end_ptr == _argv[i + 1] || end_ptr[0] != '\0') {
              fprintf(stderr,
                      "Error: -t option's TIME parameter must be an integer\n");
              error_occurred = true;
              break;
            }
            // 파라미터가 올바른 범위가 아닌 경우 에러
            if (backup_time_to_live < 60 || backup_time_to_live > 1200) {
              fprintf(
                  stderr,
                  "Error: -t option's TIME parameter range error. "
                  "-t option's TIME parameter must be between 60 and 1200.\n");
              error_occurred = true;
              break;
            }
            ++i;
          }
          // -d 옵션 처리
          else if (strcmp(_argv[i], "-d") == 0) {
            backup_dir = true;
          }
          // 이외 올바르지 않은 옵션은 에러
          else {
            fprintf(stderr, "Error: unknown option \"%s\"\n", _argv[i]);
            error_occurred = true;
            break;
          }
        }

        // 에러가 발생하면 더 진행하지 않음
        if (error_occurred)
          continue;

        // 경로 길이 확인 후 초과하면 에러
        if (strlen(path) > 255) {
          fprintf(stderr, "Error: path length limit exceeded.\n");
          continue;
        }

        // 파일이 존재하지 않으면 에러
        if (access(path, F_OK) != 0) {
          fprintf(stderr, "Error: file not exists.\n");
          continue;
        }

        // 일반 파일 또는 -d 옵션을 사용한 디렉토리만 백업에 추가
        lstat(path, &statbuf);
        if ((S_ISREG(statbuf.st_mode) && !backup_dir) ||
            (S_ISDIR(statbuf.st_mode) && backup_dir)) {
          cmd_add(path, backup_period, backup_dir, backup_limit,
                  backup_time_to_live, backup_watch_modify);
        }
        // 이외 파일은 백업에 추가되지 않는다.
        else {
          fprintf(stderr, "Error: file type is not valid.\n");
          continue;
        }
      }
      // remove 명령어
      else if (strcmp(_argv[0], "remove") == 0) {
        // 인자가 2개 아닌 경우 에러
        if (_argc != 2) {
          fprintf(stderr, "Usage:\n"
                          "  remove <FILENAME>\n"
                          "  remove -a\n");
          continue;
        }
        // -a 옵션 입력받은 경우 백업 파일 모두 제거
        if (strcmp(_argv[1], "-a") == 0) {
          backup_remove_all();
        }
        // 이외의 경우 특정 파일만 제거
        else {
          char path[PATH_MAX];
          path_normalize(_argv[1], path);
          cmd_remove(path);
        }
      }
      // compare 명령어
      else if (strcmp(_argv[0], "compare") == 0) {
        // 인자가 3개가 아닌 경우 에러
        if (_argc != 3) {
          fprintf(stderr, "Usage:\n"
                          "  compare <FILENAME1> <FILENAME2>\n");
          continue;
        }
        // 파일 비교
        cmd_compare(_argv[1], _argv[2]);
      }
      // recover 명령어
      else if (strcmp(_argv[0], "recover") == 0) {
        char path[PATH_MAX];
        // 인자의 개수가 올바르지 않은 경우 에러
        if (_argc < 2 || _argc > 4) {
          fprintf(stderr, "Usage:\n"
                          "  recover <FILENAME> [OPTION]\n");
          continue;
        }
        path_normalize(_argv[1], path);
        if (_argc > 2) {
          // 올바른 옵션이 아니면 에러
          if (strcmp(_argv[2], "-n") != 0) {
            fprintf(stderr, "Error: unknown option \"%s\"\n", _argv[2]);
            continue;
          }
          // 파라미터가 주어지지 않은 경우 에러
          if (_argc == 3) {
            fprintf(stderr, "Error: -n has no NEWFILE parameter\n");
            continue;
          }
          char new_path[PATH_MAX];
          path_normalize(_argv[3], new_path);
          // 파일이 이미 존재하는 경우 에러
          if (access(new_path, F_OK) == 0) {
            fprintf(stderr, "Error: file \"%s\" already exists.\n", new_path);
            continue;
          }
          // 새 파일을 생성하여 복구
          cmd_recover(path, new_path);
        } else {
          // 원래 파일로 복구
          cmd_recover(path, path);
        }
      }
      // 알 수 없는 명령어는 에러 출력
      else {
        fprintf(stderr, "Error: command is not valid.\n");
      }
    }
  }

  return 0;
}

// Usage 출력 후 프로그램 종료
void exit_with_print_usage(char *executable)
{
  fprintf(stderr, "Usage: %s [backup directory]\n", executable);
  exit(1);
}
