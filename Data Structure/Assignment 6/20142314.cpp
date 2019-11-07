//
//  main.cpp
//  StringSort
//
//  Created by 김승호 on 2015. 6. 10..
//  Copyright (c) 2015년 Seungho Kim. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "stringlist.h"
#include "util.h"

using namespace std;

int main(int argc, const char *argv[])
{
  ofstream *ofs[128];
  for (int i = 0; i < 128; i++) {
    string fn = "20142314_" + int_to_string(i) + ".tempcache";
    ofs[i] = new ofstream(fn.c_str());
  }
  string line;
  while (getline(cin, line)) {
    *ofs[str_hash(line)] << line << endl;
  }
  for (int i = 0; i < 128; i++) {
    (*ofs[i]).close();
    string fn = "20142314_" + int_to_string(i) + ".tempcache";
    ifstream ifs(fn.c_str());
    string fline;
    StringList list;
    while (getline(ifs, fline)) {
      string *tmp = new string(fline);
      list.add(tmp);
    }
    ifs.close();
    list.quicksort();
    list.print();
    string cmd = "rm 20142314_" + int_to_string(i) + ".tempcache";
    system(cmd.c_str());
  }
  return 0;
}
