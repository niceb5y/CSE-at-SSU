//
//  linkedlist.h
//  LinkedIntegerList
//
//  Created by 김승호 on 2015. 4. 10..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __LinkedIntegerList__linkedlist__
#define __LinkedIntegerList__linkedlist__

#include <iostream>
#include <cstdio>
#include <climits>
#include "util.h"

using namespace std;

class List;

class Node
{                    //노드 클래스
  friend class List; //리스트 클래스에서 접근 가능
private:
  int value;   //정수의 값
  Node *left;  //왼쪽 노드
  Node *right; //오른쪽 노드
public:
  Node();    //생성자
  Node(int); //주어진 값으로 초기화 하는 생성자
};

class List
{
private:
  Node *header;     //더미 노드를 가리킴
  bool rightAsNext; //오른쪽 노드를 다음 노드로 사용할지 결정
public:
  List();         //생성자
  ~List();        //소멸자
  bool isEmpty(); //연결리스트가 비어있는지 확인
  void insertNodeAfter(Node *, Node *); //노드 다음에 삽입
  void removeNode(Node *);              //노드 제거
  void reverse();                       //반전
  void process(const string &);         //명령어 처리
};

#endif /* defined(__LinkedIntegerList__linkedlist__) */
