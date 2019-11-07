//
//  maxheap.h
//  MaxHeap
//
//  Created by 김승호 on 2015. 5. 16..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#ifndef __MaxHeap__maxheap__
#define __MaxHeap__maxheap__

#include <iostream>

using namespace std;

class MaxHeap
{
private:
  int *heap;    //힙을 구성하는 배열
  int size;     //원소의 개수
  int capacity; //배열의 크기
public:
  MaxHeap();                     //생성자
  MaxHeap(const MaxHeap &h);     //복사 생성자
  ~MaxHeap();                    //소멸자
  bool isEmpty();                // Heap이 비어있는지 확인
  void Process(const string &s); // 명령어 처리
  void Pop();                    //가장 큰 값 제거
  void Push(const int n);        //값 삽입
  int Top();                     //가장 큰 값 출력
};

#endif /* defined(__MaxHeap__maxheap__) */
