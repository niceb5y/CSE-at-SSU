//
//  linkedlist.cpp
//  LinkedIntegerList
//
//  Created by 김승호 on 2015. 4. 10..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "linkedlist.h"

Node::Node()
{ //더미 노드 만들 때 사용한다
  value = INT_MIN;
  right = this;
  left = this;
}

Node::Node(int num)
{ //생성 시 초기화
  value = num;
  right = this;
  left = this;
}

List::List()
{
  rightAsNext = true;  // 기본적으로 좌->우로 읽는다
  header = new Node(); //더미 노드
}

List::~List()
{
  while (!isEmpty()) {         //빌때까지
    removeNode(header->right); //노드를 없앤다
  }
  delete header; //더미 노드 제거
}

bool List::isEmpty()
{
  return (header->right == header); //더미 노드만 있을때를 비었다고 한다
}

void List::insertNodeAfter(Node *pos, Node *n)
{
  if (rightAsNext) { //반전 여부에 따라 pos뒤에 n을 삽입한다
    Node *tmp = pos->right;
    pos->right = n;
    n->left = pos;
    n->right = tmp;
    tmp->left = n;
  } else {
    Node *tmp = pos->left;
    pos->left = n;
    n->right = pos;
    n->left = tmp;
    tmp->right = n;
  }
}

void List::removeNode(Node *n)
{ //노드를 지우고 양쪽을 서로 연결한다
  Node *l = n->left, *r = n->right;
  n->left = NULL;
  n->right = NULL;
  l->right = r;
  r->left = l;
  delete n; // n은 메모리에서 제거한다
}

void List::reverse()
{ //반대 방향으로 읽어주기로 한다
  rightAsNext = !rightAsNext;
}

void List::process(const string &s)
{
  if (startWith(s, "I ")) { // I 명령어
    if (isEmpty()) {
      PRINT_ERROR; //비었을때
    } else {
      const char *str = s.c_str() + 2;
      int a = getInt(&str), b = getInt(&(++str)); //숫자 2개를 읽어서
      Node *n = new Node(b);
      if (rightAsNext) { //방향에 따라 찾은다음 삽입
        Node *p = header->right;
        while (p != header) {
          if (p->value == a) {
            insertNodeAfter(p, n);
            PRINT_DONE;
            return;
          }
          p = p->right;
        }
      } else {
        Node *p = header->left;
        while (p != header) {
          if (p->value == a) {
            insertNodeAfter(p, n);
            PRINT_DONE;
            return;
          }
          p = p->left;
        }
      }
      delete n;    //삽입을 못하면 메모리에서 제거
      PRINT_ERROR; //삽입을 못했으니 에러를 표시한다
    }
  } else if (startWith(s, "IB ")) { // IB 명령어
    const char *str = s.c_str() + 3;
    Node *p;
    if (rightAsNext) { //마지막 노드를 가져온다
      p = header->left;
    } else {
      p = header->right;
    }
    while (*str !=
           '\0') { //입력받은 숫자 다 읽을 때 까지 읽어서 뒤에다 집어넣는다
      Node *tmp = new Node(getInt(&str));
      insertNodeAfter(p, tmp);
      p = tmp;
      if (*str == ' ') {
        str++;
      }
    }
    PRINT_DONE;                     //에러 날 일이 없으니 완료
  } else if (startWith(s, "IF ")) { //전방 삽입
    const char *str = s.c_str() + 3;
    Node *p = header; //더미 노드를 가리키고
    while (*str != '\0') {
      Node *tmp = new Node(getInt(&str)); //그 뒤에 새로 노드를 삽입
      insertNodeAfter(p, tmp); //그리고 새로 삽입한 노드를 가리킨다. 반복
      p = tmp;
      if (*str == ' ') {
        str++;
      }
    }
    PRINT_DONE;                    //완료
  } else if (startWith(s, "D ")) { // D 명령어
    if (isEmpty()) {               //비어서 지울게 없다
      PRINT_ERROR;
    } else {
      const char *str = s.c_str() + 2;
      int a = getInt(&str);
      if (rightAsNext) { //노드를 찾아서 removeNode 호출
        Node *p = header->right;
        while (p != header) {
          if (p->value == a) {
            removeNode(p);
            PRINT_DONE;
            return;
          }
          p = p->right;
        }
      } else {
        Node *p = header->left;
        while (p != header) {
          if (p->value == a) {
            removeNode(p);
            PRINT_DONE;
            return;
          }
          p = p->left;
        }
      }
    }
  } else if (s == "P") { // P 명령어
    if (isEmpty()) {
      PRINT_ERROR;
    } else {
      bool initial = true;
      if (rightAsNext) {
        Node *p = header->right;
        while (p != header) {
          if (initial) { //공백을 줄이기 위해
            printf("%d", p->value);
            initial = false;
          } else {
            printf(" %d", p->value);
          }
          p = p->right;
        }
        printf("\n");
      } else {
        Node *p = header->left;
        while (p != header) {
          if (initial) { //공백을 줄이기 위해
            printf("%d", p->value);
            initial = false;
          } else {
            printf(" %d", p->value);
          }
          p = p->left;
        }
        printf("\n");
      }
    }
  } else if (s == "R") { // R 명령어
    if (isEmpty()) {
      PRINT_ERROR;
    } else {
      reverse();
      PRINT_DONE;
    }
  } else {       //기타 명령어
    PRINT_ERROR; //에러
  }
}
