//
//  util.h
//  AOE
//
//  Created by 김승호 on 2015. 5. 31..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __AOE__util__
#define __AOE__util__

#include <iostream>

using namespace std;

#define ISINT(x) (x >= '0' && x <= '9')

int parseInt(const char **);
int max(int, int);
int min(int, int);

#endif /* defined(__AOE__util__) */
