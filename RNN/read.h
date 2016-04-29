#ifndef READ_H
#define READ_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "lm.h"

using namespace std;

map< string , vector<double> > readRNN ( ifstream & ifile  );
vector <LM> readdoc( ifstream &docfile );
LM readbg( vector<LM> &docmodel);
vector<string> readquery(ifstream& queryfile);
vector<string> readname(ifstream& filename);
void freqtoprobvec( vector<LM> &freq);
void freqtoprob ( LM &freq);



void read ( int argc , char** argv ,
	map<string,vector<double> > &RNN , LM &bgmodel ,
	vector<LM> &docmodel ,  vector<LM> &querymodel ,
	vector <string> & querylist , vector<string> &docfilename)
{
   ifstream ifile;
   ifile.open(argv[1]);
   RNN = readRNN(ifile);
   ifile.close();

   ifstream docfile;
   docfile.open(argv[2]);
   docmodel = readdoc(docfile);
   docfile.close();	

   bgmodel = readbg(docmodel);


   ifstream queryfile;
   queryfile.open(argv[3]);
   querymodel = readdoc(queryfile);
   queryfile.close();


   ifstream queryfilename;
   queryfilename.open(argv[3]);
   querylist = readquery(queryfilename);
   queryfilename.close();

   ifstream filename;
   filename.open(argv[4]);
   docfilename = readname(filename);
   filename.close();

   freqtoprob(bgmodel);
   freqtoprobvec(querymodel);
   freqtoprobvec(docmodel);

}

void freqtoprobvec( vector<LM> &freq)
{
	for ( int i=0 ; i<freq.size() ; i++ )
		freqtoprob(freq[i]);
}

void freqtoprob ( LM &freq)
{
   double total=0;
   size_t size = freq.word.size();
   for ( size_t a=0 ; a<size ; a++)
      total += freq.prob[a];
   for ( size_t a=0 ; a<size ; a++ )
      freq.prob[a] /= total;
}

map< string , vector<double> > readRNN ( ifstream &ifile )
{
   map< string , vector<double> > RNN;
   int junk , dim;
//   dim = 2;
   ifile >> junk ;
   ifile >> dim ;
   string word;
   while ( ifile >> word )
   {
      int read = 0 ; 
      vector<double> probvector;
      while ( read < dim )
      {
         double prob ;
         ifile >> prob;
         probvector.push_back(prob);
         read ++;
      }
      RNN.insert( pair<string,vector<double> >(word,probvector) );

   }
   return RNN;
}

vector <LM> readdoc( ifstream &docfile )
{
  vector <LM> docmodel;
  string doc;
//  getline(docfile,doc);
//  cout << doc << endl;
  while ( getline(docfile,doc))
  {
     LM wordbase;
     size_t space=0;
     while ( space < doc.size()  )
     {
         string wordstr;
         size_t nextspace = doc.find_first_of(" " , space+1);
         if ( nextspace > doc.size() ) break;
         if ( space == 0 )
           wordstr = doc.substr(space,nextspace-space);
         else
           wordstr = doc.substr(space+1 , nextspace-space-1 );
         
         //cout << space << " " << nextspace <<" " << word << endl;
         space = nextspace;

         vector<string> ::iterator it;
         it = find(wordbase.word.begin(), wordbase.word.end(), wordstr);
         if ( it == wordbase.word.end() )
         {
         	wordbase.word.push_back(wordstr);
         	wordbase.prob.push_back(1);
         }
         else
         {
         	int idx = it - wordbase.word.begin();
         	wordbase.prob[idx]++;
         }
     }
//    printmap(wordbase);

    docmodel.push_back(wordbase);
  }
  return docmodel;
}


LM readbg( vector<LM> &docmodel)
{
   LM bg;
   for ( size_t i=0 ; i<docmodel.size() ; i++ )
   {
   	  for ( size_t a=0 ; a<docmodel[i].word.size() ; a++)
   	  {
   	  	vector<string>:: iterator bgit;
   	  	bgit = find(bg.word.begin(), bg.word.end(), docmodel[i].word[a]);
   	  	if ( bgit == bg.word.end())
   	  	{
   	  		bg.word.push_back(docmodel[i].word[a]);
   	  		bg.prob.push_back(docmodel[i].prob[a]);
   	  	}
   	  	else
   	  	{
   	  		int idx = bgit-bg.word.begin();
   	  		bg.prob[idx] += docmodel[i].prob[a];
   	  	}
   	  }
   }
   return bg;
}

vector<string> readquery(ifstream& queryfile)
{
   vector<string> list;
   string line;
   while( getline(queryfile,line))
   {
//      cout << "line:   " << line <<endl;
      string query;
      size_t space=0;
      while(space<line.size())
      {
         string word;
         size_t nextspace = line.find_first_of(" " ,space+1);
         if (nextspace>line.size() ) break;
         if (space == 0)
            query = line.substr(space,nextspace-space);
         else
            query += line.substr(space+1,nextspace-space-1);
         space = nextspace;
      }
      list.push_back(query);
    //  cout << query << endl;
   }
   return list;
}

vector<string> readname(ifstream& filename)
{
   vector<string> namelist;
   string name;
   while(getline(filename,name))
      namelist.push_back(name);
//   cout << namelist[40];
   return namelist;
}

#endif
