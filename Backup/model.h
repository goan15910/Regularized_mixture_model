#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <algorithm>
using namespace std;

class Model
{
public:
	//basic function
   Model();
   ~Model();
   void source( const map<string,double> & ); //source bgmodel
   size_t size();
   size_t size() const;
   double &operator[]( string );
   const double &operator[]( string ) const;
   const Model &operator=( const Model & );
   const vector<double> &get_prob();

   //static function
   static const vector<string> &get_word();

private:
   //data member
   vector<double> prob;
   
   //static data member
   static bool source_yet;
   static vector<string> word;
   static vector<Model*> Model_list;

   //utility function
   void resize( size_t );

   //static utility function
   static void source_or_not();
};

//static data member
bool Model::source_yet = false;
vector<string> Model::word;
vector<Model*> Model::Model_list;

/******************
 * Basic function *
 ******************/

Model::Model()
{
   Model_list.push_back(this);
   cout << "Constructor increment Model_list to size = " << Model_list.size() << endl;

   if( source_yet == true )
      this->resize( word.size() );
}

Model::~Model()
{
   vector<Model*>::iterator it = find( Model_list.begin(), Model_list.end(), this );
   Model_list.erase(it);

   cout << "Destructor decrement Model_list to size = " << Model_list.size() << endl;
}

void Model::source( const map<string,double> &bgmodel )
{
	for( map<string, double>::const_iterator it = bgmodel.begin(); it != bgmodel.end(); it++ )
		word.push_back(it->first);

   for( int i = 0; i < Model_list.size(); i++ )
      Model_list[i]->resize( word.size() );

   source_yet = true;
}

size_t Model::size()
{
   Model::source_or_not();

	return prob.size();
}

size_t Model::size() const
{
   Model::source_or_not();

   return prob.size();
}

double &Model::operator[]( string w )
{
	Model::source_or_not();

	for( int i = 0; i < prob.size(); i++ )
		if( word[i] == w )
			return prob[i];
}

const double &Model::operator[]( string w ) const
{
	Model::source_or_not();

	for( int i = 0; i < prob.size(); i++ )
		if( word[i] == w )
			return prob[i];
}

const Model &Model::operator=( const Model &right )
{
   if( &right != this )
      for( int i = 0; i < right.size(); i++ ){
         string w = Model::get_word()[i];
         prob[i] = right[w];
      }

   return *this;
}

const vector<double> &Model::get_prob()
{
	Model::source_or_not();
	return prob;
}


/*************************
 * Static Baisc function *
 *************************/

const vector<string> &Model::get_word()
{
   return word;
}

/********************
 * Utility function *
 ********************/

void Model::resize( size_t s )
{
   prob.resize(s);
}

/***************************
 * Static Utility function *
 ***************************/

void Model::source_or_not()
{
	if( word.size() == 0 ){
		cerr << "\nError: Do not source bgmodel yet" << endl;
		exit(1);
	}
}

#endif
