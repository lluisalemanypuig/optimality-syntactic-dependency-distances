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

