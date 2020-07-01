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

