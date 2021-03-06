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
   time_t t0, tf;

   map< string, vector<double> > RNN;
   vector<string> querylist;
   vector<string> docfilename;

   LM bgmodel;
   vector<LM> docmodel;
   vector<LM> querymodel;

   cout << "Read model in ..." << endl;

   time(&t0);

   //read in
   read(argc, argv, RNN, bgmodel, docmodel, querymodel, querylist, docfilename);

   time(&tf);

   cout << "Read in model already"
        << "\nRead time: ";
   printMin(tf-t0);
   cout << endl;

   const int size = 40;
   int index[size] = {1, 2, 3, 4, 5 ,
                     6 , 7,8 ,9 ,10,
                     11 , 12 ,13,14,15,
                     16,17,18,19,20,
                     21,22,23,24,25,
                     26,27,28,29,30,
                     31,32,33,34,35,
                     36,37,38,39,40}; //query index of selected query
   vector<int> q_index;
   for( int i = 0; i < size; i++ )
      q_index.push_back(index[i]);
   int query_num = size;


   //set some parameters
   double range = (double)atof(argv[6]);
//   int query_num = querymodel.size();
   ofstream ofile;

   time(&t0);

   //rank for every query
   for( int a = 0; a < query_num; a++ ){
      int i = q_index[a]-1;

      clock_t start, end;
      time_t t_0, t_f;

      cout << "first pass retrieval"
           << "\nRanking query " << i+1 << " ..."
           << endl;

      time(&t_0);

      //first pass retrieval
      vector<string> feedback_corpus;
      vector< pair<string, double> > rank_list;

      rank(querymodel[i], docmodel, bgmodel, docfilename, rank_list);
      feedback_corpus = make_corpus( docmodel, docfilename, rank_list, 10);

      time(&t_f);

      cout << "Feedback corpus size = " << feedback_corpus.size() << endl;
      cout << "Execution time of making feedback corpus: ";
      printMin(t_f-t_0);
      cout << endl;

      cout << "\nUpdating query model " << i+1 << " ..." << endl;

      start = clock();

      //query expansion
      preLM pmodel(RNN, querymodel[i]);
      pmodel.read_corpus( feedback_corpus );
      pmodel.expand_model(range, "sub");
      querymodel[i] = mix(0.4, pmodel.get_model(), 0.6, querymodel[i]);

      end = clock();

      cout << "Execution time of updating model: ";
      printSec(end-start);
      cout << endl;

      cout << "\nModel after query expansion: " << endl;
      cout << "query" << i+1 << ": " << querylist[i] << endl;
      printModel(querymodel[i]);
      cout << endl;

      ofile.open(argv[5], fstream::in | fstream::out | fstream::app );

      cout << "\nRanking for query " << i+1 << " ..." << endl;

      start = clock();
      time(&t_0);

      //rank
      rank(querymodel[i], docmodel ,bgmodel, docfilename, rank_list);

      //write result
      write_result(i, querylist[i], rank_list, ofile);

      end = clock();
      time(&t_f);

      cout << "\nExecution time for ranking query " << i+1 << ": ";
      printMin(t_f-t_0);
      cout << endl;

      ofile.close();
   }

   time(&tf);

   cout << "\n\nTotal execution time: ";
   printHr(tf-t0);
   cout << endl;
}
