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

#include "common.hpp"

uint32_t get_v_star(adjacency_matrix & g, const size_matrix & s, const vector<uint32_t> & list_of_nodes)
// For free (unanchored) trees
// Elements de v are all the nodes connected among them in g
{
	uint32_t n=g.size()-1;
	uint32_t number_of_nodes=list_of_nodes.size();
	uint32_t h=number_of_nodes/2;
	uint32_t v_star=0; 
	uint32_t i=0;
	bool v_star_found=false;
	while(i<number_of_nodes and not v_star_found){
		uint32_t j=1;
		bool tree_too_big=false;
		while (not tree_too_big and j<=n){
			uint32_t ith_root=list_of_nodes[i];
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

uint32_t calculate_p(uint32_t root, uint32_t anchored, const ordering & ord, uint32_t & s_0, uint32_t & s_1)
{
	// anchored is ANCHOR or NO_ANCHOR
	// left or right anchored is not important for the cost
	assert(anchored==NO_ANCHOR or anchored==ANCHOR);
	uint32_t n=1; // Counting root
	for (uint32_t i=0;i<ord.size();++i) n+=ord[i].first; 
	// n is number of nodes
	uint32_t k=ord.size(); // number of subtrees
	--k; // T_0, T_1, ..., T_k
	uint32_t n_0 = ord[0].first;
	uint32_t max_p;
	// not anchored
	if (anchored==NO_ANCHOR){
		max_p = k/2; // Maximum possible p_alpha
		if (max_p==0) return 0;
		uint32_t sum=0;
		for (uint32_t i=0;i<=2*max_p;++i) sum+=ord[i].first;
		uint32_t n_star=n-sum;
		uint32_t tricky_formula=(n_0+2)/2+(n_star+2)/2; 
		// n_0 >= n_1 >= ... >= n_k
		uint32_t n_p=ord[2*max_p].first; 
		while(max_p>0 and n_p <= tricky_formula){
			sum-=ord[2*max_p].first;
			sum-=ord[2*max_p-1].first;
			--max_p;
			n_star=n-sum;
			tricky_formula=(n_0+2)/2+(n_star+2)/2;
			if (max_p>0) n_p=ord[2*max_p].first; 
		}
		s_0=0; 
		s_1=0;
		for(uint32_t i=1;i<max_p;++i) {
			s_0+=i*(ord[2*i+1].first+ord[2*i+2].first);
		}
		s_0+=max_p*(n_star+1+n_0);
	}
	// anchored
	else{
		max_p=(k+1)/2;	
		if (max_p==0) return 0;
		uint32_t sum=0;
		for (uint32_t i=0;i<=2*max_p-1;++i) sum+=ord[i].first;
		uint32_t n_star=n-sum;
		uint32_t tricky_formula=(n_0+2)/2+(n_star+2)/2; 
		uint32_t n_p=ord[2*max_p-1].first; 
		while(max_p>0 and n_p <= tricky_formula){ 
			sum-=ord[2*max_p-1].first;
			sum-=ord[2*max_p-2].first;
			--max_p; 
			n_star=n-sum;
			tricky_formula=(n_0+2)/2+(n_star+2)/2;
			if (max_p>0) n_p=ord[2*max_p-1].first; 
		}
		s_1=0; s_0=0;
		for(uint32_t i=1;i<max_p;++i) {
			s_1+=i*(ord[2*i].first+ord[2*i+1].first);
		}
		s_1+=max_p*(n_star+1+n_0)-1;	
	}
	return max_p;
	
}

void calculate_mla(adjacency_matrix & g, int alpha, uint32_t root_or_anchor, uint32_t start, vector<uint32_t> & mla, uint32_t & cost)
// start means the leftmost position in the mla for the subtree
// we could also have an anologous finish (rightmost position) but it is not needed
{
	assert(alpha==NO_ANCHOR or alpha==RIGHT_ANCHOR or alpha==LEFT_ANCHOR);

	vector<uint32_t> node_vector;
	reachable_nodes(g,node_vector,root_or_anchor);
	uint32_t size_tree=node_vector.size(); // Size of the tree
	assert(size_tree>0);
	
	// Base case
	if (size_tree==1) {
		cost=0;
		mla[root_or_anchor-1]=start;
		return;
	}
	
	// Recursion for COST A
	size_matrix s;
	size_of_all_subtrees_from_all_nodes(g,s,node_vector);
	
	uint32_t v_star;
	if (alpha==NO_ANCHOR) v_star=get_v_star(g,s,node_vector);
	else v_star=root_or_anchor;
	
	
	ordering ord;
	order_subtrees_by_size(g,s[v_star],v_star,ord);

	uint32_t v_0,n_0;  	
	v_0=ord[0].second; // Root of biggest subtree
	n_0=ord[0].first; // Size of biggest subtree 
	
	g[v_star][v_0]=g[v_0][v_star]=0;
	
	uint32_t c1,c2;
	
	calculate_mla(g,RIGHT_ANCHOR,v_0,start,mla,c1);
	if (alpha==NO_ANCHOR) calculate_mla(g,LEFT_ANCHOR,v_star,start+n_0,mla,c2);
	else calculate_mla(g,NO_ANCHOR,v_star,start+n_0,mla,c2);
	
	// Cost for A-recursion
	if (alpha==NO_ANCHOR) cost=c1+c2+1;
	else cost=c1+c2+size_tree-n_0; 
		
	// Reconstructing g 
	g[v_star][v_0]=g[v_0][v_star]=1;
	
	// Recursion B
	uint32_t s_0,s_1;
	uint32_t p_alpha=0;
	
	// left or right anchored is not important for the cost
	uint32_t anchored = NO_ANCHOR;
	if (alpha==RIGHT_ANCHOR or alpha==LEFT_ANCHOR) anchored=ANCHOR;
	
	p_alpha=calculate_p(v_star,anchored,ord,s_0,s_1); 
	

	uint32_t cost_B=0;
	vector<uint32_t> mla_B;
	mla_B=mla; 
	
	if (p_alpha!=0){
		// number of nodes not in the central tree
		uint32_t n_not_central_tree=0; 
		for (uint32_t i=1;i<=2*p_alpha-anchored;++i){
			uint32_t r=ord[i].second;
			n_not_central_tree+=ord[i].first;
			g[v_star][r]=g[r][v_star]=0;
		} 
		// T_1, T_3, ...
		uint32_t start_aux=start;
		for (uint32_t i=1;i<=2*p_alpha-anchored;i=i+2){
			uint32_t c;
			uint32_t r=ord[i].second;
			calculate_mla(g,RIGHT_ANCHOR,r,start_aux,mla_B,c);
			cost_B+=c;
			start_aux+=ord[i].first;	
		}
		
		// T-(T_1, T_2, ...)
		uint32_t c;
		calculate_mla(g,NO_ANCHOR,v_star,start_aux,mla_B,c);
		// number of nodes in the central tree
		uint32_t n_central_tree=size_tree-n_not_central_tree;
		cost_B+=c;
		start_aux+=n_central_tree;
		
		// ..., T_4, T_2
		for (uint32_t i=2*p_alpha-2*anchored;i>=2;i=i-2){
			uint32_t c;
			uint32_t r=ord[i].second;
			calculate_mla(g,LEFT_ANCHOR,r,start_aux,mla_B,c);
			cost_B+=c;
			start_aux+=ord[i].first;
		}
		
		// Reconstructing g
		for (uint32_t i=1;i<=2*p_alpha-anchored;++i){
			uint32_t r=ord[i].second;
			g[v_star][r]=g[r][v_star]=1;
		} 
	
		// We add the anchors part not previously added
		if (alpha==NO_ANCHOR) cost_B+=s_0;
		else cost_B+=s_1;
	}

	// We choose B-recursion only if it is better
	if (p_alpha!=0){
		if (cost_B<cost){
			mla.swap(mla_B);
			cost=cost_B;
		}
	}
	
	// from this on is only needed to make the arrangement
	// flip arrangement if the anchor is far from the root
	// node vector contains reachable nodes
	// it is done only here after the arrangement is choosed
	uint32_t anchor=v_star;
	if (alpha==RIGHT_ANCHOR) {
		// To flip the arrangement it must hold 
		// start + mla[anchor] < start + size_tree- mla[anchor] + 1
		// simplified and avoiding negative values
		// 2*mla[anchor] < size_tree+1
		if (2*mla[anchor-1] < size_tree+1) {
			for(uint32_t i=0;i<size_tree;++i){ 
				uint32_t aux = start+size_tree-1-mla[node_vector[i]-1]+start;
				mla[node_vector[i]-1]=aux;
			}
		}	
	}
	else if (alpha==LEFT_ANCHOR){
		// To flip the arrangement it must hold 
		// start + mla[anchor] > start + size_tree- mla[anchor] + 1
		// simplified and avoiding negative values
		// 2*mla[anchor] > size_tree+1
		if (2*mla[anchor-1] > size_tree+1) {
			for(uint32_t i=0;i<size_tree;++i){
				uint32_t aux = start+size_tree-1-mla[node_vector[i]-1]+start;
				mla[node_vector[i]-1]=aux;
			}
		}
	}
}

uint32_t calculate_D_min_Shiloach(uint32_t n, list<pair<uint32_t,uint32_t> >&tree) { 
	 
   adjacency_matrix g;
   convert_tree_to_matrix(n,tree,g);
   vector<uint32_t> arrangement(n+1,0);

   uint32_t c;
   uint32_t alpha=0;
   calculate_mla(g,alpha,1,1,arrangement,c);

   return c;
}

