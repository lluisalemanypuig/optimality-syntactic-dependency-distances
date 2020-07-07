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

#ifndef _PROBABILITY_OF_CROSSING_KNOWING_EDGE_LENGTHS_
#define _PROBABILITY_OF_CROSSING_KNOWING_EDGE_LENGTHS_

#include <vector>

using namespace std;

class probability_of_crossing_knowing_edge_lengths {
private:
   vector<vector<double> > p_crossing;
   int n;
   double get_probability_of_crossing(int d1, int d2) const;
   void check_symmetry() const;
public:
   probability_of_crossing_knowing_edge_lengths();
   probability_of_crossing_knowing_edge_lengths(int vertices);
   double probability_of_crossing(int d1, int d2) const;
   bool edge_crossing(int start1, int end1, int start2, int end2) const;
   int vertices() const;
   void write(const char *file) const;
   void write_3D(const char *file) const;
};

#endif

