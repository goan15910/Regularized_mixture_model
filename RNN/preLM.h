#ifndef PRELM_H
#define PRELM_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include "lm.h"
using namespace std;

bool sort_sec( const pair<string, double> &left, const pair<string, double> &right );

class preLM
{
   friend bool sort_sec( const pair<string, double>&, const pair<string, double> & );
   friend map<string,double> ltom( const LM & );
   friend LM mtol( const map<string, double> & );

public:
   //basic function
   preLM( const map< string, vector<double> > &, const LM & );
   void read_corpus( const vector<string> );
   double sim( string, string );
   void expand_model( double, string );
   map<string, double> get_model_map();
   LM get_model();

   //print function
   void printSpace( string );
   void printModel( const map<string, double> & );
   void printModel_list();
private:
//data member
   map< string, vector<double> > space;
   map< string, double > query;
   vector< map<string, double> > model_list;
   vector<string> model_list_name;
   int trunc_len;
   vector<string> corpus;
   map< string, vector<double> > sub_space;

//utility function
   double dis( string, string );
   void expand( string, double, string ); //expand model for one phrase given range
   void set_sub_space();
};

//basic function
preLM::preLM( const map< string, vector<double> > &m, const LM &q )
{
   space = m;
   query = ltom(q);
   model_list.clear();
   model_list_name.clear();
   trunc_len = 10;
}

void preLM::read_corpus( const vector<string> input_corpus )
{
   corpus = input_corpus;
   set_sub_space();
}

double preLM::sim( string s1, string s2 )
{
   double sim;
   sim = dis(s1,s2);
   sim *= sim;
   sim /= ( dis(s1,s1) * dis(s2,s2) );
   return sim;
}

void preLM::expand_model( double range, string flag )
{
   for( map<string, double>::iterator it = query.begin(); it != query.end(); it++ ){
      string word = it->first;
      expand(word, range, flag);
   }
}

map<string,double> preLM::get_model_map()
{
   map<string, double> model;

   map<string, double>::iterator it = query.begin();
   for( vector< map<string, double> >::iterator i = model_list.begin(); i != model_list.end(); i++ ){
      for( map<string, double>::iterator j = i->begin(); j != i->end(); j++ )
         j->second *= it->second;
      it++;
   }

   for( vector< map<string, double> >::iterator i = model_list.begin(); i != model_list.end(); i++ )
      for( map<string, double>::iterator j = i->begin(); j != i->end(); j++ )
         model[j->first] += j->second;

   return model;
}

LM preLM::get_model()
{
   return mtol( get_model_map() );
}

//print function
void preLM::printSpace( string s )
{
   cout << s << " ";
   for( int i = 0; i < space.at(s).size(); i++ )
      cout << space.at(s)[i] << " ";

   cout << endl;
}

void preLM::printModel( const map<string, double> &model )
{
   int index = 0;
   for( map<string, double>::const_iterator it = model.begin(); it != model.end(); it++ ){
      string word = it->first;
      double prob = it->second;
      cout << index << ". " << word << ": " << prob << endl;
      index++;
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

void preLM::expand( string q, double range, string flag )
{
   map<string, double> model;
   double sum = 0.0;

   vector< pair<string, double> > wordlist;
   
   map< string, vector<double> >::iterator begin, end;

   if( flag == "all" ){
      begin = space.begin();
      end = space.end();
   }
   else if( flag == "sub" ){
      if( sub_space.size() == 0 ){
         cerr << "Read subspace first!" << endl;
         exit(1);
      }

      begin = sub_space.begin();
      end = sub_space.end();      
   }
   else{
      cerr << "In function void preLM::expand( string , double , string ): Invalid flag value" << endl;
      exit(1);
   }

   for( map<string, vector<double> >::iterator it = begin; it != end; it++ ){
      string w = it->first;
      double d = sim(q, w);
      if( d > range || d == range )
         wordlist.push_back( make_pair<string, double>(w, d) );
   }
   
   sort( wordlist.begin(), wordlist.end(), sort_sec );
   
   int count = 0;
   for( vector< pair<string, double> >::iterator it = wordlist.begin(); it != wordlist.end(); it++ ){
      string word = it->first;
      double d = it->second;
   
      if( count < trunc_len ){
         model[word] = d;
         sum += d;
      }
      else
         break;

      count++;
   }

   for( map<string, double>::iterator it = model.begin(); it != model.end(); it++ )
      it->second /= sum;
   
   model_list.push_back(model);
   model_list_name.push_back(q);
}

void preLM::set_sub_space()
{
   for( vector<string>::iterator it = corpus.begin(); it != corpus.end(); it++ ){
      string word = *it;
      vector<double> proj;

      map< string, vector<double> >::iterator mit = space.find(word);

      if( mit != space.end() ){
         proj = mit->second;
         sub_space.insert( pair< string, vector<double> >(word, proj) );
      }
   }

   cout << "Size of sub space is " << sub_space.size() << endl;
}

//other function
bool sort_sec(const pair<string,double>& left, const pair<string,double>& right)
{        return left.second > right.second;            }

#endif
