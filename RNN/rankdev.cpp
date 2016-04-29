#include "read.h"
#include "preLM.h"
#include "operation.h"
#include "lm.h"
#include <stdlib.h>
#include <ctime>
using namespace std;

//cmd: ./rank wordproj corpus.txt query.txt corpus.map output $range
int main( int argc, char** argv )
{
   map< string, vector<double> > RNN;
   vector<string> querylist;
   vector<string> docfilename;

   LM bgmodel;
   vector<LM> docmodel;
   vector<LM> querymodel;

   time(&t0);

   //read in 
   read(argc, argv , RNN , bgmodel , docmodel , querymodel , querylist,docfilename);

   time(&tf);

   cout << "Read in model already" 
        << "\nRead time: ";
   printTime(tf-t0);
   cout << endl;
  
   //set query
   const int size = 40;
   int index[size] = {9, 12, 16, 17, 20 ,
                     24 , 37, 63 ,76 , 77,
                     79 , 81 ,103,123,126,
                     132,147,141,154,46,
                     84,73,112,65,7,
                     19,28,32,36,51,
                     50,59,60,85,52,
                     145,92,90,71,124}; //query index of selected query
   vector<int> q_index;
   for( int i = 0; i < size; i++ )
      q_index.push_back(index[i]);
   int query_num = size;

   //set range
   double range = (double)atof(argv[6]);

   time(&t0);

   //query expansion
   for( int i = 0; i < query_num; i++ ){
      int num = q_index[i]-1;

      cout << "Updating query model " << q_index[i] << endl;
      preLM pmodel(RNN, querymodel[num]);
      pmodel.expand_model(range);
      querymodel[num] = mix(0.4, pmodel.get_model(), 0.6, querymodel[num] );
   }

   time(&tf);

   cout << "\nExecution time of updating models: ";
   printTime(tf-t0);
   cout << endl; 

   cout << "After query expansion: " << endl;
   for( int i = 0; i < query_num; i++ ){
      int num = q_index[i]-1;

      cout << "query" << num+1 << ": " << querylist[num] << endl;
      printModel(querymodel[num]);
      cout << endl;
   }

   ofstream ofile;
   ofile.open(argv[5]);

   time_t T0, Tf;

   time(&T0);

   //rank & output
   for( int i = 0; i < query_num; i++ ){
      int num = q_index[i] -1;

      cout << "\nRanking for query " << q_index[i] << endl;

      time(&t0);

      //rank
      vector< pair<string, double> > rank_list;
      rank(querymodel[num], docmodel, bgmodel, docfilename, rank_list);

      //write result
      write_result(num, querylist[num], rank_list, ofile);

      time(&tf);

      cout << "Execution time for query " << q_index[i] << ": "; 
      printTime(tf-t0);
      cout << endl;
   }

   time(&Tf);

   cout << "\nTotal Execution time: ";
   printTime(Tf-T0);
   cout << endl;

   ofile.close();
}
