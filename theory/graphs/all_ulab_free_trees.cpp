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
 
#include "all_ulab_free_trees.hpp"

// C++ includes
#include <cassert>
#include <limits>
using namespace std;

#define inf numeric_limits<uint32_t>::max()

/* AUXILIARY FUNCTIONS */

ftree level_sequence_to_tree(const vector<uint32_t>& L, uint32_t n) {
	// a little sanity check
	assert(L[0] == 0);
	assert(L[1] == 1);
	assert(utils::to_uint32(L.size()) == n + 1);

	// edges of the tree
	vector<edge> edges(n - 1);
	auto eit = edges.begin();

	// 'stack' of root candidates: node at every level in {1,...,N}.
	// at position j, lev[j], store the last node added
	// at level j.
	vector<node> lev(n+1, 0);
	uint32_t stack_it = 0;

	// evidently,
	lev[0] = 1;

	for (node i = 2; i <= n; ++i) {

		// find in the stack the node which
		// has to be connected to node 'i'.
		if (lev[stack_it] + 2 > L[i]) {
			stack_it = L[i] - 2 + 1;
		}

		// the top of the stack is the parent of this node
		const node r = lev[stack_it];

		// add the edge...
		*eit++ = (r == 0 ? edge(r, i - 1) : edge(r - 1, i - 1));

		// the last node added at level L[i] is i.
		++stack_it;
		assert(stack_it == L[i]);
		lev[stack_it] = i;
	}

	ftree t(n);
	t.add_edges(edges);
	return t;
}

/* CLASS IMPLEMENTATION */



all_ulab_free_trees::all_ulab_free_trees(uint32_t _n) {
	init(_n);
}

void all_ulab_free_trees::init(uint32_t _n) {
	m_is_last = false;
	m_first_it = true;
	m_n = _n;

	m_L = vector<uint32_t>(m_n+1, 0);
	m_W = vector<uint32_t>(m_n+1, 0);

	// simplest cases
	if (m_n == 0) {
		m_is_last = true;
		return;
	}
	if (m_n <= 2) {
		m_is_last = false;
		return;
	}

	uint32_t k = m_n/2 + 1;
	m_p = (m_n == 4 ? 3 : m_n);
	m_q = m_n - 1;
	m_h1 = k;
	m_h2 = m_n;
	m_r = k;
	m_c = (m_n%2 == 0 ? m_n + 1 : inf);

	// initialise L and W
	for (uint32_t i = 1; i <= k; ++i) {
		m_W[i] = i - 1;
		m_L[i] = i;
	}
	m_W[k+1] = 1;
	m_L[k+1] = 2;
	for (uint32_t i = k + 2; i <= m_n; ++i) {
		m_W[i] = i - 1;
		m_L[i] = i - k + 1;
	}

	if (m_n <= 3) {
		m_first_it = false;
	}
}

bool all_ulab_free_trees::has_next() const {
	return not m_is_last;
}

void all_ulab_free_trees::next() {
	if (m_n <= 2) {
		m_is_last = true;
		return;
	}
	if (m_first_it) {
		m_first_it = false;
		return;
	}

	// implementation copied "as is" from the cited paper
	// in the description of the class. Read the paper...

	bool fixit = false;
	if (
		(m_c == m_n + 1) or
		(m_p == m_h2 and (
			(m_L[m_h1] == m_L[m_h2] + 1 and m_n - m_h2 > m_r - m_h1)
			or
			(m_L[m_h1] == m_L[m_h2] and m_n - m_h2 + 1 < m_r - m_h1)
		))
	)
	{
		if (m_L[m_r] > 3) {
			m_p = m_r;
			m_q = m_W[m_r];
			if (m_h1 == m_r) { --m_h1; }
			fixit = true;
		}
		else {
			m_p = m_r;
			--m_r;
			m_q = 2;
		}
	}

	bool needr, needc, needh2;
	needr = needc = needh2 = false;

	if (m_p <= m_h1) {
		m_h1 = m_p - 1;
	}
	if (m_p <= m_r) {
		needr = true;
	}
	else if (m_p <= m_h2) {
		needh2 = true;
	}
	else if (m_L[m_h2] == m_L[m_h1] - 1 and m_n - m_h2 == m_r - m_h1) {
		if (m_p <= m_c) {
			needc = true;
		}
	}
	else {
		m_c = inf;
	}

	const uint32_t oldp = m_p;
	const uint32_t delta = m_q - m_p;
	const uint32_t oldLq = m_L[m_q];
	const uint32_t oldWq = m_W[m_q];
	m_p = inf;

	for (uint32_t i = oldp; i <= m_n; ++i) {
		m_L[i] = m_L[i + delta];
		if (m_L[i] == 2) {
			m_W[i] = 1;
		}
		else {
			m_p = i;
			if (m_L[i] == oldLq) {
				m_q = oldWq;
			}
			else {
				m_q = m_W[i + delta] - delta;
			}
			m_W[i] = m_q;
		}
		if (needr and m_L[i] == 2) {
			needr = false;
			needh2 = true;
			m_r = i - 1;
		}
		if (needh2 and m_L[i] <= m_L[i - 1] and i > m_r + 1) {
			needh2 = false;
			m_h2 = i - 1;
			if (m_L[m_h2] == m_L[m_h1] - 1 and m_n - m_h2 == m_r - m_h1) {
				needc = true;
			}
			else {
				m_c = inf;
			}
		}
		if (needc) {
			if (m_L[i] != m_L[m_h1 - m_h2 + i] - 1) {
				needc = false;
				m_c = i;
			}
			else {
				m_c = i + 1;
			}
		}
	}
	if (fixit) {
		m_r = m_n - m_h1 + 1;
		for (uint32_t i = m_r + 1; i <= m_n; ++i) {
			m_L[i] = i - m_r + 1;
			m_W[i] = i - 1;
		}
		m_W[m_r + 1] = 1;
		m_h2 = m_n;
		m_p = m_n;
		m_q = m_p - 1;
		m_c = inf;
	}
	else {
		if (m_p == inf) {
			if (m_L[oldp - 1] != 2) {
				m_p = oldp - 1;
			}
			else {
				m_p = oldp - 2;
			}
			m_q = m_W[m_p];
		}
		if (needh2) {
			m_h2 = m_n;
			if (m_L[m_h2] == m_L[m_h1] - 1 and m_h1 == m_r) {
				m_c = m_n + 1;
			}
			else {
				m_c = inf;
			}
		}
	}

	m_is_last = (m_q == 0);
}

ftree all_ulab_free_trees::get_tree() const {
	if (m_n <= 1) { return ftree(m_n); }
	if (m_n == 2) {
		ftree t(2);
		t.add_edge(0,1);
		return t;
	}
	return level_sequence_to_tree(m_L, m_n);
}

