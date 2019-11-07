#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "backup.h"

// 백업리스트 파일 추가
void cmd_add(const char *pathname, int period, bool recursive, int node_limit,
             int time_to_live, bool watch_modify);
// 백업리스트 파일 제거
void cmd_remove(const char *pathname);
// 두 파일 비교
void cmd_compare(const char *file1, const char *file2);
// 파일 복구
void cmd_recover(const char *pathname, const char *new_pathname);
