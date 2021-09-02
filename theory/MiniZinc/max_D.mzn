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
 *         Office S124, Omega building
 *         Jordi Girona St 1-3, Campus Nord UPC, 08034 Barcelona.   CATALONIA, SPAIN
 *         Webpage: https://cqllab.upc.edu/people/rferrericancho/
 * 
 *     Carlos Gómez Rodríguez (cgomezr@udc.es)
 *         
 * 
 *     Juan Luis Esteban (esteban@cs.upc.edu)
 *         LOGPROG: Logics and Programming Research Group
 *         Office 110, Omega building
 *         Jordi Girona St 1-3, Campus Nord UPC, 08034 Barcelona.   CATALONIA, SPAIN
 *         Webpage: https://www.cs.upc.edu/~esteban/
 * 
 *     Lluís Alemany Puig (lalemany@cs.upc.edu)
 *         LARCA (Laboratory for Relational Algorithmics, Complexity and Learning)
 *         CQL (Complexity and Quantitative Linguistics Lab)
 *         Jordi Girona St 1-3, Campus Nord UPC, 08034 Barcelona.   CATALONIA, SPAIN
 *         Webpage: https://cqllab.upc.edu/people/lalemany/
 *
 ********************************************************************/

include "alldifferent.mzn";
include "lex_lesseq.mzn";
include "lex_less.mzn";
include "inverse.mzn";

% -----------------------------------

int: n;
int: m;
set of int: VERTICES = 1..n;
set of int: POSITIONS = 1..n;
set of int: EDGES = 1..m;

% edge list
array[EDGES, 1..2] of VERTICES: tree;

% adjacency matrix
array[VERTICES,VERTICES] of 0..1: adj
  = array2d(
      VERTICES,VERTICES,
      [
        exists(e in EDGES) (
          tree[e,1] = u /\ tree[e,2] = v \/
          tree[e,1] = v /\ tree[e,2] = u
        )
        | u in VERTICES, v in VERTICES
      ]
   );

% degree of every vertex
array[VERTICES] of 0..n-1: degree
  = [ sum(v in VERTICES) (adj[u,v]) | u in VERTICES ];

% -----------------------------------
% decision variables

array[VERTICES] of var POSITIONS: arrangement;
array[POSITIONS] of var VERTICES: inv_arrangement;

% -----------------------------------------
% relate arrangement to inverse arrangement

constraint inverse(arrangement, inv_arrangement);

% -----------------------------------
% correct result

constraint alldifferent(arrangement);

% -----------------------------------
% value to maximise

var int: D;
constraint
  D = sum(e in EDGES) (
	let {
		int: u = tree[e,1];
		int: v = tree[e,2]
	}
	in
    abs(arrangement[u] - arrangement[v])
  );
solve maximize D;

% ----------------------
% minimum value of D

constraint D >= (n*(n - 1)) div 2;

% -----------------------
% break mirror symmetries

constraint lex_less(inv_arrangement, reverse(inv_arrangement));

% -------------------------------------------------------------------
% permuting leaves connected to the same parent hub does not change D

constraint
  forall (u in VERTICES) (
    let {
      array[int] of VERTICES: leaves =
        [ v | v in VERTICES where adj[u,v] == 1 /\ degree[v] == 1 ];
    }
    in
    forall (l in 1..length(leaves)-1) (
      arrangement[leaves[l]] < arrangement[leaves[l + 1]]
    )
  );

% ----------------------------------------------------------
% swapping leaves of different parents can't yield a lower D

constraint
  forall (u1 in VERTICES, u2 in VERTICES where u1 < u2) (
    let {
      array[int] of VERTICES: leaves_u1 =
        [ v | v in VERTICES where adj[u1,v] == 1 /\ degree[v] == 1 ];
      
      array[int] of VERTICES: leaves_u2 =
        [ v | v in VERTICES where adj[u2,v] == 1 /\ degree[v] == 1 ];
    }
    in
    forall (l1 in leaves_u1, l2 in leaves_u2) (
      let {
        var POSITIONS: pos_u1 = arrangement[u1];
        var POSITIONS: pos_l1 = arrangement[l1];
        var POSITIONS: pos_u2 = arrangement[u2];
        var POSITIONS: pos_l2 = arrangement[l2];
      }
      in
      abs(pos_u1 - pos_l1) + abs(pos_u2 - pos_l2)
      >=
      abs(pos_u1 - pos_l2) + abs(pos_u2 - pos_l1)
    )
  );

% -----------------------------------
output
  ["arrangement: "] ++ [ show_int(3,arrangement[v]) | v in VERTICES ] ++ ["\n"] ++
  ["inv_arrangement: "] ++ [ show_int(3,inv_arrangement[p]) | p in 1..n ] ++ ["\n"] ++
  ["value_D= \(D)\n" ];
  
