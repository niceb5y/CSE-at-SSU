//
//  maxheap.cpp
//  MaxHeap
//
//  Created by 김승호 on 2015. 5. 16..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "maxheap.h"
#include "util.h"

MaxHeap::MaxHeap()
{
  size = 0;
  capacity = 10;
  heap = new int[capacity + 1];
}

MaxHeap::MaxHeap(const MaxHeap &h)
{
  size = h.size;
  capacity = h.capacity;
  heap = new int[capacity + 1];
  copy(h.heap, h.heap + capacity + 1, heap);
}

MaxHeap::~MaxHeap() { delete[] heap; }

bool MaxHeap::isEmpty() { return size == 0; }

void MaxHeap::Process(const string &s)
{
  if (startWith(s, "I ")) {
    const char *str = s.c_str() + 2;
    int x = getInt(&str);
    Push(x);
    PRINT_DONE;
    return;
  } else if (s == "D") {
    if (isEmpty()) {
      PRINT_ERROR;
    } else {
      Pop();
      PRINT_DONE;
    }
    return;
  } else if (s == "T") {
    if (isEmpty()) {
      PRINT_ERROR;
    } else {
      cout << Top() << endl;
    }
    return;
  } else if (startWith(s, "D ")) {
    if (isEmpty()) {
      PRINT_ERROR;
    } else {
      const char *str = s.c_str() + 2;
      int n = getInt(&str);
      if (n > size) {
        PRINT_ERROR;
      } else {
        int *tmp = new int[n];
        int i = 1;
        for (; i < n; i++) {
          tmp[i] = Top();
          Pop();
        }
        Pop();
        for (int i = n - 1; i > 0; i--) {
          Push(tmp[i]);
        }
        PRINT_DONE;
      }
    }
    return;
  } else if (startWith(s, "T ")) {
    if (isEmpty()) {
      PRINT_ERROR;
    } else {
      const char *str = s.c_str() + 2;
      int n = getInt(&str);
      if (n > size) {
        PRINT_ERROR;
      } else {
        MaxHeap tmp = *this;
        for (int i = 1; i < n; i++) {
          tmp.Pop();
        }
        cout << tmp.Top() << endl;
      }
    }
    return;
  } else if (s == "P") {
    if (isEmpty()) {
      PRINT_ERROR;
    } else {
      for (int i = 1; i <= size; i++) {
        cout << heap[i] << " ";
      }
      cout << endl;
    }
    return;
  }
  PRINT_ERROR;
}

void MaxHeap::Pop()
{
  if (isEmpty()) {
    throw "Heap is Empty";
  }
  int last = heap[size--];
  int cur = 1;
  int child = 2;
  while (child <= size) {
    if (child < size && heap[child] < heap[child + 1]) {
      child++;
    }
    if (last >= heap[child]) {
      break;
    }
    heap[cur] = heap[child];
    cur = child;
    child *= 2;
  }
  heap[cur] = last;
}

void MaxHeap::Push(const int n)
{
  if (size == capacity) {
    int *tmp = new int[2 * capacity + 1];
    copy(heap, heap + capacity + 1, tmp);
    delete[] heap;
    heap = tmp;
    capacity *= 2;
  }
  int cur = ++size;
  while (cur != 1 && heap[cur / 2] < n) {
    heap[cur] = heap[cur / 2];
    cur /= 2;
  }
  heap[cur] = n;
}

int MaxHeap::Top()
{
  if (isEmpty()) {
    throw "Heap is Empty";
  }
  return heap[1];
}
