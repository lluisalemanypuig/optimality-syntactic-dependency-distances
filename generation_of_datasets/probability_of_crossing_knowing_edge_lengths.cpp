/*********************************************************************
 *
 * Optimality Syntactic Dependency Distances
 * 
 * Copyright (C) 2020
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
 *     Ramon Ferrer i Cancho (rferrericancho@cs.upc.edu)
 *         LARCA (Laboratory for Relational Algorithmics, Complexity and Learning)
 *         CQL (Complexity and Quantitative Linguistics Lab)
 *         Office S124, Omega building     Jordi Girona St 1-3, Campus Nord UPC, 08034 Barcelona.    CATALONIA, SPAIN
 *         Webpage: https://cqllab.upc.edu/people/rferrericancho/
 *
 ********************************************************************/

#include <cassert>
#include <iostream>
#include <fstream>
#include "probability_of_crossing_knowing_edge_lengths.hpp"

probability_of_crossing_knowing_edge_lengths::probability_of_crossing_knowing_edge_lengths() {
   n = 0;
}

probability_of_crossing_knowing_edge_lengths::probability_of_crossing_knowing_edge_lengths(int vertices) {
   n = vertices;
   p_crossing = vector<vector<double> >(n + 1, vector<double>(n + 1, 0));
   for (int d1 = 1; d1 < n; ++d1) {
      for (int d2 = 1; d2 < n; ++d2) {
          p_crossing[d1][d2] = get_probability_of_crossing(d1, d2);
      }
   } 
   check_symmetry();
   for (int d = 1; d < n; ++d) {
       assert(p_crossing[d][1] == 0);
       assert(p_crossing[1][d] == 0);
       assert(p_crossing[d][n-1] == 0);
       assert(p_crossing[n-1][d] == 0);
   }
   assert(p_crossing[n-2][n-2] == 1);
   assert(p_crossing[n-1][n-1] == 0);
   assert(p_crossing[n-1][n-2] == 0);
   assert(p_crossing[n-2][n-1] == 0);
}

inline void probability_of_crossing_knowing_edge_lengths::check_symmetry() const {
   for (int d1 = 1; d1 < n; ++d1) {
       for (int d2 = 1; d2 < n; ++d2) {
           assert(p_crossing[d1][d2] == p_crossing[d2][d1]);
       }
   }
}

double probability_of_crossing_knowing_edge_lengths::get_probability_of_crossing(int d1, int d2) const {
   assert(n>0);
   assert(1 <= d1);
   assert(d1 < n);
   assert(1 <= d2);
   assert(d2 < n);
   int pairs = 0;
   int crossings = 0;
   for (int start1 = 1; start1 <= n - d1; ++start1) {
       int end1 = start1 + d1;
       assert(end1 <= n);
       for (int start2 = 1; start2 <= n - d2; ++start2) {
           int end2 = start2 + d2;
           assert(end2 <= n);           
           if (start1 != start2 and start1 != end2 and end1 != start2 and end1 != end2) {
              ++pairs;
              crossings += edge_crossing(start1, end1, start2, end2);
           }
       }
   }
   double p;
   if (pairs == 0) {
//      cout << "No pair could be defined with lengths " << d1 << " and " << d2 << endl;
      p = 0;
   }
   else {
      assert(pairs > 0);
      p = crossings/double(pairs);
   }
   assert(0 <= p);
   assert(p <= 1);
   return p;
   // Post: returns the probability that an edge of length d1 and another edge of length d2 cross in a sentence of length n 
   //       knowing that the edges do not share any vertex
}

bool probability_of_crossing_knowing_edge_lengths::edge_crossing(int start1, int end1, int start2, int end2) const {
   assert(start1 <= end1);
   assert(start2 <= end2);

   if (start1 < start2) return start2 < end1 and end1 < end2;
   else return start1 < end2 and end2 < end1;
}

double probability_of_crossing_knowing_edge_lengths::probability_of_crossing(int d1, int d2) const {
   assert(1 <= d1);
   assert(d1 <= n - 1);
   assert(1 <= d2);
   assert(d2 <= n - 1);
   return p_crossing[d1][d2];
}

int probability_of_crossing_knowing_edge_lengths::vertices() const {
   return n;
}

void probability_of_crossing_knowing_edge_lengths::write(const char *file) const {
  ofstream out(file);
  for (int d1 = 1; d1 < n; ++d1) {
      out << p_crossing[d1][1];
      for (int d2 = 2; d2 < n; ++d2) {
          out << " " << p_crossing[d1][d2];
          // Checking that the probability is symmetric
          assert(p_crossing[d1][d2] == p_crossing[d2][d1]);
      }
      out << endl;
  }
}

void probability_of_crossing_knowing_edge_lengths::write_3D(const char *file) const {
  ofstream out(file);
  for (int d1 = 1; d1 < n; ++d1) {
      for (int d2 = 1; d2 < n; ++d2) {
          out << d1 << " " << d2 << " " << p_crossing[d1][d2] << endl;
          // Checking that the probability is symmetric
          assert(p_crossing[d1][d2] == p_crossing[d2][d1]);
      }
  }
}

