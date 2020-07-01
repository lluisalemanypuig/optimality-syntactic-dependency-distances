#ifndef _PERMUTATION_
#define _PERMUTATION_

#include <vector>

using namespace std;

class permutation: public vector<int> {
public:
   permutation(int vertices);
   void scramble();
};

#endif

