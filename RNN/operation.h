#ifndef OPERATION_H
#define OPERATION_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include <ctime>
#include <cmath>
#include "read.h"
#include "lm.h"
using namespace std;

time_t t0;
time_t tf;

//utility function
LM mix( double a1, const LM &lm1, double a2, const LM &lm2 );
vector<double> smooth( double lm_ratio, const LM &lm, double bg_ratio, const LM &bg );
double KL_div( const LM &qmodel, const LM &docmodel, const LM &bgmodel );
double KL_div_fast( const LM &qmodel, const LM &docmodel, const LM &bgmodel );
void rank( const LM &qmodel, const vector<LM> &docmodel, const LM &bgmodel, const vector<string> &doc_filename, vector< pair<string, double> > &doclist );
void rank_fast( const LM &qmodel, const vector<LM> &docmodel, const LM &bgmodel, const vector<string> &doc_filename, vector< pair<string, double> > &doclist );
void write_result( int query_num, string query, const vector< pair<string, double> > &rank_list, ofstream &ofile );
vector<string> make_corpus( const vector<LM> &docmodel, const vector<string> &filename, const vector< pair<string, double> > &rank_list, int corpus_size );
bool sort_second(const pair<string,double>& left, const pair<string,double>& right);
void printHr( time_t );
void printMin( time_t );
void printSec( clock_t );

//print out function
void printModel( const map<string, double> & );
void printModel( const LM & );
void printRNN( const map< string, vector<double> > & );

//utility function
LM mix( double a1, const LM &lm1, double a2, const LM &lm2 )
{
   map<string, double> m1, m2, m3;
   m1 = ltom(lm1);
   m2 = ltom(lm2);
   m3 = m1;

   for( map <string,double>::iterator it = m3.begin(); it != m3.end() ; it++ ){
      it->second *= a1 ;
   }

   for( map<string, double>::const_iterator it2 = m2.begin(); it2 != m2.end(); it2++ ){
      string word = it2->first;
      m3[word] += m2.at(word)*a2;
   } 

   return mtol(m3);
}

vector<double> smooth( double lm_ratio, const LM &lm, double bg_ratio, const LM &bg )
{
   vector<double> v;
   int lm_size = lm.word.size();
   int count = 0;

   for( int i= 0; i < bg.word.size(); i++ ){
      string w = bg.word[i];
      double p;

      vector<string>::const_iterator it;
      int idx;

      if( count < lm_size ){
         it = find(lm.word.begin(), lm.word.end(), w);
         idx = it - lm.word.begin();
      }

      if( it != lm.word.end() && count < lm_size ){
         p = lm_ratio * lm.prob[idx] + bg_ratio * bg.prob[i];
         count++;
      }
      else
         p = bg_ratio * bg.prob[i];

      v.push_back(p);
   }
   
   return v;
}

double KL_div( const LM &qmodel, const LM &docmodel, const LM &bgmodel )
{
   vector<double> Q;
   vector<double> D;

   Q = smooth(0.95, qmodel, 0.05, bgmodel);
   D = smooth(0.95, docmodel, 0.05, bgmodel);

   double kl_div = 0.0;

   for( int i = 0; i < Q.size(); i++ )
      kl_div += Q[i] * log( D[i] ); //kl_div *= D[i]^Q[i]

   return exp(kl_div);
}

double KL_div_fast( const LM &qmodel, const LM &docmodel, const LM &bgmodel )
{
   vector<double> doc_smooth;

   for( int i = 0; i < qmodel.word.size(); i++ ){
      string w = qmodel.word[i];
      vector<string>::const_iterator dit, bgit;
      dit = find(docmodel.word.begin(), docmodel.word.end(), w);
      bgit = find(bgmodel.word.begin(), bgmodel.word.end(), w);

      int bg_idx = bgit - bgmodel.word.begin();
      double bg_prob = bgmodel.prob[bg_idx];

      if( dit == docmodel.word.end() )
         doc_smooth.push_back( 0.05*bg_prob );
      else{
         int doc_idx = dit - docmodel.word.begin();
         double doc_prob = docmodel.prob[doc_idx];
         doc_smooth.push_back( 0.95*doc_prob + 0.05*bg_prob );
      }
   }

   double kl_div = 0.0;

   for( int i = 0; i < qmodel.prob.size(); i++ )
      kl_div += qmodel.prob[i] * log( doc_smooth[i] );

   return kl_div;
}

void rank( const LM &qmodel, const vector<LM> &docs, const LM &bgmodel, const vector<string> &doc_filename, vector< pair<string, double> > &doclist )
{
   time(&t0);

   doclist.clear();

   for( int i = 0; i < docs.size(); i++ ){
      double score;
      string filename;
      score = KL_div(qmodel, docs[i], bgmodel);
      filename = doc_filename[i];

      doclist.push_back( make_pair<string, double>(filename, score) );
   }

   time(&tf);

   cout << "Execution time of computing KL for one query: ";
   printMin(tf-t0);

   //sort doclist
   sort( doclist.begin(), doclist.end(), sort_second );
}

void rank_fast( const LM &qmodel, const vector<LM> &docs, const LM &bgmodel, const vector<string> &doc_filename, vector< pair<string, double> > &doclist )
{
   time(&t0);

   doclist.clear();

   for( int i = 0; i < docs.size(); i++ ){
      double score;
      string filename;
      score = KL_div(qmodel, docs[i], bgmodel);
      filename = doc_filename[i];

      doclist.push_back( make_pair<string, double>(filename, score) );
   }

   time(&tf);

   cout << "Execution time of computing KL for one query: ";
   printMin(tf-t0);

   //sort doclist
   sort( doclist.begin(), doclist.end(), sort_second );
}

//for one query
void write_result( int query_num, string query, const vector< pair<string, double> > &rank_list, ofstream &ofile )
{
   for( int i = 0; i < rank_list.size(); i++ )
      ofile << query_num+1 << " 0 " << rank_list[i].first << " 0 " << rank_list[i].second << " EXP" << endl;
}

vector<string> make_corpus( const vector<LM> &docmodel, const vector<string> &filename, const vector< pair<string, double> > &rank_list, int corpus_size )
{
   vector<string> corpus;

   vector<int> index;
   for( int i = 0; i < corpus_size; i++ ){
      string name = rank_list[i].first;
      int idx;

      vector<string>::const_iterator it = find(filename.begin(), filename.end(), name);
      idx = it - filename.begin();

      index.push_back(idx);
   }

   for( int i = 0; i < index.size(); i++ ){
      int idx = index[i];
      vector<string>::const_iterator begin, end;
      begin = docmodel[idx].word.begin();
      end = docmodel[idx].word.end();

      for( vector<string>::const_iterator it = begin; it != end; it++ )
         if( find(corpus.begin(), corpus.end(), *it) == corpus.end() )
            corpus.push_back(*it);
   }

   return corpus;
}

bool sort_second(const pair<string,double>& left, const pair<string,double>& right)
{        return left.second > right.second;            }

void printModel( const map<string, double> &model )
{
   int num = 1;
   for( map<string, double>::const_iterator it = model.begin(); it != model.end(); it++ ){
      cout << num << ". " << it->first << ": " << it->second << endl;
      num++;
   }
}

void printModel( const LM &lm )
{
   int idx = 1;
   for( int i = 0; i < lm.prob.size(); i++ )
      if( lm.prob[i] != 0 ){
         cout << idx << ". " << lm.word[i] << ": " << lm.prob[i] << endl;
         idx++;
      }
}

void printRNN( const map< string, vector<double> > &RNN )
{
   for( map< string, vector<double> >::const_iterator it = RNN.begin(); it != RNN.end(); it++ )
      cout << it->first << endl;
}

void printHr( time_t t )
{
   cout << t / 3600 << "hr " 
        << t % 3600 / 60 << "min " 
        << t % 60 << "sec" << endl;
}

void printMin( time_t t )
{
   cout << t % 3600 / 60 << "min "
        << t % 60 << "sec" << endl;
}

void printSec( clock_t t )
{
   cout <<  (float) t / (float) CLOCKS_PER_SEC  << "sec" << endl;
}

#endif
