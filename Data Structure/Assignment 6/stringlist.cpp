//
//  stringlist.cpp
//  StringSort
//
//  Created by 김승호 on 2015. 6. 10..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "stringlist.h"

StringList::StringList()
{
  capacity = 10;
  size = 0;
  list = new string *[capacity]();
}

StringList::~StringList()
{
  for (int i = 0; i < size; i++) {
    delete list[i];
  }
  delete[] list;
}

void StringList::add(string *str)
{
  if (size == capacity) {
    string **tmp = new string *[capacity * 2];
    copy(list, list + capacity, tmp);
    delete list;
    list = tmp;
    capacity *= 2;
  }
  list[size] = str;
  size++;
}

void StringList::print()
{
  for (int i = 0; i < size; i++) {
    printf("%s\n", (*list[i]).c_str());
  }
}

void StringList::quicksort() { quicksort(0, size - 1); }

void StringList::quicksort(int left, int right)
{
  if (left < right) {
    int i = left, j = right + 1, pivot = left;
    do {
      do {
        if (i == right)
          break;
        i++;
      } while (compare(*list[i], *list[pivot]) < 0);
      do {
        if (j == left)
          break;
        j--;
      } while (compare(*list[j], *list[pivot]) > 0);
      if (i < j)
        swap(list[i], list[j]);
    } while (i < j);
    swap(list[left], list[j]);
    quicksort(left, j - 1);
    quicksort(j + 1, right);
  }
}
