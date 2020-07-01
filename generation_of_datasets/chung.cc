#include <cassert>
#include "common.hh"

int get_center(adjacency_matrix & g, const size_matrix & s, const vector<int> & list_of_nodes)
// For free (unanchored) trees
// Elements de v are all the nodes connected among them in g
{
	int n=g.size()-1;
	int number_of_nodes=list_of_nodes.size();
	int h=number_of_nodes/2;
	int v_star=0; 
	int i=0;
	bool v_star_found=false;
	while(i<number_of_nodes and not v_star_found){
		int j=1;
		bool tree_too_big=false;
		while (not tree_too_big and j<=n){
			int ith_root=list_of_nodes[i];
			if (g[ith_root][j]==1){
				if (s[ith_root][j]>h) tree_too_big=true;
				else j++;
			}
			else j++;
		}
		if (not tree_too_big) {
			v_star=list_of_nodes[i];
			v_star_found=true;
		}
		if (not v_star_found) ++i;
	}
	return v_star;
}


int calculate_q(const ordering & ord)
{
	//cout << "CALCULATING Q" << endl;
	int n=1; // Counting root
	for (int i=0;i<ord.size();++i) n+=ord[i].first; 
	// n is number of nodes
	int k=ord.size(); // number of subtrees
	--k; // T_0, T_1, ..., T_k
	int t_0= ord[0].first;
	int q= k/2;  // Maximum possible p_alpha
	int sum=0;
	for (int i=0;i<=2*q;++i) sum+=ord[i].first;
	int z=n-sum;
	int tricky_formula=floor(t_0/2+1)+floor(z/2+1); 
	// t_0 >= t_1 >= ... >= t_k
	int t_2q=ord[2*q].first;
	// ************ t_2q < tricky_formula **********
	while(q>=0 and t_2q <= tricky_formula){
		--q;
		sum=0;
		for (int i=0;i<=2*q;++i) sum+=ord[i].first;
		z=n-sum;
		tricky_formula=floor(t_0/2+1)+floor(z/2+1); 
		if (q>=0) t_2q=ord[2*q].first; 
	}	
	//cout << "Q DONE : " << q << endl;
	return q;
}	
	
int calculate_p(const ordering & ord)
{	
	//cout << "CALCULATING P" << endl;
	if (ord.size()<2) {
		int p=-1;
		//cout << "ORDERING TOO SHORT" << endl;
		//cout << "P DONE : " << p << endl;
		return p;
	}
	int n=1; // Counting root
	for (int i=0;i<ord.size();++i) n+=ord[i].first; 
	// n is number of nodes
	int k=ord.size(); // number of subtrees
	--k; // T_0, T_1, ..., T_k
	int t_0= ord[0].first;
	int p=(k-1)/2;	
	int sum=0;
	for (int i=0;i<=2*p+1;++i) sum+=ord[i].first;
	int y=n-sum;
	int tricky_formula=floor(t_0/2+1)+floor(y/2+1); 
	int t_2p_plus_1=ord[2*p+1].first; 
	// ************ t_2q < tricky_formula **********
	while(p>=0 and t_2p_plus_1 <=tricky_formula){  
		--p;
		sum=0;
		for (int i=0;i<=2*p+1;++i) sum+=ord[i].first;
		y=n-sum;
		tricky_formula=floor(t_0/2+1)+floor(y/2+1); 
		if (p>=0) t_2p_plus_1=ord[2*p+1].first; 
	}
	//cout << "P DONE : " << p << endl;
	return p;
	
}

void get_P(vector<int> & v, int p, int i)
{
	v=vector<int> (2*p+1+1,-1);
	int j=0;
	int pos=v.size()-1;
	int pos_der=pos;
	int pos_izq=1;
	while (j<=2*p+1){
		if (j==i) ++j;
		else {
			v[pos]=j;
			if (pos>pos_izq) {--pos_der;pos=pos_izq;}
			else {++pos_izq; pos=pos_der;} 
			++j;
		}
	}
}

void get_Q(vector<int> & v, int q, int i)
{
	v=vector<int> (2*q+1,-1);
	int j=0;
	int pos=v.size()-1;
	int pos_der=pos;
	int pos_izq=1;
	while (j<=2*q){
		if (j==i) ++j;
		else {
			v[pos]=j;
			if (pos>pos_izq) {--pos_der;pos=pos_izq;}
			else {++pos_izq; pos=pos_der;} 
			++j;
		}
	}
}
void calculate_mla_chung(adjacency_matrix & g, int root, int one_node, vector<int> & arr, int & cost)
// root 0 means is a free tree
// root different from 0 means is rooted
// one_node is used when root is 0, to find all nodes
// connected with it.
{
	int n=g.size()-1;
	vector<int> node_vector;
	reachable_nodes(g,node_vector,one_node);
	//write_reachable_nodes(node_vector);
	int size_tree=node_vector.size(); // Size of the tree
	
	if (size_tree==0) throw MyException("Empty tree");
	
	// Base case
	if (size_tree==1) {
		//cout << "ONE NODE" << endl;
		cost=0;
		arr=vector<int> (n+1,0);
		arr[node_vector[0]]=1;
		return;
	}
	
	size_matrix s;
	size_of_all_subtrees_from_all_nodes(g,s,node_vector);
	
	if (root==0){
		int u=get_center(g,s,node_vector);
		ordering ord;
		order_subtrees_by_size(g,s[u],u,ord);
		int q=calculate_q(ord);
		if (q==-1){
			//cout << "ENTERING Q SIMPLE : " << q << endl;
			int t_0=ord[0].second;
			g[u][t_0]=g[t_0][u]=0;
			int c1,c2;
			vector<int> arr1 (n+1,0);
			vector<int> arr2 (n+1,0);
			calculate_mla_chung(g,t_0,t_0,arr1,c1);
			calculate_mla_chung(g,u,u,arr2,c2);
			cost=c1+c2+1;
			arr=vector<int> (n+1,0);	
			join_arrangements(arr,arr1);
			join_arrangements(arr,arr2);
			g[u][t_0]=g[t_0][u]=1;
		}
		else{
			// cout << "ENTERING Q COMPLEX : " << q << endl;
			//write_reachable_nodes(node_vector);
			//cout << "CENTER : " << u << endl;
			//write_ordering(ord);
			cost=n*n; // More than the maximum
			edge_vector mt;
			//graph_to_tree(g,mt,u);
			//write_edge_vector(mt);
			for(int i=0;i<=2*q;++i){
				//cout << "ZERO i : " << i << endl;
				//cout << "u : " << u << "el otro : " << ord[i].second << endl;
				g[u][ord[i].second]=g[ord[i].second][u]=0;
			} // Now g is Y
			//graph_to_tree(g,mt,u);
			//write_edge_vector(mt);
			for(int i=0;i<=2*q;++i){
				vector<int> Q_i;
				get_Q(Q_i,q,i);
				//cout << "i : " << i << endl;
				//write_arr(Q_i);
				g[u][ord[i].second]=g[ord[i].second][u]=1;
				vector<int> arr_i (n+1,0);
				int c_i=0;
				for (int j=1;j<=2*q;++j){
					int c_i_j;
					vector<int> arr_i_j;
					int node=Q_i[j];
					//graph_to_tree(g,mt,ord[node].second);
					//write_edge_vector(mt);
					//vector <int> v_aux;
					//reachable_nodes(g,v_aux,ord[node].second);
					//write_reachable_nodes(v_aux);
					calculate_mla_chung(g,ord[node].second,ord[node].second,arr_i_j,c_i_j);
					join_arrangements(arr_i,arr_i_j);
					c_i+=c_i_j;
				}
				int c_i_j;
				vector<int> arr_i_j;
				//graph_to_tree(g,mt,u);
				//write_edge_vector(mt);
				//vector <int> v_aux;
				//reachable_nodes(g,v_aux,u);
				calculate_mla_chung(g,0,u,arr_i_j,c_i_j);
				//calculate_mla(g,0,root,arr_i_j,c_i_j);
				join_arrangements(arr_i,arr_i_j);
				c_i+=c_i_j;
				// Rest of property 12
				c_i+=size_tree*q;
				int subs=0;
				for (int j=1;j<=q;++j) {
					subs+=(q-j+1)*(ord[Q_i[j]].first+ ord[Q_i[2*q-j+1]].first);
				}
				c_i-=subs;
				if (c_i<cost) {
					cost=c_i;
					arr=arr_i;
				}
				g[u][ord[i].second]=g[ord[i].second][u]=0;		
			}
			for(int i=0;i<=2*q;++i){
				g[u][ord[i].second]=g[ord[i].second][u]=1;
			} // Now g is again g
		}
	}
	// Rooted tree
	else{
		ordering ord;
		order_subtrees_by_size(g,s[root],root,ord);
		int p=calculate_p(ord);
		if (p==-1){
			//cout << "ENTERING P SIMPLE : " << p << endl;
			int t_0=ord[0].second;
			g[root][t_0]=g[t_0][root]=0;
			if (root==t_0) throw MyException("OMG!!!!");
			int c1,c2;
			vector<int> arr1 (n+1,0);
			vector<int> arr2 (n+1,0);
			calculate_mla_chung(g,t_0,t_0,arr1,c1);
			//calculate_mla(g,0,root,arr2,c2);
			calculate_mla_chung(g,0,one_node,arr2,c2);
			cost=c1+c2+size_tree-ord[0].first;
			arr=vector<int> (n+1,0);	
			join_arrangements(arr,arr1);
			join_arrangements(arr,arr2);
			g[root][t_0]=g[t_0][root]=1;
		}
		else{
			// cout << "ENTERING P COMPLEX : " << p << endl;
			cost=n*n; // More than the maximum
			for(int i=0;i<=2*p+1;++i){
				g[root][ord[i].second]=g[ord[i].second][root]=0;
			} // Now g is Y
			for(int i=0;i<=2*p+1;++i){
				vector<int> P_i;
				get_P(P_i,p,i);
				g[root][ord[i].second]=g[ord[i].second][root]=1;
				vector<int> arr_i (n+1,0);
				int c_i=0;
				for (int j=1;j<=2*p+1;++j){
					int c_i_j;
					vector<int> arr_i_j;
					int node=P_i[j];
					calculate_mla_chung(g,ord[node].second,ord[node].second,arr_i_j,c_i_j);
					join_arrangements(arr_i,arr_i_j);
					c_i+=c_i_j;
				}
				int c_i_j;
				vector<int> arr_i_j;
				calculate_mla_chung(g,0,one_node,arr_i_j,c_i_j);
				//calculate_mla(g,0,root,arr_i_j,c_i_j);
				join_arrangements(arr_i,arr_i_j);
				c_i+=c_i_j;
				// Rest of property 13
				c_i+=size_tree*(p+1);
				c_i-=(p+1)*ord[P_i[P_i.size()-1]].first;
				int subs=0;
				for (int j=1;j<=p;++j) {
					subs+=(p-j+1)*(ord[P_i[j]].first+ ord[P_i[2*p-j+1]].first);
				}
				c_i-=subs;
				if (c_i<cost) {
					cost=c_i;
					arr=arr_i;
				}
				g[root][ord[i].second]=g[ord[i].second][root]=0;		
			}
			for(int i=0;i<=2*p+1;++i){
				g[root][ord[i].second]=g[ord[i].second][root]=1;
			} // Now g is again g
		}
	
	}
}



int calculate_D_min_chung_aux(int n, list<pair<int,int> >&tree) 
{
   adjacency_matrix g;
   convert_tree_to_matrix(n,tree,g);
   vector<int> arrangement(n+1,0);
   int c;
   int alpha=0;
   calculate_mla_chung(g,alpha,1,arrangement,c);
   return c;
}



