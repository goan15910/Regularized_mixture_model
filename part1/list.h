#include <iostream>
#include <string>
using namespace std;

typedef struct{
   string* list;
   int size;
}LIST;

//int* ptr = new int[10];

static void initialize( LIST* l )
{
   l->size = 1;
   l->list = new string[l->size];   
}

static void resize( LIST* l )
{
   string accumulator[l->size];
   for( int i = 0; i < l->size; i++ ){
      accumulator[i] = l->list[i];
   }

   delete [] l->list;
   l->size++;
   l->list = new string[l->size];
   for( int i = 0; i < l->size-1; i++ ){
      l->list[i] = accumulator[i];
   }
}

static void list_expansion( LIST* l, const string* new_name )
{
   resize(l);
   l->list[l->size-1] = *new_name;
}

static void assign( LIST* l, const int query_num, const string* filename)
{
   list_expansion( &l[query_num], filename );
}


