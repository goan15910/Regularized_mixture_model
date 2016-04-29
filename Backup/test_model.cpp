#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "model.h"
using namespace std;

int main()
{
   const int list_size = 5;
   string charlist[list_size] = {"a", "b", "c", "d", "e"};

   map<string, double> m;

   for( int i = 0; i < list_size; i++ ){
      string w = charlist[i];
      m[w] = (double)i / (double)10;
   }

   Model m01;
   Model m02;

   m01.source(m);

   for( int i = 0; i < m02.size(); i++ ){
      string w = charlist[i];
      m02[w] = m[w];
   }

   cout << "\nSize of m02 is " << m02.size()
        << "\nContent in m02:" << endl;

   for( int i = 0; i < m02.size(); i++ ){
      string w = charlist[i];
      cout << m02[w] << endl;
   }

   Model m03;
   Model m04;

   for( int i = 0; i < m03.size(); i++ ){
      string w = charlist[i];
      m03[w] = 2 * m[w];
   }

   cout << "\nSize of m03 is " << m03.size()
        << "\nContent in m03:" << endl;

   for( int i = 0; i < m03.size(); i++ ){
      string w = charlist[i];
      cout << m03[w] << endl;
   }

   m04 = m03;
   
   cout << "\nSize of m04 is " << m04.size()
        << "\nContent in m04:" << endl;

   for( int i = 0; i < m04.size(); i++ ){
      string w = charlist[i];
      cout << m04[w] << endl;
   }
}
