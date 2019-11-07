//
//  20142314.h
//  polyCalc
//
//  Created by 김승호 on 2015. 3. 22..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include <iostream>
#include <fstream>

using namespace std;

#define ISINT(x) (x >= '0' && x <= '9') // char형에서 숫자인지 확인
#define ISCHAR(x)                                                              \
  ((x >= 'a' && x <= 'z') ||                                                   \
   (x >= 'A' && x <= 'Z')) // char형에서 알파벳 문자인지 확인

int getInt(const char **,
           int = 0); // char형 배열에서 숫자를 가져오는 함수, 숫자를 가져오며
                     // 포인터를 이동시키기 위해 이중 포인터 사용.

class polynomial;
class term;
class var;

class var
{ //변수 클래스
  friend class term;
  friend ostream &operator<<(ostream &, term &);

public:
  var();            //생성자
  int compare(var); //두 변수를 비교

private:
  char c;  //변수의 문자
  int exp; //변수의 지수
};

class term
{ //항 클래스
  friend class polynomial;
  friend ostream &operator<<(ostream &, term &);
  friend ostream &operator<<(ostream &, polynomial &);

public:
  term();              //기본 생성자
  term(const char **); //문자열에서 값을 읽어들여 초기화 하는 생성자
  void init();         //생성자에서 공통적으로 호출되는 부분
  void set(const char **); //생성자의 값을 설정하는 함수
  int compare(term);       //항끼리 비교
  term operator+(term);    //항 덧셈을 위한 +연산자 재지정

private:
  int coef;       //항의 계수
  int size;       //변수의 개수
  var vars[26];   //변수를 저장하는 배열
  int totalExp(); //현재 항의 차수의 합을 출력
};

ostream &operator<<(ostream &, term &); //항 출력을 위한 <<연산자 재지정

class polynomial
{ //다항식 클래스
  friend ostream &operator<<(ostream &, polynomial &);

public:
  polynomial();       //기본 생성자
  polynomial(string); //문자열 값을 읽어들여 초기화 하는 생성자
  void init();        //생성자가 공통적으로 호출하는 부분
  void set(string);   //생성자의 값을 설정하는 함수
  polynomial operator+(polynomial); //다항식끼리의 덧셈을 위한 +연산자 재지정

private:
  term terms[100]; //항을 100개 지원 하는 고정 길이 배열
  int size;        //배열에서 저장된 항의 개수
};

ostream &operator<<(ostream &,
                    polynomial &); //다항식 출력을 위한 <<연산자 재지정

using namespace std;
