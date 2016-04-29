#include "read.h"
#include "preLM.h"
#include "operation.h"
#include <stdlib.h>
#include "model.h"
#include <ctime>
using namespace std;

//cmd: ./rank wordproj corpus.txt query.txt corpus.map output $range
int main( int argc, char** argv )
{
   time_t t0;
   time_t tf;

   map< string, vector<double> > RNN;
   vector<string> querylist;
   vector<string> docfilename;

   Model bgmodel;
   vector<Model> docmodel;
   vector<Model> querymodel;


   //read in 
//   read(argc, argv, RNN, doc_w, doc_c, bg_w, bg_c, query_w, query_c, querylist, docfilename);
   read(argc, argv , RNN , bgmodel , docmodel , querymodel , querylist,docfilename);

   cout << "Read in model already" << endl;
   
   const int size = 5;
   int index[size] = {132, 128, 77, 86, 60}; //query index of selected query
   vector<int> q_index;
   for( int i = 0; i < size; i++ )
      q_index.push_back(index[i]);

   int query_num = size;

   double range = (double)atof(argv[6]);

   time(&t0);

   //query expansion
   for( int i = 0; i < query_num; i++ ){
      int num = q_index[i]-1;

      cout << "Updating query model " << i+1 << endl;
      preLM pmodel(RNN, querymodel[num]);
      pmodel.expand_model(range);
      querymodel[num] = mix(0.4, pmodel.get_model(), 0.6, querymodel[num] );
   }

   time(&tf);

   cout << "\nExecution time of updating model: ";
   printTime(tf-t0);
   cout << endl; 

   cout << "After query expansion: " << endl;
   for( int i = 0; i < query_num; i++ ){
      int num = q_index[i]-1;

      cout << "query" << num << ": " << querylist[num] << endl;
      printModel(querymodel[num]);
      cout << endl;
   }

   ofstream ofile;
   ofile.open(argv[5]);

   time_t T0, Tf;

   time(&T0);

   //rank & output
   for( int i = 0; i < query_num; i++ ){
      int num = q_index[i];

      cout << "\nRanking for query " << num << endl;

      time(&t0);

      //rank
      vector< pair<string, double> > rank_list;
      rank(querymodel[num], docmodel, bgmodel, docfilename, rank_list);

      //write result
      write_result(num, querylist[num], rank_list, ofile);

      time(&tf);

      cout << "Execution time for query " << num << ": "; 
      printTime(tf-t0);
      cout << endl;
   }

   time(&Tf);

   cout << "\nTotal Execution time: ";
   printTime(Tf-T0);
   cout << endl;

   ofile.close();
}
