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

#ifndef _AVERAGE_
#define _AVERAGE_

#include <cassert>
#include <iostream> // #include <fstream>

using namespace std;

class average {
protected:
  bool biased;
  double sum;
  double sum2;
  int n;
  double safe_sqrt(double temp) const;
  double var(double mean) const;
  double dev(double mean) const;
public:
  double max;
  double min;
  average();  
  average(bool bias);
  void insert(double v,int x=1);
  bool defined() const;
  double mean() const;
  double first_moment() const;
  double second_moment() const;
  double var() const;
  double dev() const;
  double coefficient_of_variation() const;
  void statistics(double &x,double &y) const;
  int samples() const;
  double total() const;
  double total2() const;
  void reset();
};

template <class T> void write(T x,const average &y,ofstream &out)
{
double mean,dev;

assert(y.defined());
y.statistics(mean,dev);
out << x << " " << mean << " " << dev << endl;
}


#endif
