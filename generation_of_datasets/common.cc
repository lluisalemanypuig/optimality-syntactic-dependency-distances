#include "common.hh"

// Implementation of Shiloach's algorithm for Minimal Linear Arrangement 
// for undirected trees.

#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <cmath>
using namespace std; 



void read_adjacency_matrix(adjacency_matrix & m)
// Neither row 0 nor column 0 is used.
{
	if (m.size()>0) throw MyException("Adjacency matrix should be empty");	
	int n=m.size()-1;
	for(int i=1;i<=n;++i){
		for(int j=1;j<=n;++j){
			cin >> m[i][j];
		}
	}
}

void write_matrix(const vector<vector<int> > & m)
// Neither row 0 nor column 0 is used.
{	
	cout << "ADJACENCY MATRIX :" << endl;
	int n=m.size()-1;
	for(int i=1;i<=n;++i){
		cout << m[i][1];
		for(int j=2;j<=n;++j){
			cout << " " << m[i][j];
		}
		cout << endl;
	}
}

void write_reachable_nodes(const vector<int> & v)
{
	cout << "REACHABLE NODES :";
	int n=v.size();
	for(int j=0;j<n;++j){
		cout << " " << v[j];
	}
	cout << endl;
}

void write_arr(const vector<int> & v)
{
	cout << "ARRANGEMENT :";
	int n=v.size();
	for(int j=1;j<n;++j){
		cout << " " << v[j];
	}
	cout << endl;
}

void write_edge_vector(const vector<pair<int, int> > & v)
{	
	cout << "EDGE VECTOR : ";
	int n=v.size()-1;
	for(int j=0;j<=n;++j){
		cout << "(" << v[j].first << "," << v[j].second << ") ";
	}
	cout << endl;
}

void write_ordering(const vector<pair<int, int> > & v)
{	
	cout << "ORDERING : ";
	int n=v.size()-1;
	for(int j=0;j<=n;++j){
		cout << "Size : " << v[j].first << ", root : " << v[j].second << " ";
	}
	cout << endl;
}

void size_of_all_subtrees_from_one_node(adjacency_matrix & g,vector<int> &v, int id_root)
// v[i] is the size of the subtree rooted in i inside the tree 
// rooted in id_root, if there is a path from id_root to i.
// There can be several nodes unconnected to id_root
// after some recursive calls.
{
	int n=v.size()-1;
	v[id_root]=1;
	for (int i=1;i<=n;++i){
		if (g[id_root][i]==1){
			g[i][id_root]=-1;
			size_of_all_subtrees_from_one_node(g,v,i);
			v[id_root]+=v[i];
			g[i][id_root]=1; 
		}
	}
}

void reachable_nodes(adjacency_matrix & g, vector<int> &v, int root)
// We get all nodes reachable from node root
{
	v.push_back(root);
	int n=g.size()-1;
	for (int i=1;i<=n;++i){
		if (g[root][i]==1){
			g[i][root]=-1;
			reachable_nodes(g,v,i);
			g[i][root]=1; 
		}
	}
}

void size_of_all_subtrees_from_all_nodes(adjacency_matrix & g, size_matrix & s, const vector<int> & v)
// elements de v are all the nodes connected among them in g
{
	int n=g.size()-1;
	s = size_matrix(n+1,vector<int> (n+1,0));
	for (int i=0;i<v.size();++i) {
		size_of_all_subtrees_from_one_node(g,s[v[i]],v[i]);
	}
}

bool order_pair(const pair<int,int> &p1, const pair<int,int> & p2)
{
	if (p1.first==p2.first) return p1.second<p2.second;
	else return p1.first > p2.first;
}

void order_subtrees_by_size(const adjacency_matrix & g, vector<int> & s, int root, ordering & ord)
{
	int n=g.size()-1;
	if (ord.size()!=0) throw MyException("Ordering should be empty");
	pair<int,int> elem;
	for (int i=1;i<=n;++i){
		if (g[root][i]==1) {
			elem.first=s[i];
			elem.second=i;
			ord.push_back(elem);
		}
	}
	sort(ord.begin(),ord.end(),order_pair);
}

//void join_arrangements(vector<int> &v1, const vector<int> & v2)
//{	
	//int n=v1.size()-1;
	//for(int i=1;i<=n;++i){
		//if (v2[i]!=0) {
			//if (v1[i]==0) v1[i]=1;
			//else throw MyException("Same node in disjoint arr.");
		//}
	//}
//}

int evaluate_arrangement(const vector<int> & arr,const edge_vector & mt)
{
	int res=0;
	for (int i=0;i<mt.size();++i){
		int x=arr[mt[i].first]-arr[mt[i].second];
		if (x<0) x=-x;
		res+=x;
	}
	return res;
}

void graph_to_tree(adjacency_matrix & g, edge_vector & mt, int root)
{
	int n=g.size()-1;
	for (int i=1;i<=n;++i){
		if (g[root][i]==1){
			pair <int,int> node;
			node.first=root;
			node.second=i;
			mt.push_back(node);
			g[i][root]=-1;
			graph_to_tree(g,mt,i);
			g[i][root]=1; 
		}
	}
	sort(mt.begin(),mt.end(),order_pair);
}

void edge_vector_to_list(const edge_vector & mt, list<pair<int,int> > & t){
	t.clear();
	list<pair<int,int> >::iterator it=t.begin();
	for(int i=0;i<mt.size();++i) t.insert(it,mt[i]);
}


void convert_tree_to_matrix(int n,const list<pair<int,int> > & t, adjacency_matrix & g)
{
	g = adjacency_matrix (n+1,vector<int> (n+1,0));
	list<pair<int,int> >::const_iterator it;
	for (it=t.begin();it!=t.end();++it){
		int x,y;
		x=(*it).first;
		y=(*it).second;
		if (g[x][y]!=g[y][x]) throw MyException("ERROR: 'DIRECTED NODE'");
		else if (g[x][y]!=0) throw MyException("ERROR: REPEATED NODE");
		else {
			g[x][y]=g[y][x]=1;
		}
	}
	
}


void read_matrix(adjacency_matrix & g)
{
	int n;
	cin >> n;
	g = adjacency_matrix (n+1,vector<int> (n+1,0));
	int x,y;
	cin >> x;
	while (x!=0){
		cin >> y;
		if (g[x][y]!=g[y][x]) throw MyException("ERROR: 'DIRECTED NODE'");
		else if (g[x][y]!=0) throw MyException("ERROR: REPEATED NODE");
		else g[x][y]=g[y][x]=1;
		cin >> x;	
	}
}
