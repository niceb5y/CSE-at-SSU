//
//  expression.cpp
//  fixyou
//
//  Created by 김승호 on 2015. 4. 7..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "expression.h"

expression::expression(string s)
{
  initial = true;
  str = s;
}

void expression::postfix()
{
  const string &e = str;
  initial = true;
  int size = 0;
  string *term = new string[e.length() + 1]; //값 서로 분리
  for (int i = 0; i < e.length(); i++) {
    if (i == 0) {        //첫번째 문자인 경우
      if (e[i] == '-') { // '-' 단항 연산자 분리 저장
        term[size] += '-';
      } else if (e[i] == '+') { //'+' 단항 연산자 무시
        continue;
      } else if (e[i] == '(') {
        term[size] += '(';
        size++;
      }
    } else { //첫번째 문자가 아닌 경우
      if (ISINT(e[i - 1]) && !ISINT(e[i])) {
        size++; //숫자 & 기호 조합이면 크기 1 증가
      }
      if (e[i] == '-') { //'-' 연산자 분리 저장
        if (e[i - 1] == '+' || e[i - 1] == '-' || e[i - 1] == '*' ||
            e[i - 1] == '/' || e[i - 1] == '(') {
          term[size] += '-'; //단항 '-'
        } else {
          term[size] += '-';
          size++; //단항이 아닌경우 크기 증가
        }
      } else if (e[i] == '+') { //'+' 연산자 분리 저장
        if (e[i - 1] == '+' || e[i - 1] == '-' || e[i - 1] == '*' ||
            e[i - 1] == '/' || e[i - 1] == '(') {
          continue; //단항 '+' 무시
        } else {
          term[size] += '+'; //단항이 아닌경우 분리 저장
          size++;
        }
      } else if (e[i] == '*' || e[i] == '/' || e[i] == '(' ||
                 e[i] == ')') { //기타 연산자 분리 저장
        term[size] += e[i];
        size++; //사이즈 증가
      }
    }
    if (ISINT(e[i])) { //숫자 분리 저장
      term[size] += e[i];
    }
  }
  if (term[size] != "") { //공백이 아닌 경우 사이즈 증가
    size++;
  }
  term[size] += '#'; //'#' 저장
  stack stk;         //스택 생성
  stk.push("#");     //스택에 "#" 추가
  int i = 0;
  for (string x = term[i]; i < size; x = term[++i]) { //문자열 끝까지
    if (x != "+" && x != "-" && x != "*" && x != "/" && x != "(" && x != ")" &&
        x != "#") {
      print(x);            //숫자인경우 출력
    } else if (x == ")") { //")"인 경우, "("까지 스택에서 꺼내기
      for (; stk.top_str() != "("; stk.pop()) {
        print(stk.top_str());
      }
      stk.pop();
    } else { //기타 연산자
      for (; isp(stk.top_str()) <= icp(x);
           stk.pop()) { //우선순위 비교해서 자신과 우선순위가 같거나 작은것
                        //꺼내기
        print(stk.top_str()); //꺼내진 연산자 출력
      }
      stk.push(x); //새 연산자 삽입
    }
  }
  for (; !stk.isEmpty();) {
    print(stk.top_str()); //남은 연산자 모두 출력
    stk.pop();
  }
  delete[] term;
}

int expression::isp(const string s)
{
  if (s == "*" || s == "/") { //숫자가 작을수록 우선순위가 높음
    return 1;
  } else if (s == "+" || s == "-") {
    return 2;
  } else if (s == "(" || s == "#") {
    return 3;
  } else {
    return 0;
  }
}

int expression::icp(const string s)
{
  if (s == "(") { //숫자가 작을수록 우선순위가 높음
    return 0;
  } else if (s == "*" || s == "/") {
    return 1;
  } else if (s == "+" || s == "-") {
    return 2;
  } else {
    return 0;
  }
}

void expression::print(const string s)
{
  if (s == "#") { //스택의 바닥과 문장의 끝을 알리는 "#"은 출력하지 않는다.
    return;
  }
  if (!initial) { //처음 출력하는게 아니면 공백을 앞에 붙인다.
    cout << " ";
  } else {
    initial = false;
  }
  cout << s;
}
