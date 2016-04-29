#ifndef PRELM_H
#define PRELM_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include "model.h"
using namespace std;

bool sort_sec( const pair<string, double> &left, const pair<string, double> &right );

class preLM
{
   friend bool sort_sec( const pair<string, double>&, const pair<string, double> & );

public:
   //basic function
   preLM( const map< string, vector<double> > &, const Model & );
   double sim( string, string );
   void expand_model( double );
   Model get_model(); //get model

   //print function
   void printSpace( string );
   void printModel( const Model & );
   void printModel_list();
private:
//data member
   map< string, vector<double> > space;
   Model query;
   vector< Model > model_list;
   vector<string> model_list_name;
   int trunc_len;

//utility function
   double dis( string, string );
   void expand( string, double ); //expand model for one phrase given range
};

//basic function
preLM::preLM( const map< string, vector<double> > &m, const Model &q )
{
   space = m;
   query = q;
   model_list.clear();
   model_list_name.clear();
   trunc_len = 10;
}

double preLM::sim( string s1, string s2 )
{
   double sim;
   sim = dis(s1,s2);
   sim *= sim;
   sim /= ( dis(s1,s1) * dis(s2,s2) );
   return sim;
}

void preLM::expand_model( double range )
{
   for( int i = 0; i < query.size(); i++ ){
      string word = Model::get_word()[i];
      expand(word, range);
   }
}

Model preLM::get_model()
{
   Model model;

   for( int n = 0; n < model_list.size(); n++ ){
      string q = model_list_name[n];

      for( int i = 0; i < model_list[n].size(); i++ ){
         string w = Model::get_word()[i];
         model[w] += model_list[n][w] * query[q];
      }

   }

   return model;
}

//print function
void preLM::printSpace( string s )
{
   cout << s << " ";
   for( int i = 0; i < space.at(s).size(); i++ )
      cout << space.at(s)[i] << " ";

   cout << endl;
}

void preLM::printModel( const Model &model )
{
   for( int i = 0; i < model.size(); i++ ){
      string word = Model::get_word()[i];
      double prob = model[word];
      cout << i+1 << ". " << word << ": " << prob << endl;
   }
}

void preLM::printModel_list()
{
   cout << "Model list for preLM expansion:" << endl;
   for( int i = 0; i < model_list.size(); i++ ){
      cout << model_list_name[i] << endl;
      printModel( model_list[i] );
      cout << endl;
   }
}

//utility function
double preLM::dis( string s1, string s2 )
{
   double distance = 0.0;
   for( int i = 0; i < space.at(s1).size(); i++ )
      distance += space.at(s1)[i] * space.at(s2)[i];
   return distance;
}

void preLM::expand( string q, double range )
{
   Model model;
   double sum = 0.0;

   vector< pair<string, double> > wordlist;
   for( map<string, vector<double> >::iterator it = space.begin(); it != space.end(); it++ ){
      string w = it->first;
      double d = sim(q, w);
      if( d > range || d == range )
         wordlist.push_back( make_pair<string, double>(w, d) );
   }

   sort( wordlist.begin(), wordlist.end(), sort_sec );

   for( int i = 0; i < wordlist.size(); i++ ){
      string word = wordlist[i].first;
      double d = wordlist[i].second;

      if( i < trunc_len ){
         model[word] = d;
         sum += d;
      }
      else
         break;
   }

   for( int i = 0; i < model.size(); i++ ){
      string w = Model::get_word()[i];
      model[w] /= sum;
   }

   model_list.push_back(model);
   model_list_name.push_back(q);
}

bool sort_sec(const pair<string,double>& left, const pair<string,double>& right)
{        return left.second > right.second;            }

#endif
