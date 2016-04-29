#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main( int argc, char** argv )
{
   ifstream infile;
   infile.open(argv[1]);
   string s;
   /*
   vector<string> v;
   vector<string> error;

   while( getline(infile, s) ){
      if( find(v.begin(), v.end(), s) == v.end() )
         v.push_back(s);
      else
         error.push_back(s);
   }
   
   if( error.size() == 0 )
      cout << "no error" << endl;
   else{
      for( int i = 0; i < error.size(); i++ )
         cout << error[i] << endl;

      cout << "error number " << error.size() << endl;
   }
   */

   while( getline(infile, s) ){
      
   }
}
