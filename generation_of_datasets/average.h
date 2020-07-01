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
