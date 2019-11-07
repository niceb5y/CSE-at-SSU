//
//  util.cpp
//  LinkedIntegerList
//
//  Created by 김승호 on 2015. 4. 12..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "util.h"

int getInt(const char **str)
{ //숫자를 가져오고, 그만큼 포인터를 이동시킨다
  int num = 0, sign = 1;
  bool initial = true;
  if (**str == '-') {
    sign = -1;
    (*str)++;
  }
  while (ISINT(**str)) {
    if (initial) {
      num = **str - '0';
      initial = false;
    } else {
      num *= 10;
      num += **str - '0';
    }
    (*str)++;
  }
  return num * sign;
}