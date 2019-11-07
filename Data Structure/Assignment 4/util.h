//
//  util.h
//  MaxHeap
//
//  Created by 김승호 on 2015. 5. 16..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __MaxHeap__util__
#define __MaxHeap__util__

#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

#define ISINT(x) (x >= '0' && x <= '9')
#define PRINT_DONE printf("done\n")
#define PRINT_ERROR printf("error\n")

inline bool startWith(const string &str, const string &keyword)
{
  return strncmp(str.c_str(), keyword.c_str(), strlen(keyword.c_str())) == 0;
}
int getInt(const char **str);

#endif /* defined(__MaxHeap__util__) */
