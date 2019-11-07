//
//  main.cpp
//  MaxHeap
//
//  Created by 김승호 on 2015. 5. 16..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include <iostream>
#include "maxheap.h"

using namespace std;

int main(int argc, const char *argv[])
{
  string line;
  MaxHeap heap;
  while (getline(cin, line)) {
    if (line != "") {
      heap.Process(line);
    }
  }
  return 0;
}
