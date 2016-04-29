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
#include "model.h"
using namespace std;

//utility function
map<string, double> mix( double a1, const map<string, double> &m1, double a2, const map<string, double> &m2 );
void mtov( const map<string, double> &m, vector<double> &v );
double KL_div( const map<string, double> &qmodel, const map<string, double> &docmodel, const map<string, double> &bgmodel );
void rank( const map<string, double> &qmodel, const vector< map<string, double> > &docmodel, const map<string, double> &bgmodel, const vector<string> &doc_filename, vector< pair<string, double> > &doclist );
void write_result( int query_num, string query, const vector< pair<string, double> > &rank_list, ofstream &ofile );
bool sort_second(const pair<string,double>& left, const pair<string,double>& right);
void printTime( time_t );

//print out function
void printModel( const map<string, double> & );
void printModel( const Model & );
void printRNN( const map< string, vector<double> > & );

//new model
Model mix(double a1 , const Model &m1 , double a2 , const Model &m2);

double KL_div( const Model &qmodel, const Model &docmodel, const Model &bgmodel );



Model mix(double a1 , const Model &m1 , double a2 , const Model &m2)
{
   Model m3;

   for ( int i= 0 ; i<m3.size() ; i++ )
   {
      string w= Model::get_word()[i];
      m3[w] = m1[w]*a1 + m2[w]*a2 ;
   }
   return m3;
}

map<string, double> mix( double a1, const map<string, double> &m1, double a2, const map<string, double> &m2 )
{
   map<string, double> m3;
   m3 = m1;

   for ( map <string,double>::iterator it = m3.begin(); it != m3.end() ; it++ )
   {
      it->second *= a1 ;
   }

   for( map<string, double>::const_iterator it2 = m2.begin(); it2 != m2.end(); it2++ ){
      string word = it2->first;
      m3[word] += m2.at(word)*a2;
   }

   return m3;
}

void mtov( const map<string, double> &m, vector<double> &v )
{
   v.clear();

   for( map<string, double>::const_iterator it = m.begin(); it != m.end(); it++ )
      v.push_back(it->second);
}

double KL_div( const Model &qmodel, const Model &docmodel, const Model &bgmodel )
{
   Model Q;
   Model D;

   Q = mix(0.95, qmodel, 0.05, bgmodel);
   D = mix(0.95, docmodel, 0.05, bgmodel);

/*   vector<double> Qv;
   vector<double> Dv;

   mtov(Q, Qv);
   mtov(D, Dv);
*/
   int size = Q.size();
   double kl_div = 0.0;

   for( int i = 0; i < size; i++ )
   {
      string w = Model::get_word()[i];

      kl_div += Q[w] * log( D[w] ); //kl_div *= D[i]^Q[i]
   }
   return exp(kl_div);
}
double KL_div( const map<string, double> &qmodel, const map<string, double> &docmodel, const map<string, double> &bgmodel )
{
   map<string, double> Q;
   map<string, double> D;

   Q = mix(0.95, qmodel, 0.05, bgmodel);
   D = mix(0.95, docmodel, 0.05, bgmodel);

   vector<double> Qv;
   vector<double> Dv;

   mtov(Q, Qv);
   mtov(D, Dv);

   int size = Qv.size();
   double kl_div = 0.0;

   for( int i = 0; i < size; i++ )
      kl_div += Qv[i] * log( Dv[i] ); //kl_div *= D[i]^Q[i]
 
   return exp(kl_div);
}

void rank( const Model &qmodel, const vector<Model> &docs, const Model  &bgmodel, const vector<string> &doc_filename, vector< pair<string, double> > &doclist )
{
   time_t t0;
   time_t tf;

   time(&t0);

   for( int i = 0; i < docs.size(); i++ ){
      //cout << "rank doc " << i << endl;
      double score;
      string filename;
      score = KL_div(qmodel, docs[i], bgmodel);
      filename = doc_filename[i];

      doclist.push_back( make_pair<string, double>(filename, score) );
         
   }

   time(&tf);

   cout << "Execution time of computing KL for one query: ";
   printTime(tf-t0);

   time(&t0);

   //sort doclist
   sort( doclist.begin(), doclist.end(), sort_second );

   time(&tf);

   cout << "Execution time of sorting for one query: ";
   printTime(tf-t0);
}

void rank( const map<string, double> &qmodel, const vector< map<string, double> > &docs, const map<string, double> &bgmodel, const vector<string> &doc_filename, vector< pair<string, double> > &doclist )
{
   time_t t0;
   time_t tf;

   time(&t0);

   for( int i = 0; i < docs.size(); i++ ){
      //cout << "rank doc " << i << endl;
      double score;
      string filename;
      score = KL_div(qmodel, docs[i], bgmodel);
      filename = doc_filename[i];

      doclist.push_back( make_pair<string, double>(filename, score) );
         
   }

   time(&tf);

   cout << "Execution time of computing KL for one query: ";
   printTime(tf-t0);

   time(&t0);

   //sort doclist
   sort( doclist.begin(), doclist.end(), sort_second );

   time(&tf);

   cout << "Execution time of sorting for one query: ";
   printTime(tf-t0);
}

//for one query
void write_result( int query_num, string query, const vector< pair<string, double> > &rank_list, ofstream &ofile )
{
   for( int i = 0; i < rank_list.size(); i++ )
      ofile << query_num+1 << " 0 " << rank_list[i].first << " 0 " << rank_list[i].second << " EXP" << endl;   
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

void printModel( const Model &model )
{
   int idx = 1;
   for( int i = 0; i < model.size(); i++ ){
      string w = Model::get_word()[i];
      if( model[w] != 0 ){
         cout << idx << ". " << Model::get_word()[i] << ": " << model[w] << endl;
         idx++;
      }
   }
}

void printRNN( const map< string, vector<double> > &RNN )
{
   for( map< string, vector<double> >::const_iterator it = RNN.begin(); it != RNN.end(); it++ )
      cout << it->first << endl;
}


void printTime( time_t t )
{
   cout << t / 3600 << "hr " 
        << t % 3600 / 60 << "min " 
        << t % 60 << "sec" << endl;
}

#endif
