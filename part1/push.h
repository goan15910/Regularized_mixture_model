#include<iostream>
#include<fstream>
#include<string>

using namespace std;

void cut(const string line , string& trans , string& num)
{
/*   string line[824];
   ifstream result;
   result.open(argv[1]);
   int i = 0;

   while ( i<824 )
   {
      getline(result,line[i]);
      i++;
   }
*/
      trans = line.substr(0,25);
      size_t begin = line.find_first_of(" ");

      num = line.substr(begin+1);

//   cout << trans << endl;
//   cout << num << endl;
}
