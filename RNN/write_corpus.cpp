#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//docfile: document file, doclist: all document content in one file, docmap:mapping between file name & file order

//cmd: ./write docfile doclist docmap
int main( int argc, char** argv )
{
   ifstream docfile;
   docfile.open(argv[1]);

   ofstream doclist;
   doclist.open( argv[2], fstream::in | fstream::out | fstream::app );

   ofstream docmap;
   docmap.open( argv[3], fstream::in | fstream::out | fstream::app );

   string s;
   while ( getline(docfile,s) )
      doclist << s << " " ;
   doclist << endl;

   string filename = argv[1];
   docmap << filename.substr(43) << endl;

   docfile.close();
   doclist.close();
   docmap.close();
}
