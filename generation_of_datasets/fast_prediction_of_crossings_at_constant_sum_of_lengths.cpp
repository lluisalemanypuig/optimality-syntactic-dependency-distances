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
#include <cmath>
#include <iostream>
#include "undirected_graph.hpp"
#include "frequency_spectrum.hpp"

const bool simple = false;
const bool equivalence_classes = true;
const double numerical_error = 1e-8;
 
double crossings_at_constant_sum_of_lengths_naive(const undirected_graph &g, int target_sum_of_lengths, vector<int> &father);
undirected_graph rearrange_vertices_in_depth_first_order(const undirected_graph &g, int start,  vector<int> &father);
void depth_first_order(const undirected_graph &g, int start, permutation &p, vector<bool> &visited, int & nvisited, vector<int> &father);
void get_sum_of_lengths_max(int n, vector<int> &sum_of_lengths_max);
void i_crossings_brute_force(const undirected_graph &g, const vector<int> &predecessor, int equivalent_permutations, const vector<int> &sum_of_lengths_max, const vector<int> &father, int target_sum_of_lengths, int start, permutation &p, int edges, int sum_of_lengths, vector<int> &remaining_degree, int &double_sum_of_lengths_of_remaining_edges_lower, int &double_sum_of_lengths_of_remaining_edges_upper, frequency_spectrum &crossings_statistics);
int get_equivalent_permutations(const undirected_graph &g, vector<int> &predecessor);
double maximum_number_of_crossings(const undirected_graph &g);
double factorial(int n);
int minimum_linear_arrangement_of_star_tree(int k);
int maximum_linear_arrangement_of_star_tree(int n, int k);
int summation(int m);

inline undirected_graph rearrange_vertices_in_depth_first_order(const undirected_graph &g, int start, vector<int> &father) {
   undirected_graph h;
   permutation p(g.vertices());
   vector<bool> visited(g.vertices(), false);
   int nvisited = 0;
   father = vector<int>(g.vertices());
   father[0] = -1;
   depth_first_order(g, start, p, visited, nvisited, father);
   assert(nvisited == g.vertices());
   h = scrambled_graph(g, p);
   assert(h.edges() == g.edges());
   assert(h.tree());
   return h;
}

void depth_first_order(const undirected_graph &g, int start, permutation &p, vector<bool> &visited, int  &nvisited, vector<int> &father) {
   if (nvisited < g.vertices() and not visited[start]) {
      visited[start] = true;
      p[start] = nvisited;
      ++nvisited;
      for (set<int>::const_iterator j = g.adjacent[start].begin(); j != g.adjacent[start].end(); ++j) {
          bool ok = not visited[*j];
          depth_first_order(g, *j, p, visited, nvisited, father);
          if (ok) father[p[*j]] = p[start];  
      }
   }
}

inline void get_sum_of_lengths_max(int n, vector<int> &sum_of_lengths_max) {
   sum_of_lengths_max = vector<int>(n);
   sum_of_lengths_max[0] = 0;
   int d = n - 1;
   int edges_of_length_d = 1;
   int x = 1; 
   while (x < n) {
       sum_of_lengths_max[x] = sum_of_lengths_max[x - 1] + d;
       --edges_of_length_d;
       if (edges_of_length_d == 0) {
          --d;
          edges_of_length_d = n - d;  
       }
       ++x; 
   }  
// Post: for x = 0, 1, ... n - 1, sum_of_lengths_max[x] is the maximum length of x edges in a graph of n vertices 
}

double crossings_at_constant_sum_of_lengths(const undirected_graph &g) {
   assert(g.tree());
   vector<int> father;
   undirected_graph h = rearrange_vertices_in_depth_first_order(g, 0, father);
   return crossings_at_constant_sum_of_lengths_naive(h, g.edge_length_sum(), father);
}

double crossings_at_constant_sum_of_lengths_naive(const undirected_graph &g, int target_sum_of_lengths, vector<int> &father) {
/* Pre: g.vertices() > 0 */
   permutation p(g.vertices());
   vector<int> predecessor(g.vertices());
   int equivalent_permutations;
   if (equivalence_classes) {
      equivalent_permutations = get_equivalent_permutations(g, predecessor); 
   }
   else { 
      equivalent_permutations = 1;
      fill(predecessor.begin(), predecessor.end(), -1);
   }
   double crossings_max = maximum_number_of_crossings(g);
   frequency_spectrum crossings_statistics(ceil(crossings_max));
   vector<int> sum_of_lengths_max;
   get_sum_of_lengths_max(g.vertices(), sum_of_lengths_max);
   vector<int> remaining_degree(g.vertices());
   int double_sum_of_lengths_of_remaining_edges_lower = 0;
   int double_sum_of_lengths_of_remaining_edges_upper = 0;
   for (int i = 0; i < g.vertices(); ++i) {
       int k = g.adjacent[i].size();
       remaining_degree[i] = k;
       double_sum_of_lengths_of_remaining_edges_lower += minimum_linear_arrangement_of_star_tree(k); 
       double_sum_of_lengths_of_remaining_edges_upper += maximum_linear_arrangement_of_star_tree(g.vertices(), k); 
   }
   i_crossings_brute_force(g, predecessor, equivalent_permutations, sum_of_lengths_max, father, target_sum_of_lengths, 0, p, 0, 0, remaining_degree, double_sum_of_lengths_of_remaining_edges_lower, double_sum_of_lengths_of_remaining_edges_upper, crossings_statistics);  
   assert(crossings_statistics.samples() > 0);
   assert(log(crossings_statistics.samples())+log(equivalent_permutations)>= log(2) - numerical_error); // there are at least two possible linear arrangements keeping the same sum of edge lengths: the original arrangement and its reverse
//   assert(int(crossings_statistics.samples()) % 2 == 0 or equivalent_permutations % 2 == 0);
   assert(fmod(crossings_statistics.samples(), 2) == 0 or equivalent_permutations % 2 == 0);
   // the total number of permutations keeping the same edge length is a multiple of two
   double crossings_mean = crossings_statistics.mean();
   assert(0 <= crossings_mean);
   assert(crossings_mean <= crossings_max);
   return crossings_mean;
/* Post: ... */
}

void i_crossings_brute_force(const undirected_graph &g, const vector<int> &predecessor, int equivalent_permutations, const vector<int> &sum_of_lengths_max, const vector<int> &father,
int target_sum_of_lengths, int start, permutation &p, int edges, int sum_of_lengths, vector<int> &remaining_degree, int &double_sum_of_lengths_of_remaining_edges_lower, int &double_sum_of_lengths_of_remaining_edges_upper, frequency_spectrum &crossings_statistics) {
   // Pre: the edge (u,v) is visited iff u < start and v < start;
   //      if predecessor[v] != -1 then v is a leaf and predecessor[v] indicates the previous leaf attacched to the same vertex than v;
   //      equivalent_permutatons indicates the number of permutations such that they only differ in the relative ordering of the leafs attached to the same node;
   //      target_sum_of_lengths is the sum of the lengths of the edges of g;     
   //      p[v] = the new position of vertex v for v = 0,1,...start - 1;    
   //      edges is the number of edges of g that have already been visited;
   //      sum_of_lengths is the sum of the lengths of the visited edges;     
   //      crossings_statistics = CS 
   assert(1 <= equivalent_permutations);
   int n = g.vertices();
   assert(start <= n);
   assert(0 <= edges);
   assert(edges <= n - 1);
   assert(edges <= sum_of_lengths);
   assert(edges < start or start == 0);  
   if (start == n) { 
      if (not simple) {
         for (int i = 0; i < n; ++i) {
             assert(remaining_degree[i] == 0);
         }
      }  
      assert(edges == n - 1);
      undirected_graph h = scrambled_graph(g, p);
      assert(h.tree());
      assert(h.edges() == n - 1);
      assert(sum_of_lengths == h.edge_length_sum()); 
      if (target_sum_of_lengths == sum_of_lengths) {
         int C = h.crossings();
         // cout << "      Crossings: " << C << endl;
         crossings_statistics.insert(C);
      }
   }
   else {
      int edges_left = n - 1 - edges;
      double lower_bound, upper_bound;
      if (simple) { 
         lower_bound = sum_of_lengths + edges_left;
         assert(sum_of_lengths_max[edges_left] <= edges_left*(n-1));
         upper_bound = sum_of_lengths + sum_of_lengths_max[edges_left];
      }
      else {
         lower_bound = sum_of_lengths + double_sum_of_lengths_of_remaining_edges_lower/double(2);
         assert(sum_of_lengths + edges_left <= lower_bound);
         upper_bound = sum_of_lengths + sum_of_lengths_max[edges_left];
/* ???        upper_bound = sum_of_lengths + double_sum_of_lengths_of_remaining_edges_upper/double(2);
         cout << upper_bound << " " << sum_of_lengths << " " << sum_of_lengths_max[edges_left] << endl;
         assert(upper_bound <= sum_of_lengths + sum_of_lengths_max[edges_left]); */
      }
      if (lower_bound <= target_sum_of_lengths and target_sum_of_lengths <= upper_bound) {
         for (int i = start; i < n; ++i) {
             if (predecessor[start] < 0 or predecessor[start] < start) {
                swap(p[start], p[i]);
                int new_edges;
                int new_lengths;
                if (start > 0) {
                   if (simple) {
                      new_edges = new_lengths = 0; 
                      for (set<int>::const_iterator j = g.adjacent[start].begin(); j != g.adjacent[start].end() and *j < start; ++j) {
                          ++new_edges;
                          int length = abs(p[start] - p[*j]);
                          assert(father[start] == *j);
                          assert(1 <= length); 
                          new_lengths += length;
                      }  
                      assert(new_edges <= new_lengths);
                      assert(new_edges <= 1);
                   }
                   else {
                      new_edges = 1;
                      new_lengths = abs(p[start] - p[father[start]]);
                      assert(1 <= new_lengths);
                      double_sum_of_lengths_of_remaining_edges_lower -= minimum_linear_arrangement_of_star_tree(remaining_degree[start]); 
                      double_sum_of_lengths_of_remaining_edges_lower -= minimum_linear_arrangement_of_star_tree(remaining_degree[father[start]]);
                      double_sum_of_lengths_of_remaining_edges_upper -= maximum_linear_arrangement_of_star_tree(n, remaining_degree[start]); 
                      double_sum_of_lengths_of_remaining_edges_upper -= maximum_linear_arrangement_of_star_tree(n, remaining_degree[father[start]]);
                      --remaining_degree[start]; 
                      --remaining_degree[father[start]];
                      assert(0 <= remaining_degree[start]);
                      assert(0 <= remaining_degree[father[start]]);
                      double_sum_of_lengths_of_remaining_edges_lower += minimum_linear_arrangement_of_star_tree(remaining_degree[start]); 
                      double_sum_of_lengths_of_remaining_edges_lower += minimum_linear_arrangement_of_star_tree(remaining_degree[father[start]]);
                      double_sum_of_lengths_of_remaining_edges_upper += maximum_linear_arrangement_of_star_tree(n, remaining_degree[start]); 
                      double_sum_of_lengths_of_remaining_edges_upper += maximum_linear_arrangement_of_star_tree(n, remaining_degree[father[start]]);
                      assert(0 <= double_sum_of_lengths_of_remaining_edges_lower);  
                   } 
                }
                else {
                   new_edges = new_lengths = 0;
                }
                i_crossings_brute_force(g, predecessor, equivalent_permutations, sum_of_lengths_max, father, target_sum_of_lengths, start + 1, p, edges + new_edges, sum_of_lengths + new_lengths, remaining_degree, double_sum_of_lengths_of_remaining_edges_lower, double_sum_of_lengths_of_remaining_edges_upper, crossings_statistics);
                swap(p[start], p[i]);   
                if (start > 0 and not simple) {
                   double_sum_of_lengths_of_remaining_edges_lower -= minimum_linear_arrangement_of_star_tree(remaining_degree[start]); 
                   double_sum_of_lengths_of_remaining_edges_lower -= minimum_linear_arrangement_of_star_tree(remaining_degree[father[start]]);
                   double_sum_of_lengths_of_remaining_edges_upper -= maximum_linear_arrangement_of_star_tree(n, remaining_degree[start]); 
                   double_sum_of_lengths_of_remaining_edges_upper -= maximum_linear_arrangement_of_star_tree(n, remaining_degree[father[start]]);
                   ++remaining_degree[start]; 
                   ++remaining_degree[father[start]];
                   assert(remaining_degree[start] <= int(g.adjacent[start].size()));
                   assert(remaining_degree[father[start]] <= int(g.adjacent[father[start]].size()));
                   double_sum_of_lengths_of_remaining_edges_lower += minimum_linear_arrangement_of_star_tree(remaining_degree[start]); 
                   double_sum_of_lengths_of_remaining_edges_lower += minimum_linear_arrangement_of_star_tree(remaining_degree[father[start]]);
                   double_sum_of_lengths_of_remaining_edges_upper += maximum_linear_arrangement_of_star_tree(n, remaining_degree[start]); 
                   double_sum_of_lengths_of_remaining_edges_upper += maximum_linear_arrangement_of_star_tree(n, remaining_degree[father[start]]);
                }
             } 
         }
      } 
   }
   // Post: 
   // 
}

inline int get_equivalent_permutations(const undirected_graph &g, vector<int> &predecessor) { 
   int equivalent_permutations = 1;
   fill(predecessor.begin(), predecessor.end(), -1);
   for (int i = 0; i < g.vertices(); ++i) {
       if (g.adjacent[i].size() >= 2) {
          int leaves = 0;
          int last_leaf = -1;
          for (set<int>::const_iterator j = g.adjacent[i].begin(); j != g.adjacent[i].end(); ++j) {
              bool leaf = g.adjacent[*j].size() == 1;
              if (leaf) { 
                 ++leaves;
                 if (last_leaf >= 0) { 
                    predecessor[*j] = last_leaf;
                    last_leaf = *j;
                 }
              }
          }  
          // cout << "leaves: " << leaves << endl;
          equivalent_permutations *= factorial(leaves);
       }
   }
   assert(1 <= equivalent_permutations);
   return equivalent_permutations;
}

inline double maximum_number_of_crossings(const undirected_graph &g) {
   int n = g.vertices();
   double degree_2nd_moment = g.degree_2nd_moment();
   double crossings_max = n*(n - 1 - degree_2nd_moment)/double(2);
   return crossings_max;
}

double factorial(int n) {
   if (n < 2) return 1;
   else return n*factorial(n - 1);
}

inline int minimum_linear_arrangement_of_star_tree(int k) {
   assert(0 <= k);
   // Pre: k >= 0
   // Following Eqs. B26 and B27 of 
   // Ferrer-i-Cancho, R. (2013). Hubiness, length, crossings and their relationships in dependency trees. Glottometrics 25, 1-21.
   if (k % 2 == 0) {
      int aux = k/2;
      return aux*(aux + 1);
   }
   else { 
      int aux = (k+1)/2;
      return aux*aux;
   }  
   // Post: it returns the sum of edge lengths of a minimum linear arrangement of a star tree of k + 1 vertices
}

inline int maximum_linear_arrangement_of_star_tree(int n, int k) {
   // Pre: 0 <= k < n
   assert(0 <= k);
   assert(k <= n);
   int upper = summation(n - 1) - summation(n - 1 - k);
   assert(upper <= (n - 1)*k);
   return upper; 
   // Post: it returns the sum of edge lengths of a maximum linear arrangement of a star tree of k + 1 vertices on a sequence of n positions
}

inline int summation(int m) {
   // Pre: 0 <= m;
   assert(0 <= m);
   return m*(m + 1)/2;
   // Post: it returns the sum of 1, 2, ... m.
}

