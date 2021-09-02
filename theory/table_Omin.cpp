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

// C includes
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// C++ includes
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <fstream>
#include <random>
#include <limits>
#include <vector>
#include <string>
#include <list>
using namespace std;

// graph-related includes
#include "graphs/ftree.hpp"
#include "graphs/all_ulab_free_trees.hpp"

// utils
#include "numeric/rational.hpp"

// algorithms for Dmin includes
#include "Dmin/common.hpp"

// definitions
typedef vector<uint32_t> linearrgmnt;

// output progress of program into a file
ofstream table_log;
// number of calls to MiniZinc (for statistics)
uint32_t n_calls_to_MiniZinc = 0;

// useful macros
#define empty_read(in,k) for(auto __i = static_cast<decltype(k)>(0); __i < k; ++__i) { int ___dummy; in >> ___dummy; }
#define square(x) ((x)*(x))
#define to_str2(x) ((x < 10 ? std::string("0") : string("")) + std::to_string(x))
#define default_MZN_exe "/path/to/minizinc/executable"

/* -------- */
// The variables to modify so that this executable works in your system
// are marked with a '**' in the comment above them.

// ** output directory (where to place the compiled model file .fzn, data file .dzn, and output file .res)
const string out_dir = "/tmp/";
// ** path to MiniZinc executable
const string exe_file = default_MZN_exe;
/* -------- */

// name of the .dzn file
string tree_file;
// name of the .fzn file
string max_fzn;
// name of output file .res
string max_res_file;
// execution command
string max_command;

// initialise the appropriate variables with this process id
// in order to make a correct execution command for MiniZinc
void make_minizinc_command(uint32_t n) {
	const string n_str = to_str2(n);

	tree_file = out_dir + "__Omin__tree__" + n_str + ".dzn";
	max_fzn = out_dir + "__Omin__model__" + n_str + ".fzn";
	max_res_file = out_dir + "__Omin__result__" + n_str + ".res";
	
	max_command =
		exe_file + " " \
			+ "--fzn " + max_fzn + " " \
			+ "--solver Chuffed" + " " \
			+ "-p 1" + " " \
			+ "-O1" + " " \
			+ "MiniZinc/max_D.mzn" + " " \
			+ tree_file + " > " \
			+ max_res_file;
}

// Computes the sum of the length of the edges in a linear arrangement.
uint32_t sum_length_edges(const ftree& t, const linearrgmnt& pi) {
	uint32_t l = 0;

	for (node u = 0; u < t.n_nodes(); ++u) {
		for (const node v : t.get_neighbours(u)) {
			
			// accumulate length of edge
			l += (u < v)*(pi[u] < pi[v] ? pi[v] - pi[u] : pi[u] - pi[v]);
		}
	}
	return l;
}

// Parse the MiniZinc output file and retrieve the result.
// The format of the output file depends on the model used.
uint32_t parse_minizinc_file(const ftree& t, const string& filename, const linearrgmnt& relab) {

	uint32_t d;
	const uint32_t n = t.n_nodes();
	linearrgmnt relab_arrmnt(n);
	
	{
	ifstream fin;
	fin.open(filename);
	string field;
	while (fin >> field) {
		if (field == "arrangement:") {
			// parse arrangement
			for (uint32_t i = 0; i < n; ++i) {
				fin >> relab_arrmnt[i];
				--relab_arrmnt[i];
			}
		}
		else if (field == "----------") { /* do nothing */ }
		else if (field == "inv_arrangement:") {
			empty_read(fin, n);
		}
		else if (field == "value_D=") {
			fin >> d;
		}
		else if (field == "=====UNSATISFIABLE=====") {
			cerr << "UNSATISFIABLE FOUND!" << endl;
			cerr << "For tree" << endl;
			cerr << t << endl;
			exit(1);
		}
	}
	fin.close();
	}

	linearrgmnt arrmnt(n);
	for (node u = 0; u < n; ++u) {
		// relab_arrmnt[lab(u)] = p <-> position of lab(u) is p
		const node lab_u = relab[u];
		arrmnt[u] = relab_arrmnt[lab_u];
	}
	
	table_log << "    Optimal arrangement for tree" << endl;
	table_log << "        ";
	for (uint32_t i = 0; i < n; ++i) { table_log << arrmnt[i] << " "; }
	table_log << endl;
	
	// ensure that the value calculated by MiniZinc is correct
	const uint32_t D = sum_length_edges(t, arrmnt);
	if (D != d) {
		table_log << "        Error! Value of D computed by MiniZinc is not the same" << endl;
		table_log << "            as the value computed by the library!" << endl;
		table_log << "            Library says:  D=" << D << endl;
		table_log << "            MiniZinc says: D=" << d << endl;

		table_log << "            Relabelling used:";
		for (uint32_t i = 0; i < n; ++i) { table_log << " " << relab[i]; }
		table_log << endl;

		table_log << "                    Arrangement (using relabelling):";
		for (uint32_t i = 0; i < n; ++i) { table_log << " " << relab_arrmnt[i]; }
		table_log << endl;

		table_log << "            Arrangement (without relabelling):";
		for (uint32_t i = 0; i < n; ++i) { table_log << " " << arrmnt[i]; }
		table_log << endl;

		table_log << "            For tree:" << endl;
		table_log << t << endl;
		cerr << "ERROR!" << endl;

		exit(1);
	}
	return d;
}

bool is_bistar_tree(const ftree& t) {
	uint32_t n_internal_verts = 0;
	for (node u = 0; u < t.n_nodes(); ++u) {
		n_internal_verts += (t.degree(u) >= 2);
	}
	return n_internal_verts <= 2;
}

bool is_k_quasistar_tree(const ftree& t) {
	// we can assume that n >= 12
	const uint32_t n = t.n_nodes();

	vector<uint32_t> deg_seq(n);
	for (node u = 0; u < n; ++u) {  deg_seq[u] = t.degree(u);  }
	std::sort(deg_seq.begin(), deg_seq.end());
	
	// one internal vertex must have degree larger than 2.
	// the next internal vertices must have degree exactly 2.
	return deg_seq[n - 1] > 2 and deg_seq[n - 2] == 2;
}

uint32_t compute_Dmin(const ftree& t) {
	list<pair<node,node>> list_edges;

	for (node u = 0; u < t.n_nodes(); ++u) {
		for (const node v : t.get_neighbours(u)) {
			if (u < v) {
				list_edges.push_back(make_pair(u + 1, v + 1));
			}
		}
	}
	
	return calculate_D_min_Shiloach(t.n_nodes(), list_edges);
}

linearrgmnt output_tree(const ftree& t, ostream& os) {
	// output basic information
	const uint32_t n = t.n_nodes();
	os << "n= " << n << ";" << endl;
	os << "m= " << n - 1 << ";" << endl;

	// label vertices uniformly at random
	linearrgmnt relab(t.n_nodes());
	{
	std::iota(relab.begin(), relab.end(), 0);
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(relab.begin(), relab.end(), g);
	}

	// output labelling of vertices
	table_log << "        Relabelling of the vertices:" << endl;
	for (size_t i = 0; i < relab.size(); ++i) {
		table_log << "            " << i << " -> " << relab[i] << endl;
	}
	table_log << "        D= " << sum_length_edges(t, relab) << endl;
	table_log << "            n choose 2= " << (n*(n - 1))/2 << endl;
	
	// output tree with its vertices relabelled
	os << "tree = [|";
	bool first_edge = true;
	for (node u = 0; u < n; ++u) {
		for (const node v : t.get_neighbours(u)) {
			if (u < v) {
				if (not first_edge) { os << "|"; }
				os << relab[u] + 1 << "," << relab[v] + 1;
				first_edge = false;
			}
		}
	}
	os << "|];" << endl;

	return relab;
}

uint32_t compute_DMax_k_quasistar(const ftree& t) {
	const uint32_t n = t.n_nodes();

	// find the hub
	node hub = 0;
	for (node u = 0; u < n; ++u) {
		hub = t.degree(u) > 2 ? u : hub;
	}

	// Determine parameter 'l' of tree.
	// By definition, l := |W|
	uint32_t l = 0;
	for (node u : t.get_neighbours(hub)) {
		l += t.degree(u) == 1;
	}

	// Determine parameter 'k' of tree.
	// Recall that:  n = 2k + l + 1
	const uint32_t k = (n - l - 1)/2;

	return ((k + l)*(5*k + l + 1))/2;
}

uint32_t compute_DMax(const ftree& t) {
	table_log << "Compute DMax" << endl;
	table_log << "    For tree:" << endl;
	table_log << t << endl;
	
	const uint32_t n = t.n_nodes();

	if (n >= 12 and is_k_quasistar_tree(t)) {
		// the tree is a k-quasistar
		table_log << "    Tree is k-quasistar -> use formula" << endl;
		const uint32_t D = compute_DMax_k_quasistar(t);
		table_log << "    D= " << D << endl;
		return D;
	}

	++n_calls_to_MiniZinc;
	
	// make a file with MiniZinc's format
	ofstream _ttf; // this_tree_file
	_ttf.open(tree_file);
	const linearrgmnt relab = output_tree(t, _ttf);	
	_ttf.close();
	
	// execute MiniZinc using threads
	table_log << "    launching MiniZinc to compute DMax" << endl;
	const int child_DMax = fork();
	if (child_DMax == 0) {
		int r = system(max_command.c_str());
		if (r == -1) {
			cerr << "Error:" << endl;
			cerr << "    Neither child could not be created or status" << endl;
			cerr << "    could not be retrieved" << endl;
			exit(1);
		}
		exit(0);
	}
	
	// wait until MiniZinc has finished
	int max_wstatus;
	const int child_DMax_ret = waitpid(child_DMax, &max_wstatus, 0);
	if (not WIFEXITED(max_wstatus)) {
		cerr << "Error: child process '" << child_DMax << "' did not terminate correctly." << endl;
		exit(1);
	}
	if (child_DMax_ret == -1) {
		// this should not happen
		cerr << "Error: error in call to waitpid for process '" << child_DMax << "'." << endl;
		cerr << "    Return value of 'waitpid' is -1" << endl;
		exit(1);
	}
	
	// read the output files, compute D and so on....
	table_log << "    processing result for DMax" << endl;
	
	return parse_minizinc_file(t, max_res_file, relab);
}

// compute the minimum Omega_min among all bistar trees of n vertices.
void Omin_of_all_bistar
(
	const uint32_t n,
	vector<pair<ftree,uint64_t>>& tree_Omin,
	vector<pair<uint32_t, uint32_t>>& Omin_D_values,
	rational& Omin
)
{
	const rational Drla(n*n - 1, 3);
	Omin = rational(numeric_limits<uint32_t>::max(), 1);

	// generator
	all_ulab_free_trees TreeGen(n);
	int i = 0;
	
	// degree sequence
	vector<uint32_t> deg_seq(n);

	while (TreeGen.has_next()) {
		TreeGen.next();
		const ftree T = TreeGen.get_tree();
		++i;
		
		// 1. Retrieve degree sequence and sort it.
		// 2. Count the amount of internal vertices
		uint32_t num_internal_verts = 0;
		for (node u = 0; u < n; ++u) {
			deg_seq[u] = T.degree(u);
			num_internal_verts += (deg_seq[u] >= 2);
		}
		std::sort(deg_seq.begin(), deg_seq.end());

		// if there are more than 2 internal vertices then
		// the tree is not a bistar tree
		if (num_internal_verts > 2) { continue; }
	
		// largest degree
		const uint32_t k1 = deg_seq[n - 1];
	
		// calcuate Dmin and DMax for bistar trees using the formulae
		const uint32_t Dmin = (square(k1 + 1))/4 + (square(n - k1 + 1))/4 - 1;
		const uint32_t DMax = k1*(n - k1) + (n*(n - 3))/2 + 1;
		const rational Omega_t = (Drla - DMax)/(Drla - Dmin);
		
		// update Omega min
		if (Omega_t < Omin) {
			Omin = Omega_t;
			
			tree_Omin.clear();
			tree_Omin.push_back(make_pair(T,i));
			
			Omin_D_values.clear();
			Omin_D_values.push_back(make_pair(Dmin, DMax));
		}
		else if (Omin == Omega_t) {
			tree_Omin.push_back(make_pair(T,i));
			Omin_D_values.push_back(make_pair(Dmin, DMax));
		}
	}
	
	// output progress
	table_log << "the minimising bistar trees are:" << endl;
	for (size_t s = 0; s < tree_Omin.size(); ++s) {
		table_log << s << ")" << endl;
		table_log << "Dmin= " << Omin_D_values[s].first << endl;
		table_log << "DMax= " << Omin_D_values[s].second << endl;
		table_log << tree_Omin[s].first << endl;
	}
}

int main(int argc, char *argv[]) {
	if (exe_file == default_MZN_exe) {
		cerr << "Error:" << endl;
		cerr << "    You did not set the path to MiniZinc's executable file." << endl;
		cerr << "    Change the value of the variable 'exe_file'." << endl;
		return 1;
	}
	
	const uint64_t num_trees[31] = {
		1,1,1,1,2,3,6,11,23,47,106,235,551,1301,3159,7741,19320,48629,123867,317955,
		823065,2144505,5623756,14828074,39299897,104636890,279793450,751065460,
		2023443032,5469566585,14830871802
	};
	if (argc != 2) {
		cerr << "Error: wrong number of parameters." << endl;
		cerr << "    ./table_Omin n" << endl;
		cerr << "" << endl;
		cerr << "Find the trees of 'n' vertices minimises Omega_min" << endl;
		return 1;
	}

	// number of vertices
	const uint32_t n = atoi(argv[1]);

	make_minizinc_command(n);
	
	// should we output the header for the table file?
	bool output_header;
	{
	const filesystem::path p = "data/table_file.tsv";
	output_header = not std::filesystem::exists(p);
	}
	
	// file containing the trees that yield Omin
	ofstream trees_file;
	// file with tabulator-separated data
	ofstream table_file;

	table_log.open("data/log-" + to_str2(n));
	table_file.open("data/table_file.tsv", std::ios_base::app);
	trees_file.open("data/tree_file.tsv", std::ios_base::app);
	
	if (output_header) {
		// if 'table_file' is empty then output the header
		table_file
			<< "n" << "\t"
			<< "num_trees" << "\t"
			<< "Omega_min" << "\t"
			<< "trees_min" << "\t"
			<< "D_min" << "\t"
			<< "D_max"
			<< endl;
	}
	
	// generator of all unlabelled free trees
	all_ulab_free_trees TreeGen;
	
	// DMax for a balanced bistar tree
	const uint32_t DMax_bbistar = (3*(n - 1)*(n - 1) + 1 - n%2)/4;
	// expected value of D over all n! arrangements
	const rational Drla(n*n - 1, 3);
	// minimum value of Omega
	rational Omin(numeric_limits<uint32_t>::max(), 1);
	// values of the magnitudes used to calculate Omin (<Dmin, DMax>)
	vector<pair<uint32_t, uint32_t>> Omin_D_values;
	// the trees (and their indices) that yield Omin
	vector<pair<ftree,uint64_t>> Omin_trees;
	
	// Find the bistar tree that minimises Omega. Retrieve the
	// values of Dmin and DMax that yielded such value.
	Omin_of_all_bistar(n, Omin_trees, Omin_D_values, Omin);
	
	// initialise tree generator
	TreeGen.init(n);
	// index of the generated tree
	uint64_t tree_idx = 0;
	
	while (TreeGen.has_next()) {
		TreeGen.next();
		const ftree T = TreeGen.get_tree();

		// output progress
		++tree_idx;
		const double percentage = (100.0*tree_idx)/num_trees[n];
		if (
			((percentage <= 90.0) and (tree_idx%5000 == 0))
			or
			(90.0 < percentage and percentage <= 99.0 and tree_idx%10 == 0)
			or
			99.0 < percentage
		)
		{
			cout << "    T " << tree_idx
				 << " (n= " << n << ") - "
				 << (100.0*tree_idx)/num_trees[n] << "% "
				 << "(MZ= " << n_calls_to_MiniZinc << ")"
				 << endl;
		}

		// ignore bistar trees
		if (is_bistar_tree(T)) { continue; }
		
		// compute dummy omega
		const uint32_t Dmin = compute_Dmin(T);
		const rational dummy_Omega = (Drla - DMax_bbistar)/(Drla - Dmin);
		
		if (dummy_Omega <= Omin) {
			// compute actual Omega_min
			const uint32_t DMax = compute_DMax(T);
			const rational Omega_min = (Drla - DMax)/(Drla - Dmin);
			
			// update info about omega min
			if (Omin == Omega_min) {
				Omin_trees.push_back(make_pair(T,tree_idx));
				Omin_D_values.push_back(make_pair(Dmin, DMax));
			}
			else if (Omega_min < Omin) {
				Omin = Omega_min;
				
				Omin_trees.clear();
				Omin_trees.push_back(make_pair(T,tree_idx));
				
				Omin_D_values.clear();
				Omin_D_values.push_back(make_pair(Dmin, DMax));
			}
		}
	}
	
	// output results
	for (size_t s = 0; s < Omin_trees.size(); ++s) {
		table_file
			 << n << "\t"
			 << tree_idx << "\t"
			 << Omin.to_double() << "\t"
			 << Omin_trees.size() << "\t"
			 << Omin_D_values[s].first << "\t"
			 << Omin_D_values[s].second
			 << endl;
	}
	
	// output all trees that minimise Omega_min
	trees_file << "******************************************" << endl;
	trees_file << "n= " << n << endl;
	for (const auto& p : Omin_trees) {
		trees_file << p.first << endl;
	}
	trees_file << "******************************************" << endl;
	
	table_log.close();
	table_file.close();
	trees_file.close();
}

