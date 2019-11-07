//
//  expression.h
//  fixyou
//
//  Created by 김승호 on 2015. 4. 7..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __fixyou__expression__
#define __fixyou__expression__

#include <iostream>
#include "stack.h"

#define ISINT(x) (x >= '0' && x <= '9')

using namespace std;

class expression
{ //식 클래스
public:
  expression(string s); //생성자
  void postfix();       // postfix 변환
private:
  bool initial;             //공백 출력에 사용
  int isp(const string);    //스택 내부 우선순위 결정
  int icp(const string);    //스택 외부 우선순위 결정
  void print(const string); //요소 출력
  string str;               //문자열
};

#endif /* defined(__fixyou__expression__) */
