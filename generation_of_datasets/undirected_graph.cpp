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
#include <cmath>
#include <iostream>
#include <fstream>
#include "undirected_graph.hpp"
// #include "shiloach.hh"
#include "common.hpp"

void connected_components(const undirected_graph &g, int c, int start, int &vertices, vector<int> &color, vector<int> &new_vertex);
undirected_graph read_edges_from_ifstream(int vertices, int edges, ifstream &in);
void write_edges_on_ofstream(const undirected_graph &g, ofstream &out);

int sum_of_edge_lengths_lower_bound(int degree) {
   assert(0 <= degree);
   // formula borrowed from the article for Glottometrics
   int n=degree/2;
   double sum = n*(n+1);
   if (degree % 2 == 1) sum += (degree+1)/2;
   return sum;
   // Post: it returns the sum of edge lengths of a minimum linear arrangement of a star tree with degree+1 nodes
}

undirected_graph scrambled_graph(const undirected_graph &g, const permutation &p) {
   undirected_graph g_scrambled(g.vertices());
   for (int v = 0; v < int(g.vertices()); ++v) {
       for (set<int>::const_iterator j = g.adjacent[v].begin();  j != g.adjacent[v].end(); ++j) {
           if (v < *j) g_scrambled.add_edge(p[v],p[*j]);
       }
   }
   assert(g.edges() == g_scrambled.edges());
   return g_scrambled;
}

vector<undirected_graph> get_connected_components(const undirected_graph &g) {
   vector<int> color(g.vertices(),-1);
   vector<int> new_vertex(g.vertices(),-1);
   vector<undirected_graph> cc(0);
   int total_vertices = 0;
   int c = 0;
   for (int v = 0; v < g.vertices(); ++v) {
      if (color[v] < 0) {
         int vertices = 0;
         connected_components(g,c,v,vertices, color, new_vertex);
         assert(vertices >= 1);
         undirected_graph g(vertices);
         cc.push_back(g);
         total_vertices += vertices;
         ++c;
      }
   }
   for (int v = 0; v < g.vertices(); ++v) {
      for (set<int>::const_iterator i = g.adjacent[v].begin(); i != g.adjacent[v].end(); ++i) {
          assert(color[v] == color[*i]);
          if (v < *i) cc[color[v]].add_edge(new_vertex[v],new_vertex[*i]);
      }
   }
   assert(total_vertices == g.vertices());
   return cc;
}

void connected_components(const undirected_graph &g, int c, int start, int &vertices, vector<int> &color, vector<int> &new_vertex) {
   new_vertex[start] = vertices;
   ++vertices;
   color[start] = c;
   for (set<int>::const_iterator i = g.adjacent[start].begin(); i != g.adjacent[start].end(); ++i) {
     if (color[*i] < 0) {
        connected_components(g,c,*i, vertices, color, new_vertex);
     } 
   }
}

undirected_graph read_graph(const char *file) {
   ifstream in(file);
   assert(in);
   return read_graph_from_ifstream(in);
} 

undirected_graph read_tree(const char *file) {
   ifstream in(file);
   assert(in);
   return read_tree_from_ifstream(in);
} 

undirected_graph read_graph_from_ifstream(ifstream &in) {
   int vertices, edges;
   in >> vertices >> edges;
   return read_edges_from_ifstream(vertices,edges,in);
}

undirected_graph read_tree_from_ifstream(ifstream &in) {
   int vertices;
   in >> vertices;
   return read_edges_from_ifstream(vertices,vertices-1,in);
}

undirected_graph read_edges_from_ifstream(int vertices, int edges, ifstream &in) {
   undirected_graph g(vertices);
   int u, v;
   for (int i = 0; i < edges; ++i) {
      assert(in >> u >> v);
      // cout << vertices << " " << u << " " << v << endl;
      assert(0 <= u);
      assert(u < vertices);
      assert(0 <= v);
      assert(v < vertices);
      g.add_edge(u,v);     
   }
/*   if (edges != g.edges()) {
      cout << "           **** " << vertices << " " << edges << " " << g.edges() << endl;
   } */
   if (edges >= 0) assert(edges == g.edges());
   // ??? assert(g.tree());
   return g;
}

void write_graph(const undirected_graph &g, const char *file) {
   ofstream out(file);
   assert(out);
   write_graph_on_ofstream(g, out);
}

void write_tree(const undirected_graph &g, const char *file) {
   ofstream out(file);
   assert(out);
   write_tree_on_ofstream(g, out);
}

void write_graph_on_ofstream(const undirected_graph &g, ofstream &out) {
   assert(out);
   out << g.vertices() << " " << g.edges() << endl;
   write_edges_on_ofstream(g, out);
}

void write_tree_on_ofstream(const undirected_graph &g, ofstream &out) {
   assert(g.tree()); 
   assert(out);
   out << g.vertices() << endl;
   write_edges_on_ofstream(g, out);
}

void write_edges_on_ofstream(const undirected_graph &g, ofstream &out) {
   for (int u = 0; u < g.vertices(); ++u) {
       for (set<int>::const_iterator i = g.adjacent[u].begin(); i != g.adjacent[u].end(); ++i) {
           if (u < *i) out << u << " " << *i << endl;
       }
   }
}

undirected_graph::undirected_graph() {
   V=E=0;
}

undirected_graph::undirected_graph(int vertices) {
   assert(0 <= vertices);
   V = vertices;
   E = 0;
   adjacent = vector<set<int> >(V);
}

void undirected_graph::add_edge(int u, int v) {
   assert(not edge(u, v));
   adjacent[u].insert(v);
   adjacent[v].insert(u);
   ++E;
}

void undirected_graph::remove_edges_of_vertices(const vector<int> &v, int n) {
   int sum = 0;
   for (int i = 0; i < n; ++i) {
       sum += degree(v[i]);
       adjacent[v[i]].clear();
   }
   assert(sum % 2 == 0);
   E -= sum/2;
   assert(E >= 0);
}

void undirected_graph::empty_edges() {
   for (int v = 0; v < int(adjacent.size()); ++v) {
       adjacent[v].clear(); 
   }
   E = 0;
}

bool undirected_graph::edge(int u, int v) const {
   assert(0 <= u);
   assert(u < vertices());
   assert(0 <= v);
   assert(v < vertices());
   return adjacent[u].find(v) != adjacent[u].end();
}

int undirected_graph::vertices() const {
   return V;
}

int undirected_graph::edges() const {
   return E;
}

int undirected_graph::degree(int v) const {
   assert(0 <= v);
   assert(0 < adjacent.size());
   return adjacent[v].size();
}

void undirected_graph::degree_statistics(double &mean_degree, double &mean_degree2, double &variance_degree) const {
   mean_degree = mean_degree2 = 0;
   for (int i = 0; i < int(adjacent.size()); ++i) {
       int degree = adjacent[i].size(); 
       mean_degree += degree;
       mean_degree2 += degree*degree;
   }
   mean_degree /= vertices();
   mean_degree2 /= vertices();
   variance_degree = mean_degree2 - mean_degree*mean_degree;
}

int undirected_graph::sum_of_squared_degrees() const
{
	int sum = 0;
	for (int i = 0; i < int(adjacent.size()); ++i) {
       int degree = adjacent[i].size(); 
       sum += degree*degree;
   }
   return sum;
}

double undirected_graph::degree_2nd_moment() const {
   double mean_degree, mean_degree2, variance_degree;
   degree_statistics(mean_degree, mean_degree2, variance_degree);
   return mean_degree2;
}

bool undirected_graph::connected() const {
   assert(vertices() > 0);
   vector<bool> visited(V, false);
   int visits = 0;
   depth_first_search(0, visited, visits);
   return visits == vertices();
}

void undirected_graph::depth_first_search(int v, vector<bool> &visited, int &visits) const {
   // Prec: not visited[v]
   assert(not visited[v]);
   visited[v]=true;
   ++visits;
   for (set<int>::const_iterator i = adjacent[v].begin(); i!=adjacent[v].end(); ++i) {
       if (not visited[*i]) depth_first_search(*i, visited, visits);
   }
}

bool undirected_graph::tree() const {
   return vertices() == 0 or (edges() == vertices() - 1 and connected());
}

int undirected_graph::edge_length_sum() const {
   int sum = 0;
   for (int i = 0; i<int(adjacent.size()); ++i) {
      for (set<int>::const_iterator j = adjacent[i].begin(); j != adjacent[i].end() and *j < i; ++j) {
          int d = i - *j;
          assert(d > 0);
          sum += d;
      }
   } 
   assert(sum >= edges()); // mala fita si hi ha llaços perque que donen llargada 0
   return sum;
}

int undirected_graph::minimum_edge_length_sum_of_a_tree() const {
   assert(tree());
   list<pair<uint32_t, uint32_t> > l;
   for (int i = 0; i<int(adjacent.size()); ++i) {
      for (set<int>::const_iterator j = adjacent[i].begin(); j != adjacent[i].end() and *j < i; ++j) {
          l.push_back(pair<uint32_t, uint32_t>(i+1, *j+1));
      }
   } 
   assert(l.size() == vertices() - 1);
   return calculate_D_min_Shiloach(vertices(), l);
//   return calculate_D_min_chung_aux(vertices(), l);
}

void undirected_graph::edge_length_statistics(double &mean_length, double &mean_length2, double &variance_length) const {
   mean_length = mean_length2 = 0;
   for (int i = 0; i<int(adjacent.size()); ++i) {
      for (set<int>::const_iterator j = adjacent[i].begin(); j != adjacent[i].end(); ++j) {
          if (i <= *j) {
             int length = abs(i - *j);   
             mean_length += length;
             mean_length2 += length*length;
          }
      }
   }
   mean_length /= edges();
   mean_length2 /= edges();
   variance_length = mean_length2 - mean_length*mean_length;
}

vector<int> undirected_graph::edge_length_spectrum() const {
   vector<int> spectrum(vertices(),0);
   for (int i = 0; i<int(adjacent.size()); ++i) {
      for (set<int>::const_iterator j = adjacent[i].begin(); j != adjacent[i].end(); ++j) {
          if (i <= *j) {
             int length = abs(i - *j);
             ++spectrum[length];
          }
      }
   }
   assert(spectrum[1] <= vertices() - 1);
   assert(spectrum[vertices() - 1] <= 1);
   return spectrum;
}

/*
int undirected_graph::crossings() const {
   int crossings = 0;  
   if (vertices() >= 4) {
      for (int i = 0; i<int(adjacent.size()); ++i) {
         for (set<int>::const_iterator j = adjacent[i].begin(); j != adjacent[i].end(); ++j) {
             int inc = *j - i; 
             if (inc >= 0 and inc != 1 and i != vertices() - 1) {
                for (int k = i + 1; k < *j; ++k) {
                    for (set<int>::const_iterator l = adjacent[k].begin(); l != adjacent[k].end(); ++l) {
                        if (k <= *l) {
                           bool cross = *l < i or *l > *j;
                           crossings += cross;
                        }
                    }
                }
             }
         }
      }
      assert(crossings <= edges()*(edges()-1)/2);
   } 
   return crossings;
}
*/

int undirected_graph::crossings() const {
   return crossings_lazy(edges()*edges());
}

int undirected_graph::crossings_lazy(int crossings_min) const {
   int crossings = 0;  
   if (vertices() >= 4) {
      for (int i = 0; i<int(adjacent.size()); ++i) {
         for (set<int>::const_reverse_iterator j = adjacent[i].rbegin(); j != adjacent[i].rend() and *j > i; ++j) {
             int inc = *j - i;
             assert(inc >= 0); 
             if (inc > 1 and i != vertices() - 1) {
                for (int k = i + 1; k < *j; ++k) {
//                    for (set<int>::const_iterator l = adjacent[k].begin(); l != adjacent[k].end(); ++l) {
                    for (set<int>::const_reverse_iterator l = adjacent[k].rbegin(); l != adjacent[k].rend() and *l > k; ++l) {
                        assert(k <= *l);
//                        if (k <= *l) {
                           bool cross = *l < i or *l > *j;
                           crossings += cross;
                           if (crossings > crossings_min) return crossings;
//                        }
                    }
                }
             }
         }
      }
      assert(crossings <= edges()*(edges()-1)/2);
   } 
   // Post: if crossings <= crossings_min then crossings is the number of edge of crossings; 
   //       if crossings > crossings_min then crossings > crossings_min
   return crossings;
}

int undirected_graph::crossings_with_edge(int i, int j) const {
   return crossings_with_edge_lazy(i, j, edges() + 1);
}

int undirected_graph::crossings_with_edge_lazy(int i, int j, int crossings_max) const {
// Pre: i < j
    assert(i < j);
    assert(crossings_max >= 0);
    int crossings = 0;
    if (j - i < vertices() - 1) {
       for (int k = i + 1; k < j; ++k) {
           for (set<int>::const_reverse_iterator l = adjacent[k].rbegin(); l != adjacent[k].rend(); ++l) { 
              bool cross = *l < i or *l > j;
              crossings += cross;
              if (crossings > crossings_max) return crossings; 
           }
       }
    }
    return crossings;
// Post: the number of edges that cross with the edge formed by vertices i and j is returned
}

/* 
double undirected_graph::predicted_crossings() const {
   double crossings = 0;
   if (vertices() >= 4) { 
      for (int i = 0; i<int(adjacent.size()); ++i) {
         for (set<int>::const_iterator j = adjacent[i].begin(); j != adjacent[i].end(); ++j) {
             int d = abs(i - *j);
             assert(1 <= d);
             assert(d <= vertices());
             double probability_of_crossing = 2*(d - 1)*(vertices() - d - 1)/double((vertices()-2)*(vertices()-3));
             assert(0 <= probability_of_crossing);
             assert(probability_of_crossing <= 1); 
             int potential_crossings = vertices() - adjacent[i].size() - adjacent[*j].size();
             assert(0 <= potential_crossings);
             assert(potential_crossings <= vertices() - 2);
             crossings += potential_crossings*probability_of_crossing;
         }
      }
      crossings/=4;
//      assert(crossings <= edges()*(edges()-1)/2);
   } 
   return crossings;
} 
*/

double undirected_graph::predicted_crossings() const {
   double crossings = 0;
   if (vertices() >= 4) {
      for (int i = 0; i<int(adjacent.size()); ++i) {
         for (set<int>::const_reverse_iterator j = adjacent[i].rbegin(); j != adjacent[i].rend() and *j > i; ++j) {
             int d = *j - i; 
             assert(d > 0);
             assert(d <= vertices() - 1);
             double probability_of_crossing = 2*(d - 1)*(vertices() - d - 1)/double((vertices()-2)*(vertices()-3));
             assert(0 <= probability_of_crossing);
             assert(probability_of_crossing <= 1); 
             int potential_crossings = vertices() - adjacent[i].size() - adjacent[*j].size();
             assert(0 <= potential_crossings);
             assert(potential_crossings <= vertices() - 2);
             crossings += potential_crossings*probability_of_crossing;
         }
      }
      crossings/=2;
//      assert(crossings <= edges()*(edges()-1)/2);
   } 
   assert(crossings >= 0);
   return crossings;
}

/*
double undirected_graph::fully_predicted_crossings(const probability_of_crossing_knowing_edge_lengths &c) const {
   assert(c.vertices() == vertices());
   int d_max = int(adjacent.size()) - 1;
   double crossings = 0;
   if (vertices() >= 4) { 
      for (int i = 0; i<int(adjacent.size()); ++i) {
         for (set<int>::const_iterator j = adjacent[i].begin(); j != adjacent[i].end(); ++j) {
             int d1 = *j - i;
             if (d1 > 1 and d1 < d_max) {  
                for (int k = 0; k < int(adjacent.size()); ++k) {
                    if (k != i and k != *j) {
//                    if (k < i or (k == i and k < *j)) {
                       for (set<int>::const_iterator l = adjacent[k].begin(); l != adjacent[k].end(); ++l) {
                           int d2 = *l - k;
                           if (d2 > 1 and *l != i and *l != *j) {
                              assert(d2 <= d_max);
                              crossings += c.probability_of_crossing(d1, d2);
                           }
                       }
                    }
                }
             }
         }
      }
      crossings/=2;
   } 
   return crossings;
}
*/

double undirected_graph::fully_predicted_crossings(const probability_of_crossing_knowing_edge_lengths &c) const {
   int potential_crossings;
   return fully_predicted_crossings(c, potential_crossings);
}

double undirected_graph::fully_predicted_crossings(const probability_of_crossing_knowing_edge_lengths &c, int &potential_crossings) const {
   int n = vertices();
   assert(c.vertices() == n);
   assert(adjacent.size() == n);
   int d_max = n - 1;
   double crossings = 0;
   potential_crossings = 0;
   if (vertices() >= 4) { 
      for (int i = 0; i < n; ++i) {
         for (set<int>::const_reverse_iterator j = adjacent[i].rbegin(); j != adjacent[i].rend() and *j > i; ++j) {
             int d1 = *j - i;
             assert(d1 > 0); 
             if (d1 > 1 and d1 < d_max) {  
//                for (int k = i + 1; k < *j; ++k) { // amb aquest codi no es calcula be E_2[C]
                  for (int k = i + 1; k < n; ++k) {
                    if (k != i and k != *j) {
                       assert(k != i and k != *j);
                       for (set<int>::const_reverse_iterator l = adjacent[k].rbegin(); l != adjacent[k].rend() and *l > k; ++l) {
                           int d2 = *l - k;
                           assert(d2 > 0);
                           if (d2 > 1 and d2 < d_max and *l != i and *l != *j) {
                              assert(d2 < d_max);
                              ++potential_crossings;
//                              cout << "   " << i << "~" << *j << " might cross " << k << "~" << *l << endl; 
                              crossings += c.probability_of_crossing(d1, d2);
                           }
                       } 
                    }
                }
             }
         }
      }
   } 
   assert(crossings >= 0);
   return crossings;
}

double undirected_graph::mean_edge_length() const {
   double mean = edge_length_sum()/double(edges());
   assert(mean >= 1);
   return mean;
}

int undirected_graph::edge_length_sum_lower_bound() const {
   int sum = 0;
   int degree_sum = 0;
   for (int v = 0; v < vertices(); ++v) {
       int degree = adjacent[v].size();
       degree_sum += degree;
       sum += sum_of_edge_lengths_lower_bound(degree);
   }   
   assert(degree_sum == 2*edges());
   assert(sum >= 2*edges());
   return sum;
}

double undirected_graph::mean_edge_length_lower_bound() const {
   double mean = edge_length_sum_lower_bound()/double(2*edges());
   assert(mean >= 1);
   return mean;
}

