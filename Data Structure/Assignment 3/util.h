//
//  util.h
//  LinkedIntegerList
//
//  Created by 김승호 on 2015. 4. 12..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __LinkedIntegerList__util__
#define __LinkedIntegerList__util__

#include <iostream>
#include <cstdio>
#include <cstring>

#define ISINT(x) (x >= '0' && x <= '9')
#define PRINT_DONE printf("done\n")
#define PRINT_ERROR printf("error\n")

using namespace std;

inline bool startWith(const string &str, const string &keyword)
{ // str이 keyword로 시작하는지를 평가
  return strncmp(str.c_str(), keyword.c_str(), strlen(keyword.c_str())) == 0;
}

void error();
int getInt(const char **);

#endif /* defined(__LinkedIntegerList__util__) */
