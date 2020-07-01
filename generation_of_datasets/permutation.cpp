#include <algorithm>
#include "permutation.hpp"

permutation::permutation(int vertices): vector<int>(vertices) {
   for (int i = 0; i < vertices; ++i) {
       (*this)[i] = i;
   }
}

void permutation::scramble() {
   random_shuffle(begin(),end());
}

