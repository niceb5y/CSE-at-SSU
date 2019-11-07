#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>

#include "util.h"

// 백업된 파일의 정보를 담은 링크드 리스트 노드의 구조체
typedef struct _backup_file_node {
  time_t timestamp;
  struct _backup_file_node *next;
} backup_file_node;

// 백업한 파일의 정보를 담은 링크드 리스트 노드의 구조체
typedef struct _backup_node {
  char pathname[PATH_MAX];
  int period;
  int node_limit;
  int time_to_live;
  bool watch_modify;
  pthread_t tid;
  backup_file_node *snapshots;
  struct _backup_node *next;
} backup_node;

// 백업 초기 설정
void backup_init(const char *backup_path);
// 백업 마무리
void backup_finalize();
// 백업 로그 함수
void backup_log(const char *format, ...);
// 백업 디렉토리 반환
char *backup_get_backup_path();
// 특정 경로의 백업 파일에 대한 노드 반환
backup_node *backup_get_backup_node(const char *pathname);
// 백업 파일 추가
int backup_add(const char *pathname, int period, int node_limit,
               int time_to_live, bool watch_modify);
// 백업 파일 제거
int backup_remove(const char *pathname);
// 백업 파일 모두 제거
void backup_remove_all();
// 백업 파일 리스트 출력
void backup_print_list();
// 백업 파일의 이름 생성
void backup_get_filename(char *pathname, const char *original,
                         const time_t *time_val);
// 백업 파일이 수정되었는지 확인
bool backup_check_modified(backup_node *node);

// 백업 쓰레드가 수행하는 함수
void *backup_thread(void *arg);
