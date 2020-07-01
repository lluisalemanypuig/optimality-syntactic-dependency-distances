#ifndef _UNDIRECTED_GRAPH_

#define _UNDIRECTED_GRAPH_

#include <vector>
#include <set>
#include <fstream>
#include "permutation.hpp"
#include "probability_of_crossing_knowing_edge_lengths.hpp"

using namespace std;

class undirected_graph {
private:
   int V; // number of vertices
   int E; // number of edges

   void depth_first_search(int v, vector<bool> &visited, int &visits) const;
public:
   vector<set<int> > adjacent;
   undirected_graph();
   undirected_graph(int vertices);
   void add_edge(int u, int v);
   void empty_edges();
   void remove_edges_of_vertices(const vector<int> &v, int n);
   bool edge(int u, int v) const;
   int vertices() const;
   int edges() const;
   int degree(int v) const;
   void degree_statistics(double &mean_degree, double &mean_degree2, double &variance_degree) const;
   double degree_2nd_moment() const;
   int sum_of_squared_degrees() const;
   bool connected() const;
   bool tree() const;
   int crossings_with_edge(int i, int j) const;
   int crossings_with_edge_lazy(int i, int j, int crossings_max) const;
   int crossings() const;
   int crossings_lazy(int crossings_min) const;
   double predicted_crossings() const;
   double fully_predicted_crossings(const probability_of_crossing_knowing_edge_lengths &c) const;
   double fully_predicted_crossings(const probability_of_crossing_knowing_edge_lengths &c, int &potential_crossings) const;
   void edge_length_statistics(double &mean_length, double &mean_length2, double &variance_length) const;
   vector<int> edge_length_spectrum() const;
   int edge_length_sum() const;
   int minimum_edge_length_sum_of_a_tree() const;
   double mean_edge_length() const;
   int edge_length_sum_lower_bound() const;
   double mean_edge_length_lower_bound() const;
};

int sum_of_edge_lengths_lower_bound(int degree);
undirected_graph scrambled_graph(const undirected_graph &g, const permutation &p);
vector<undirected_graph> get_connected_components(const undirected_graph &g);
undirected_graph read_graph(const char *file);
undirected_graph read_tree(const char *file);
undirected_graph read_graph_from_ifstream(ifstream &in);
undirected_graph read_tree_from_ifstream(ifstream &in);
void write_graph(const undirected_graph &g, const char *file);
void write_tree(const undirected_graph &g, const char *file);
void write_graph_on_ofstream(const undirected_graph &g, ofstream &out);
void write_tree_on_ofstream(const undirected_graph &g, ofstream &out);

#endif

