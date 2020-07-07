/*********************************************************************
 *
 * Optimality Syntactic Dependency Distances
 * 
 * Copyright (C) 2020 Juan Luis Esteban
 *
 * This file is part of Optimality Syntactic Dependency Distances.
 *
 * Optimality Syntactic Dependency Distances is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Optimality Syntactic Dependency Distances is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Linear Arrangement Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact:
 * 
 *     Juan Luis Esteban (esteban@cs.upc.edu)
 *         Office 110, Omega building     Jordi Girona St 1-3, Campus Nord UPC, 08034 Barcelona.    CATALONIA, SPAIN
 *         Webpage: https://www.cs.upc.edu/~esteban/
 *
 ********************************************************************/

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

