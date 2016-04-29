#include <iostream>
#include <fstream>
#include <string>
#include "list.h"
#include "push.h"
using namespace std;

int main(int argc, char** argv)
{
   const int query_size = 30;
   LIST filename[query_size];
   for( int i = 0; i < 30; i++ )
      initialize( &filename[i] );

   const int str_size = 824;
   string str[str_size];
   ifstream infile;
   infile.open (argv[1]);

   int i = 0;

   while( i < str_size ){
      string trans;
      string num;
      getline(infile, str[i]);
      cut( str[i], trans, num );
      //stoi(num);
      assign( filename, stoi(num), &trans );
      i++;
   }

   infile.close();

   for( int i = 0; i < 30; i++ )
      for( int j = 0; j < filename[i].size; j++ ){
         if( filename[i].list[j] != "\0" ){
            cout << i + 1 << " 0 " 
                 << filename[i].list[j] 
                 << " 1"
                 << endl;
         }
      }
}

