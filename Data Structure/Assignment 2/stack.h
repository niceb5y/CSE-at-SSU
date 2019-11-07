//
//  stack.h
//  fixyou
//
//  Created by 김승호 on 2015. 4. 6..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __fixyou__stack__
#define __fixyou__stack__

#include <iostream>

using namespace std;

class stack
{ //링크드 리스트로 구현된 스택 클래스
private:
  class node
  { //노드 클래스
    friend class stack;

  public:
    node();  //생성자
    ~node(); //소멸자
  private:
    string data; //값
    node *next;  //다음 노드
  };
  node *first; //처음 노드
  int size;    //크기

public:
  stack();           //생성자
  ~stack();          //소멸자
  bool isEmpty();    //스택이 비었는지 확인
  node *top() const; //맨 위 노드 반환
  string top_str();  //맨 위 노드의 값 반환
  void push(string); //노드 삽입
  void pop();        //노드 삭제
};

#endif /* defined(__fixyou__stack__) */
