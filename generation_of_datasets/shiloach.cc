#include "common.hh"


//// Implementation of Shiloach's algorithm for Minimal Linear Arrangement 
//// for undirected trees.

//#include <iostream>
//#include <vector>
//#include <algorithm>
//using namespace std; 

//class MyException: public exception {
  //public:
   //MyException (const char* mot) : exception(), mensaje(mot) {}
   //const char* what() const throw() {return mensaje;}; 
  //private:
   //const char* mensaje;
//};

//typedef vector<vector<int> > adjacency_matrix; 
//// Neither row 0 nor column 0 is used.

//typedef vector<vector<int> > size_matrix; 
//// Neither row 0 nor column 0 is used
//// It's the same as adjacency_matrix but is
//// used to keep the sizes of all subtrees
//// using all nodes as roots.
//// elem[i][j] is the size of the subtree
//// rooted in j in the tree rooted in i
//// In one matrix there can be several
//// unconnected trees.

//typedef vector<pair<int,int> > ordering;
//// Elements are size and root of subtrees ordered by size

//typedef vector<pair<int,int> > edge_vector;
//// Vector with all edges

//void read_adjacency_matrix(adjacency_matrix & m)
//// Neither row 0 nor column 0 is used.
//{
	//if (m.size()>0) throw MyException("Adjacency matrix should be empty");	
	//int n=m.size()-1;
	//for(int i=1;i<=n;++i){
		//for(int j=1;j<=n;++j){
			//cin >> m[i][j];
		//}
	//}
//}

//void write_matrix(const vector<vector<int> > & m)
//// Neither row 0 nor column 0 is used.
//{	
	//cout << "ADJACENCY MATRIX :" << endl;
	//int n=m.size()-1;
	//for(int i=1;i<=n;++i){
		//cout << m[i][1];
		//for(int j=2;j<=n;++j){
			//cout << " " << m[i][j];
		//}
		//cout << endl;
	//}
//}

//void write_reachable_nodes(const vector<int> & v)
//{
	//cout << "REACHABLE NODES :";
	//int n=v.size();
	//for(int j=0;j<n;++j){
		//cout << " " << v[j];
	//}
	//cout << endl;
//}

//void write_arr(const vector<int> & v)
//{
	////cout << "ARR : ";
	//int n=v.size();
	//for(int j=1;j<n;++j){
		//cout << " " << v[j];
	//}
	//cout << endl;
//}

//void write_edge_vector(const vector<pair<int, int> > & v)
//{	
	//cout << "EDGE VECTOR : ";
	//int n=v.size()-1;
	//for(int j=0;j<=n;++j){
		//cout << "(" << v[j].first << "," << v[j].second << ") ";
	//}
	//cout << endl;
//}

//void write_ordering(const vector<pair<int, int> > & v)
//{	
	//cout << "ORDERING : ";
	//int n=v.size()-1;
	//for(int j=0;j<=n;++j){
		//cout << "Size : " << v[j].first << ", root : " << v[j].second << " ";
	//}
	//cout << endl;
//}

//void size_of_all_subtrees_from_one_node(adjacency_matrix & g,vector<int> &v, int id_root)
//// v[i] is the size of the subtree rooted in i inside the tree 
//// rooted in id_root, if there is a path from id_root to i.
//// There can be several nodes unconnected to id_root
//// after some recursive calls.
//{
	//int n=v.size()-1;
	//v[id_root]=1;
	//for (int i=1;i<=n;++i){
		//if (g[id_root][i]==1){
			//g[i][id_root]=-1;
			//size_of_all_subtrees_from_one_node(g,v,i);
			//v[id_root]+=v[i];
			//g[i][id_root]=1; 
		//}
	//}
//}



//void reachable_nodes(adjacency_matrix & g, vector<int> &v, int root)
//// We get all nodes reachable from node root
//{
	//v.push_back(root);
	//int n=g.size()-1;
	//for (int i=1;i<=n;++i){
		//if (g[root][i]==1){
			//g[i][root]=-1;
			//reachable_nodes(g,v,i);
			//g[i][root]=1; 
		//}
	//}
//}

//void size_of_all_subtrees_from_all_nodes(adjacency_matrix & g, size_matrix & s, const vector<int> & v)
//// elements de v are all the nodes connected among them in g
//{
	//int n=g.size()-1;
	//s = size_matrix(n+1,vector<int> (n+1,0));
	//for (int i=0;i<v.size();++i) {
		//size_of_all_subtrees_from_one_node(g,s[v[i]],v[i]);
	//}
//}

int get_v_star(adjacency_matrix & g, const size_matrix & s, const vector<int> & list_of_nodes)
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

//bool order_pair(const pair<int,int> &p1, const pair<int,int> & p2)
//{
	//if (p1.first==p2.first) return p1.second<p2.second;
	//else return p1.first > p2.first;
//}

//void order_subtrees_by_size(const adjacency_matrix & g, vector<int> & s, int root, ordering & ord)
//{
	//int n=g.size()-1;
	//if (ord.size()!=0) throw MyException("Ordering should be empty");
	//pair<int,int> elem;
	//for (int i=1;i<=n;++i){
		//if (g[root][i]==1) {
			//elem.first=s[i];
			//elem.second=i;
			//ord.push_back(elem);
		//}
	//}
	//sort(ord.begin(),ord.end(),order_pair);
//}

int calculate_p(int root, int alpha, const ordering & ord, int & s_0, int & s_1)
{
	int n=1; // Counting root
	for (int i=0;i<ord.size();++i) n+=ord[i].first; 
	// n is number of nodes
	int k=ord.size(); // number of subtrees
	--k; // T_0, T_1, ..., T_k
	if (alpha==-1) alpha=1;
	int n_0= ord[0].first;
	int max_p;
	if (alpha==0){
		max_p=k/2;  // Maximum possible p_alpha
		if (max_p==0) return 0;
		int sum=0;
		for (int i=0;i<=2*max_p;++i) sum+=ord[i].first;
		int n_star=n-sum;
		int tricky_formula=(n_0+2)/2+(n_star+2)/2; 
		// n_0 >= n_1 >= ... >= n_k
		int n_p=ord[2*max_p].first; 
		while(max_p>0 and n_p <= tricky_formula){
			--max_p;
			sum=0;
			for (int i=0;i<=2*max_p;++i) sum+=ord[i].first;
			n_star=n-sum;
			tricky_formula=(n_0+2)/2+(n_star+2)/2;
			if (max_p>0) n_p=ord[2*max_p].first; 
		}
		s_0=0; s_1=0;
		for(int i=1;i<max_p;++i) {
			s_0+=i*(ord[2*i+1].first+ord[2*i+2].first);
		}
		//if (max_p>0){
			//cout << "P_ALPHA " << max_p << endl;
			//cout << "S_0 " << s_0 << endl;
			s_0+=max_p*(n_star+1+n_0);
			//cout << "S_0 " << s_0 << endl;
		//}
	}	
	else if (alpha==1){
		max_p=(k+1)/2;	
		if (max_p==0) return 0;
		int sum=0;
		for (int i=0;i<=2*max_p-1;++i) sum+=ord[i].first;
		int n_star=n-sum;
		int tricky_formula=(n_0+2)/2+(n_star+2)/2; 
		int n_p=ord[2*max_p-1].first; 
		while(max_p>0 and n_p <= tricky_formula){  
 			--max_p;
			sum=0;
			for (int i=0;i<=2*max_p-1;++i) sum+=ord[i].first;
			n_star=n-sum;
			tricky_formula=(n_0+2)/2+(n_star+2)/2;
			if (max_p>0) n_p=ord[2*max_p-1].first; 
		}
		s_1=0; s_0=0;
		for(int i=1;i<max_p;++i) {
			s_1+=i*(ord[2*i].first+ord[2*i+1].first);
		}
		//if (max_p>0){
			//cout << "P_ALPHA " << max_p << endl;
			//cout << "S_1 " << s_1 << endl;
			s_1+=max_p*(n_star+1+n_0)-1;
			//cout << "S_1 " << s_1 << endl;
		//}
	}
	else throw MyException("WRONG ALPHA");
	
	return max_p;
	
}

void join_arrangements(vector<int> &v1, const vector<int> & v2)
{	
	int n=v1.size()-1;
	int m=0;
	for(int i=1;i<=n;++i) if (v1[i]!=0) ++m;
	for(int i=1;i<=n;++i){
		if (v2[i]!=0) {
			if (v1[i]==0) v1[i]=m+v2[i];
			else throw MyException("Same node in disjoint arr.");
		}
	}
}

//int evaluate_arrangement(const vector<int> & arr,const edge_vector & mt)
//{
	//int res=0;
	//for (int i=0;i<mt.size();++i){
		//int x=arr[mt[i].first]-arr[mt[i].second];
		//if (x<0) x=-x;
		//res+=x;
	//}
	//return res;
//}

//void graph_to_tree(adjacency_matrix & g, edge_vector & mt, int root)
//{
	//int n=g.size()-1;
	//for (int i=1;i<=n;++i){
		//if (g[root][i]==1){
			//pair <int,int> node;
			//node.first=root;
			//node.second=i;
			//mt.push_back(node);
			//g[i][root]=-1;
			//graph_to_tree(g,mt,i);
			//g[i][root]=1; 
		//}
	//}
	//sort(mt.begin(),mt.end(),order_pair);
//}

void calculate_mla(adjacency_matrix & g, int alpha, int root_or_anchor, vector<int> & mla, int & cost)
// alpha 0 means free tree 
// alpha 1 means right anchored tree
// alpha -1 means left anchored tree
{
	int n=g.size()-1;
	vector<int> node_vector;
	reachable_nodes(g,node_vector,root_or_anchor);
	int size_tree=node_vector.size(); // Size of the tree
	
	if (size_tree==0) throw MyException("Empty tree");
	
	// Base case
	if (size_tree==1) {
		cost=0;
		mla=vector<int> (n+1,0);
		mla[root_or_anchor]=1;
		return;
	}
	
	// Recursion for COST A
	size_matrix s;
	size_of_all_subtrees_from_all_nodes(g,s,node_vector);
	
	int v_star;
	if (alpha==0) v_star=get_v_star(g,s,node_vector);
	else if (alpha==1 or alpha==-1) v_star=root_or_anchor;
	else throw MyException("--- WRONG ALPHA ---");
	
	edge_vector mt;
	graph_to_tree(g,mt,v_star);
	
	
	ordering ord;
	order_subtrees_by_size(g,s[v_star],v_star,ord);

	int v_0,n_0;  	
	v_0=ord[0].second; // Root of biggest subtree
	n_0=ord[0].first; // Size of biggest subtree 
	
	g[v_star][v_0]=g[v_0][v_star]=0;
	
	int c1,c2;
	vector<int> arr1 (n+1,0);
	vector<int> arr2 (n+1,0);
	
	calculate_mla(g,1,v_0,arr1,c1);
	if (alpha==0) calculate_mla(g,-1,v_star,arr2,c2);
	else if (alpha==-1 or alpha==1) calculate_mla(g,0,v_star,arr2,c2);
	else throw MyException("--- WRONG ALPHA ---");	
	
	vector<int> arr_A (n+1,0);	
	
	join_arrangements(arr_A,arr1);
	join_arrangements(arr_A,arr2);
	
	int anchor=v_star;
	if (alpha==1) {
		if (arr_A[anchor]<size_tree-arr_A[anchor]+1) {
			vector<int> zxy (n+1,0);	
			for(int i=1;i<=n;++i){
				if (arr_A[i]!=0) zxy[i]=size_tree-arr_A[i]+1;
			}
			arr_A=zxy;	
		}	
	}
	else if (alpha==-1){
		if (arr_A[anchor] > size_tree-arr_A[anchor]+1){
			vector<int> zxy (n+1,0);	
			for(int i=1;i<=n;++i){
				if (arr_A[i]!=0) zxy[i]=size_tree-arr_A[i]+1;
			}
			arr_A=zxy;	
		}
	}

	int cost_A_arr=evaluate_arrangement(arr_A,mt);
	if (alpha==1) cost_A_arr=cost_A_arr+ size_tree-arr_A[anchor];
	else if (alpha==-1) cost_A_arr=cost_A_arr+arr_A[anchor]-1;
	
	int cost_A_calc; 
	if (alpha==0) cost_A_calc=c1+c2+1;
	else if (alpha==1 or alpha==-1) {
		cost_A_calc=c1+c2+size_tree-n_0; 
	}
	else throw MyException("--- WRONG ALPHA ---");
	
	//if (cost_A_arr!=cost_A_calc)  cout << "DIFA" << endl;
	
	// Reconstructing g 
	g[v_star][v_0]=g[v_0][v_star]=1;
	
	// Recursion for COST B
	int s_0,s_1;
	int p_alpha=0;
	
	int remember_alpha=alpha;  // Probably is not needed
	if (alpha==-1) alpha=1; 
	if (ord.size()>0) p_alpha=calculate_p(v_star,alpha,ord,s_0,s_1); 

	int cost_B_calc=0;
	int cost_B_arr=0;
	vector<int> arr_B(n+1,0);
	if (p_alpha!=0){
		////T_1, T_3, ...
		//cout << "ENTERING P COMPLEX : " << p_alpha << " WITH ALPHA : " << remember_alpha << endl;	
		//cout << "s_0 : " << s_0 << " s_1 : " << s_1 << endl;
		//write_edge_vector(mt);
		//write_ordering(ord);
		
		for (int i=1;i<=2*p_alpha-alpha;++i){
			int r=ord[i].second;
			g[v_star][r]=g[r][v_star]=0;
		} 
		
		for (int i=1;i<=2*p_alpha-alpha;i=i+2){
			int c;
			//cout << "ENTRO AQUI" << endl;
			vector<int> arr(n+1,0);
			int r=ord[i].second;
			calculate_mla(g,1,r,arr,c);
			cost_B_calc+=c;
			join_arrangements(arr_B,arr);
			
		}
		
		// T-(T_1,...)
		int c;
		vector<int> arr(n+1,0);
		calculate_mla(g,0,v_star,arr,c);
		
		// One way of counting the size of the three rooted at v_star
		vector<int> nodes_from_v_star;
		reachable_nodes(g,nodes_from_v_star,v_star);
		int n_tree=nodes_from_v_star.size();
		// Another way of counting the size of the three rooted at v_star
		// This one is faster
		int n_tree_bis=1; // Counting v_star
		for (int i=2*p_alpha-alpha+1;i<ord.size();++i) n_tree_bis+=ord[i].first;
		n_tree_bis+=ord[0].first;
		if (n_tree!=n_tree_bis) throw MyException("Problem with the size of the central tree");
		cost_B_calc+=c;
		join_arrangements(arr_B,arr);
		
		for (int i=2*p_alpha-2*alpha;i>=2;i=i-2){
			int c;
			//cout << "ENTRO ALLA" << endl;
			vector<int> arr(n+1,0);
			int r=ord[i].second;
			//g[v_star][r]=g[r][v_star]=0; 
			calculate_mla(g,-1,r,arr,c);
			cost_B_calc+=c;
			join_arrangements(arr_B,arr);
		}
		
		
		
		int anchor=root_or_anchor;
		if (remember_alpha==1) {
			
			if (arr_B[anchor]<size_tree-arr_B[anchor]+1) {
			
				vector<int> zxy (n+1,0);	
				for(int i=1;i<=n;++i){
					if (arr_B[i]!=0) zxy[i]=size_tree-arr_B[i]+1;
				}
				arr_B=zxy;	
			}	
			
		}
		else if (remember_alpha==-1){
			
			if (arr_B[anchor] > size_tree-arr_B[anchor]+1){
				
				vector<int> zxy (n+1,0);	
				for(int i=1;i<=n;++i){
					if (arr_B[i]!=0) zxy[i]=size_tree-arr_B[i]+1;
				}
				arr_B=zxy;	
			}
		}

	// Reconstructing g
		for (int i=1;i<=2*p_alpha-alpha;++i){
			int r=ord[i].second;
			g[v_star][r]=g[r][v_star]=1;
		} 
	
	
	edge_vector mt2;
	graph_to_tree(g,mt2,v_star);
	if (mt!=mt2) cout << "ARBOLES DIFERENTES" << endl;
	
	cost_B_arr=evaluate_arrangement(arr_B,mt2);
	
	
	//write_edge_vector(mt);
	//cout << "COST_ARR ANTES ANCLA " << cost_B_arr << endl;
	if (remember_alpha==1) cost_B_arr=cost_B_arr+ size_tree-arr_B[anchor];
	else if (remember_alpha==-1) cost_B_arr=cost_B_arr+arr_B[anchor]-1;
	//cout << "COST_ARR DESP ANCLA " << cost_B_arr << endl;	
		
	//cout << "COST B ANTES SUMA " << cost_B << endl;
	if (alpha==0) cost_B_calc+=s_0;
	else if (alpha==-1 or alpha==1) cost_B_calc+=s_1;
	else throw MyException("--- WRONG ALPHA ---");
	//cout << "COST B DESPUEs SUMA " << cost_B << endl;
	int valor_ancla=size_tree-arr_B[anchor];
	//cout << "VALOR ANCLA " << valor_ancla << endl;
	//if (cost_B_calc!=cost_B_arr) cout << "DIFB" << endl;
		//cout << "P_ALPHA " << p_alpha << " ALPHA " << remember_alpha << " ROOT OR ANCHOR " << anchor <<endl;
		//cout << "COST A CALC " << cost_A_calc << " COST A ARR " << cost_A_arr <<  " COST B CALC " << cost_B_calc << " COST B ARR " << cost_B_arr <<endl;
		//write_arr(arr_A);
		//write_arr(arr_B);
		//write_edge_vector(mt);
	}

	if (p_alpha!=0){
		// COST B CALC WAS OK
		if (cost_B_arr<cost_A_calc){
			//cout << "COSTE B ES MEJOR" << endl;
			mla=arr_B;
			cost=cost_B_calc;
		}
		else{
			mla=arr_A;
			cost=cost_A_calc;
		}
	}
	else{
		mla=arr_A;
		cost=cost_A_calc;
	}
}

//int read_matrix(adjacency_matrix & g)
//{
	//int n;
	//cin >> n;
	//if (n==0) return 0;
	//g = adjacency_matrix (n+1,vector<int> (n+1,0));
	//int x,y;
	//cin >> x;
	//while (x!=0){
		//cin >> y;
		//if (g[x][y]!=g[y][x]) throw MyException("ERROR: 'DIRECTED NODE'");
		//else if (g[x][y]!=0) throw MyException("ERROR: REPEATED NODE");
		//else g[x][y]=g[y][x]=1;
		//cin >> x;	
	//}
	//return n;
//}


//void calculate_mla_compara(adjacency_matrix & g, int alpha, int root_or_anchor, vector<int> & mla, int & cost)
//{
	//calculate_mla(g,0,1,mla,cost);	
	//edge_vector mt;
	//graph_to_tree(g,mt,1);
	//int cost_mla=evaluate_arrangement(mla,mt);
	//if (cost_mla!=cost) throw MyException("COSTS DIFFER");
	
//}


int calculate_D_min_Shiloach_aux(int n, list<pair<int,int> >&tree) { 
	 
   adjacency_matrix g;
   convert_tree_to_matrix(n,tree,g);
   vector<int> arrangement(n+1,0);
   int c;
   int alpha=0;
   calculate_mla(g,alpha,1,arrangement,c);
   edge_vector mt;
	graph_to_tree(g,mt,1);
	int cost_mla=evaluate_arrangement(arrangement,mt);
	
	if (cost_mla!=c) throw MyException("COSTS DIFFER");
   
   return c;
}




//int main(){
	//adjacency_matrix graph; 
	//int n_nodos;
	//n_nodos=read_matrix(graph);
	//while(n_nodos!=0){
		//int n=graph.size()-1;
		//vector<int> arr;
		//int c;
		//calculate_mla_compara(graph,0,1,arr,c);		
		//edge_vector mt;
		//graph_to_tree(graph,mt,1);
		//int c_of_c_mla=evaluate_arrangement(arr,mt);
		//cout << c_of_c_mla << " " << c << " :";
		//write_arr(arr);
		//n_nodos=read_matrix(graph);
	//}
//}
