/*********************************************************************
 *
 * Optimality Syntactic Dependency Distances
 * 
 * Copyright (C) 2020 Lluís Alemany-Puig
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
 *     Lluís Alemany Puig (lalemany@cs.upc.edu)
 *         LARCA (Laboratory for Relational Algorithmics, Complexity and Learning)
 *         CQL (Complexity and Quantitative Linguistics Lab)
 *         Jordi Girona St 1-3, Campus Nord UPC, 08034 Barcelona.    CATALONIA, SPAIN
 *         Webpage: https://cqllab.upc.edu/people/lalemany/
 *
 ********************************************************************/

#include "ftree.hpp"

// C++ includes
#include <algorithm>
#include <cassert>
#include <cmath>
#include <set>
using namespace std;

/* PUBLIC */

ftree::ftree(uint32_t n) {
	init(n);
}

/* MODIFIERS */

void ftree::init(uint32_t n) {
	clear();
	
	m_num_edges = 0;
	m_adjacency_list = vector<neighbourhood>(n);
}

void ftree::clear() {
	m_num_edges = 0;
	m_adjacency_list.clear();
}

ftree& ftree::add_edge(node u, node v) {
	assert(u != v);
	assert(has_node(u));
	assert(has_node(v));

	neighbourhood& nu = m_adjacency_list[u];
	neighbourhood& nv = m_adjacency_list[v];
	nu.push_back(v);
	nv.push_back(u);
	++m_num_edges;
	
	return *this;
}

ftree& ftree::add_edges(const vector<edge>& edges) {
	for (const edge& e : edges) {
		const node u = e.first;
		const node v = e.second;
		assert(has_node(u));
		assert(has_node(v));
		assert(u != v);

		m_adjacency_list[u].push_back(v);
		m_adjacency_list[v].push_back(u);
		++m_num_edges;
	}

	return *this;
}

/* SETTERS */

/* GETTERS */

const neighbourhood& ftree::get_neighbours(node u) const {
	assert(has_node(u));
	return m_adjacency_list[u];
}

uint32_t ftree::degree(node u) const {
	assert(has_node(u));
	return static_cast<uint32_t>(m_adjacency_list[u].size());
}

uint32_t ftree::n_nodes() const {
	return static_cast<uint32_t>(m_adjacency_list.size());
}

uint32_t ftree::n_edges() const {
	return m_num_edges;
}
