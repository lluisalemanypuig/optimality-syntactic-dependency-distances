#ifndef _FREQUENCY_SPECTRUM_

#define _FREQUENCY_SPECTRUM_

#include <vector>

using namespace std;

class frequency_spectrum: vector<int> {
   double n;
   public: 
      frequency_spectrum(int x_max);
      void insert(int x);
      double samples() const;
      double mean() const;
};

#endif

