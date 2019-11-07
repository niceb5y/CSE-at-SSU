#include "transpile.h"

/*
 * 코드 변환기 관련 함수
 */

// 코드 변환기를 초기화 하는 함수
void transpile_init()
{
  brace_level = 0;
  class_context[0] = 0;
  file_list_count = 0;
  func_list_count = 0;
  is_comment = false;
  indent_next = false;
  skip_next = false;
  is_waiting_paren = false;
  is_main_func = false;
  source_file = NULL;
  header_table.head = load_header_map();
}

// 코드를 한 줄 변환하는 함수
source_code *transpile(const char *file_name, const char *output_path,
                       const char *line, pid_t pid)
{
  // 문자열 좌우 공백 제거
  char *line_trim = string_trim(line);
  int line_trim_len = strlen(line_trim);

  // 토큰 변수 초기화
  token_type token[TOKEN_LEN];
  memset(&token, 0, sizeof(token));
  int token_count = 0;

  // 들여쓰기 레벨을 설정한다.
  int indent_level = brace_level;

  /*
   * 토큰 분리
   */
  for (int i = 0; i < line_trim_len;) {
    // 주석인 경우
    if (is_comment) {
      // '/* */' 주석의 끝을 만나지 못한 경우 끝을 만날때까지 추가
      int count = 0;
      while (i + count < line_trim_len &&
             strncmp(line_trim + i + count, "*/", 2) != 0) {
        count++;
      }
      strncat(token[token_count].content, line_trim + i, count);
      token[token_count].type = TOK_COMMENT;
      i += count;
      if (strncmp(line_trim + i, "*/", 2) == 0) {
        strcat(token[token_count].content, "*/");
        token[token_count].type = TOK_COMMENT;
        i += 2;
        is_comment = false;
      }
      ++token_count;
    } else if (strncmp(line_trim + i, "/*", 2) == 0) {
      // '/* */' 주석의 시작인 경우
      strcpy(token[token_count].content, "/*");
      token[token_count].type = TOK_COMMENT;
      is_comment = true;
      i += 2;
    } else if (strncmp(line_trim + i, "//", 2) == 0) {
      // '//' 주석인 경우
      strcpy(token[token_count].content, line_trim + i);
      token[token_count].type = TOK_COMMENT;
      i = line_trim_len - 1;
      ++token_count;
    } else if (line_trim[i] == '"') {
      // 문자열을 만난 경우
      int end = i + 1;
      bool is_escaped = false;
      while (!(!is_escaped && line_trim[end] == '"')) {
        if (is_escaped) {
          is_escaped = false;
        } else if (line_trim[end] == '\\') {
          is_escaped = true;
        }
        ++end;
      }
      strncpy(token[token_count].content, line_trim + i, end - i + 1);
      token[token_count].content[end - i + 1] = '\0';
      token[token_count].type = TOK_STRING;
      i = end;
      ++token_count;
    } else if (isalpha(line_trim[i]) || line_trim[i] == '_') {
      // 식별자를 만난 경우
      int end = i + 1;
      while (isalnum(line_trim[end]) || line_trim[end] == '_') {
        ++end;
      }
      strncpy(token[token_count].content, line_trim + i, end - i);
      token[token_count].content[end - i] = '\0';
      token[token_count].type = TOK_IDENTIFIER;
      i = end - 1;
      ++token_count;
    } else if (isdigit(line_trim[i])) {
      // 숫자를 만난 경우
      int end = i + 1;
      while (isdigit(line_trim[end])) {
        ++end;
      }
      strncpy(token[token_count].content, line_trim + i, end - i);
      token[token_count].content[end - i] = '\0';
      token[token_count].type = TOK_NUMBER;
      i = end - 1;
      ++token_count;
    } else if (!isspace(line_trim[i])) {
      // 연산자를 만난 경우
      token[token_count].content[0] = line_trim[i];
      token[token_count].content[1] = 0;
      token[token_count].type = TOK_OPERATOR;
      // 들여쓰기 레벨 설정
      if (line_trim[i] == '{')
        brace_level++;
      else if (line_trim[i] == '}') {
        brace_level--;
        indent_level--;
      }
      ++token_count;
    }
    ++i;
  }

  /*
   * 토큰 전처리
   */
  int i, j;
  // 여러 토큰으로 이루어진 연산자를 합친다.
  for (i = 0, j = 0; i < token_count; ++i, ++j) {
    char *repeats[] = {"&", "|", "+", "-", ">", "<", "="};
    char *equals[] = {"+", "-",  "*",  "/", "%", "!", "<",
                      ">", "<<", ">>", "^", "|", "~"};
    bool matched = false;
    for (int k = 0; k < 7; ++k) {
      char *sym = repeats[k];
      if (j > 0 && strcmp(token[i].content, sym) == 0 &&
          strcmp(token[j - 1].content, sym) == 0) {
        matched = true;
        --j;
        strcat(token[j].content, sym);
        break;
      }
    }
    if (matched)
      continue;
    for (int k = 0; k < 13; ++k) {
      char *sym = equals[k];
      if (j > 0 && strcmp(token[i].content, "=") == 0 &&
          strcmp(token[j - 1].content, sym) == 0) {
        matched = true;
        --j;
        strcat(token[j].content, "=");
        break;
      }
    }
    if (matched)
      continue;
    if (j > 0 && strcmp(token[i].content, ".") == 0 &&
        token[j - 1].type == TOK_IDENTIFIER) {
      bool object_matched = false;
      for (int k = 0; k < object_list_count; ++k) {
        if (strcmp(object_list[k], token[j - 1].content) == 0) {
          object_matched = true;
          break;
        }
      }
      if (object_matched) {
        j -= 2;
      } else {
        --j;
        strcat(token[j].content, ".");
      }
      continue;
    }
    if (j > 0 && token[i].type == TOK_IDENTIFIER &&
        string_ends_with(token[j - 1].content, ".") &&
        token[j - 1].type == TOK_IDENTIFIER) {
      --j;
      strcat(token[j].content, token[i].content);
      continue;
    }
    if (i > 0 && strcmp(token[i].content, ">") == 0 &&
        strcmp(token[j - 1].content, "-") == 0) {
      --j;
      strcat(token[j].content, ">");
    } else {
      if (i != j) {
        strcpy(token[j].content, token[i].content);
        token[j].type = token[i].type;
      }
    }
  }
  token_count = j;

  // 문자열의 길이가 0인 경우 변환을 마친다.
  if (strlen(line_trim) == 0)
    return source_file;

  /*
   * 토큰 변환
   */
  token_type token_result[TOKEN_LEN];
  memset(&token_result, 0, sizeof(token_result));
  int token_result_count = 0;

  // 토큰을 규칙에 따라 변환한다.
  for (i = 0, j = 0; i < token_count; ++i, ++j) {
    // 다음 토큰을 생략할 필요가 있는 경우 생략한다.
    if (skip_next) {
      --j;
      skip_next = false;
      continue;
    }

    // 다음과 같은 행으로 시작하는 문자열은 무시한다.
    if (strcmp(token[i].content, "import") == 0 ||
        strcmp(token[i].content, "Scanner") == 0) {
      break;
    }

    // 지원하지 않거나 불필요한 토큰은 제거한다.
    if (strcmp(token[i].content, "public") == 0 ||
        strcmp(token[i].content, "private") == 0 ||
        strcmp(token[i].content, "default") == 0 ||
        strcmp(token[i].content, "protected") == 0 ||
        strcmp(token[i].content, "static") == 0 ||
        strcmp(token[i].content, "throws") == 0 ||
        strcmp(token[i].content, "IOException") == 0) {
      --j;
      continue;
    }

    // 메인 함수 변환
    if (strcmp(token[i].content, "main") == 0 &&
        strcmp(token[0].content, "public") == 0) {
      strcpy(token_result[0].content, "int");
      token_result[0].type = TOK_IDENTIFIER;
      strcpy(token_result[1].content, "main");
      token_result[1].type = TOK_IDENTIFIER;
      strcpy(token_result[2].content, "(");
      token_result[2].type = TOK_OPERATOR;
      strcpy(token_result[3].content, "void");
      token_result[3].type = TOK_IDENTIFIER;
      strcpy(token_result[4].content, ")");
      token_result[4].type = TOK_OPERATOR;
      j = 4;
      while (strcmp(token[i].content, ")") != 0) {
        ++i;
      }
      is_main_func = true;
      continue;
    }

    // 새로운 클래스가 시작되는 경우
    if (i > 0 && strcmp(token[i - 1].content, "class") == 0 &&
        token[i].type == TOK_IDENTIFIER) {
      // 클래스 컨텍스트를 변환한다.
      // 파일 저장은 클래스 컨텍스트 단위로 저장된다.
      strcpy(class_context, token[i].content);
      // 파일 리스트에 추가
      // Makefile을 만들때 사용된다.
      sprintf(file_list[file_list_count], "%s.c", class_context);
      ++file_list_count;
      // 소스 파일 초기화
      source_file = (source_code *)malloc(sizeof(source_code));
      source_file->header = (line_node *)malloc(sizeof(line_node));
      source_file->header->next = NULL;
      source_file->user_header = (line_node *)malloc(sizeof(line_node));
      source_file->user_header->next = NULL;
      source_file->define = (line_node *)malloc(sizeof(line_node));
      source_file->define->next = NULL;
      source_file->body = (line_node *)malloc(sizeof(line_node));
      source_file->body->next = NULL;
      source_file->header_file = (line_node *)malloc(sizeof(line_node));
      source_file->header_file->next = NULL;
      skip_next = true;
      j = -1;
      memset(object_list, 0, sizeof(object_list));
      object_list_count = 0;
      continue;
    }

    // 함수가 끝나는 경우
    if (strcmp(token[i].content, "}") == 0 && indent_level == 1) {
      is_main_func = false;
    }

    // 클래스 파일이 끝나는경우
    if (strcmp(token[i].content, "}") == 0 && indent_level == 0) {
      // 메인 함수를 포함한 클래스가 아닌 경우
      // 헤더 파일을 생성한다.
      if (strcmp(class_context, file_name) != 0) {
        char class_name[TOKEN_LEN];
        sprintf(class_name, "\"%s.h\"", class_context);
        add_user_include(class_name);
      }

      // 자식 프로세스가 아닌 경우 실제로 파일을 생성한다.
      if (pid > 0) {
        char output_file_name[PATH_MAX];
        strcpy(output_file_name, output_path);
        strcat(output_file_name, class_context);
        strcat(output_file_name, ".c");

        FILE *fp;

        if ((fp = fopen(output_file_name, "w")) == NULL) {
          print_fatal_error("file open error for '%s'", output_file_name);
        }
        // '.c' 파일 기록
        write_source_code(source_file, fp);
        fclose(fp);

        if (strcmp(class_context, file_name) != 0) {
          output_file_name[strlen(output_file_name) - 1] = 'h';
          if ((fp = fopen(output_file_name, "w")) == NULL) {
            print_fatal_error("file open error for '%s'", output_file_name);
          }
          // 필요한 경우 '.h' 파일 기록
          write_source_header(source_file, fp);
          fclose(fp);
        }
      }

      // 할당된 메모리 회수

      class_context[0] = 0;

      line_node *header = source_file->header;
      while (header != NULL) {
        line_node *tmp = header->next;
        free(header);
        header = tmp;
      }

      line_node *user_header = source_file->user_header;
      while (user_header != NULL) {
        line_node *tmp = user_header->next;
        free(user_header);
        user_header = tmp;
      }

      line_node *header_file = source_file->header_file;
      while (header_file != NULL) {
        line_node *tmp = header_file->next;
        free(header_file);
        header_file = tmp;
      }

      line_node *define = source_file->define;
      while (define != NULL) {
        line_node *tmp = define->next;
        free(define);
        define = tmp;
      }

      line_node *body = source_file->body;
      while (body != NULL) {
        line_node *tmp = body->next;
        free(body);
        body = tmp;
      }

      free(source_file);
      source_file = NULL;

      --j;
      break;
    }

    // System.out.printf -> printf 변환
    if (strcmp(token[i].content, "System.out.printf") == 0) {
      strcpy(token_result[j].content, "printf");
      add_func_map("System.out.printf", "printf");
      add_func_header("printf");
      token_result[j].type = token[i].type;
      continue;
    }

    // final 변수의 경우 #define 처리
    if (i > 4 && strcmp(token[i - 5].content, "final") == 0 &&
        token[i - 4].type == TOK_IDENTIFIER &&
        token[i - 3].type == TOK_IDENTIFIER &&
        strcmp(token[i - 2].content, "=") == 0 &&
        strcmp(token[i].content, ";") == 0) {
      line_node *new_define_line = malloc(sizeof(line_node));
      new_define_line->next = NULL;
      sprintf(new_define_line->line, "#define %s %s", token[i - 3].content,
              token[i - 1].content);
      line_node *cur = source_file->define;
      while (cur->next != NULL) {
        cur = cur->next;
      }
      cur->next = new_define_line;
      j -= 6;
      continue;
    }

    // null -> NULL 변환
    if (strcmp(token[i].content, "null") == 0) {
      strcpy(token_result[j].content, "NULL");
      token_result[j].type = token[i].type;
      continue;
    }

    // 파일 이름 타입 변환
    if (i > 0 && strcmp(token[i - 1].content, "File") == 0 &&
        token[i].type == TOK_IDENTIFIER) {
      strcpy(token_result[j - 1].content, "char");
      strcpy(token_result[j].content, "*");
      strcat(token_result[j].content, token[i].content);
      token[j].type = TOK_IDENTIFIER;
      continue;
    }

    // 파일 이름 타입 변환
    if (i > 4 && strcmp(token[i - 4].content, "new") == 0 &&
        strcmp(token[i - 3].content, "File") == 0 &&
        strcmp(token[i - 2].content, "(") == 0 &&
        token[i - 1].type == TOK_STRING && strcmp(token[i].content, ")") == 0) {
      j -= 4;
      strcpy(token_result[j].content, token[i - 1].content);
      token[j].type = TOK_STRING;
      continue;
    }

    // FileWriter -> fopen 변환
    if (i > 0 && strcmp(token[i - 1].content, "FileWriter") == 0 &&
        token[i].type == TOK_IDENTIFIER) {
      strcpy(token_result[j - 1].content, "FILE");
      strcpy(token_result[j].content, "*");
      strcat(token_result[j].content, token[i].content);
      token[j].type = TOK_IDENTIFIER;
      continue;
    }

    // FileWriter -> fopen 변환
    if (i > 8 && strcmp(token[i - 7].content, "new") == 0 &&
        strcmp(token[i - 6].content, "FileWriter") == 0 &&
        strcmp(token[i - 5].content, "(") == 0 &&
        strcmp(token[i - 3].content, ",") == 0 &&
        token[i - 2].type == TOK_IDENTIFIER &&
        strcmp(token[i - 1].content, ")") == 0 &&
        strcmp(token[i].content, ";") == 0) {
      j -= 7;
      strcpy(token_result[j].content, "fopen");
      token[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, "(");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, token[i - 4].content);
      token[j].type = token[i - 4].type;
      strcpy(token_result[++j].content, ",");
      token[j].type = TOK_OPERATOR;
      char *mode = strcmp(token[i - 2].content, "false") ? "\"a\"" : "\"w\"";
      strcpy(token_result[++j].content, mode);
      token[j].type = TOK_STRING;
      strcpy(token_result[++j].content, ")");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, ";");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "if");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "(");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, token[i - 9].content);
      token[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, "==");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "NULL");
      token[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, ")");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "exit");
      token[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, "(");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "1");
      token[j].type = TOK_NUMBER;
      strcpy(token_result[++j].content, ")");
      token[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, ";");
      token[j].type = TOK_OPERATOR;
      // 에러 처리 추가
      add_func_header("exit");
      continue;
    }

    // .write -> fprintf 변환
    if (i > 0 && string_ends_with(token[i - 1].content, ".write") &&
        strcmp(token[i].content, "(") == 0) {
      strcpy(token_result[j - 1].content, "fprintf");
      strcpy(token_result[j].content, "(");
      token_result[j].type = TOK_OPERATOR;
      strncpy(token_result[++j].content, token[i - 1].content,
              strlen(token[i - 1].content) - 6);
      token_result[j].content[strlen(token[i - 1].content) - 6] = '\0';
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, ",");
      token_result[j].type = TOK_OPERATOR;
      add_func_map(token[i - 1].content, "fprintf");
      add_func_header("fprintf");
      continue;
    }

    // .flush -> fflush 변환
    if (i > 0 && string_ends_with(token[i - 1].content, ".flush") &&
        strcmp(token[i].content, "(") == 0) {
      strcpy(token_result[j - 1].content, "fflush");
      strcpy(token_result[j].content, "(");
      token_result[j].type = TOK_OPERATOR;
      strncpy(token_result[++j].content, token[i - 1].content,
              strlen(token[i - 1].content) - 6);
      token_result[j].content[strlen(token[i - 1].content) - 6] = '\0';
      token_result[j].type = TOK_IDENTIFIER;
      add_func_map(token[i - 1].content, "fflush");
      add_func_header("fflush");
      continue;
    }

    // .close -> fclose 변환
    if (i > 0 && string_ends_with(token[i - 1].content, ".close") &&
        strcmp(token[i].content, "(") == 0) {
      strcpy(token_result[j - 1].content, "fclose");
      strcpy(token_result[j].content, "(");
      token_result[j].type = TOK_OPERATOR;
      strncpy(token_result[++j].content, token[i - 1].content,
              strlen(token[i - 1].content) - 6);
      token_result[j].content[strlen(token[i - 1].content) - 6] = '\0';
      token_result[j].type = TOK_IDENTIFIER;
      add_func_map(token[i - 1].content, "fclose");
      add_func_header("fclose");
      continue;
    }

    // .nextInt -> scanf 변환
    if (i > 0 && strcmp(token[i - 1].content, "=") == 0 &&
        string_ends_with(token[i].content, ".nextInt")) {
      add_func_map(token[i].content, "scanf");
      add_func_header("scanf");
      j = -1;
      if (strcmp(token[0].content, "int") == 0) {
        strcpy(token_result[++j].content, "int");
        token_result[j].type = TOK_IDENTIFIER;
        strcpy(token_result[++j].content, token[i - 2].content);
        token_result[j].type = TOK_IDENTIFIER;
        strcpy(token_result[++j].content, ";");
        token_result[j].type = TOK_OPERATOR;
      }
      strcpy(token_result[++j].content, "scanf");
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, "(");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "\"%d\"");
      token_result[j].type = TOK_STRING;
      strcpy(token_result[++j].content, ",");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "&");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, token[i - 2].content);
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, ")");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, ";");
      token_result[j].type = TOK_OPERATOR;
      ++j;
      break;
    }

    // 배열 -> 포인터 변환
    if (i > 2 && token[i - 3].type == TOK_IDENTIFIER &&
        strcmp(token[i - 2].content, "[") == 0 &&
        strcmp(token[i - 1].content, "]") == 0 &&
        token[i].type == TOK_IDENTIFIER) {
      j -= 2;
      strcpy(token_result[j].content, "*");
      strcat(token_result[j].content, token[i].content);
      token_result[j].type = TOK_IDENTIFIER;
      continue;
    }

    // 메인 함수 리턴 변환
    if (i > 0 && strcmp(token[i - 1].content, "return") == 0 &&
        strcmp(token[i].content, ";") == 0 && is_main_func) {
      strcpy(token_result[j].content, "0");
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, ";");
      token_result[j].type = TOK_OPERATOR;
      continue;
    }

    // new [] -> malloc 변환
    if (i > 3 && strcmp(token[i - 4].content, "new") == 0 &&
        token[i - 3].type == TOK_IDENTIFIER &&
        strcmp(token[i - 2].content, "[") == 0 &&
        token[i - 1].type == TOK_IDENTIFIER &&
        strcmp(token[i].content, "]") == 0) {
      j -= 4;
      strcpy(token_result[j].content, "(");
      strcat(token_result[j].content, token[i - 3].content);
      strcat(token_result[j].content, " *)");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "malloc");
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, "(");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, "sizeof(");
      strcat(token_result[j].content, token[i - 3].content);
      strcat(token_result[j].content, ")");
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, "*");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, token[i - 1].content);
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, ")");
      token_result[j].type = TOK_OPERATOR;
      add_func_header("malloc");
      continue;
    }

    // 클래스 생성자 변환
    if (i > 2 && strcmp(token[i - 2].content, class_context) == 0 &&
        strcmp(token[i - 1].content, "(") == 0 &&
        strcmp(token[i].content, ")") == 0) {
      j -= 2;
      strcpy(token_result[j].content, "void");
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, token[i - 2].content);
      token_result[j].type = TOK_IDENTIFIER;
      strcpy(token_result[++j].content, "(");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, ")");
      token_result[j].type = TOK_OPERATOR;
      continue;
    }

    // 사용자 정의 객체 생성 변환
    if (i > 5 && token[i - 6].type == TOK_IDENTIFIER &&
        token[i - 5].type == TOK_IDENTIFIER &&
        strcmp(token[i - 4].content, "=") == 0 &&
        strcmp(token[i - 3].content, "new") == 0 &&
        token[i - 2].type == TOK_IDENTIFIER &&
        strcmp(token[i - 1].content, "(") == 0 &&
        strcmp(token[i].content, ")") == 0 &&
        strcmp(token[i - 6].content, token[i - 2].content) == 0) {
      j -= 6;
      strcpy(token_result[++j].content, "(");
      token_result[j].type = TOK_OPERATOR;
      strcpy(token_result[++j].content, ")");
      token_result[j].type = TOK_OPERATOR;
      strcpy(object_list[object_list_count], token[i - 5].content);
      ++object_list_count;
      char class_name[TOKEN_LEN];
      sprintf(class_name, "\"%s.h\"", token[i - 6].content);
      add_user_include(class_name);
      continue;
    }
    strcpy(token_result[j].content, token[i].content);
    token_result[j].type = token[i].type;
  }

  token_result_count = j;

  // 토큰 길이가 0보다 크면 새 라인 노드를 생성한다.
  if (token_result_count > 0) {
    line_node *new_line = (line_node *)malloc(sizeof(line_node));
    new_line->next = NULL;
    new_line->line[0] = 0;

    // 인덴트 구현
    for (i = 0; i < indent_level - 1; ++i) {
      strcat(new_line->line, "  ");
    }
    if (indent_next) {
      strcat(new_line->line, "  ");
      indent_next = false;
    }

    // 토큰 간 띄어쓰기 구현
    for (i = 0; i < token_result_count; ++i) {
      if (i > 0) {
        if (strcmp(token_result[i].content, ";") != 0 &&
            strcmp(token_result[i].content, ",") != 0 &&
            strcmp(token_result[i].content, ")") != 0 &&
            !(string_ends_with(token_result[i - 1].content, ")") &&
              token_result[i].type != TOK_OPERATOR) &&
            strcmp(token_result[i - 1].content, "(") != 0 &&
            strcmp(token_result[i].content, "[") != 0 &&
            strcmp(token_result[i].content, "]") != 0 &&
            strcmp(token_result[i - 1].content, "[") != 0 &&
            strcmp(token_result[i - 1].content, "&") != 0 &&
            !((strcmp(token_result[i].content, "++") == 0 ||
               strcmp(token_result[i].content, "--") == 0) &&
              token_result[i - 1].type == TOK_IDENTIFIER) &&
            !((strcmp(token_result[i - 1].content, "++") == 0 ||
               strcmp(token_result[i - 1].content, "--") == 0) &&
              token_result[i].type == TOK_IDENTIFIER) &&
            !(strcmp(token_result[i].content, "(") == 0 &&
              (strcmp(token_result[i - 1].content, "for") != 0 &&
               strcmp(token_result[i - 1].content, "if") != 0 &&
               strcmp(token_result[i - 1].content, "while") != 0))) {
          strcat(new_line->line, " ");
        }
      }
      // 토큰 삽입
      strcat(new_line->line, token_result[i].content);

      // 라인의 끝이 ')'로 끝나면 다음 라인을 들여쓰기한다.
      if (i == token_result_count - 1 &&
          strcmp(token_result[i].content, ")") == 0) {
        indent_next = true;
      }
    }
    // 라인을 소스 파일에 삽입
    line_node *body = source_file->body;
    while (body->next != NULL) {
      body = body->next;
    }
    body->next = new_line;
  }

  // 토큰 길이가 0보다 크면 헤더 파일 노드를 생성한다.
  if (token_result_count > 0) {
    line_node *header_file_line = malloc(sizeof(line_node));
    header_file_line->next = NULL;
    // 함수 프로토타입인 경우
    if (token_result_count > 3 && token_result[0].type == TOK_IDENTIFIER &&
        token_result[1].type == TOK_IDENTIFIER &&
        strcmp(token_result[2].content, "(") == 0) {
      i = 0;
      is_waiting_paren = true;
    }
    // 이전 라인에서 ')'을 만나지 못한 경우
    else if (is_waiting_paren) {
      i = 0;
    }
    // 아닌 경우 건너뛰기
    else {
      i = token_result_count;
    }

    // 프로토타입 토큰 간 띄어쓰기 구현
    for (; i < token_result_count; ++i) {
      if (i > 0 && strcmp(token_result[i].content, ";") != 0 &&
          strcmp(token_result[i].content, ",") != 0 &&
          strcmp(token_result[i].content, ")") != 0 &&
          !(string_ends_with(token_result[i - 1].content, ")") &&
            token_result[i].type != TOK_OPERATOR) &&
          strcmp(token_result[i - 1].content, "(") != 0 &&
          strcmp(token_result[i].content, "[") != 0 &&
          strcmp(token_result[i].content, "]") != 0 &&
          strcmp(token_result[i - 1].content, "[") != 0 &&
          strcmp(token_result[i - 1].content, "&") != 0 &&
          !(strcmp(token_result[i].content, "(") == 0 &&
            (strcmp(token_result[i - 1].content, "for") != 0 &&
             strcmp(token_result[i - 1].content, "if") != 0 &&
             strcmp(token_result[i - 1].content, "while") != 0))) {
        strcat(header_file_line->line, " ");
      }
      strcat(header_file_line->line, token_result[i].content);
      // ')'를 만난 경우
      if (strcmp(token_result[i].content, ")") == 0) {
        is_waiting_paren = false;
        // 세미콜론 삽입
        strcat(header_file_line->line, ";");
        break;
      }
    }

    // 헤더 파일 라인 길이가 0보다 크면 라인을 삽입한다.
    if (strlen(header_file_line->line) > 0) {
      line_node *header_file = source_file->header_file;
      while (header_file->next != NULL) {
        header_file = header_file->next;
      }
      header_file->next = header_file_line;
    } else {
      // 길이가 0인 경우 메모리 반환
      free(header_file_line);
    }
  }

  // 트림된 라인 메모리 반환
  free(line_trim);

  // 소스 코드 리턴
  // 한 줄씩 변환하여 출력할 때 사용
  return source_file;
}

// 코드 변환기를 종료하고 필요한 메시지를 출력하는 함수
void transpile_finalize(const char *output_path, const char *file_name,
                        bool print_java, bool print_c, bool print_size,
                        bool print_line_num, bool print_func_map)
{
  // 파일 변환이 종료되었다는 메시지를 출력한다.
  for (int i = 0; i < file_list_count; ++i) {
    printf("file %s converting is finished\n", file_list[i]);
  }
  // 변환한 자바 파일 출력
  if (print_java) {
    char path[PATH_MAX];
    char title[FILENAME_MAX];
    sprintf(path, "%s%s.java", output_path, file_name);
    sprintf(title, "%s.java", file_name);
    printf("\n");
    file_print_code(title, path);
  }
  // 변환된 C 파일 출력
  if (print_c) {
    char path[PATH_MAX];
    char title[FILENAME_MAX];
    for (int i = 0; i < file_list_count; ++i) {
      if (strncmp(file_list[i], file_name, strlen(file_name)) != 0) {
        sprintf(path, "%s%s", output_path, file_list[i]);
        path[strlen(path) - 1] = 'h';
        sprintf(title, "%s", file_list[i]);
        title[strlen(title) - 1] = 'h';
        printf("\n");
        file_print_code(title, path);
      }
      sprintf(path, "%s%s", output_path, file_list[i]);
      sprintf(title, "%s", file_list[i]);
      printf("\n");
      file_print_code(title, path);
    }
  }
  // 파일 크기 출력
  if (print_size) {
    // 변환한 자바 파일 크기 출력
    char path[PATH_MAX];
    sprintf(path, "%s%s.java", output_path, file_name);
    printf("\n");
#ifdef __APPLE__
    printf("%s file size is %lld bytes\n", path, file_get_size(path));
#else
    printf("%s file size is %ld bytes\n", path, file_get_size(path));
#endif
    // 변환된 C 파일 크기 출력
    for (int i = 0; i < file_list_count; ++i) {
      if (strncmp(file_list[i], file_name, strlen(file_name)) != 0) {
        // 헤더 파일이 존재하는 경우 헤더 파일 크기 출력
        sprintf(path, "%s%s", output_path, file_list[i]);
        path[strlen(path) - 1] = 'h';
#ifdef __APPLE__
        printf("%s file size is %lld bytes\n", path, file_get_size(path));
#else
        printf("%s file size is %ld bytes\n", path, file_get_size(path));
#endif
      }
      sprintf(path, "%s%s", output_path, file_list[i]);
#ifdef __APPLE__
      printf("%s file size is %lld bytes\n", path, file_get_size(path));
#else
      printf("%s file size is %ld bytes\n", path, file_get_size(path));
#endif
    }
  }
  // 파일 라인 수 출력
  if (print_line_num) {
    // 변환한 자바 파일 라인 수 출력
    char path[PATH_MAX];
    sprintf(path, "%s%s.java", output_path, file_name);
    printf("\n");
    printf("%s line number is %d lines\n", path, file_get_line(path));
    for (int i = 0; i < file_list_count; ++i) {
      // 변환된 C 파일 라인 수 출력
      if (strncmp(file_list[i], file_name, strlen(file_name)) != 0) {
        // 헤더 파일이 존재하는 경우 헤더 파일 라인 수 출력
        sprintf(path, "%s%s", output_path, file_list[i]);
        path[strlen(path) - 1] = 'h';
        printf("%s line number is %d lines\n", path, file_get_line(path));
      }
      sprintf(path, "%s%s", output_path, file_list[i]);
      printf("%s line number is %d lines\n", path, file_get_line(path));
    }
  }
  // 함수 매핑 출력
  if (print_func_map) {
    printf("\n");
    for (int i = 0; i < func_list_count; ++i) {
      printf("%d: %s() -> %s()\n", (i + 1), func_list[i].java, func_list[i].c);
    }
  }
}

/*
 * 코드 출력 관련 함수
 */

// 소스 코드를 쓰는 함수
void write_source_code(source_code *code, FILE *fp)
{
  // 표준 출력에 출력하고 코드가 NULL인 경우
  // == 현재 어느 클래스에도 포함되지 않은 경우
  // 빈 칸 출력
  if (code == NULL) {
    if (fp == stdout) {
      fprintf(fp, "--------\n\n--------\n");
    }
    return;
  }
  // 표준 출력에 출력하는 경우 포맷을 다르게 출력
  if (fp == stdout) {
    fprintf(fp, "--------\n%s.c\n--------\n", class_context);
  }
  // 표준 출력에는 라인 수 출력
  int line_num = 0;
  // 헤더 인클루드 출력
  line_node *header = code->header;
  while (header->next != NULL) {
    if (fp == stdout) {
      fprintf(fp, "%3d  ", ++line_num);
    }
    fprintf(fp, "%s\n", header->next->line);
    header = header->next;
  }
  // 사용자 정의 헤더 인클루드 출력
  line_node *user_header = code->user_header;
  while (user_header->next != NULL) {
    if (fp == stdout) {
      fprintf(fp, "%3d  ", ++line_num);
    }
    fprintf(fp, "%s\n", user_header->next->line);
    user_header = user_header->next;
  }
  // DEFINE 전처리문 출력
  line_node *define = code->define;
  while (define->next != NULL) {
    if (fp == stdout) {
      fprintf(fp, "%3d  ", ++line_num);
    }
    fprintf(fp, "%s\n", define->next->line);
    define = define->next;
  }
  // 전처리문과 코드 사이 공백 삽입
  if (fp == stdout) {
    fprintf(fp, "%3d  ", ++line_num);
  }
  fprintf(fp, "\n");
  // 코드 출력
  line_node *body = code->body;
  while (body->next != NULL) {
    if (fp == stdout) {
      fprintf(fp, "%3d  ", ++line_num);
    }
    fprintf(fp, "%s\n", body->next->line);
    body = body->next;
  }
}

// 소스 코드의 헤더 파일을 쓰는 함수
void write_source_header(source_code *code, FILE *fp)
{
  line_node *header_file = code->header_file;
  while (header_file->next != NULL) {
    fprintf(fp, "%s\n", header_file->next->line);
    header_file = header_file->next;
  }
}

// 'Makefile'을 쓰는 함수
void write_makefile(const char *output_path, const char *file_name)
{
  FILE *fp;
  char makefile_path[PATH_MAX];
  sprintf(makefile_path, "%s%s_Makefile", output_path, file_name);
  if ((fp = fopen(makefile_path, "w")) == NULL) {
    print_fatal_error("open file error for %s", makefile_path);
  }
  fprintf(fp, "%s:", file_name);
  for (int i = 0; i < file_list_count; ++i) {
    fprintf(fp, " %s", file_list[i]);
  }
  fprintf(fp, "\n\tgcc");
  for (int i = 0; i < file_list_count; ++i) {
    fprintf(fp, " %s", file_list[i]);
  }
  fprintf(fp, " -o %s;\n\n.PHONY: clean\nclean:\n\trm -rf ./%s\n", file_name,
          file_name);
  fclose(fp);
}

/*
 * 기타 함수
 */

// 헤더 테이블을 불러오는 함수
header_map_node *load_header_map()
{
  header_map_node *ret = (header_map_node *)malloc(sizeof(header_map_node));
  ret->next = NULL;

  // header_table.txt 로딩
  char *header_table_file = file_get_data("header_table.txt");
  if (header_table_file == NULL) {
    // 헤더 테이블 파일이 존재하지 않은 경우 에러 출력
    print_fatal_error("header table not exist.");
  }

  char *line;
  char *save_ptr;

  // 헤더 테이블 라인 간 분리
  line = strtok_r(header_table_file, "\n", &save_ptr);
  while (line != NULL) {

    // 링크드 리스트 생성
    header_map_node *node = (header_map_node *)malloc(sizeof(header_map_node));
    node->header = (header_node *)malloc(sizeof(header_node));
    node->header->next = NULL;
    node->next = NULL;

    char *func;
    char *includes;
    char *save_ptr_2;

    // 띄어쓰기 간 분리
    func = strtok_r(line, " ", &save_ptr_2);
    strcpy(node->func, func);
    while (strtok_r(NULL, " ", &save_ptr_2) != NULL) {
      includes = strtok_r(NULL, " ", &save_ptr_2);
      if (includes == NULL) {
        // 헤더 테이블 포맷이 올바르지 않은 경우 에러 출력
        print_fatal_error("header table format error");
      }
      header_node *header = (header_node *)malloc(sizeof(header_node));
      header->next = NULL;
      strcpy(header->header, includes);
      header_node *cur = node->header;
      while (cur->next != NULL) {
        cur = cur->next;
      }
      cur->next = header;
    }
    header_map_node *cur = ret;
    while (cur->next != NULL) {
      cur = cur->next;
    }
    cur->next = node;
    line = strtok_r(NULL, "\n", &save_ptr);
  }
  free(header_table_file);
  return ret;
}

// 함수에 필요한 헤더를 인클루드하는 함수
void add_func_header(const char *func_name)
{
  header_map_node *cur = header_table.head->next;
  while (cur != NULL) {
    if (strcmp(cur->func, func_name) == 0) {
      header_node *cur_header = cur->header->next;
      while (cur_header != NULL) {
        add_include(cur_header->header);
        cur_header = cur_header->next;
      }
      return;
    } else {
      cur = cur->next;
    }
  }
}

// 헤더 파일이 존재하지 않으면 추가하는 함수
void add_include(const char *include_str)
{
  line_node *cur = source_file->header;
  while (cur->next != NULL) {
    // 헤더 파일 중복 검사
    if (string_ends_with(cur->next->line, include_str)) {
      return;
    }
    cur = cur->next;
  }
  // 중복이 없는 경우 헤더 파일 추가
  line_node *new_line = (line_node *)malloc(sizeof(line_node));
  sprintf(new_line->line, "#include %s", include_str);
  new_line->next = NULL;
  cur->next = new_line;
}

// 사용자 정의 헤더 파일을 인클루드 하는 함수
void add_user_include(const char *include_str)
{
  line_node *cur = source_file->user_header;
  while (cur->next != NULL) {
    // 헤더 파일 중복 검사
    if (string_ends_with(cur->next->line, include_str)) {
      return;
    }
    cur = cur->next;
  }
  // 중복이 없는 경우 헤더 파일 추가
  line_node *new_line = (line_node *)malloc(sizeof(line_node));
  sprintf(new_line->line, "#include %s", include_str);
  new_line->next = NULL;
  cur->next = new_line;
}

// JAVA 함수 - C 함수의 매핑을 추가하는 함수
void add_func_map(const char *java_func, const char *c_func)
{
  for (int i = 0; i < func_list_count; ++i) {
    if (strcmp(java_func, func_list[i].java) == 0) {
      return;
    }
  }
  strcpy(func_list[func_list_count].java, java_func);
  strcpy(func_list[func_list_count].c, c_func);
  ++func_list_count;
}
