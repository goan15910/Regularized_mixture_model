#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

int main()
{
   string str1;
   string str2;
   
   str1 = "12";
   str2 = " 8";

   char num1[2];
   char num2[2];

   for(int i = 0; i < 2; i++ ){
      num1[i] = str1[i];
      num2[i] = str2[i];
   }

   cout << "\nstr1 is " << int(num1) << endl
        << "str2 is " << int(num2) << endl;
}
