//
//  util.cpp
//  AOE
//
//  Created by 김승호 on 2015. 5. 31..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "util.h"

int parseInt(const char **str)
{
  int num = 0;
  bool initial = true;
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
  return num;
}

int max(int a, int b)
{
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

int min(int a, int b)
{
  if (a < b) {
    return a;
  } else {
    return b;
  }
}