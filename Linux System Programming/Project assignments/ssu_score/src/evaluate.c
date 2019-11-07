#include "evaluate.h"

const char *token_stack[200];
int _token_stack_top;

// postfix 전환에 사용되는 스택 연산자

void token_stack_init(void) { _token_stack_top = -1; }

const char *token_stack_push(const char *token)
{
  if (_token_stack_top >= 199) {
    return NULL;
  }
  token_stack[++_token_stack_top] = token;
  return token;
}

const char *token_stack_pop(void)
{
  if (_token_stack_top < 0) {
    return NULL;
  }
  return token_stack[_token_stack_top--];
}

bool token_stack_is_empty(void) { return (_token_stack_top < 0); }

const char *token_stack_top(void)
{
  return token_stack_is_empty() ? NULL : token_stack[_token_stack_top];
}

bool token_is_operator(const char *token)
{
  const char *op[] = {"++",  "--",  ".",  "->", "!",  "~",  "&",  "sizeof",
                      "*",   "/",   "%",  "+",  "-",  "<<", ">>", "<",
                      ">",   ">=",  "<=", "==", "!=", "&",  "^",  "|",
                      "&&",  "||",  "=",  "+=", "-=", "*=", "/=", "%=",
                      "<<=", ">>=", "&=", "^=", "|=", ","};
  for (int i = 0; i < 38; ++i) {
    if (strcmp(token, op[i]) == 0)
      return true;
  }
  return false;
}

int token_stack_get_priority(const char *token)
{
  const char *braces[] = {"(", "["};
  for (int i = 0; i < 2; ++i) {
    if (strcmp(token, braces[i]) == 0)
      return 0;
  }
  const char *p0[] = {"++", "--", ".", "->"};
  for (int i = 0; i < 4; ++i) {
    if (strcmp(token, p0[i]) == 0)
      return 14;
  }
  const char *p1[] = {"!", "~", "&", "sizeof"};
  for (int i = 0; i < 4; ++i) {
    if (strcmp(token, p1[i]) == 0)
      return 13;
  }
  const char *p2[] = {"*", "/", "%"};
  for (int i = 0; i < 3; ++i) {
    if (strcmp(token, p2[i]) == 0)
      return 12;
  }
  const char *p3[] = {"+", "-"};
  for (int i = 0; i < 2; ++i) {
    if (strcmp(token, p3[i]) == 0)
      return 11;
  }
  const char *p4[] = {"<<", ">>"};
  for (int i = 0; i < 2; ++i) {
    if (strcmp(token, p4[i]) == 0)
      return 10;
  }
  const char *p5[] = {"<", ">", ">=", "<="};
  for (int i = 0; i < 4; ++i) {
    if (strcmp(token, p5[i]) == 0)
      return 9;
  }
  const char *p6[] = {"==", "!="};
  for (int i = 0; i < 2; ++i) {
    if (strcmp(token, p6[i]) == 0)
      return 8;
  }
  if (strcmp(token, "&") == 0)
    return 7;
  if (strcmp(token, "^") == 0)
    return 6;
  if (strcmp(token, "|") == 0)
    return 5;
  if (strcmp(token, "&&") == 0)
    return 4;
  if (strcmp(token, "||") == 0)
    return 3;
  const char *p12[] = {
      "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|="};
  for (int i = 0; i < 11; ++i) {
    if (strcmp(token, p12[i]) == 0)
      return 2;
  }
  if (strcmp(token, ",") == 0)
    return 1;
  return 100;
}

token_set_type *token_infix_to_postfix(token_set_type *token_set)
{
  // 스택을 이용하여 postfix로 변환한다.
  token_set_type *ret = (token_set_type *)malloc(sizeof(token_set_type));
  ret->size = 0;
  token_stack_init();
  int i, j;
  for (i = 0, j = 0; i < token_set->size; ++i) {
    const char *cur = token_set->token[i];
    if (strcmp(cur, "(") == 0) {
      token_stack_push(cur);
    } else if (strcmp(cur, ")") == 0) {
      while (!token_stack_is_empty() && strcmp(token_stack_top(), "(") != 0) {
        strcpy(ret->token[j], token_stack_pop());
        ret->size = ++j;
      }
      token_stack_pop();
    } else if (strcmp(cur, "[") == 0) {
      token_stack_push(cur);
    } else if (strcmp(cur, "]") == 0) {
      while (strcmp(token_stack_top(), "[") != 0) {
        strcpy(ret->token[j], token_stack_pop());
        ret->size = ++j;
      }
      token_stack_pop();
    } else if (token_is_operator(cur)) {
      while (!token_stack_is_empty() &&
             (token_stack_get_priority(token_stack_top()) >=
              token_stack_get_priority(cur))) {
        strcpy(ret->token[j], token_stack_pop());
        ret->size = ++j;
      }
      token_stack_push(cur);
    } else {
      token_stack_push(cur);
    }
  }

  while (!token_stack_is_empty()) {
    strcpy(ret->token[j], token_stack_pop());
    ret->size = ++j;
  }
  return ret;
}

char *preprocess_code(const char *str)
{
  // 문자열 안 공백 제거
  char *ret = (char *)malloc(strlen(str) + 1);
  bool dqoute = false;
  size_t i, j;
  for (i = 0, j = 0; i < strlen(str); i++, j++) {
    if (str[i] == '"') {
      if (i == 0 || str[i - 1] != '\\') {
        dqoute = !dqoute;
      }
    }
    if (str[i] != ' ' || !dqoute)
      ret[j] = str[i];
    else
      j--;
  }
  ret[j] = '\0';
  return ret;
}

token_set_type *get_token_set(const char *str)
{
  // strtok_r을 통해 문자열 토크나이징하여 키워드 셋에 저장
  keyword_set_type keyword_set;
  keyword_set.size = 0;
  const char *code_delim = "+-*/!%^|&()<>[],.:';\\=\"~ ";
  char *tmp = strdup(str);
  char *lasts;
  char *token = strtok_r(tmp, code_delim, &lasts);
  while (token != NULL) {
    strcpy(keyword_set.keyword[keyword_set.size], token);
    keyword_set.size++;
    token = strtok_r(NULL, code_delim, &lasts);
  }
  free(tmp);
  tmp = NULL;

  const char *cur = str;

  // 키워드를 다시 매칭하여 토큰셋으로 저장
  int keyword_index = 0;
  token_set_type *token_set = (token_set_type *)malloc(sizeof(token_set_type));
  token_set->size = 0;
  while (*cur != '\0') {
    if (*cur == ' ') {
      ++cur;
      continue;
    }
    if (keyword_index < keyword_set.size) {
      char *keyword = keyword_set.keyword[keyword_index];
      if (strncmp(cur, keyword, strlen(keyword)) == 0) {
        strcpy(token_set->token[token_set->size], keyword);
        token_set->size++;
        cur += strlen(keyword);
        ++keyword_index;
        continue;
      }
    }
    if (strchr(code_delim, *cur) != NULL) {
      sprintf(token_set->token[token_set->size], "%c", *cur);
      token_set->size++;
    }
    ++cur;
  }
  return token_set;
}

token_set_type *get_norm_token_set(const token_set_type *token_set)
{
  // 2자 이상의 연산자를 하나의 토큰으로 합친다.
  token_set_type *ret = (token_set_type *)malloc(sizeof(token_set_type));
  int i, j;
  for (i = 0, j = 0; i < token_set->size; i++, j++) {
    const char *cur = token_set->token[i];
    const char *prev = ret->token[j - 1];
    char *repeats[] = {"&", "|", "+", "-", ">", "<", "="};
    char *equals[] = {"+", "-",  "*",  "/", "%", "!", "<",
                      ">", "<<", ">>", "^", "|", "~"};
    bool matched = false;
    for (int k = 0; k < 7; ++k) {
      char *sym = repeats[k];
      if (j > 0 && strcmp(cur, sym) == 0 && strcmp(prev, sym) == 0) {
        matched = true;
        --j;
        strcat(ret->token[j], sym);
        ret->size = j + 1;
        break;
      }
    }
    if (matched)
      continue;
    for (int k = 0; k < 13; ++k) {
      char *sym = equals[k];
      if (j > 0 && strcmp(cur, "=") == 0 && strcmp(prev, sym) == 0) {
        matched = true;
        --j;
        strcat(ret->token[j], "=");
        ret->size = j + 1;
        break;
      }
    }
    if (matched)
      continue;
    if (i > 0 && strcmp(cur, ">") == 0 && strcmp(prev, "-") == 0) {
      --j;
      strcat(ret->token[j], ">");
      ret->size = j + 1;
    } else {
      strcpy(ret->token[j], token_set->token[i]);
      ret->size = j + 1;
    }
  }
  return ret;
}

char *get_norm_text(const char *str)
{
  // 문자열을 토큰화하여 postfix로 바꾸고 다시 문자열로 변환
  char *str_preprocessed = preprocess_code(str);
  token_set_type *token_set = get_token_set(str_preprocessed);
  free(str_preprocessed);
  token_set_type *norm_token_set = get_norm_token_set(token_set);
  token_set_type *post_token_set = token_infix_to_postfix(norm_token_set);

  char *ret = (char *)malloc(sizeof(char) * 1024 * 200);
  strcpy(ret, "");
  for (int i = 0; i < post_token_set->size; ++i) {
    strcat(ret, post_token_set->token[i]);
  }

  free(token_set);
  free(norm_token_set);
  free(post_token_set);
  return ret;
}

bool compare_code(const char *str1, const char *str2)
{
  // 두 코드 비교
  bool ret;
  char *norm_str1 = get_norm_text(str1);
  char *norm_str2 = get_norm_text(str2);
  ret = strcmp(norm_str1, norm_str2) == 0;
  free(norm_str1);
  free(norm_str2);
  return ret;
}
