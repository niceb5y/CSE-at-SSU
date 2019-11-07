//
//  main.cpp
//  LinkedIntegerList
//
//  Created by 김승호 on 2015. 4. 10..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include <iostream>
#include "linkedlist.h"

using namespace std;

int main(int argc, const char *argv[])
{
  string line;
  List l;                      //리스트 생성
  while (getline(cin, line)) { //라인별로 읽기
    if (line != "") {          //공백이 아닐때만
      l.process(line);         //처리
    }
  }
  return 0;
}
