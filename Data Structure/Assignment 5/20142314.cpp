//
//  main.cpp
//  AOE
//
//  Created by 김승호 on 2015. 5. 31..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include <iostream>
#include "linkeddigraph.h"

using namespace std;

int main(int argc, const char *argv[])
{
  LinkedDigraph Graph;
  string line;
  while (getline(cin, line)) {
    Graph.Parse(line);
  }
  Graph.ShowAOEResult();
  return 0;
}
