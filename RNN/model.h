#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

class Model
{
public:
   //data member
   vector<string> word;
   vector<double> prob;

   //member function
   Model();
   void source( const map<string,double> &, const map<string, double> & );
   size_t size();
   size_t size() const;
   void set_size( size_t );
   const Model&operator=( const Model & );
   void resize( const Model & );

private:
   //data member
   size_t model_size;

};

Model::Model()
{
   //enpty body
}

void Model::source( const map<string, double> &bgmodel, const map<string,double> &model )
{
   for( map<string,double>::const_iterator it = bgmodel.begin(); it != bgmodel.end(); it++ )
      word.push_back(it->first);

   model_size = word.size();

   prob.resize( model_size );

   for( map<string,double>::const_iterator it = model.begin(); it != model.end(); it++ ){
      string w = it->first;
      double p = it->second;
      
      for( int i = 0; i < model_size; i++ )
         if( word[i] == w )
            prob[i] = p;
   }
}

size_t Model::size()
{
   return model_size;
}

size_t Model::size() const
{
   return model_size;
}

void Model::set_size( size_t s )
{
   model_size = s;
}

const Model &Model::operator=( const Model & right )
{
   if( this != &right ){
      this->word = right.word;
      this->prob = right.prob;
      set_size( right.size() );
   }
}

void Model::resize( const Model & model )
{
   word = model.word;
   model_size = word.size();
   prob.resize( model_size );
}

#endif
