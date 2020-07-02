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

#pragma once

// C++ includes
#include <cinttypes>
#include <ostream>
#include <utility>
#include <vector>

typedef uint32_t node;
typedef std::pair<node,node> edge;
typedef std::vector<node> neighbourhood;

/**
 * @brief Class for ftrees.
 *
 * A usual way of initialising classes inheriting from this one is to
 * use one of the @ref init methods available. While this class internal
 * memory can be initialised, it can also be cleared using method
 * @ref clear. Each class reimplements this method to carry this task
 * appropriately.
 */
class ftree {
	public:
		/// Default constructor.
		ftree() = default;
		/**
		 * @brief Constructor with number of nodes.
		 * @param n Number of nodes.
		 */
		ftree(uint32_t n);
		/// Default destructor.
		~ftree() = default;

		/* OPERATORS */

		/**
		 * @brief Standard output operator for free trees.
		 *
		 * Usable by @ref ugraph, @ref ftree.
		 * @param os ostream C++ object.
		 * @param t Input tree.
		 * @returns Returns the output stream.
		 */
		friend inline
		std::ostream& operator<< (std::ostream& os, const ftree& t)
		{
			const auto n = t.n_nodes();
			for (node u = 0; u < n; ++u) {
				os << u << ":";
				for (auto v : t.get_neighbours(u)) {
					os << " " << v;
				}
				os << (u < n - 1 ? "\n" : "");
			}
			return os;
		}

		/* MODIFIERS */

		/**
		 * @brief Allocates the necessary memory for this class.
		 *
		 * See @ref _init for details.
		 * @param n Number of nodes.
		 */
		void init(uint32_t n);
		/**
		 * @brief Frees the memory occupied by this ftree.
		 *
		 * See @ref _clear for details.
		 * @post The ftree is normalised. The number of edges is 0.
		 */
		void clear();
		
		/**
		 * @brief Adds an edge to the graph.
		 * @param s Valid node index: \f$0 \le s < n\f$.
		 * @param t Valid node index: \f$0 \le t < n\f$.
		 * @pre \f$u \neq v\f$. The edge \f$\{s,t\}\f$ is not part of the graph.
		 */
		ftree& add_edge(node s, node t);

		/**
		 * @brief Adds a list of edges to the graph.
		 *
		 * This operation is faster than calling @ref add_edge(node,node,bool)
		 * since the edges are added in bulk.
		 * @param edges The edges to be added.
		 * @pre All the edges in @e edges must meet the precondition of method
		 * @ref add_edge(node,node,bool).
		 */
		ftree& add_edges(const std::vector<edge>& edges);

		/* GETTERS */

		/**
		 * @brief Returns the neighbourhood of node @e u.
		 * @param u Node.
		 * @return In undirected ftrees, returns the list of nodes adjacent to
		 * node @e u. In a directed ftree, returns the outgoing nodes.
		 */
		const neighbourhood& get_neighbours(node u) const;

		/**
		 * @brief Returns the number of neighbours of @e u.
		 * @param u Node to be queried.
		 * @return In undirected ftrees, returns the number of neighbours. In
		 * a directed ftree, returns the number of outgoing edges.
		 */
		uint32_t degree(node u) const;

		/// Returns the number of ndoes.
		uint32_t n_nodes() const;

		/// Returns the number of edges.
		uint32_t n_edges() const;

	protected:
		/// Data structure that implements the ftree.
		std::vector<neighbourhood> m_adjacency_list;
		/// Amount of edges of this ftree.
		uint32_t m_num_edges = 0;

};
