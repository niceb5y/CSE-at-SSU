//
//  linkeddigraph.cpp
//  AOE
//
//  Created by 김승호 on 2015. 6. 1..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include "linkeddigraph.h"

LinkedNode::LinkedNode()
{
  vertex = -1;
  duration = -1;
  link = NULL;
}

LinkedNode::LinkedNode(int ver, int dur)
{
  vertex = ver;
  duration = dur;
  link = NULL;
}

LinkedDigraph::LinkedDigraph()
{
  size = 0;
  capacity = 20;
  count = new int[capacity]();
  order = new int[capacity]();
  ee = new int[capacity]();
  le = new int[capacity]();
  first = new LinkedNode *[capacity]();
}

void LinkedDigraph::Expand(int newCapacity)
{
  int *tmp_count = new int[newCapacity]();
  int *tmp_order = new int[newCapacity]();
  int *tmp_ee = new int[newCapacity]();
  int *tmp_le = new int[newCapacity]();
  LinkedNode **tmp_first = new LinkedNode *[newCapacity];
  copy(count, count + capacity, tmp_count);
  copy(order, order + capacity, tmp_order);
  copy(ee, ee + capacity, tmp_ee);
  copy(le, le + capacity, tmp_le);
  copy(first, first + capacity, tmp_first);
  delete count;
  delete order;
  delete ee;
  delete le;
  delete first;
  count = tmp_count;
  order = tmp_order;
  ee = tmp_ee;
  le = tmp_le;
  first = tmp_first;
  capacity = newCapacity;
}

void LinkedDigraph::AddNode(int src, int dest, int dur)
{
  if (size == capacity) {
    Expand(2 * capacity);
  }
  LinkedNode *n = new LinkedNode(dest, dur);
  if (first[src] == NULL) {
    first[src] = n;
  } else {
    LinkedNode *p = first[src];
    while (p->link) {
      p = p->link;
    }
    p->link = n;
  }
  if (capacity < dest + 1) {
    Expand(dest * 2);
  }
  count[dest]++;
}

void LinkedDigraph::Print()
{
  for (int i = 0; i < size; i++) {
    cout << i << " - count:" << count[i] << endl;
    LinkedNode *n = first[i];
    while (n) {
      cout << "   data:" << n->vertex << " duration:" << n->duration;
      n = n->link;
    }
    cout << endl;
    cout << "   order : " << order[i];
    cout << " ee:" << ee[i];
    cout << " le:" << le[i] << endl;
  }
}

int LinkedDigraph::TopologicalOrder()
{
  int top = -1;
  for (int i = 0; i < size; i++) {
    if (count[i] == 0) {
      count[i] = top;
      top = i;
    }
  }
  for (int i = 0; i < size; i++) {
    if (top == -1) {
      return -1;
    }
    int j = top;
    top = count[top];
    order[i] = j;
    LinkedNode *ji = first[j];
    while (ji) {
      count[ji->vertex]--;
      ee[ji->vertex] = max(ee[ji->vertex], ee[j] + ji->duration);
      if (count[ji->vertex] == 0) {
        count[ji->vertex] = top;
        top = ji->vertex;
      }
      ji = ji->link;
    }
  }
  for (int i = size - 1; i >= 0; i--) {
    int j = order[i];
    LinkedNode *p = first[j];
    if (!p) {
      le[j] = ee[j];
    } else {
      int minimum = le[p->vertex] - (p->duration);
      while (p) {
        minimum = min(minimum, le[p->vertex] - (p->duration));
        p = p->link;
      }
      le[j] = minimum;
    }
  }
  int ee_count = 0;
  for (int i = 0; i < size; i++) {
    if (ee[i] == 0) {
      ee_count++;
    }
  }
  if (ee_count > 1) {
    return -1;
  }
  return 0;
}

void LinkedDigraph::Parse(string s)
{
  const char *c = s.c_str();
  int src = parseInt(&c);
  while (*c == ' ') {
    c++;
    int dest = parseInt(&c);
    c++;
    int dur = parseInt(&c);
    AddNode(src, dest, dur);
  }
  size = src + 1;
}

void LinkedDigraph::ShowAOEResult()
{
  int res = this->TopologicalOrder();
  if (res == -1) {
    cout << "AOE network error" << endl;
  } else {
    for (int i = 0; i < size; i++) {
      LinkedNode *p = first[i];
      while (p) {
        cout << i << ",";
        cout << p->vertex << " ";
        cout << ee[i] << " ";
        cout << le[p->vertex] - (p->duration) << " ";
        int diff = ee[i] - le[p->vertex] + (p->duration);
        cout << abs(diff) << " ";
        if (diff) {
          cout << "No" << endl;
        } else {
          cout << "Yes" << endl;
        }
        p = p->link;
      }
    }
  }
}
