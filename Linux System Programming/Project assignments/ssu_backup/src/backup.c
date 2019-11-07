#include "backup.h"

// 백업 파일 정보를 저장하는 링크드 리스트
backup_node *backup_files = NULL;
// 파일이 백업될 경로
char backup_directory[PATH_MAX];
// 로그 파일의 포인터
FILE *fp_log = NULL;
// 로그 기록에 사용될 뮤텍스
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 백업 초기 설정
void backup_init(const char *backup_path)
{
  // 백업 파일 링크드 리스트에 더미 노드를 만든다.
  backup_files = calloc(1, sizeof(backup_node));
  backup_files->next = NULL;
  // 백업 디렉토리 설정
  strcpy(backup_directory, backup_path);
  // 로그 파일 경로 설정
  char log_path[PATH_MAX];
  sprintf(log_path, "%s/log.txt", backup_directory);
  fp_log = fopen(log_path, "a");
  // 로그 버퍼 설정
  setvbuf(fp_log, NULL, _IOLBF, BUFSIZ);
}

// 백업 마무리
void backup_finalize()
{
  // 로그 플러시
  fflush(fp_log);
  // 뮤텍스 해제
  pthread_mutex_destroy(&mutex);
}

// 백업 로그 함수
void backup_log(const char *format, ...)
{
  // 순차 기록을 위해 뮤텍스 사용
  pthread_mutex_lock(&mutex);
  // 현재 시간을 형식에 맞게 출력한다.
  time_t now = time(NULL);
  char suffix[14];
  struct tm local_time;
  localtime_r(&now, &local_time);
  strftime(suffix, 14, "%y%m%d %H%M%S", &local_time);
  fprintf(fp_log, "[%s] ", suffix);
  // 인자로 받은 내용 출력
  va_list args;
  va_start(args, format);
  vfprintf(fp_log, format, args);
  va_end(args);
  fprintf(fp_log, "\n");
  //줄바꿈 해제
  pthread_mutex_unlock(&mutex);
}

// 백업 디렉토리 반환
char *backup_get_backup_path() { return backup_directory; }

// 특정 경로의 백업 파일에 대한 노드 반환
backup_node *backup_get_backup_node(const char *pathname)
{
  // 노드를 탐색하여 값을 반환한다.
  backup_node *cur = backup_files->next;
  while (cur != NULL) {
    if (strcmp(pathname, cur->pathname) == 0)
      return cur;
    cur = cur->next;
  }
  // 해당 노드가 존재하지 않으면 NULL 반환
  return NULL;
}

// 백업 파일 추가
int backup_add(const char *pathname, int period, int node_limit,
               int time_to_live, bool watch_modify)
{
  backup_node *cur = backup_files;
  // 이름이 중복되는 파일은 추가하지 않는다.
  while (cur->next != NULL) {
    char *fn1 = path_get_filename(pathname);
    char *fn2 = path_get_filename(cur->next->pathname);
    if (strcmp(fn1, fn2) == 0)
      return -1;
    cur = cur->next;
  }

  // 노드 생성
  cur->next = calloc(1, sizeof(backup_node));
  cur = cur->next;
  // 노드 값 할당
  cur->next = NULL;
  strcpy(cur->pathname, pathname);
  cur->period = period;
  cur->node_limit = node_limit;
  cur->time_to_live = time_to_live;
  cur->watch_modify = watch_modify;
  cur->snapshots = calloc(1, sizeof(backup_file_node));
  cur->snapshots->next = NULL;
  // 쓰레드 생성
  if (pthread_create(&cur->tid, NULL, backup_thread, (void *)cur) != 0) {
    fprintf(stderr, "Error: pthread_create error\n");
  }
  // 쓰레드 분리
  pthread_detach(cur->tid);
  // 로그 출력
  backup_log("%s added.", cur->pathname);
  return 0;
}

// 백업 파일 제거
int backup_remove(const char *pathname)
{
  backup_node *cur = backup_files;
  // 리스트를 순회하며 해당 노드를 찾아 제거한다.
  while (cur->next != NULL) {
    if (strcmp(pathname, cur->next->pathname) == 0) {
      // 쓰레드 중지
      pthread_cancel(cur->next->tid);
      // 현재 노드 분리
      backup_node *temp = cur->next;
      cur->next = temp->next;
      // 파일 노드 메모리 반환
      backup_file_node *temp_file = temp->snapshots;
      while (temp_file != NULL) {
        backup_file_node *next = temp_file->next;
        free(temp_file);
        temp_file = next;
      }
      // 로그 출력
      backup_log("%s removed.", temp->pathname);
      // 노드 메모리 반환
      free(temp);
      return 0;
    }
    cur = cur->next;
  }
  // 노드가 존재하지 않으면 종료한다.
  return -1;
}

// 백업 파일 모두 제거
void backup_remove_all()
{
  backup_node *cur = backup_files->next;
  // 노드 분리
  backup_files->next = NULL;
  // 모든 노드를 순회하며 제거한다.
  while (cur != NULL) {
    // 쓰레드 종료
    pthread_cancel(cur->tid);
    // 현재 노드 분리
    backup_node *temp = cur->next;
    // 파일 노드 메모리 반환
    backup_file_node *temp_file = cur->snapshots;
    while (temp_file != NULL) {
      backup_file_node *next = temp_file->next;
      free(temp_file);
      temp_file = next;
    }
    // 로그 출력
    backup_log("%s removed.", cur->pathname);
    // 노드 메모리 반환
    free(cur);
    cur = temp;
  }
}

// 백업 파일 리스트 출력
void backup_print_list()
{
  backup_node *cur = backup_files->next;
  while (cur != NULL) {
    // 경로 및 period 출력
    printf("%s %d", cur->pathname, cur->period);
    // 옵션 출력
    if (cur->watch_modify) {
      printf(" m");
    }
    if (cur->node_limit != -1) {
      printf(" n");
    }
    if (cur->time_to_live != -1) {
      printf(" t");
    }
    printf("\n");
    cur = cur->next;
  }
}

// 백업 파일의 이름 생성
void backup_get_filename(char *pathname, const char *original,
                         const time_t *time_val)
{
  // 시간을 형식에 맞게 문자열로 만든다.
  char suffix[13];
  struct tm local_time;
  localtime_r(time_val, &local_time);
  strftime(suffix, 13, "%y%m%d%H%M%S", &local_time);
  // 배열에 복사
  sprintf(pathname, "%s/%s_%s", backup_directory, path_get_filename(original),
          suffix);
}

// 백업 파일이 수정되었는지 확인
bool backup_check_modified(backup_node *node)
{
  backup_file_node *file_node = node->snapshots->next;
  // 파일이 존재하지 않는 경우 백업
  if (file_node == NULL)
    return true;
  char backup_pathname[PATH_MAX];
  backup_get_filename(backup_pathname, node->pathname, &file_node->timestamp);
  // 파일이 수정 시간이나 크기가 다른경우 백업
  return file_compare(node->pathname, backup_pathname) == 0 ? false : true;
}

// 백업 쓰레드가 수행하는 함수
void *backup_thread(void *arg)
{
  backup_node *node = (backup_node *)arg;
  while (true) {
    // period 만큼 대기
    sleep(node->period);

    char path[PATH_MAX];
    backup_file_node *snapshot = node->snapshots->next;

    // -m 옵션이 적용되지 않았거나, 적용되었는데 파일이 변경된 경우 백업
    if (!node->watch_modify || backup_check_modified(node)) {
      // 새 파일 노드 생성
      snapshot = calloc(1, sizeof(backup_file_node));
      snapshot->timestamp = time(NULL);
      snapshot->next = node->snapshots->next;
      node->snapshots->next = snapshot;
      // 파일 경로 생성
      backup_get_filename(path, node->pathname, &snapshot->timestamp);
      // 파일 복사
      file_copy(node->pathname, path);
      // 로그 출력
      backup_log("%s generated.", path);
    }

    backup_file_node *cur = node->snapshots;
    int n = 1;
    while (cur->next != NULL) {
      // -t 옵션이 적용 되고 특정 시간이 지난 경우
      // 또는 -n 옵션이 적용되고 파일의 개수가 초과된 경우 파일을 삭제한다.
      if ((node->time_to_live > 0 &&
           difftime(snapshot->timestamp, cur->next->timestamp) >
               node->time_to_live) ||
          (node->node_limit > 0 && n > node->node_limit)) {

        // 파일 노드 분리
        backup_file_node *tmp = cur->next;
        cur->next = tmp->next;
        // 삭제 파일 경로 생성
        char del_path[PATH_MAX];
        backup_get_filename(del_path, node->pathname, &tmp->timestamp);
        // 로그 출력
        backup_log("%s deleted.", del_path);
        // 파일 삭제
        remove(del_path);
        // 파일 노드 메모리 반환
        free(tmp);
      }
      cur = cur->next;
      ++n;
      if (cur == NULL)
        break;
    }
  }
}
