#include <cassert>
#include "frequency_spectrum.hpp"

frequency_spectrum::frequency_spectrum(int x_max): vector<int>(x_max + 1, 0) { 
   n = 0;
}

void frequency_spectrum::insert(int x) {
   assert(0 <= x);
   assert(x <= int(size()) - 1);    
   ++(*this)[x];
   ++n;
} 

double frequency_spectrum::mean() const {
   // the calculation of the mean is designed to minimize the risk of overflow
   double m = 0;
   for (int x = 1; x < int(size()); ++x) {
       m += ((*this)[x]/n)*x; 
   }
   assert(m >= 0);
   assert(m <= int(size()) - 1);
   return m;   
} 

double frequency_spectrum::samples() const {
   return n;
}

