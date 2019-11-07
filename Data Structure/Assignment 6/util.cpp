//
//  util.cpp
//  StringSort
//
//  Created by 김승호 on 2015. 6. 10..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "util.h"

int compare(const string &str1, const string &str2)
{
  if (str1.length() > str2.length()) {
    for (int i = 0; i < str2.length(); i++) {
      int diff = str1[i] - str2[i];
      if (diff) {
        return diff;
      }
    }
    return 1;
  } else if (str1.length() < str2.length()) {
    for (int i = 0; i < str1.length(); i++) {
      int diff = str1[i] - str2[i];
      if (diff) {
        return diff;
      }
    }
    return -1;
  } else {
    for (int i = 0; i < str1.length(); i++) {
      int diff = str1[i] - str2[i];
      if (diff) {
        return diff;
      }
    }
    return 0;
  }
}

int str_hash(const string &key)
{
  int h1 = key[0] - 64;
  if (h1 < 0) {
    h1 = 0;
  }
  int h = 2 * h1;
  if (key[1] >= 96) {
    h++;
  }
  return h;
}

string int_to_string(int num)
{
  char c[15];
  sprintf(c, "%d", num);
  string s = c;
  return s;
}