#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "lm.h"
using namespace std;

int main()
{
   LM lm;

   const int size = 5;
   string wlist[size] = {"a", "b", "c", "d", "e"};

   for( int i = 0; i < size; i++ ){
      lm.word.push_back(wlist[i]);
      lm.prob.push_back(i);
   }

   const LM const_lm = lm;
   
   cout << "Testing operator[] for ordinary LM object:" << endl;
   for( int i = 0; i < size; i++ ){
      string w = wlist[i];
      cout << "lm[" << w << "] = " << lm[w] << endl;
   }

   cout << "Testing operator[] for const LM object:" << endl;
   for( int i = 0; i < size; i++ ){
      string w = wlist[i];
      cout << "const_lm[" << w << "] = " << const_lm[w] << endl;
   }

   cout << "Testing for clear():" << endl;
   lm.clear();
   cout << "Size of word after lm.clear(): " << lm.word.size()
        << "\nSize of prob after lm.clear(): " << lm.prob.size()
        << endl;
}
