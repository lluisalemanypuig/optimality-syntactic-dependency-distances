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

#include <cmath>
#include <cmath>
#include <iostream> // #include <fstream>
// #include <limits.h> // si s'usa <limits.h> dona problemes!!!???
#include "average.h"

average::average()
{
biased=false;
reset();
}

average::average(bool bias)
{
biased=bias;
reset();
}

void average::insert(double v,int x)
{
if (n == 0) { 
    min = v; 
    max = v; 
}
else {
    if (v>max)
       {
       max=v;
       }
    else
       {
       if (v<min)
          {
          min=v;
          }
       }
}
sum+=x*v;
sum2+=x*v*v;
n+=x;
// Post: v has been added x times
}

double average::mean() const
{
if (n>0)
   {
   return sum/n;
   }
else
   {
   assert(false);
   return 0; // en realitat no és zero sino que està indefinit ???
   }
// Post: mean is the mean value
}

double average::first_moment() const
{
return mean();
}

double average::second_moment() const
{
assert(n>0);
return sum2/n;
// Post: mean is the 2nd moment
}

/* double average::dev(double mean) const
{
if (biased)
   {
   return safe_sqrt(sum2/n-mean*mean);
   }
else
   {
   return safe_sqrt((sum2/n-mean*mean)*n/(n-1));
   }
// Post: dev is the standard deviation
}

double average::dev() const
{
return dev(mean());
// Post: dev is the standard deviation
} */

double average::var(double mean) const
{
if (biased)
   {
   return sum2/n-mean*mean;
   }
else
   {
   return (sum2/n-mean*mean)*n/(n-1);
   }
// Post: var is the variance
}

double average::var() const
{
return var(mean());
// Post: var is the variance
}

double average::dev(double mean) const
{
return safe_sqrt(var(mean));
// Post: dev is the standard deviation
}

double average::dev() const
{
return dev(mean());
// Post: dev is the standard deviation
}

double average::coefficient_of_variation() const
{
double mean, dev;

statistics(mean, dev);
assert(mean > 0);
return dev/mean;
// Post: 
}

void average::statistics(double &x,double &y) const
{
x=mean();
y=dev(x);
}

bool average::defined() const
{
return ((!biased) && (n>1)) || ((biased) && (n>0));
}

int average::samples() const
{
return n;
}

double average::total() const
{
return sum;
}

double average::total2() const
{
return sum2;
}

inline double average::safe_sqrt(double temp) const
{
if (temp<0)
   {
   cout << "Warning: negative square root parameter " << temp << endl;
   temp=0;
   }
return sqrt(temp);
}

void average::reset()
{
// provisional ???
min=1 << 30; // numeric_limits<double>::max();
max=-(1 << 30); // numeric_limits<int>::min(); // amb double no va bé; provisional ???
sum=0;
sum2=0;
n=0;
}

