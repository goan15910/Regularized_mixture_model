#ifndef LM_H
#define LM_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class LM
{
   friend map<string,double> ltom( const LM &lm );
   friend LM mtol( const map<string,double> & );

public:
   //data member
   vector<string> word;
   vector<double> prob;

   //basic function
   LM();
   const LM &operator=( const LM & );
   double &operator[]( const string & );
   const double &operator[]( const string & ) const;
   void clear();

private:

};

LM::LM()
{
   //empty body
}

const LM &LM::operator=( const LM &right )
{
   if( this != &right ){
      this->word = right.word;
      this->prob = right.prob;
   }

   return (*this);
}

double &LM::operator[]( const string &w )
{
   vector<string>::iterator it;
   int idx;

   it = find(word.begin(), word.end(), w);
   idx = it - word.begin();

   return prob[idx];
}

const double &LM::operator[]( const string &w ) const
{
   vector<string>::const_iterator it;
   int idx;

   it = find(word.begin(), word.end(), w);
   idx = it - word.begin();

   return prob[idx];
}

void LM::clear()
{
   word.clear();
   prob.clear();
}

map<string, double> ltom( const LM &lm )
{
   map<string,double> m;

   for( int i = 0; i < lm.word.size(); i++ ){
      string w = lm.word[i];
      double p = lm.prob[i];
      m.insert( pair<string,double>(w, p) );
   }

   return m;
}

LM mtol( const map<string, double> &m )
{
   LM lm;

   for( map<string,double>::const_iterator it = m.begin(); it != m.end(); it++ ){
      lm.word.push_back(it->first);
      lm.prob.push_back(it->second);
   }

   return lm;
}

#endif
