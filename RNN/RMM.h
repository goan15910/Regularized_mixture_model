#ifndef RMM_H
#define RMM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include "lm.h"
using namespace std;

bool comp_second( const pair<string, double> &left, const pair<string, double> &right);

class RMM
{
   friend map<string,double> ltom( const LM &lm );
   friend LM mtol( const map<string,double> & );
public:
   //basic function
   RMM( const map<string, double> &, const vector<string> &, const map<string, double> &, const map<string, double> &, const map<string, double> & );
   RMM( const map<string, double> &, const vector<string> &, const LM &, const LM & );
   void train();

   //get function
   const map<string, double> &get_alpha();
   const map<string, double> &get_R();

   //print function
   void printMap( const map<string, double> & );
   void printMap( const map<string, int> & );
private:
   //data member
   map<string, double> alpha;
   vector<string> doclist;
   map<string, double> R;
   //map<string, double> B;
   LM B;
   map<string, double> Q;
   vector< map<string, int> > count;
   vector< map<string, double> > postprob;
   vector<string> corpus;
   int mu;
   int trunc_len;
   
   //utility function
   void set_count();
   void set_corpus();
   void Estep();
   void Mstep();
   void clear();
   void adjust_alpha();
   void adjust_R();
   void EM();
   void EM( int );
};

//basic function
RMM::RMM( const map<string, double> &ALPHA, const vector<string> &DOCLIST, const map<string, double> &Rmodel, const map<string, double> &Bmodel, const map<string, double> &Qmodel )
{
   alpha = ALPHA;
   doclist = DOCLIST;
   R = Rmodel;
   //B = Bmodel;
   B = mtol(Bmodel);
   Q = Qmodel;

   set_count();
   set_corpus();
   mu = 0;
   trunc_len = 20;
}

RMM::RMM( const map<string, double> &ALPHA, const vector<string> &DOCLIST, const LM &Rmodel, const LM &Bmodel )
{
   alpha = ALPHA;
   doclist = DOCLIST;
   R = ltom( Rmodel );
   //B = ltom( Bmodel );
   B = Bmodel;
   Q = R;

   set_count();
   set_corpus();
   mu = 0;
   trunc_len = 50;
}

void RMM::train()
{
   EM();
}

//get function
const map<string, double> &RMM::get_alpha()
{
   return alpha;
}

const map<string, double> &RMM::get_R()
{
   return R;
}

//print function
void RMM::printMap( const map<string, double> &m )
{
   for( map<string, double>::const_iterator it = m.begin(); it != m.end(); it++ )
      cout << it->first << ": " << it->second << endl;
}

void RMM::printMap( const map<string, int> &m )
{
   for( map<string, int>::const_iterator it = m.begin(); it != m.end(); it++ )
      cout << it->first << ": " << it->second << endl;
}

/**********************************
 *        Utility function        *
 **********************************/

//utility function
void RMM::set_count()
{
   for( int i = 0; i < doclist.size(); i++ ){
      map<string, int> c;

      for( int j = 0; j < doclist[i].size(); j += 3 ){
         string ch = doclist[i].substr(j, 3);
         c[ch]++;
      }

      count.push_back(c);
   }
}

void RMM::set_corpus()
{
   for( int i = 0; i < doclist.size(); i++ )
      for( int j = 0; j < doclist[i].size(); j += 3 ){
         string ch = doclist[i].substr(j, 3);
         if( find( corpus.begin(), corpus.end(), ch ) == corpus.end() )
            corpus.push_back(ch);
      }
}

void RMM::Estep()
{
   for( int i = 0; i < doclist.size(); i++ ){
      map<string, double> prob;
      
      for( map<string, int>::iterator it = count[i].begin(); it != count[i].end(); it++ ){
         string sentence = doclist[i];
         string word = it->first;
         prob[word] = ( alpha[sentence]*R[word] ) / ( alpha[sentence]*R[word] + (1-alpha[sentence])*B[word] ); 
      }

      postprob.push_back(prob);
   }
}

void RMM::Mstep()
{
   adjust_alpha();
   adjust_R();
}

void RMM::adjust_alpha()
{
   for( int i = 0; i < doclist.size(); i++ ){
      double total = 0.0;
      double exp = 0.0;
      for( map<string, int>::iterator it = count[i].begin(); it != count[i].end(); it++ ){
         double prob = 0.0;
         prob = postprob[i].at(it->first) * count[i].at(it->first);
         exp += prob;
         total += count[i].at(it->first);
      }

      alpha.at(doclist[i]) = exp / total;
   }
}

void RMM::adjust_R()
{
   map<string, double> newR;
   double totalExp = 0.0;

   for( vector<string>::iterator itv = corpus.begin(); itv != corpus.end(); itv++ ){
      double wordExp = 0.0;
      double corpusExp = 0.0;

      for( int i = 0; i < doclist.size(); i++ ){
         if( postprob[i].find(*itv) != postprob[i].end() )
            wordExp += postprob[i].at(*itv) * count[i].at(*itv);

         for( map<string, int>::iterator it = count[i].begin(); it != count[i].end(); it++ ){
            if( postprob[i].find(*itv) != postprob[i].end() )
               corpusExp += postprob[i].at(it->first) * count[i].at(it->first);
         }
      }

      if( Q.find(*itv) != Q.end() )
         newR[*itv] = (wordExp + mu*Q.at(*itv) )/ (corpusExp + mu);
      else
         newR[*itv] = wordExp / (corpusExp + mu);
   }

   vector< pair<string, double> > container;

   for( map<string, double>::iterator it = newR.begin(); it != newR.end(); it++ )
      container.push_back( make_pair<string, double>(it->first, it->second) );

   sort( container.begin(), container.end(), comp_second );

   R.clear();
   double totalprob = 0.0;

   int len;
   if( trunc_len > container.size() )
      len = container.size();
   else
      len = trunc_len;

   for( int i = 0; i < len; i++ ){
      string key = container[i].first;
      double prob = container[i].second;
      R[key] = prob;
      totalprob += prob;
   }

   double sum = 0.0;
   for( map<string, double>::iterator it = R.begin(); it != R.end(); it++ ){
      it->second /= totalprob;
      sum += it->second;
   }
}

void RMM::clear()
{
   alpha.clear();
   doclist.clear();
   R.clear();
   B.clear();
   Q.clear();
   count.clear();
   corpus.clear();
   postprob.clear();
   mu = 0;
}

void RMM::EM()
{
   Estep();
   Mstep();
}

void RMM::EM( int m )
{
   mu = m;
   Estep();
   Mstep();
}

/*********************************/
/*     Global scope function     */   
/*********************************/
bool comp_second( const pair<string, double> &left, const pair<string, double> &right )
{  return left.second > right.second;  }

#endif

