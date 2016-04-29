#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "read.h"
#include "preLM.h"
using namespace std;

double sim( string, string, const map< string, vector<double> > & );
double dis( string, string, const map< string, vector<double> > & );

//cmd: ./RNNtest wordproj
int main( int argc, char** argv )
{
   ifstream infile;
   infile.open(argv[1]);
   map< string, vector<double> > RNN;
   string s;

   RNN = readRNN(infile);

   cout << "Size = " << RNN.size() << endl;

   while( true ){
      cout << "Input Word: " << endl;
      cin >> s;

      vector< pair<string, double> > score_list;

      for( map<string, vector<double> >::iterator it = RNN.begin(); it != RNN.end(); it++ ){
         string w = it->first;
         double score = sim(s, w, RNN);
         score_list.push_back( make_pair<string, double>(w, score) );
      }

      sort( score_list.begin(), score_list.end(), sort_sec );

      cout << "Top similar words: " << endl;

      for( int i = 0; i < 20; i++ ){
         string word = score_list[i].first;
         double score = score_list[i].second;
         cout << word << " " << score << endl;
      }
   }
}

double sim( string s1, string s2, const map< string, vector<double> > &proj )
{
   double sim;
   sim = dis(s1, s2, proj);
   sim *= sim;
   sim /= ( dis(s1, s1, proj) * dis(s2, s2, proj) );
   return sim;
}

double dis( string s1, string s2, const map< string, vector<double> > &proj )
{
   int dim = proj.at(s1).size();
   double dis = 0.0;
   for( int i = 0; i < dim; i++ )
      dis += proj.at(s1)[i] * proj.at(s2)[i];
   return dis;
}
