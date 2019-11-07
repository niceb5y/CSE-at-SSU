//
//  linkeddigraph.h
//  AOE
//
//  Created by 김승호 on 2015. 6. 1..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __AOE__linkeddigraph__
#define __AOE__linkeddigraph__

#include <iostream>
#include <cmath>
#include "util.h"

using namespace std;

#endif /* defined(__AOE__linkeddigraph__) */

class LinkedDigraph;

class LinkedNode
{
  friend class LinkedDigraph;
  int vertex;
  int duration;
  LinkedNode *link;

public:
  LinkedNode();
  LinkedNode(int, int);
};

class LinkedDigraph
{
  int size;
  int capacity;
  int *count;
  int *order;
  int *ee;
  int *le;
  LinkedNode **first;
  void Expand(int);

public:
  LinkedDigraph();
  void AddNode(int, int, int);
  int TopologicalOrder();
  void Parse(string);
  void Print();
  void ShowAOEResult();
};