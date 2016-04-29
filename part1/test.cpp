#include<iostream>
#include<fstream>
#include<string>
#include<vector>


using namespace std;
/*vector <string> query1;
vector <string> query2;
vector <string> query3;
vector <string> query4;
vector <string> query5;
vector <string> query6;
vector <string> query7;
vector <string> query8;
vector <string> query9;
vector <string> query10;
vector <string> query11;
vector <string> query12;
vector <string> query13;
vector <string> query14;
vector <string> query15;
vector <string> query16;
vector <string> query17;
vector <string> query18;
vector <string> query19;
vector <string> query20;
vector <string> query21;
vector <string> query22;
vector <string> query23;
vector <string> query24;
vector <string> query25;
vector <string> query26;
vector <string> query27;
vector <string> query28;
vector <string> query29;
vector <string> query30;
*/

int main (int argc,char** argv)
{
    string STRING;
    ifstream infile;
    infile.open (argv[1]);
    getline(infile,STRING);
//    cout << STRING <<endl;


    string query[30];
    ifstream infileq;

    infileq.open ( argv[2] );
    int i = 0;
   
    while ( i<30)
    {
       getline(infileq,query[i]);
//       cout << query[i] << endl;
       i++;
       
    }
     string str = argv[1];
//    cout << str.substr(40) ;

    infileq.close();

    infile.close();
    int comp[30];
    for ( int a=0 ; a<30 ; a++ )
    {
      comp[a] = STRING.find(query[a],0);
//      cout << comp[a];
      if ( comp[a] >=0 ) 
      {
         cout << str.substr(40) << " " << a << endl; 
      }
    }
  //  cout << endl;
//    string str = argv[1];
  //  cout << str.substr(40) << endl;
//    for ( int i=0 ; i<30 ; i++ )
  //  {
/*      vector<string>::iterator it;
      for ( it=query1.begin() ; it!=query1.end() ; it++ )
        cout << *it << endl;
*/}
/*
void push(int a , const string s)
{
   switch(a)
   {
      case 0: query1.push_back(s);
               break;
      case 1: query2.push_back(s);
               break;
      case 2: query3.push_back(s);
               break;
      case 3: query4.push_back(s);
               break;
      case 4: query5.push_back(s);
               break;
      case 5: query6.push_back(s);
               break;
      case 6: query7.push_back(s);
               break;
      case 7: query8.push_back(s);
               break;
      case 8: query9.push_back(s);
               break;
      case 9: query10.push_back(s);
               break;
      case 10: query11.push_back(s);
               break;
      case 11: query12.push_back(s);
               break;
      case 12: query13.push_back(s);
               break;
      case 13: query14.push_back(s);
               break;
      case 14: query15.push_back(s);
               break;
      case 15: query16.push_back(s);
               break;
      case 16: query17.push_back(s);
               break;
      case 17: query18.push_back(s);
               break;
      case 18: query19.push_back(s);
               break;
      case 19: query20.push_back(s);
               break;
      case 20: query21.push_back(s);
               break;
      case 21: query22.push_back(s);
               break;
      case 22: query23.push_back(s);
               break;
      case 23: query24.push_back(s);
               break;
      case 24: query25.push_back(s);
               break;
      case 25: query26.push_back(s);
               break;
      case 26: query27.push_back(s);
               break;
      case 27: query28.push_back(s);
               break;
      case 28: query29.push_back(s);
               break;
      case 29: query30.push_back(s);
               break;
   }

}
*/
