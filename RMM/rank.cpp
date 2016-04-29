#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include <numeric>
#include <math.h>
#include <map>
#include <iomanip>
using namespace std;
double map_acc(int previous , const pair<string,double> &r);
void print(map<string,double>& bgmodel);
void calprob ( map<string,double> & model);
void createnewQ ( map<string,double> & model , vector<string> word , vector<double>& newQ);
void mix ( vector<double>& q , vector<double>& b , vector<double>& d );
void ranking ( double &score , vector<double> q , vector<double> d );
bool sort_second(const pair<string,double>& left, const pair<string,double>& right);

int main (int argc , char** argv)
{  
   ifstream queryfile;
   queryfile.open(argv[1]);
   string query;
   string doc;
   vector<string> q;
   vector<double> qp;
   map<string,double> bgmodel;
   map<string,double> querymodel;

   int query_num = 1;

   ifstream createbg;
   createbg.open (argv[2]);
   string bg;
   while ( getline( createbg , bg ) )
   {
      size_t wordstrbegin = bg.find_first_of(" ");
      string sentence = bg.substr(wordstrbegin+1);
//      cout << sentence << endl;

      for ( int a=0 ; a < sentence.size() ; a=a+3 )
      {
         map<string,double>::iterator mapit;
         mapit = bgmodel.find(sentence.substr(a,3));
         if ( mapit == bgmodel.end() )
         {
            bgmodel.insert( pair<string,double> ( sentence.substr(a,3) , 1 ));
         }
         else
         {
            mapit->second ++;
         }
      }
   }

   calprob ( bgmodel);

//   print ( bgmodel );
//   print ( bgmodel, ofile );
   createbg.close();
   int x=1;
   for ( int i =0 ; i<1 ; i++ )
   { getline(queryfile, query);
//   while ( getline(queryfile,query) )
  // {
      ifstream docfile;
      docfile.open(argv[2]);
      vector<pair<string,double> > rank; 
      vector<pair<string,double> > ranksentence;

      for ( int i=0 ; i< query.size() ; i+=3 )
         querymodel.insert ( pair<string,double> ( query.substr(i,3) , 1 ));
      calprob ( querymodel );
//      print (querymodel );
//      for (int b=0; b<10 ; b++ )
  //    { getline(docfile,doc);
      while ( getline (docfile,doc) )
      {
         size_t wordstrbegin = doc.find_first_of(" ");
         string sentence = doc.substr(wordstrbegin+1);
         string name = doc.substr(0,wordstrbegin);
         map<string,double> docmodel;
         for ( int a=0 ; a < sentence.size() ; a=a+3 )
         {
            map<string,double>::iterator mapit;
            mapit = docmodel.find(sentence.substr(a,3));
            if ( mapit == docmodel.end() )
            {
               docmodel.insert( pair<string,double> ( sentence.substr(a,3) , 1 ));
            }
            else
            {
               mapit->second ++;
            }
         }
         //cout << sentence <<endl;
         calprob ( docmodel );
  //       print ( docmodel );
         vector<string> word;
         map<string,double>::iterator wordit;
         wordit = querymodel.begin();
         for (  ; wordit!=querymodel.end() ; wordit++ )
         {
            word.push_back(wordit->first);
         }
         wordit = docmodel.begin();
         for ( ; wordit != docmodel.end() ; wordit++ )
         {
            vector<string>::iterator findword;
            findword = find( word.begin() , word.end() , wordit->first );
            if ( findword == word.end() )
               word.push_back(wordit->first);
         }
         vector<double> newQ;
         vector<double> newD;
         vector<double> newB;
         createnewQ(querymodel,word,newQ);
         createnewQ(docmodel , word,newD);
         createnewQ(bgmodel , word ,newB);
        
         mix( newQ , newB , newD );
//         cout << name << endl;
/*         if (name == "N200108151200-32-02" || name == "N200108241200-27-02")
         {
            cout << sentence << endl;
            for ( int i=0 ; i<word.size() ;i++ )
            cout << word[i] << " doc: " << fixed << setprecision(9) << newD[i] << " bg: " << newB[i] << endl;
            }
*/         double score = 1;
         ranking ( score , newQ , newD );
         rank.push_back(make_pair(name,score));
         ranksentence.push_back(make_pair(sentence,score));
   
      }

   ofstream ofile;
   ofile.open(argv[3] , fstream::in | fstream::out | fstream::app );


   sort (rank.begin() , rank.end() , sort_second );
   sort (ranksentence.begin() , ranksentence.end() , sort_second);


         
//         for ( int i=0 ; i<ranksentence.size() ; i++)
 //          ofile << query  << " " << fixed << setprecision(9) << ranksentence[i].second << " " << ranksentence[i].first<< endl;
         for ( int i=0 ; i<rank.size() ; i++ )
           ofile << x << " 0 " << rank[i].first << " 0 " << fixed << setprecision(9) <<  rank[i].second << " EXP "<< endl;
      x++ ;

   ofile.close();
   }

   


}


double map_acc(int previous , const pair<string,double> &r)
{return previous + r.second;      }

void print ( map<string,double>& model)
{
   map<string,double>::iterator printit;
   printit = model.begin();
   while( printit != model.end() )
   {
      cout << printit->first << " prob : " << printit->second << endl;
      printit++;
      }
}
void calprob ( map<string,double> & model)
{
   double sum = accumulate(model.begin() , model.end() , 0 , map_acc);
   map<string,double>::iterator it = model.begin();
   while( it != model.end() )
   {
      it->second = ((it->second) / sum) ;
      it++;
   }
}

void createnewQ ( map<string,double> & model , vector<string> word , vector<double>& newQ)
{
   vector<string>::iterator wordit;
   wordit = word.begin();
   for ( ; wordit != word.end() ; wordit ++ )
   {
      map<string,double>::iterator mapit;
      mapit = model.find( *wordit );
      if ( mapit == model.end() )
         newQ.push_back(0);
      else
         newQ.push_back( mapit->second );
   }
}

void mix ( vector<double>& q , vector<double>& b , vector<double>& d )
{
   for ( int i=0 ; i< q.size() ; i++ )
   {
//      q[i] = 0.95 * q[i] + 0.05 * b[i] ;
      d[i] = 0.95 * d[i] + 0.05 * b[i] ;
   }
}
void ranking ( double &score , vector<double> q , vector<double> d )
{
   for ( int i=0 ; i<q.size() ; i++ )
      score *= pow ( d[i] ,q[i] );
}

bool sort_second(const pair<string,double>& left, const pair<string,double>& right)
{        return left.second > right.second;            }

