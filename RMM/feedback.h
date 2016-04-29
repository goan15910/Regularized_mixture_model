#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <map>
using namespace std;

static void EM( map<string, double> &alpha, const vector<string> &docList, map<string, double> &R, const map<string, double> &B, const map<string, double> &Q );
static void cal_count( vector< map<string, int> > &count, const vector<string> &docList );
static void set_alpha( map<string, double> &a, const map<string, double> &alpha, const vector<string> &docList );
static void cal_postProb( vector< map<string, double> > &postProb, const map<string, double> &a, const map<string, double> &R, const map<string, double> &B , const vector< vector<string> > &vocab );
bool sort_second( const pair<string, double> &left, const pair<string, double> &right );
static void adjust_alpha( const map<string, double> &a, map<string, double> &alpha );

static void EM( map<string, double> &alpha, const vector<string> &docList, map<string, double> &R, const map<string, double> &B, const map<string, double> &Q )
{
   const int doc_num = docList.size();
   vector< map<string, int> > count;
   vector<double> a;
   count.resize(doc_num);
   a.resize(doc_num);

   cal_count(count, docList);
   set_alpha(a, alpha, docList);

   vector< map<string, double> > postProb;
   postProb.resize(doc_num);

   vector<string> corpusV;
   vector< vector<string> > vocab;
   vocab.resize(doc_num);
   for( int i = 0; i < doc_num; i++ )
      for( map<string, int>::iterator it = count[i].begin(); it != end(); it++ ){
         vocab[i].push_back( it->first );
         corpusV.push_back( it->first );
      }

   //Estep
   cal_postProb( postProb, a, R, B, vocab );

   //Mstep
   const int mu = 1000;

   vector< pair<string, double> > ACC;

   //updating R
   double alpha_dacc = 0.0;
   double d_acc = 0.0;
   map<string, double> n_acc;
   map<string, double> alpha_nacc;
   //alpha_nacc.resize( a.size() );
   for( vector<string>::iterator it = corpusV.begin(); it != corpusV.end(); it++ ){
      //updating alpha D
      for( int i = 0; i < a.size(); i++ ){
         map<string, double>::iterator itP = postProb.find(*it);
         if( count[i].find(*it) != map::end ){
            alpha_nacc.insert( pair<string, double>(itP->first, itP->second) );
            alpha_dacc += itP->second;
         }
      }

      //updating R model
      for( int i = 0; i < doc_num; i++ ){
         map<string, double>::iterator itW = count[i].find(*it);
         map<string, double>::iterator itP = postProb[i].find(*it);
         if( itW != map::end )
            n_acc.insert( pair<string, double>(itW->first, itP->second) );
      }
   }

   //finish setting new alpha D
   for( map<string, double>iterator it = a.begin(); it != a.end(); it++ ){
      map<string, double>iterator itA = alpha_nacc.find(it->first);
      it->second = itA-second / alpha_dacc;
   }

   for( map<string, double>::iterator it = n_acc.begin(); it != n_acc.end(); it++ )
      d_acc += it->second;
            
   for( map<string, double>::iterator it = n_acc.begin(); it != n_acc.end(); it++ ){
      double probQ = 0.0;
      if( Q.find(it->first) != map::end )
         probQ = Q.find(it->first)->second;
      ACC.push_back( make_pair(it->first, ( it->second + mu * probQ ) / ( d_acc + mu ) ) );
   }

   sort( ACC.begin(), ACC.end(), sort_second );
   const int Rsize = 20;
   R.clear();
   for( vector< pair<string, double> >::iterator it = ACC.begin(); *it != ACC[Rsize-1]; it++  )
      R.insert( pair<string, double>(it->first, it->second) );

   adjust_alpha(a, alpha);
   
}

static void cal_count( vector< map<string, int> > &count, const vector<string> &docList )
{
   for( int i = 0; i < count.size(); i++ )
      for( int j = 0; j < docList[i].size(); j += 3 ){
         string c;
         c = docList[i].substr(j, 3);
         if( j == 0 )
            count[i].insert( pair<string, int>(c, 1) );
         else{
            map<string, int>::iterator it = count[i].find(c);
            if( it != map::end )
               it->second++;
            else{
               it->first = c;
               it->second = 1;
            }
         }
      }
}

static void set_alpha( map<string, double> &a, const map<string, double> &alpha, const vector<string> &docList )
{
   for( int i = 0; i < docList.size(); i++ ){
      map<string, double>::iterator it = alpha.find( docList[i] );
      a.insert( pair<string, double>(it->first, it->second) );
   }
}

static void cal_postProb( vector< map<string, double> > &postProb, const map<string, double> &a, const map<string, double> &R, const map<string, double> &B, const vector< vector<string> > &vocab )
{
   for( int i = 0; i < postProb.size(); i++ )
      for( int j = 0; j < vocab[i].size(); j++ ){
         map<string, double>::iterator itR = R.find( vocab[i][j] );
         map<string, double>::iterator itB = B.find( vocab[i][j] );
         map<string, double>::iterator itA = a.find( vocab[i][j] );
         double prob = itA.second*(*itR) / ( itA->second*(*itR) + (1-itA.second)*(*itB) );
         postProb[i].insert( pair<string, double>(vocab[i][j], prob) );
      }
   
}

bool sort_second( const pair<string, double> &left, const pair<string, double> &right )
{
   return left.second > right.second;
}

static void adjust_alpha( const map<string, double> &a, map<string, double> &alpha )
{
   for( map<string, double>::iterator it = a.begin(); it != a.end(); it++ ){
      map<string, double>::iterator itA = alpha.find(it->first);
      itA->second = it->second;
   }
}

#endif
