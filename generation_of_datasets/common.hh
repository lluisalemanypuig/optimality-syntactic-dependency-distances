#ifndef COMMON_HH
#define COMMON_HH

#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <cmath>

using namespace std;

typedef vector<vector<int> > adjacency_matrix; 
// Neither row 0 nor column 0 is used.

typedef vector<vector<int> > size_matrix; 
// Neither row 0 nor column 0 is used
// It's the same as adjacency_matrix but is
// used to keep the sizes of all subtrees
// using all nodes as roots.
// elem[i][j] is the size of the subtree
// rooted in j in the tree rooted in i
// In one matrix there can be several
// unconnected trees.

typedef vector<pair<int,int> > ordering;
// Elements are size and root of subtrees
// ordered by size

typedef vector<pair<int,int> > edge_vector;
// Vector with all edges


void read_adjacency_matrix(adjacency_matrix & m);

void write_matrix(const vector<vector<int> > & m);

void write_reachable_nodes(const vector<int> & v);

void write_arr(const vector<int> & v);

void write_edge_vector(const vector<pair<int, int> > & v);

void write_ordering(const vector<pair<int, int> > & v);

void size_of_all_subtrees_from_one_node(adjacency_matrix & g,vector<int> &v, int id_root);

void reachable_nodes(adjacency_matrix & g, vector<int> &v, int root);

void size_of_all_subtrees_from_all_nodes(adjacency_matrix & g, size_matrix & s, const vector<int> & v);

bool order_pair(const pair<int,int> &p1, const pair<int,int> & p2);

void order_subtrees_by_size(const adjacency_matrix & g, vector<int> & s, int root, ordering & ord);

void join_arrangements(vector<int> &v1, const vector<int> & v2);

int evaluate_arrangement(const vector<int> & arr,const edge_vector & mt);

void graph_to_tree(adjacency_matrix & g, edge_vector & mt, int root);

void convert_tree_to_matrix(int n,const list<pair<int,int> > & t, adjacency_matrix & g);

void read_matrix(adjacency_matrix & g);

void edge_vector_to_list(const edge_vector & mt, list<pair<int,int> > & t);

/* Class MyException */
class MyException: public exception {
  public:
   MyException (const char* mot) : exception(), mensaje(mot) {}
   const char* what() const throw() {return mensaje;}; 
  private:
   const char* mensaje;
};
/* End Class MyException */




int calculate_D_min_chung_aux(int n, list<pair<int,int> >&tree);
int calculate_D_min_Shiloach_aux(int n, list<pair<int,int> >&tree);



#endif
