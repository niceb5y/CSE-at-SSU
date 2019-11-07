#ifndef SSU_CONVERT_TRANSPILE_H
#define SSU_CONVERT_TRANSPILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "util.h"

// 한 토큰의 길이
#define TOKEN_LEN 256
// 한 라인의 길이
#define LINE_LEN 1024
// 변환할 수 있는 클래스의 최댓값
#define CLASS_LIMIT 16
// 변환할 수 있는 함수의 종류의 최댓값
#define FUNC_LIMIT 128

//토큰의 종류
typedef enum _TOK_TYPE {
  // 연산자
  TOK_OPERATOR,
  // 숫자
  TOK_NUMBER,
  // 식별자
  TOK_IDENTIFIER,
  // 문자열
  TOK_STRING,
  // 주석
  TOK_COMMENT
} TOK_TYPE;

// 토큰의 종류와 내용을 저장하는 구조체
typedef struct _token_type {
  char content[TOKEN_LEN];
  TOK_TYPE type;
} token_type;

// 라인을 저장하는 링크드 리스트
typedef struct _line_node {
  struct _line_node *next;
  char line[LINE_LEN];
} line_node;

// 소스 코드를 저장하는 구조체
typedef struct _source_code {
  // 표준 헤더
  line_node *header;
  // 사용자 정의 클래스의 헤더
  line_node *user_header;
  // '.h' 파일을 저장하는 링크드 리스트
  line_node *header_file;
  // #define을 저장하는 링크드 리스트
  line_node *define;
  // 코드 부분을 저장하는 링크드 리스트
  line_node *body;
} source_code;

// 헤더 테이블에서 헤더 파일의 이름을 저장하는 데 사용되는 링크드 리스트
typedef struct _header_node {
  struct _header_node *next;
  char header[TOKEN_LEN];
} header_node;

// 헤더 테이블에서 함수 이름과 헤더 노드를 저장하는 데 사용되는 링크드 리스트
typedef struct _header_map_node {
  struct _header_map_node *next;
  header_node *header;
  char func[TOKEN_LEN];
} header_map_node;

// 헤더 테이블의 헤드를 저장하는 구조체
typedef struct _header_map {
  header_map_node *head;
} header_map;

// JAVA 함수 - C 함수의 매핑을 저장하는 구조체
typedef struct _func_map {
  char c[TOKEN_LEN];
  char java[TOKEN_LEN];
} func_map;

// '{' '}'의 깊이
int brace_level;
// 현재 클래스
char class_context[TOKEN_LEN];
// 사용자 정의 객체의 리스트
char object_list[CLASS_LIMIT][TOKEN_LEN];
// 사용자 정의 객체의 수
int object_list_count;
// 생성된 파일의 리스트
char file_list[CLASS_LIMIT][TOKEN_LEN];
// 생성된 파일의 수
int file_list_count;
// 매핑된 함수의 리스트
func_map func_list[FUNC_LIMIT];
// 매핑된 함수의 수
int func_list_count;
// 현재 문자열의 위치가 주석인지 여부
bool is_comment;
// 다음 행을 한단계 더 들여쓰기할지 여부
bool indent_next;
// 다음 토큰을 건너뛸지의 여부
bool skip_next;
// '.h' 파일의 프로토 타입을 생성 할 때 사용되는 변수
// ')'을 기다리고 있는지 여부
bool is_waiting_paren;
// 현재 문자열의 위치가 메인 함수인지 여부
bool is_main_func;
// 변환 된 소스 파일
source_code *source_file;
// 헤더 테이블
header_map header_table;

/*
 * 코드 변환기 관련 함수
 */
// 코드 변환기를 초기화 하는 함수
void transpile_init();
// 코드를 한 줄 변환하는 함수
source_code *transpile(const char *file_name, const char *output_path,
                       const char *line, pid_t pid);
// 코드 변환기를 종료하고 필요한 메시지를 출력하는 함수
void transpile_finalize(const char *output_path, const char *file_name,
                        bool print_java, bool print_c, bool print_size,
                        bool print_line_num, bool print_func_map);
/*
 * 코드 출력 관련 함수
 */
// 소스 코드를 쓰는 함수
void write_source_code(source_code *code, FILE *fp);
// 소스 코드의 헤더 파일을 쓰는 함수
void write_source_header(source_code *code, FILE *fp);
// 'Makefile'을 쓰는 함수
void write_makefile(const char *output_path, const char *file_name);

/*
 * 기타 함수
 */
// 헤더 테이블을 불러오는 함수
header_map_node *load_header_map();
// 함수에 필요한 헤더를 인클루드하는 함수
void add_func_header(const char *func_name);
// 헤더 파일이 존재하지 않으면 추가하는 함수
void add_include(const char *include_str);
// 사용자 정의 헤더 파일을 인클루드 하는 함수
void add_user_include(const char *include_str);
// JAVA 함수 - C 함수의 매핑을 추가하는 함수
void add_func_map(const char *java_func, const char *c_func);

#endif
