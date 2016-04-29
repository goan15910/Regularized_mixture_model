#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main( int argc, char** argv )
{
   ifstream infile;
   infile.open(argv[1]);

   ofstream ofile;
   ofile.open( argv[2], fstream::in | fstream::out | fstream::app );

   string name = argv[1];
   string s;
   while( getline(infile, s) ){
      ofile << name.substr( name.find("N2001"), 19 ) << " " << s << endl;
   }

   infile.close();
   ofile.close();
}
