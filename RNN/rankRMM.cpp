#include "read.h"
#include "RMM.h"
#include "operation.h"
#include "lm.h"
#include <stdlib.h>
#include <ctime>
using namespace std;

//cmd: ./rankRMM $wordproj corpus.txt query.txt corpus.map $output
int main( int argc, char** argv )
{
   map< string, vector<double> > RNN;
   vector<string> querylist;
   vector<string> docfilename;

   LM bgmodel;
   vector<LM> docmodel;
   vector<LM> querymodel;

   time_t start, end;
   time_t t0, tf;

   time(&t0);

   //read in
   cout << "Step00: Reading model in ..." << endl; 

   time(&start);

   read( argc, argv, RNN, bgmodel, docmodel, querymodel, querylist, docfilename );

   time(&end);

   cout << "Read in model already" 
        << "\nRead time";
   printMin(end-start);
   cout << endl;
   

   vector< vector< pair<string,double> > > rank_list;

   //baseline model ranking
   time(&start);
   cout << "\nStep01: Ranking by baseline model" << endl;
   for( int i = 0; i < querymodel.size(); i++ ){
      cout << "Ranking for query " << i << endl;

      //ranking
      vector< pair<string, double> > temp_list;
      rank(querymodel[i], docmodel, bgmodel, docfilename, temp_list);

      rank_list.push_back( temp_list );

      cout << endl;
   }
   time(&end);

   cout << "\nRanking by baseline model for ";
   printMin(end-start);
   cout << endl;

   //Pseudo Relevant Feedback
   time(&start);
   cout << "\nStep02: Pseudo Relevant Feedback by RMM" << endl;
   int iter = 5;
   for( int n = 0; n < querymodel.size(); n++ ){
      //Initialize Alpha list
      map<string, double> temp;
      for( int i = 0; i < rank_list[n].size(); i++ ){
         string docname = rank_list[n][i].first;
         temp.insert( pair<string,double>(docname, 0.5) );
      }

      cout << "Updating query " << n << "..." << endl;

      //RMM
      for( int i = 0; i < iter; i++ ){
         vector<string> topDoc;
         for( vector< pair<string,double> >::iterator it = rank_list[n].begin(); it != rank_list[n].end(); it++ )
            topDoc.push_back( it->first );

         RMM rmm( temp, topDoc, querymodel[n], bgmodel );

         //EM training
         rmm.train();

         //update querymodel & alphalist
         querymodel[n] = mtol( rmm.get_R() );
         temp = rmm.get_alpha();

         //ranking by feedback result
         vector< pair<string,double> > temp_list;
         rank(querymodel[n], docmodel, bgmodel, docfilename, temp_list);
         rank_list[n] = temp_list;
      }
   }

   cout << "\nModel after expansion:" << endl;

   for( int i = 0; i < querymodel.size(); i++ )
      printModel(querymodel[i]);

   time(&end);
   cout << "Pseudo relevant feedback for ";
   printMin(end-start);
   cout << endl;

   //wirte result
   ofstream ofile;
   ofile.open(argv[5]);

   vector< pair<string,double> > output_list;

   for( int i = 0; i < rank_list.size(); i++ )
      output_list.insert( output_list.end(), rank_list[i].begin(), rank_list[i].end() );
   
   for( int i = 0; i < querymodel.size(); i++ )
      write_result(i, querylist[i], output_list, ofile);
  
   cout << "\nAll works done" << endl;

   time(&tf);

   cout << "Total Execution time:";
   printHr(tf-t0);
   cout << endl;
}
