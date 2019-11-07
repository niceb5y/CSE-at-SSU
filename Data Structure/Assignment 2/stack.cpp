//
//  stack.cpp
//  fixyou
//
//  Created by 김승호 on 2015. 4. 6..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "stack.h"

stack::stack()
{
  first = NULL;
  size = 0;
}

stack::~stack()
{
  delete first; //첫번째 노드부터 삭제
}

bool stack::isEmpty()
{
  return size == 0; //크기 확인
}

stack::node *stack::top() const
{
  if (size == 0) {
    return NULL;
  } else {
    node *n = first;
    for (int i = 1; i < size; i++) {
      n = n->next; //노드 끝까지 이동
    }
    return n;
  }
}

string stack::top_str()
{
  if (size == 0) {
    return "";
  }
  return top()->data; //값 반환
}

void stack::push(string str)
{
  if (size == 0) {
    first = new node;
    first->data = str;
  } else {
    node *n = new node, *t = top();
    n->data = str;
    t->next = n;
  }
  size++; //사이즈 증가
}

void stack::pop()
{
  if (size == 0) {
    return;
  }
  if (size == 1) {
    delete first;
    first = NULL;
  } else {
    node *n = first, *t = top();
    while (n->next != t) {
      n = n->next; // top을 가리키는 노드까지 이동
    }
    n->next = NULL;
    delete t;
  }
  size--; //사이즈 감소
}

stack::node::node()
{
  data = "";
  next = NULL;
}

stack::node::~node()
{
  delete next; //다음 노드 삭제
}
