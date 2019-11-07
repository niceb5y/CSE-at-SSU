#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 키워드 저장 타입
typedef struct _keyword_set_type {
  int size;
  char keyword[100][1024];
} keyword_set_type;

// 토큰 저장 타입
typedef struct _token_set_type {
  int size;
  char token[200][1024];
} token_set_type;

//(Postfix로의 변환을 위한) 스택 관련 연산자
void token_stack_init(void);
const char *token_stack_push(const char *token);
const char *token_stack_pop(void);
bool token_stack_is_empty(void);
const char *token_stack_top(void);

//연산자 우선순위 얻기
int token_stack_get_priority(const char *token);
//연산자인지 확인
bool token_is_operator(const char *token);
// postfix로 변환
token_set_type *token_infix_to_postfix(token_set_type *token_set);

//코드 전처리
char *preprocess_code(const char *str);
// 토큰 분리
token_set_type *get_token_set(const char *str);
// 토큰 노멀라이즈
token_set_type *get_norm_token_set(const token_set_type *token_set);
// 토큰을 텍스트로 바꾸기
char *get_norm_text(const char *str);
// 두 텍스트 비교
bool compare_code(const char *str1, const char *str2);
