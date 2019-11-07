//
//  stringlist.h
//  StringSort
//
//  Created by 김승호 on 2015. 6. 10..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __StringSort__stringlist__
#define __StringSort__stringlist__

#include <iostream>
#include <cstdio>
#include "util.h"

using namespace std;

class StringList
{
  string **list;
  int capacity;
  int size;
  void quicksort(int left, int right);

public:
  StringList();
  ~StringList();
  void add(string *);
  void print();
  void quicksort();
};

#endif /* defined(__StringSort__stringlist__) */
