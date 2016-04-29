#include "model.h"

int main()
{
   map<string, double> m;

   const int size = 5;
   string charlist[size] = {"a", "b", "c", "d", "e"};

   for( int i = 0; i < size; i++ ){
      string w = charlist[i];
      m[w] = (double)i;
   }

   Model model;
   Model clone;
   Model test_model;
   model.source(m, m);

   cout << "\nTest for regular object:" << endl;
   for( int i = 0; i < model.size(); i++ )
      cout << "Word: " << model.word[i] << " Prob: " << model.prob[i] << endl;

   clone = model;

   cout << "\nTest for cloning:" << endl;
   for( int i = 0; i < clone.size(); i++ )
      cout << "Word: " << clone.word[i] << " Prob: " << clone.prob[i] << endl;

   test_model.resize(model);

   cout << "\nTest for size:"
        << "\nSize of model is " << model.size()
        << "\nSize of clone is " << clone.size()
        << "\nSize of test_model is " << test_model.size()
        << endl;
   
}
