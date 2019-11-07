//
//  util.cpp
//  MaxHeap
//
//  Created by 김승호 on 2015. 5. 16..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "util.h"

int getInt(const char **str)
{
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
