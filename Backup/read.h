// ./readtest RNN corpus query.txt corpus.map



#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "model.h"

using namespace std;

map< string , vector<double> > readRNN ( ifstream & ifile  );
void printRNNmap ( map< string , vector<double> > & );
vector< map< string , double> > readdoc (ifstream & );
void printmap ( map<string,double> & );
vector< map< string , double> > wvtocv( vector< map<string,double> > & );
map<string,double> readbg( vector<map <string,double> > & );
map<string,double> wtoc( map<string,double> &);
void freqtoprob_doc( vector< map<string,double> > &);
void freqtoprob ( map<string,double> &);

vector<string> readname(ifstream& );
vector<string> readquery(ifstream&);

void vectormodel(vector < map<string,double> > &, 
                  map <string,double> &,
                  vector < map<string,double> > &,
                  Model & , vector<Model> &,
                  vector<Model> &);

void read ( int argc , char** argv ,
            map <string,vector<double> > &RNN ,
            Model &bgmodel , vector<Model> & docmodel ,
            vector<Model> &querymodel,
            vector <string> &querylist,
            vector <string> &docfilename)
{
  vector < map<string,double> > doc_w ;
  vector < map<string,double> > doc_c ;
  map <string,double> bg_w ;
  map <string,double> bg_c ;
  vector < map<string,double> > query_w;
  vector < map<string,double> > query_c;

  ifstream ifile;
  ifile.open(argv[1]);
  RNN = readRNN(ifile);
//   printRNNmap(RNN);
  ifile.close();
  ifstream docfile;
  docfile.open(argv[2]);
  doc_w = readdoc(docfile);
  docfile.close();
//   printmap(doc_w[4267]);
  doc_c = wvtocv(doc_w);
//   printmap(doc_c[5046]);
  bg_w = readbg(doc_w);
//   printmap(bg_w);
  bg_c = wtoc(bg_w);
//   printmap(bg_c);

  ifstream queryfile;
  queryfile.open(argv[3]);
  query_w = readdoc(queryfile);
  query_c = wvtocv(query_w);
  queryfile.close();

   ifstream queryfilename;
   queryfilename.open(argv[3]);
 
   querylist = readquery(queryfilename);
   queryfilename.close();


   freqtoprob_doc(query_w);
   freqtoprob_doc(query_c);
 

   freqtoprob_doc(doc_w);
   freqtoprob_doc(doc_c);
   freqtoprob(bg_w);
   freqtoprob(bg_c);


   ifstream filename;
   filename.open(argv[4]);
   docfilename = readname(filename);
   filename.close();

   cout << "Set map already" << endl;

   bgmodel.source(bg_w);
   cout << "source ready" << endl;
vectormodel(doc_w,bg_w,query_w,bgmodel,docmodel,querymodel);

   cout << "Set Model already" << endl;
}


void vectormodel(vector < map<string,double> > &doc_w, 
                  map <string,double> &bg_w,
                  vector < map<string,double> > &query_w,
                  Model &bgmodel , vector<Model> &docmodel,
                  vector<Model> &querymodel)
{
   cout << "\nIn function vectormodel:" << endl;

   for( int i = 0; i < doc_w.size(); i++){
      cout << "Pushing back temp[" << i << "]" << endl;
      Model temp;
      
      for(map<string, double>::iterator it=doc_w[i].begin() ; it!=doc_w[i].end() ; it++)
         temp[it->first] = it->second;

      docmodel.push_back(temp);
   }

   cout << "docmodel done" << endl;

   for(int i=0 ; i<query_w.size() ; i++){
      Model temp;

      for( map<string,double>::iterator it=query_w[i].begin() ; it!=query_w[i].end();it++)
         temp[it->first] = it->second;

      querymodel.push_back(temp);
   }

   cout << "querymodel done" << endl;

   for (map<string,double>::iterator it = bg_w.begin() ; it != bg_w.end() ; it++)
      bgmodel[it->first] = it->second;

   cout << " bgmodel done" << endl;

   cout << "Leaving function vectormodel\n" << endl;
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


void freqtoprob_doc( vector< map<string,double> > &doc )
{
   for ( int i=0 ; i<doc.size() ; i++ )
      freqtoprob(doc[i]);
}
void freqtoprob ( map<string,double> &freq)
{
   map<string,double>:: iterator it;
   double total=0;
   for ( it=freq.begin() ; it!=freq.end(); it++ )
      total += it->second;
   for ( it=freq.begin() ; it!=freq.end(); it++ )
      it->second /= total;
}

map<string,double> wtoc( map<string,double> &bgword)
{
   map<string,double> bgchar;
   map<string,double>::iterator wordit;
   for ( wordit = bgword.begin() ; wordit!= bgword.end(); wordit++)
   {
      map<string,double>::iterator charit;
      for ( size_t i=0 ; i<wordit->first.size() ; i+=3 )
      {
         string character;
          character = wordit->first.substr(i,3);
          charit = bgchar.find(character);
          if(charit == bgchar.end())
            bgchar.insert(pair <string,double> ( character,wordit->second));
          else
            charit->second += wordit->second;
      }
   }
   return bgchar;
}

map<string,double> readbg( vector<map <string,double> > &docword)
{
   map <string,double> bg;
   for ( size_t i=0 ; i<docword.size() ; i++ )
   {
      map<string,double>::iterator docit;
      for ( docit=docword[i].begin() ; docit != docword[i].end() ; docit++ )
      {
         map<string,double>::iterator bgit = bg.find(docit->first);
         if (bgit == bg.end() )
            bg.insert( pair<string,double> ( docit->first , docit->second ) );
         else
            bgit->second += docit->second;
      }
   }
   return bg;
}


vector < map<string,double> > wvtocv( vector< map<string,double> > &wordbase)
{
   vector < map<string,double> > charbase;
   for ( int i=0 ; i<wordbase.size() ; i++ )
   {
      map<string,double>:: iterator wordit;
      map<string,double> docchar;
      for ( wordit=wordbase[i].begin() ; wordit != wordbase[i].end() ; wordit++ )
      {
         for ( size_t a=0 ; a< wordit->first.size() ; a+=3 )
         {
            string character = wordit->first.substr(a,3);
            //cout << character << " ";

            map<string,double>:: iterator charit = docchar.find(character);
            if ( charit == docchar.end() )
               docchar.insert( pair<string,double> (character,wordit->second));
            else
               charit->second += wordit->second;
         }
         //cout << endl;
         
      }
      charbase.push_back(docchar);
   }
   return charbase;
}


map< string , vector<double> > readRNN ( ifstream &ifile )
{
   map< string , vector<double> > RNN;
   int junk , dim;
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
/*
   string s;
   while ( getline ( ifile , s ) )
   {
      string word = s.substr( s.find_first_of(" ") );

   }
  */
}

vector <map <string, double> > readdoc( ifstream &docfile )
{
  vector <map <string,double> > docmodel;
  string doc;
//  getline(docfile,doc);
//  cout << doc << endl;
  while ( getline(docfile,doc))
  {
     map <string,double> wordbase;
     size_t space=0;
     while ( space < doc.size()  )
     {
         string word;
         size_t nextspace = doc.find_first_of(" " , space+1);
         if ( nextspace > doc.size() ) break;
         if ( space == 0 )
           word = doc.substr(space,nextspace-space);
         else
           word = doc.substr(space+1 , nextspace-space-1 );
         
         //cout << space << " " << nextspace <<" " << word << endl;
         space = nextspace;
         map < string,double >::iterator it;
         it = wordbase.find( word );
         if ( it == wordbase.end() )
            wordbase.insert( pair<string,double> ( word , 1 ));
         else
            it->second ++;
     }
//    printmap(wordbase);

    docmodel.push_back(wordbase);
  }
  return docmodel;
}

void printmap ( map<string, double> &pmap )
{
   map < string,double >::iterator it;
   for ( it=pmap.begin() ; it!=pmap.end() ; it++ )
   {
      cout << it->first << ":  " << it->second << endl;
   }

}
void printRNNmap ( map<string, vector<double> > &pmap )
{
   map< string,vector<double> >::iterator it;
   for ( it=pmap.begin() ; it!=pmap.end() ; it++ )
   {
      cout << it->first << ":   " ;
  //    for ( int i=0 ; i<it->second.size() ; i++ )
//      cout << it->second[i] << " " ;
      cout << endl;
   }

}
