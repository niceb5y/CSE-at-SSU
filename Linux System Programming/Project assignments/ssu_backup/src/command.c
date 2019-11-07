#include "command.h"

// 백업리스트 파일 추가
void cmd_add(const char *pathname, int period, bool recursive, int node_limit,
             int time_to_live, bool watch_modify)
{
  // -d 옵션을 통해 디렉토리가 추가된 경우
  if (recursive) {
    // 백업 경로는 백업에 포함 될 수 없다.
    if (strcmp(pathname, backup_get_backup_path()) == 0) {
      fprintf(stderr, "Error: backup path cannot be added.\n");
      return;
    }
    // 디렉토리 순회
    DIR *dirp = opendir(pathname);
    struct dirent *dirent;
    while ((dirent = readdir(dirp)) != NULL) {
      if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
        continue;
      // 절대 경로를 가져온다.
      char abs_path[PATH_MAX];
      sprintf(abs_path, "%s/%s", pathname, dirent->d_name);
      struct stat statbuf;
      // 파일 정보를 가져온다.
      lstat(abs_path, &statbuf);
      // 일반 파일인 경우 -d 옵션을 제외하고 재귀호출
      if (S_ISREG(statbuf.st_mode)) {
        cmd_add(abs_path, period, false, node_limit, time_to_live,
                watch_modify);
      }
      // 디렉토리 파일인 경우 -d 옵션을 포함하고 재귀호출
      if (S_ISDIR(statbuf.st_mode)) {
        cmd_add(abs_path, period, true, node_limit, time_to_live, watch_modify);
      }
    }
  }
  // 일반 파일이 추가 된 경우
  else {
    if (backup_add(pathname, period, node_limit, time_to_live, watch_modify) <
        0) {
      // 중복 파일인 경우 에러 메시지 출력
      char filename[PATH_MAX];
      strcpy(filename, pathname);
      fprintf(stderr, "Error: duplicated filename \"%s\". skipping.\n",
              path_get_filename(filename));
    }
  }
}

// 백업리스트 파일 제거
void cmd_remove(const char *pathname)
{
  if (backup_remove(pathname) < 0) {
    fprintf(stderr, "Error: file \"%s\" is not exist.\n", pathname);
  }
}

// 두 파일 비교
void cmd_compare(const char *file1, const char *file2)
{
  // 파일이 존재하지 않으면 에러 메시지 출력
  if (access(file1, F_OK) != 0) {
    fprintf(stderr, "Error: file \"%s\" is not exist.\n", file1);
    return;
  }
  if (access(file2, F_OK) != 0) {
    fprintf(stderr, "Error: file \"%s\" is not exist.\n", file2);
    return;
  }
  // 두 파일 비교
  int comp = file_compare(file1, file2);
  if (comp == -1) {
    // 파일 비교 과정에서 에러 발생 시 메시지 출력
    fprintf(stderr, "Error: stat error\n");
  } else if (comp == 0) {
    // 파일이 같은 경우 메시지를 출력한다.
    printf("Files are equal.\n");
  } else {
    // 파일이 다른 경우 파일 크기와 수정 시간을 출력한다.
    printf("Files are different.\n");
    struct stat statbuf1, statbuf2;
    char mod_time1[26], mod_time2[26];
    // 파일 정보를 가져온다.
    if (lstat(file1, &statbuf1) != 0 || lstat(file2, &statbuf2) != 0) {
      fprintf(stderr, "Error: stat error\n");
      return;
    }
    // 시간을 문자열로 변환한다.
    ctime_r(&statbuf1.st_mtime, mod_time1);
    ctime_r(&statbuf2.st_mtime, mod_time2);
    // 파일 정보 출력
    printf("  %s\n"
#ifdef __APPLE__
           "    size: %lld bytes\n"
#else
           "    size: %ld bytes\n"
#endif
           "    last modified time: %s",
           file1, statbuf1.st_size, mod_time1);
    printf("  %s\n"
#ifdef __APPLE__
           "    size: %lld bytes\n"
#else
           "    size: %ld bytes\n"
#endif
           "    last modified time: %s",
           file2, statbuf2.st_size, mod_time2);
  }
}

// 파일 복구
void cmd_recover(const char *pathname, const char *new_pathname)
{
  // 주어진 경로에 대한 노드를 가져온다.
  backup_node *node = backup_get_backup_node(pathname);
  // 노드가 존재하지 않는 경우 에러 메시지 출력 후 종료
  if (node == NULL) {
    fprintf(stderr, "Error: filename \"%s\" not found.\n", pathname);
    return;
  }
  // 리스트를 만들어 오름차순 출력
  backup_file_node *list = NULL, *cur = node->snapshots->next;
  while (cur != NULL) {
    backup_file_node *tmp = calloc(1, sizeof(backup_file_node));
    tmp->timestamp = cur->timestamp;
    tmp->next = list;
    list = tmp;
    cur = cur->next;
  }
  printf("0. exit\n");
  int index = 0;
  cur = list;
  while (cur != NULL) {
    char suffix[13];
    struct tm local_time;
    // 시간 포맷팅
    localtime_r(&cur->timestamp, &local_time);
    strftime(suffix, 13, "%y%m%d%H%M%S", &local_time);
    // 파일 크기를 가져온다.
    char backup_pathname[PATH_MAX];
    backup_get_filename(backup_pathname, pathname, &cur->timestamp);
    struct stat statbuf;
    lstat(backup_pathname, &statbuf);
    printf("%d. %s\t%d bytes\n", ++index, suffix, statbuf.st_mode);
    cur = cur->next;
  }
  // 복구할 파일에 대한 입력을 받는다.
  char buf[LINE_MAX];
  memset(buf, 0, sizeof(0));
  printf("Choose file to recover: ");
  getln(buf);

  // 받은 입력을 숫자로 전환
  int file_id;
  char *end_ptr = NULL;
  file_id = strtol(buf, &end_ptr, 10);
  // 입력받은 숫자가 올바른 숫자가 아닌경우
  if (end_ptr == buf || end_ptr[0] != '\0') {
    fprintf(stderr, "Error: file id must be an integer.\n");
  }
  // 입력 받은 숫자가 범위를 초과한 경우
  else if (file_id < 0 || file_id > index) {
    fprintf(stderr, "Error: file id is not valid.\n");
  }
  // 입력 받은 숫자가 exit이 아닌경우
  else if (file_id != 0) {
    // 해당 노드를 가져온다.
    cur = list;
    index = 1;
    while (index < file_id) {
      ++index;
      cur = cur->next;
    }
    if (cur != NULL) {
      // 파일 이름을 가져온다.
      char backup_pathname[PATH_MAX];
      backup_get_filename(backup_pathname, pathname, &cur->timestamp);
      // 파일 복사에 성공한 경우
      if (file_copy(backup_pathname, new_pathname) != -1) {
        // 로그 출력
        backup_log("%s recovered.", pathname);
        // 백업 중단
        backup_remove(pathname);
        // 성공 메시지 출력
        printf("Recovery Success\n");
        printf("================\n");
        // 파일 내용 출력
        FILE *fp = fopen(new_pathname, "r");
        char file_buf[4096];
        while (fgets(file_buf, 4096, fp)) {
          fputs(file_buf, stdout);
        }
      }
      // 도중에 -n, -t 옵션에 의해 파일이 삭제된 경우
      else {
        // 에러 메시지 출력
        fprintf(stderr, "Error: backup file not exist. file copy failed.\n");
      }
    } else {
      // 해당 id에 대한 노드가 없는 경우 에러 메시지 출력
      fprintf(stderr, "Error: file id is not valid.\n");
    }
  }
  cur = list;
  // 사용한 리스트 노드 제거
  while (cur != NULL) {
    backup_file_node *tmp = cur;
    cur = cur->next;
    free(tmp);
  }
}
