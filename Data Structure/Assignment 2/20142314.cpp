//
//  main.cpp
//  fixyou
//
//  Created by 김승호 on 2015. 4. 5..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include <iostream>
#include "expression.h"

using namespace std;

int main(int argc, const char *argv[])
{
  string line;
  bool init = true;
  while (getline(cin, line)) {
    if (init) { //처음 루프가 아닐때만 줄 바꾸기
      init = false;
    } else {
      cout << endl;
    }
    expression e(line); //초기화
    e.postfix();        //변환
  }
  return 0;
}
