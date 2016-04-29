#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main( int argc, char** argv )
{
   ifstream infile;
   infile.open(argv[1]);
   string s;

   while( getline(infile, s) ){
      int wordlength = s.find(" ", 5) - s.find(" ", 3) + 1;
      int pos = s.find(" ", 3) + 1;


   }
}
