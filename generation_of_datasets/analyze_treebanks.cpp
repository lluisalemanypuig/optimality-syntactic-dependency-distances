/*******************************************************************************
 * Optimality Syntactic Dependency Distances
 *  
 * Copyright (C) 2020 Carlos Gómez Rodríguez
 *
 * This file is part of Optimality Syntactic Dependency Distances.
 *  
 *  Optimality Syntactic Dependency Distances is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU Affero General Public License
 *  as published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  Optimality Syntactic Dependency Distances is distributed in the hope that
 *  it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *   
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Optimality Syntactic Dependency Distances.  
 *  If not, see <http://www.gnu.org/licenses/>.
 *  
 *  Contact:
 *       
 *      Carlos Gómez Rodríguez (carlos.gomez@udc.es)
 *          LyS Research Group (Grupo de investigación Lengua y Sociedad de la Información)
 *          FASTPARSE Lab (Fast Natural Language Processing for Large-Scale NLP).
 *          Office 0.19 Facultade de Informática     Elviña, 15071 A Coruña.    GALICIA, SPAIN
 *          Webpage: http://www.grupolys.org/~cgomezr/
 *          
 *******************************************************************************/#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <string.h>
#include <limits>
#include <algorithm>
#include <filesystem>
#include <sys/types.h>
#include <dirent.h>
#include "probability_of_crossing_knowing_edge_lengths.hpp"
#include "fast_prediction_of_crossings_at_constant_sum_of_lengths.hpp"
#include "undirected_graph.hpp"

using namespace std;

const double numerical_error = 1e-10;

void calculate(const string &label, const undirected_graph &g);

//metrics on crossings
void compute_metrics ( std::vector<undirected_graph> graphs );
void compute_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix = "" , bool verbose = true , bool c_d = false );
void compute_by_length_groups ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup , bool verbose = true , bool c_d = false , const std::string& linePrefix = "" );

//main template methods to call the other methods
void process_heads_file ( const char * heads_file_name );
void process_heads_file_incrementally ( const char * heads_file_name , const char * output_file_name );

//utilities
double overflow_safe_average ( std::vector<double> myvector );
std::vector<undirected_graph> length_filter ( std::vector<undirected_graph> input , int min_length , int max_length );
std::string get_iso_code ( std::string language_or_filename );
int ends_with(const char *str, const char *suffix);
std::vector<std::string> get_files ( const char * directory , const char * extension );

//omega (optimality) metrics
void compute_optimality_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix = "" , bool verbose = true );
void compute_grammar_optimality_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix , bool verbose = true );
void compute_optimality_by_length_groups ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup , bool verbose = true , const std::string& linePrefix = ""  );
void compute_grammar_optimality_by_lengths ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup  , bool verbose = true , const std::string& linePrefix =""  );
void output_individual_omegas_to_file ( std::vector<undirected_graph> graphs , char * filename );
void output_individual_grammar_optimality_metrics_to_file( ofstream& output_stream , undirected_graph& mygraph );

//anti-dlm metrics
void compute_antidlm_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix = "" , bool verbose = true );
void compute_antidlm_by_length_groups ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup , bool verbose = true , const std::string& linePrefix = ""  );
void output_individual_antidlm_metrics_to_file ( std::vector<undirected_graph> graphs , const char * filename , const std::string& linePrefix = "" , bool append = false );



  //default values if no command-line arguments are supplied:

  //directory to read input files
  std::string directory = std::string("D:\\Treebanks\\ud-testsubset-v2.6-merged-bylanguage-shuffled\\");
  
  //extension of the input files that should be read (.heads, .heads2 or .heads3)
  std::string extension = std::string(".heads2");   
  
  //path to file to output, only used by some functions (like antidlm tables)
  std::string outfile = std::string("test-antidlm-ud26.txt");

  //command to execute
  std::string command = "individual_table";
  


int main( int argc , char * argv[] )
{
  
     //process arguments
     std::vector<std::string> args(argv, argv+argc);
     if ( argc > 1 )
     {
        command = argv[1];
     }
     if ( argc > 2 )
     {
         directory = argv[2];
     }
     if ( argc > 3 )
     {
         extension = argv[3];
     }
     if ( argc > 4 )
     {
         outfile = argv[4];
     }

   
  //int sz = sizeof(files) / sizeof(const char *);

    std::vector<std::string> filenames = get_files(directory.c_str(),extension.c_str());

    for ( std::vector<std::string> :: iterator it = filenames.begin(); it != filenames.end(); ++it )
    {
        process_heads_file((*it).c_str());
    }


}

//replace extension heads2 to omegas
char * output_file_name ( const char * input_file_name )
{
	char * copy = strdup(input_file_name);
	int len = strlen(copy);
	assert(len>6);
	assert(copy[len-7]=='.');
	strcpy(&copy[len-6],"omegas");
	return copy;
}

void process_heads_file_incrementally ( const char * heads_file_name , const char * output_file_name ) //for very large files where we can't put all trees into a vector
{
  //open the heads file:
  std::ifstream heads_stream(heads_file_name);
  if ( !heads_stream )
  std::cerr << "Problem with " << heads_file_name << endl;
  std::string line;
  std::vector<int> current_tree;
  
  //open the output file:
  //this should set max precision
  ofstream output_stream;
  output_stream.open(output_file_name);
  //17 (digits10+2) is "the minimum number of digits needed to survive a round-trip to string form and back and get the same double in the end."
  output_stream << setprecision(numeric_limits<double>::digits10 + 2);
  
  //read one tree per line and build undirected_graph
  while ( getline( heads_stream , line ) ) 
  {
    std::istringstream is( line );
    current_tree = 
    std::vector<int>( std::istream_iterator<int>(is),
                        std::istream_iterator<int>() );
    undirected_graph mygraph(current_tree.size());
    //cout << "Creating graph with size " << current_tree.size() << endl; //debug info
    for ( unsigned int i = 0 ; i < current_tree.size() ; i++ ) 
    {
      int dependent = i;
      int head = current_tree[i] - 1;
      if ( head >= 0 ) 
      { //if head is -1 (0 was in the file), node is the sentence root
        int min = (dependent<head)?dependent:head;
        int max = (dependent>head)?dependent:head;
        mygraph.add_edge(min,max);
        //cout << "Adding edge: " << min << " " << max << endl; //debug info
      }
    }
    //assert: mygraph is now an undirected_graph from the file
    
    //here we work with the graph
    output_individual_grammar_optimality_metrics_to_file(output_stream,mygraph);
  }
  
  output_stream.close();
}

void process_heads_file ( const char * heads_file_name )
{

  //cout << "Processing file: " << heads_file_name << endl;
  
  //uncomment this to show heads file names (now unneeded, done thanks to filePrefix parameter passed to files).
  //cout << heads_file_name << endl; //" ";

  //open the heads file:
  std::ifstream heads_stream(heads_file_name);
  if ( !heads_stream )
    std::cerr << "Problem with " << heads_file_name << endl;
  std::string line;
  std::vector< std::vector<int> > my_heads;

  //read one tree per line into the vector of vectors:
  while ( getline( heads_stream , line ) ) 
  {
    std::istringstream is( line );
    my_heads.push_back(
      std::vector<int>( std::istream_iterator<int>(is),
                        std::istream_iterator<int>() ) );
  }

  std::vector< undirected_graph > graphs;

  for( std::vector< std::vector<int> >::iterator it = my_heads.begin() ; it != my_heads.end() ; ++it ) {
    std::vector<int> current_tree = *it;

    undirected_graph mygraph(current_tree.size());
    //cout << "Creating graph with size " << current_tree.size() << endl; //debug info
    for ( unsigned int i = 0 ; i < current_tree.size() ; i++ ) 
    {
      int dependent = i;
      int head = current_tree[i] - 1;
      if ( head >= 0 ) 
      { //if head is -1 (0 was in the file), node is the sentence root
        int min = (dependent<head)?dependent:head;
        int max = (dependent>head)?dependent:head;
        mygraph.add_edge(min,max);
        //cout << "Adding edge: " << min << " " << max << endl; //debug info
      }
    }
    graphs.push_back( mygraph );
    //calculate ( "Test "  , mygraph );
  }
  
  //PROCESS THE VECTOR OF GRAPHS HERE

  //compute_detailed_metrics ( /*length_filter(graphs,1,12)*/ graphs , false , false );
  //compute_by_length_groups(graphs,1,false,false); //by length groups - final table in coli squib
  //compute_optimality_detailed_metrics ( /*length_filter(graphs,1,8)*/ graphs , false );
  
  //this was the last mode (for omeagas), changed 2016-11-14 to recompute general metrics for small.tex:
  //compute_optimality_by_length_groups(graphs,1,false);
  //general metrics for small.tex:
  //compute_detailed_metrics ( /*length_filter(graphs,1,12)*/ graphs , graph_file_name, false , false );
  //metrics by length for small.tex:
  //compute_by_length_groups ( graphs , 1 , false , false );
  
  //output_individual_omegas_to_file( graphs , output_file_name(heads_file_name) );

  //omegas again, for ud:
  //compute_optimality_detailed_metrics(graphs,heads_file_name,false);
  //compute_optimality_by_length_groups(graphs,1,false,string(heads_file_name)+string("\n"));
  //output_individual_omegas_to_file( graphs , output_file_name(heads_file_name) );
  //compute_antidlm_by_length_groups(length_filter(graphs,3,4),1,false,heads_file_name);
  //output_individual_antidlm_metrics_to_file( length_filter(graphs,3,4) /*graphs*/ , "antidlm-all-languages-34-fixed.txt" /*"antidlm-all-languages-all-fixed.txt"*/ , get_iso_code(heads_file_name) , true );
  //output_individual_antidlm_metrics_to_file( graphs , "antidlm-prague-all.txt" /*"antidlm-all-languages-all-fixed.txt"*/ , get_iso_code(heads_file_name) , true );
  //output_individual_antidlm_metrics_to_file( graphs , "antidlm-all-languages-all-shuffled.txt" /*"antidlm-all-languages-all-fixed.txt"*/ , get_iso_code(heads_file_name) , true );
  //compute_grammar_optimality_by_lengths ( graphs , 1  , false , string(heads_file_name).substr(string(heads_file_name).find_last_of("/\\")+1,3)  );
  //output_individual_antidlm_metrics_to_file( graphs , outfile.c_str() /*"antidlm-ud25-all-shuffled.txt"*/ /*"antidlm-sud25-all.txt"*/ /*"antidlm-proiel-all.txt"*/ /*"antidlm-pud-all.txt"*/ /*"antidlm-ud25-all2.txt"*/ /*"antidlm-ud24-all.txt"*/ /*"antidlm-all-languages-all-fixed.txt"*/ , get_iso_code(heads_file_name) , true );
  if ( command == "global_optimality_table") //not currently used much
    compute_optimality_detailed_metrics( graphs , heads_file_name , false );
  else if ( command == "optimality_by_length_groups") //not currently used much
    compute_optimality_by_length_groups(graphs,1,false,string(heads_file_name)+string("\n"));
  else if ( command == "individual_table") //antidlm
    output_individual_antidlm_metrics_to_file( graphs , outfile.c_str() , get_iso_code(heads_file_name) , true );
  else if ( command == "omega_files" ) //.omegas files for Monte Carlo
    output_individual_omegas_to_file( graphs , output_file_name(heads_file_name) );
  else
    cout << "Unknown command " << command << endl;

}


void output_individual_grammar_optimality_metrics_to_file( ofstream& output_stream , undirected_graph& mygraph )
{
    //bool invalid_length_for_omega = false;
    int n = mygraph.vertices();
    //if ( n < 3 ) invalid_length_for_omega = true;
    double length = double(n);

    double D = mygraph.edge_length_sum();
    double Dmin = mygraph.minimum_edge_length_sum_of_a_tree();
    double Drandom = (n-1)*(n+1)/double(3);
    double Omega;

    if ( n >= 3 )
    {
        Omega = (Drandom- D)/(Drandom - Dmin);
        assert(-8 <= Omega);
        assert(Omega <= 1);
    }
    
    double K2 = mygraph.sum_of_squared_degrees();
    double crossings = double(mygraph.crossings());
    output_stream << length << " " << crossings << " " << D << " " << K2 << " " << Omega << endl;
}

/**
 * @brief Calculates optimality metrics (omega, etc.) of each of the graphs in the graph vector
 * and outputs them to the file (metrics of one graph per line).
 * @param graphs
 * @param filename
 */
void output_individual_omegas_to_file ( std::vector<undirected_graph> graphs , char * filename )
{
	cout << "Outputting to file: " << filename << endl;
	
	std::size_t sz = graphs.size();

	//vectors of metrics for each tree. Aren't really needed here (as we can just output in each iterations without storing them)
	//but who knows.
	std::vector<double> length = std::vector<double>(sz,0.0);
	std::vector<double> D = std::vector<double>(sz,0.0);
	std::vector<double> D_min = std::vector<double>(sz,0.0);
	std::vector<double> D_random = std::vector<double>(sz,0.0);
	std::vector<double> Omega = std::vector<double>(sz,0.0);
  
  	//this should set max precision
	ofstream output_stream;
	output_stream.open(filename);
	
	//apparently 17 (digits10+2) is "the minimum number of digits needed to survive a round-trip to string form and back and get the same double in the end."
	output_stream << setprecision(numeric_limits<double>::digits10 + 2);
  
	for ( std::size_t i = 0 ; i < sz ; i++ ) 
	{
		undirected_graph graph = graphs[i];

		//process graph
		int n = graph.vertices();
		length[i] = double(n);

		D[i] = graph.edge_length_sum();
		D_min[i] = graph.minimum_edge_length_sum_of_a_tree();
		D_random[i] = (n-1)*(n+1)/double(3);
		Omega[i] = (D_random[i] - D[i])/(D_random[i] - D_min[i]);

		output_stream << length[i] << " " << D[i] << " " << D_min[i] << " " << D_random[i] << " " << Omega[i] << endl;

		assert(-8 <= Omega[i]);
		assert(Omega[i] <= 1);
	}
	
	output_stream.close();
	
}


/**
 * @brief Calculates anti-dlm related metrics of each of the graphs in the graph vector
 * and outputs them to the file (metrics of one graph per line).
 * @param graphs
 * @param filename
 */
void output_individual_antidlm_metrics_to_file ( std::vector<undirected_graph> graphs , const char * filename , const std::string& linePrefix , bool append )
{
    
    static bool printed_header = false;
    
	cout << "Outputting to file: " << filename << endl;
	
	std::size_t sz = graphs.size();

	//vectors of metrics for each tree. Aren't really needed here (as we can just output in each iterations without storing them)
	//but who knows.
	std::vector<double> length = std::vector<double>(sz,0.0);
    std::vector<double> K2 = std::vector<double>(sz,0.0);
	std::vector<double> D = std::vector<double>(sz,0.0);
	std::vector<double> D_min = std::vector<double>(sz,0.0);
	
  
  	//this should set max precision
	ofstream output_stream;
    if ( append )
        output_stream.open(filename, std::ofstream::out | std::ofstream::app );
    else
        output_stream.open(filename);
    
    //apparently 17 (digits10+2) is "the minimum number of digits needed to survive a round-trip to string form and back and get the same double in the end."
    output_stream << setprecision(numeric_limits<double>::digits10 + 2);
  
    //this is done only in the first invocation
    if ( !printed_header )
    {
        if ( !linePrefix.empty() )
            output_stream << "language" << " ";
        output_stream << "n" << " " << "K2" << " " << "D" << " " << "D_min" << " " << endl;
        printed_header = true;
    }
  
	for ( std::size_t i = 0 ; i < sz ; i++ ) 
	{
		undirected_graph graph = graphs[i];

		//process graph
		int n = graph.vertices();
		length[i] = double(n);

        K2[i] = graph.sum_of_squared_degrees();
		D[i] = graph.edge_length_sum();
		D_min[i] = graph.minimum_edge_length_sum_of_a_tree();

        if ( !linePrefix.empty() )
            output_stream << linePrefix << " ";
		output_stream << length[i] << " " << K2[i] << " " << D[i] << " " << D_min[i] << " "  << endl;
	}
	
	output_stream.close();
	
}


/**
 * Computes metrics dividing the graphs into groups by their lengths.
 */
void compute_by_length_groups ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup  , bool verbose /*= true*/ , bool c_d /*=false*/ , const std::string& linePrefix /*=""*/ )
{
  int minLength = 4;
  while ( minLength < 600 /*300*/ )
  {
    int maxLength = minLength;
    std::vector< undirected_graph > current_group;
    while ( (current_group = length_filter(graphs,minLength,maxLength)).size() < minAmountPerGroup && maxLength < 600 /*300*/ ) maxLength++;
    if ( !linePrefix.empty() && current_group.size() > 0 ) cout << linePrefix << " ";
    if ( verbose || current_group.size() > 0 )
      cout << minLength << " " << maxLength << " " << current_group.size() << " ";
    if ( current_group.size() > 0 )
      compute_detailed_metrics ( current_group , "" , verbose , c_d );
    minLength = maxLength + 1;
  }
}

/**
 * Computes omega metrics dividing the graphs into groups by their lengths.
 */
void compute_optimality_by_length_groups ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup  , bool verbose /*= true*/ , const std::string& linePrefix /*=""*/  )
{
  cout << linePrefix; //in this case, once per language (it's not really a line prefix but a language prefix, could maybe change parameter name).
  int minLength = 3;
  //while ( minLength < 300 )
  while ( minLength < 600 )
  {
    int maxLength = minLength;
    std::vector< undirected_graph > current_group;
    while ( (current_group = length_filter(graphs,minLength,maxLength)).size() < minAmountPerGroup && maxLength < 600 /*300*/ ) maxLength++;
    //cout << linePrefix;
    //if ( !linePrefix.empty() && current_group.size() > 0 ) cout << linePrefix << " ";
    if ( verbose || current_group.size() > 0 )
    cout << minLength << " " << maxLength << " " << current_group.size() << " ";
    if ( current_group.size() > 0 )
      compute_optimality_detailed_metrics ( current_group , "" , verbose );
    minLength = maxLength + 1;
  }
}

/**
 * Computes C, D, K2, omega metrics for "classes of grammars" paper, dividing the graphs into groups by their lengths.
 */
void compute_grammar_optimality_by_lengths ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup  , bool verbose /*= true*/ , const std::string& linePrefix /*=""*/  )
{
  for ( int length = 2 ; length < 30 ; length++ )  //this analysis is done on artificial trees of quite limited length
  {
    std::vector< undirected_graph > current_group;
    //cout << "len " << length << "\n";
    current_group = length_filter(graphs,length,length);
    if  ( current_group.size() >= minAmountPerGroup )
    {
        cout << length << " ";
        if ( !linePrefix.empty() ) cout << linePrefix << " ";
        cout << current_group.size() << " ";
        compute_grammar_optimality_detailed_metrics ( current_group , "" , verbose );
    }
  }
}

/**
 * Computes antiDLM metrics dividing the graphs into groups by their lengths.
 */
void compute_antidlm_by_length_groups ( std::vector< undirected_graph > graphs , std::size_t minAmountPerGroup  , bool verbose /*= true*/ , const std::string& linePrefix /*=""*/ )
{
  int minLength = 3;
  //while ( minLength < 300 )
  while ( minLength < 600 )
  {
    int maxLength = minLength;
    std::vector< undirected_graph > current_group;
    while ( (current_group = length_filter(graphs,minLength,maxLength)).size() < minAmountPerGroup && maxLength < 600 /*300*/ ) maxLength++;
    if ( !linePrefix.empty() && current_group.size() > 0 ) cout << linePrefix << " ";
    if ( verbose || current_group.size() > 0 )
    cout << minLength << " " << maxLength << " " << current_group.size() << " ";
    if ( current_group.size() > 0 )
      compute_antidlm_detailed_metrics ( current_group , "" , verbose );
    minLength = maxLength + 1;
  }
}

void compute_metrics ( std::vector<undirected_graph> graphs )
{

  /*
  int sum_n = 0;
  double sum_degree_2nd_moment = 0.0;
  //int sum_potential_crossings = 0;
  double sum_expected_crossings2 = 0.0;
  double sum_crossings_max = 0.0;
  double sum_expected_crossings0 = 0.0;
  int sum_crossings = 0;
  double sum_rel_crossings = 0.0;
  double sum_predicted_rel_crossings0 = 0.0;
  double sum_predicted_rel_crossings2 = 0.0;
  double sum_abs_delta0 = 0.0;
  double sum_abs_delta2 = 0.0;
   */

  double d_num_graphs = double(graphs.size());

  double avg_n = 0.0;
  double avg_degree_2nd_moment = 0.0;
  double avg_expected_crossings2 = 0.0;
  double avg_crossings_max = 0.0;
  double avg_expected_crossings0 = 0.0;
  double avg_crossings = 0.0;
  double avg_rel_crossings = 0.0;
  double avg_predicted_rel_crossings0 = 0.0;
  double avg_predicted_rel_crossings2 = 0.0;
  double avg_abs_delta0 = 0.0;
  double avg_abs_delta2 = 0.0;
  double avg_delta0 = 0.0;
  double avg_delta2 = 0.0;

  for ( std::size_t i = 0 ; i < graphs.size() ; i++ ) 
  {

    undirected_graph graph = graphs[i];

    //process graph: current_graph
    int n = graph.vertices();
    probability_of_crossing_knowing_edge_lengths p(n);
    double degree_2nd_moment = graph.degree_2nd_moment();
    int potential_crossings;
    double expected_crossings2;

    double crossings_max = n*(n - 1 - degree_2nd_moment)/double(2);
    assert(0 <= crossings_max);
    assert(crossings_max <= (n-1)*(n-2)/2);
    double expected_crossings0 = n*(n - 1 - degree_2nd_moment)/double(6);
    expected_crossings2 = graph.fully_predicted_crossings(p, potential_crossings);
    int crossings = graph.crossings();
    //cout << "Tree number: " << (i+1) << endl;

    double rel_crossings = crossings/crossings_max;
    double predicted_rel_crossings0 = expected_crossings0/crossings_max;
    double predicted_rel_crossings2 = expected_crossings2/crossings_max;
    double abs_delta0 = abs((expected_crossings0 - crossings)/crossings_max);
    double abs_delta2 = abs((expected_crossings2 - crossings)/crossings_max);
    double delta0 = (expected_crossings0 - crossings)/crossings_max;
    double delta2 = (expected_crossings2 - crossings)/crossings_max;

    /*
        //update sums
        sum_n += n;
        sum_degree_2nd_moment += degree_2nd_moment;
        //sum_potential_crossings += potential_crossings;
        sum_expected_crossings2 += expected_crossings2;
        sum_crossings_max += crossings_max;
        sum_expected_crossings0 += expected_crossings0;
        sum_crossings += crossings;
        sum_rel_crossings += rel_crossings;
        sum_predicted_rel_crossings0 += predicted_rel_crossings0;
        sum_predicted_rel_crossings2 += predicted_rel_crossings2;
        sum_abs_delta0 += abs_delta0;
        sum_abs_delta2 += abs_delta2;
       */
    //update averages
    avg_n += n / d_num_graphs;
    avg_degree_2nd_moment += degree_2nd_moment / d_num_graphs;
    //sum_potential_crossings += potential_crossings;
    avg_expected_crossings2 += expected_crossings2 / d_num_graphs;
    avg_crossings_max += crossings_max / d_num_graphs;
    avg_expected_crossings0 += expected_crossings0 / d_num_graphs;
    avg_crossings += crossings / d_num_graphs;
    avg_rel_crossings += rel_crossings / d_num_graphs;
    avg_predicted_rel_crossings0 += predicted_rel_crossings0 / d_num_graphs;
    avg_predicted_rel_crossings2 += predicted_rel_crossings2 / d_num_graphs;
    avg_abs_delta0 += abs_delta0 / d_num_graphs;
    avg_abs_delta2 += abs_delta2 / d_num_graphs;
    avg_delta0 += delta0 / d_num_graphs;
    avg_delta2 += delta2 / d_num_graphs;

    //cout << "Crossings " << crossings << "Crossings max " << crossings_max << " Rel crossings " << rel_crossings << "Avg rel crossings " << avg_rel_crossings;

    //cout << setprecision(2);

    /*
    // Information for Table 1
    cout << "   Number of vertices ($n$): " << n << endl;
    cout << "   Degree 2nd moment ($\\left< k^2 \\right>$): " << degree_2nd_moment << endl;
    cout << "   Maximum number of crossings ($C_{max}$): " << crossings_max << endl;
    cout << "   Crossings ($C_{true}$): " << crossings << endl;
    cout << "   Predicted crossings0 ($E_0[C]$): " << expected_crossings0 << endl;
    cout << "   Predicted crossings2 ($E_2[C]$): " << expected_crossings2 << endl;
    cout << "   Relative crossings ($\\bar{C}_{true}$): " << crossings/crossings_max << endl;
    cout << "   Predicted relative crossings0 ($E_0[\\bar{C}]$): " << expected_crossings0/crossings_max << endl;
    cout << "   Predicted relative crossings2 ($E_2[\\bar{C}]$): " << expected_crossings2/crossings_max << endl;
    // Information for Fig. 4 (applying Eq. 25)
    cout << "   Relative error of prediction0 ($\\Delta_0$): " << (expected_crossings0 - crossings)/crossings_max << endl;
    cout << "   Relative error of prediction2 ($\\Delta_2$): " << (expected_crossings2 - crossings)/crossings_max << endl;
     */
  }

  cout << "Average metrics:" << endl;


  /*
  double avg_n = double(sum_n) / d_num_graphs;
  double avg_degree_2nd_moment = sum_degree_2nd_moment / d_num_graphs;
  //double avg_potential_crossings = double(sum_potential_crossings) / d_num_graphs;
  double avg_expected_crossings2 = sum_expected_crossings2 / d_num_graphs;
  double avg_crossings_max = sum_crossings_max / d_num_graphs;
  double avg_expected_crossings0 = sum_expected_crossings0 / d_num_graphs;
  double avg_crossings = double(sum_crossings) / d_num_graphs;
  double avg_rel_crossings = sum_rel_crossings / d_num_graphs;
  double avg_predicted_rel_crossings0 = sum_predicted_rel_crossings0 / d_num_graphs;
  double avg_predicted_rel_crossings2 = sum_predicted_rel_crossings2 / d_num_graphs;
  double avg_abs_delta0 = sum_abs_delta0 / d_num_graphs;
  double avg_abs_delta2 = sum_abs_delta2 / d_num_graphs;
  */

  cout << setprecision(5);
  cout << "   Number of vertices ($n$): " << avg_n << endl;
  cout << "   Degree 2nd moment ($\\left< k^2 \\right>$): " << avg_degree_2nd_moment << endl;
  cout << "   Maximum number of crossings ($C_{max}$): " << avg_crossings_max << endl;
  cout << "   Crossings ($C_{true}$): " << avg_crossings << endl;
  cout << "   Predicted crossings0 ($E_0[C]$): " << avg_expected_crossings0 << endl;
  cout << "   Predicted crossings2 ($E_2[C]$): " << avg_expected_crossings2 << endl;
  cout << "   Relative crossings ($\\bar{C}_{true}$): " << avg_rel_crossings << endl;
  cout << "   Predicted relative crossings0 ($E_0[\\bar{C}]$): " << avg_predicted_rel_crossings0 << endl;
  cout << "   Predicted relative crossings2 ($E_2[\\bar{C}]$): " << avg_predicted_rel_crossings2 << endl;
  // Information for Fig. 4 (applying Eq. 25)
  cout << "   Abs. Relative error of prediction0 ($\\Delta_0$): " << avg_abs_delta0 << endl;
  cout << "   Abs. Relative error of prediction2 ($\\Delta_2$): " << avg_abs_delta2 << endl;
  cout << "   Relative error of prediction0 ($\\Delta_0$): " << avg_delta0 << endl;
  cout << "   Relative error of prediction2 ($\\Delta_2$): " << avg_delta2 << endl;

}

void calculate(const string &label, const undirected_graph &g)
{
  assert(g.tree());
  int n = g.vertices();
  probability_of_crossing_knowing_edge_lengths p(n);
  double degree_2nd_moment = g.degree_2nd_moment();
  int potential_crossings;
  double expected_crossings2;

  cout << label << endl;
  double crossings_max = n*(n - 1 - degree_2nd_moment)/double(2);
  assert(0 <= crossings_max);
  assert(crossings_max <= (n-1)*(n-2)/2);
  double expected_crossings0 = n*(n - 1 - degree_2nd_moment)/double(6);
  expected_crossings2 = g.fully_predicted_crossings(p, potential_crossings);
  int crossings = g.crossings();
  cout << setprecision(2);
  // Information for Table 1
  cout << "   Number of vertices ($n$): " << n << endl;
  cout << "   Degree 2nd moment ($\\left< k^2 \\right>$): " << degree_2nd_moment << endl;
  cout << "   Maximum number of crossings ($C_{max}$): " << crossings_max << endl;
  cout << "   Crossings ($C_{true}$): " << crossings << endl;
  cout << "   Predicted crossings0 ($E_0[C]$): " << expected_crossings0 << endl;
  cout << "   Predicted crossings2 ($E_2[C]$): " << expected_crossings2 << endl;
  cout << "   Relative crossings ($\\bar{C}_{true}$): " << crossings/crossings_max << endl;
  cout << "   Predicted relative crossings0 ($E_0[\\bar{C}]$): " << expected_crossings0/crossings_max << endl;
  cout << "   Predicted relative crossings2 ($E_2[\\bar{C}]$): " << expected_crossings2/crossings_max << endl;
  // Information for Fig. 4 (applying Eq. 25)
  cout << "   Relative error of prediction0 ($\\Delta_0$): " << (expected_crossings0 - crossings)/crossings_max << endl;
  cout << "   Relative error of prediction2 ($\\Delta_2$): " << (expected_crossings2 - crossings)/crossings_max << endl;
}


double overflow_safe_average ( std::vector<double> myvector )
{
  double avg = 0.0;
  std::size_t sz = myvector.size();
  for ( std::size_t i = 0 ; i < sz ; i++ ) 
  {
    avg += myvector[i] / sz;
  }
  return avg;
}

double overflow_safe_stdev ( std::vector<double> myvector )
{
  double avg = overflow_safe_average(myvector);
  double variance = 0.0;
  std::size_t sz = myvector.size();
  for ( std::size_t i = 0 ; i < sz ; i++ ) 
  {
    variance += ( ((myvector[i]-avg)*(myvector[i]-avg)) / sz );
  }
  return sqrt(variance);
}

double median ( std::vector<double> myvector )
{
  double median;
  std::size_t sz = myvector.size();
  sort(myvector.begin(), myvector.end());
  if (sz  % 2 == 0) 
  {
    median = (myvector[sz/2-1] + myvector[sz/2]) / 2.0;
  } else 
  {
    median = myvector[sz/2];
  }
  return median;
}

std::vector<undirected_graph> length_filter ( std::vector<undirected_graph> input , int min_length , int max_length )
{
	std::vector<undirected_graph> result;
	std::size_t sz = input.size();
	for ( std::size_t i = 0 ; i < sz ; i++ )
	{
		undirected_graph g = input[i];
		int len = g.vertices();
		if ( len >= min_length && len <= max_length )
			result.push_back(g);
	} 
	return result;
}

/**
 * Includes standard deviation, etc.
 */
void compute_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix , bool verbose /*= true*/ , bool c_d /*=false*/ )
{

  std::size_t sz = graphs.size();


  //vectors of metrics for each tree:
  std::vector<double> length = std::vector<double>(sz,0.0);
  std::vector<double> degree_2nd_moment = std::vector<double>(sz,0.0);
  std::vector<double> crossings_max = std::vector<double>(sz,0.0);
  std::vector<double> crossings = std::vector<double>(sz,0.0);
  std::vector<double> expected_crossings0 = std::vector<double>(sz,0.0);
  std::vector<double> expected_crossings2 = std::vector<double>(sz,0.0);
  std::vector<double> expected_crossingsD = std::vector<double>(sz,0.0);
  std::vector<double> rel_crossings = std::vector<double>(sz,0.0);
  std::vector<double> predicted_rel_crossings0 = std::vector<double>(sz,0.0);
  std::vector<double> predicted_rel_crossings2 = std::vector<double>(sz,0.0);
  std::vector<double> predicted_rel_crossingsD = std::vector<double>(sz,0.0);
  std::vector<double> abs_delta0 = std::vector<double>(sz,0.0);
  std::vector<double> abs_delta2 = std::vector<double>(sz,0.0);
   std::vector<double> abs_deltaD = std::vector<double>(sz,0.0);
  std::vector<double> delta0 = std::vector<double>(sz,0.0);
  std::vector<double> delta2 = std::vector<double>(sz,0.0);
  std::vector<double> deltaD = std::vector<double>(sz,0.0);
  
  //even more vectors of metrics (for small.tex, 2016-11-14):
  std::vector<double> crossings_max_linear = std::vector<double>(sz,0.0); //Q_linear
  std::vector<double> expected_crossings_urlt = std::vector<double>(sz,0.0); //E_URLT(Q/3)
  std::vector<double> crossings_ratio_to_qlinear = std::vector<double>(sz,0.0); //Ctrue/Qlinear
  std::vector<double> crossings_ratio_to_exp_urlt = std::vector<double>(sz,0.0); //Ctrue/Eurlt
  std::vector<double> crossings_ratio_to_exp_urla = std::vector<double>(sz,0.0); //Ctrue/Eurla = Ctrue/expected_crossings_0
  std::vector<double> is_planar = std::vector<double>(sz,0.0); //1.0 if crossings == 0, 0.0 otherwise
  std::vector<double> hubiness = std::vector<double>(sz,0.0); //hubiness coefficient h
  std::vector<double> exp_urlt_hubiness = std::vector<double>(sz,0.0); //expected hubiness coefficient for a random linear tree of this size, eq. 1/n
  std::vector<double> crossings_ratio_to_exp_urlt_minus_star = std::vector<double>(sz,0.0); //Ctrue/Eurlt(Q/3 not star)
  std::vector<double> expected_crossings_urlt_minus_star = std::vector<double>(sz,0.0); //E_URLT(Q/3 not star)
  std::vector<double> exp_urlt_hubiness_minus_star = std::vector<double>(sz,0.0); //expected hubiness coefficient for u.r.l.t. excluding star trees
  
  
  //std::ofstream fout;
  //fout.open("log.txt");

  for ( std::size_t i = 0 ; i < sz ; i++ ) 
  {

    undirected_graph graph = graphs[i];
    
    //if ( i % 100 == 0 ) cout << i << endl;
	//write_graph_on_ofstream ( graph , fout );
	
	//cout << "Tree " << i << " size " << graph.vertices() << endl;

    //process graph: current_graph
    int n = graph.vertices();
    length[i] = double(n);

    probability_of_crossing_knowing_edge_lengths p(n);
    degree_2nd_moment[i] = graph.degree_2nd_moment();
    int potential_crossings;

    crossings_max[i] = n*(n - 1 - degree_2nd_moment[i])/double(2); //a.k.a. |Q| in small.tex
    assert(0 <= crossings_max[i]);
    assert(crossings_max[i] <= (n-1)*(n-2)/2);
    expected_crossings0[i] = n*(n - 1 - degree_2nd_moment[i])/double(6);
    expected_crossings2[i] = graph.fully_predicted_crossings(p, potential_crossings);
	if ( c_d ) expected_crossingsD[i] = crossings_at_constant_sum_of_lengths(graph);
    crossings[i] = double(graph.crossings());
    //cout << "Tree number: " << (i+1) << endl;

    rel_crossings[i] = crossings[i]/crossings_max[i]; //a.k.a. C_true/|Q| in small.tex
    predicted_rel_crossings0[i] = expected_crossings0[i]/crossings_max[i];
    predicted_rel_crossings2[i] = expected_crossings2[i]/crossings_max[i];
	if ( c_d ) predicted_rel_crossingsD[i] = expected_crossingsD[i]/crossings_max[i];
    abs_delta0[i] = abs((expected_crossings0[i] - crossings[i])/crossings_max[i]);
    abs_delta2[i] = abs((expected_crossings2[i] - crossings[i])/crossings_max[i]);
	if ( c_d ) abs_deltaD[i] = abs((expected_crossingsD[i] - crossings[i])/crossings_max[i]);
    delta0[i] = (expected_crossings0[i] - crossings[i])/crossings_max[i];
    delta2[i] = (expected_crossings2[i] - crossings[i])/crossings_max[i];
	if ( c_d ) deltaD[i] = (expected_crossingsD[i] - crossings[i])/crossings_max[i];

    //even more vectors of metrics (for small.tex, 2016-11-14):
    crossings_max_linear[i] = double((n-2)*(n-3))/double(2);
    expected_crossings_urlt[i] = double((n-1)*(n-2)*(n-3))/double(6*n);
    crossings_ratio_to_qlinear[i] = crossings[i] / crossings_max_linear[i];
    crossings_ratio_to_exp_urlt[i] = crossings[i] / expected_crossings_urlt[i];
    crossings_ratio_to_exp_urla[i] = crossings[i] / expected_crossings0[i];
    is_planar[i] = (graph.crossings()==0)?1.0:0.0;
    hubiness[i] = (double(n) * (degree_2nd_moment[i]-4.0) + 6.0) / double((n-2)*(n-3));
    exp_urlt_hubiness[i] = (double(1.0) / double(n));
	expected_crossings_urlt_minus_star[i] = double((n-1)*(n-2)*(n-3))/double(6.0*(n-pow(n,4.0-n)));
	crossings_ratio_to_exp_urlt_minus_star[i] = crossings[i] / expected_crossings_urlt_minus_star[i];
	if ( n > 100 )
		exp_urlt_hubiness_minus_star[i] = (double(1.0) / double(n)); //practically equal to n
	else
		exp_urlt_hubiness_minus_star[i] = ( (pow(n,n) - pow(n,4.0) ) /  (pow(n,n+1) - pow(n,4.0)  ) ); //use the formula

  }
  

  cout << setprecision(5);

  if ( verbose ) 
  {

		if ( !linePrefix.empty() ) cout << linePrefix << endl;
    cout << "   Number of vertices ($n$): " << overflow_safe_average(length) << endl;
    cout << "   Degree 2nd moment ($\\left< k^2 \\right>$): " << overflow_safe_average(degree_2nd_moment) << endl;
    cout << "   Maximum number of crossings ($C_{max}$): " << overflow_safe_average(crossings_max) << endl;
    cout << "   Crossings ($C_{true}$): " << overflow_safe_average(crossings) << endl;
    cout << "   Predicted crossings0 ($E_0[C]$): " << overflow_safe_average(expected_crossings0) << endl;
    cout << "   Predicted crossings2 ($E_2[C]$): " << overflow_safe_average(expected_crossings2) << endl;
	if ( c_d ) cout << "   Predicted crossings at constant D ($E[C|D]$): " << overflow_safe_average(expected_crossingsD) << endl; 
    cout << "   Relative crossings ($\\bar{C}_{true}$): " << overflow_safe_average(rel_crossings) << endl;
    cout << "   Predicted relative crossings0 ($E_0[\\bar{C}]$): " << overflow_safe_average(predicted_rel_crossings0) << endl;
    cout << "   Predicted relative crossings2 ($E_2[\\bar{C}]$): " << overflow_safe_average(predicted_rel_crossings2) << endl;
	if ( c_d ) cout << "   Predicted relative crossings at constant D ($E[\\bar{C}| D]$): " << overflow_safe_average(predicted_rel_crossingsD) << endl;
    // Information for Fig. 4 (applying Eq. 25)
    cout << "   Abs. Relative error of prediction0 ($\\Delta_0$): " << overflow_safe_average(abs_delta0) << endl;
    cout << "   Abs. Relative error of prediction2 ($\\Delta_2$): " << overflow_safe_average(abs_delta2) << endl;
	if ( c_d ) cout << "   Abs. Relative error of expected crossings at constant D ($?$): " << overflow_safe_average(abs_deltaD) << endl;
    cout << "   Relative error of prediction0 ($\\Delta_0$): " << overflow_safe_average(delta0) << endl;
    cout << "   Relative error of prediction2 ($\\Delta_2$): " << overflow_safe_average(delta2) << endl;
	if ( c_d ) cout << "   Relative error of expected crossings at constant D ($?$): " << overflow_safe_average(deltaD) << endl;

    //even more vectors of metrics (for small.tex, 2016-11-14):
    cout << "   Max crossings of linear tree of this length ($Q_{linear}$): " << overflow_safe_average(crossings_max_linear) << endl;
    cout << "   Expected crossings for uniformly random labelled tree ($E_{URLT}[|Q|/3]$): " << overflow_safe_average(expected_crossings_urlt) << endl;
    cout << "   Crossings ratio to Qlinear: " << overflow_safe_average(crossings_ratio_to_qlinear) << endl;
    cout << "   Crossings ratio to expected for u.r.l.t.: " << overflow_safe_average(crossings_ratio_to_exp_urlt) << endl;
    cout << "   Crossings ratio to expected for u.r.l.a.: " << overflow_safe_average(crossings_ratio_to_exp_urla) << endl;
    cout << "   Planarity: " << overflow_safe_average(is_planar) << endl;
    cout << "   Hubiness: " << overflow_safe_average(hubiness) << endl;
    cout << "   Expected hubiness for u.r.l.t.: " << overflow_safe_average(exp_urlt_hubiness) << endl;
	cout << "   Expected crossings for uniformly random labelled tree ($E_{URLT}[|Q|/3]$) corrected for star trees: " << overflow_safe_average(expected_crossings_urlt_minus_star) << endl;
	cout << "   Crossings ratio to expected for u.r.l.t. corrected for star trees: " << overflow_safe_average(crossings_ratio_to_exp_urlt_minus_star) << endl;
	cout << "   Expected hubiness for u.r.l.t. corrected for star trees: " << overflow_safe_average(exp_urlt_hubiness_minus_star) << endl;

    cout << "CSV format metrics for table:" << endl;

  }

	if ( !linePrefix.empty() ) cout << linePrefix << " ";
  cout << overflow_safe_average(length) << " " << median(length) << " " << overflow_safe_stdev(length) << " ";
  //metrics[0]-metrics[32]
  cout << overflow_safe_average(crossings) << " " << median(crossings) << " " << overflow_safe_stdev(crossings) << " ";
  cout << overflow_safe_average(crossings_max) << " " << median(crossings_max) << " " << overflow_safe_stdev(crossings_max) << " ";
  cout << overflow_safe_average(expected_crossings0) << " " << median(expected_crossings0) << " " << overflow_safe_stdev(expected_crossings0) << " ";
  cout << overflow_safe_average(expected_crossings2) << " " << median(expected_crossings2) << " " << overflow_safe_stdev(expected_crossings2) << " ";
  if ( c_d ) cout << overflow_safe_average(expected_crossingsD) << " " << median(expected_crossingsD) << " " << overflow_safe_stdev(expected_crossingsD) << " ";
  cout << overflow_safe_average(rel_crossings) << " " << median(rel_crossings) << " " << overflow_safe_stdev(rel_crossings) << " ";
  cout << overflow_safe_average(predicted_rel_crossings0) << " " << median(predicted_rel_crossings0) << " " << overflow_safe_stdev(predicted_rel_crossings0) << " ";
  cout << overflow_safe_average(predicted_rel_crossings2) << " " << median(predicted_rel_crossings2) << " " << overflow_safe_stdev(predicted_rel_crossings2) << " ";
  if ( c_d ) cout << overflow_safe_average(predicted_rel_crossingsD) << " " << median(predicted_rel_crossingsD) << " " << overflow_safe_stdev(predicted_rel_crossingsD) << " ";
  cout << overflow_safe_average(abs_delta0) << " " << median(abs_delta0) << " " << overflow_safe_stdev(abs_delta0) << " ";
  cout << overflow_safe_average(abs_delta2) << " " << median(abs_delta2) << " " << overflow_safe_stdev(abs_delta2) << " ";
  if ( c_d ) cout << overflow_safe_average(abs_deltaD) << " " << median(abs_deltaD) << " " << overflow_safe_stdev(abs_deltaD) << " ";
  cout << overflow_safe_average(delta0) << " " << median(delta0) << " " << overflow_safe_stdev(delta0) << " ";
  cout << overflow_safe_average(delta2) << " " << median(delta2) << " " << overflow_safe_stdev(delta2) << " ";
  if ( c_d ) cout << overflow_safe_average(deltaD) << " " << median(deltaD) << " " << overflow_safe_stdev(deltaD) << " ";
  //even more vectors of metrics (for small.tex, 2016-11-14). metrics[33]-metrics[59] in length group entries in the Java code:
  cout << overflow_safe_average(crossings_max_linear) << " " << median(crossings_max_linear) << " " << overflow_safe_stdev(crossings_max_linear) << " ";
  cout << overflow_safe_average(expected_crossings_urlt) << " " << median(expected_crossings_urlt) << " " << overflow_safe_stdev(expected_crossings_urlt) << " ";
  cout << overflow_safe_average(crossings_ratio_to_qlinear) << " " << median(crossings_ratio_to_qlinear) << " " << overflow_safe_stdev(crossings_ratio_to_qlinear) << " ";
  cout << overflow_safe_average(crossings_ratio_to_exp_urlt) << " " << median(crossings_ratio_to_exp_urlt) << " " << overflow_safe_stdev(crossings_ratio_to_exp_urlt) << " ";
  cout << overflow_safe_average(crossings_ratio_to_exp_urla) << " " << median(crossings_ratio_to_exp_urla) << " " << overflow_safe_stdev(crossings_ratio_to_exp_urla) << " ";
  cout << overflow_safe_average(is_planar) << " " << median(is_planar) << " " << overflow_safe_stdev(is_planar) << " ";
  cout << overflow_safe_average(degree_2nd_moment) << " " << median(degree_2nd_moment) << " " << overflow_safe_stdev(degree_2nd_moment) << " ";
  cout << overflow_safe_average(hubiness) << " " << median(hubiness) << " " << overflow_safe_stdev(hubiness) << " ";
  cout << overflow_safe_average(exp_urlt_hubiness) << " " << median(exp_urlt_hubiness) << " " << overflow_safe_stdev(exp_urlt_hubiness) << " ";
  cout << overflow_safe_average(expected_crossings_urlt_minus_star) << " " << median(expected_crossings_urlt_minus_star) << " " << overflow_safe_stdev(expected_crossings_urlt_minus_star) << " ";
  cout << overflow_safe_average(crossings_ratio_to_exp_urlt_minus_star) << " " << median(crossings_ratio_to_exp_urlt_minus_star) << " " << overflow_safe_stdev(crossings_ratio_to_exp_urlt_minus_star) << " ";
  cout << overflow_safe_average(exp_urlt_hubiness_minus_star) << " " << median(exp_urlt_hubiness_minus_star) << " " << overflow_safe_stdev(exp_urlt_hubiness_minus_star) << " ";
  cout << endl;


}



/**
 * Compute optimality (omega) metrics for new project on dependency length minimization.
 * Includes standard deviation, etc.
 */
void compute_optimality_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix , bool verbose /*= true*/ )
{

  std::size_t sz = graphs.size();


  //vectors of metrics for each tree:
  std::vector<double> length = std::vector<double>(sz,0.0);
  std::vector<double> D = std::vector<double>(sz,0.0);
  std::vector<double> D_min = std::vector<double>(sz,0.0);
  std::vector<double> D_random = std::vector<double>(sz,0.0);
  std::vector<double> Omega = std::vector<double>(sz,0.0);
  
  for ( std::size_t i = 0 ; i < sz ; i++ ) 
  {

    undirected_graph graph = graphs[i];
    
    //if ( i % 100 == 0 ) cout << i << endl;
	//write_graph_on_ofstream ( graph , fout );
	
	//cout << "Tree " << i << " size " << graph.vertices() << endl;

    //process graph: current_graph
    int n = graph.vertices();
    length[i] = double(n);

    D[i] = graph.edge_length_sum();
    D_min[i] = graph.minimum_edge_length_sum_of_a_tree();
    D_random[i] = (n-1)*(n+1)/double(3);
    Omega[i] = (D_random[i] - D[i])/(D_random[i] - D_min[i]);

    assert(-8 <= Omega[i]);
    assert(Omega[i] <= 1);

  }
  cout << setprecision(5);

#ifdef DEBUG_OUTPUT
  for(std::size_t i = 0; i < sz; i++) {
      undirected_graph g = graphs[i];
      cout << Omega[i] << ": ";
	  cout << D[i] << ": ";
	  cout << D_min[i] << ": ";
      cout << g.vertices() << " " << g.edges() << " & ";
      for(int u = 0; u < g.vertices(); ++u) {
	  for(set<int>::const_iterator i = g.adjacent[u].begin(); i != g.adjacent[u].end(); ++i) {
	      if(u < *i)
		  cout << u << " " << *i << " # ";
	  }
      }
      cout << endl;
  }
#endif

  if ( verbose ) 
  {

		if ( !linePrefix.empty() ) cout << linePrefix << endl;
    cout << "   Number of sentences processed: " << sz << endl;
    cout << "   Number of vertices ($n$): " << overflow_safe_average(length) << endl;
    cout << "   Sum of dependency lengths ($D$): " << overflow_safe_average(D) << endl;
    cout << "   Minimum sum of dependency lengths ($D_{min}$): " << overflow_safe_average(D_min) << endl;
    cout << "   Expected sum of dependency lengths in a random linear arrangement ($D_{random}$): " << overflow_safe_average(D_random) << endl;
    cout << "   Optimality measure ($\\Omega$): " << overflow_safe_average(Omega) << endl;

    cout << "CSV format metrics for table:" << endl;

  }

	if ( !linePrefix.empty() ) cout << linePrefix << " ";
  cout << overflow_safe_average(length) << " " << median(length) << " " << overflow_safe_stdev(length) << " ";
  cout << overflow_safe_average(D) << " " << median(D) << " " << overflow_safe_stdev(D) << " ";
  cout << overflow_safe_average(D_min) << " " << median(D_min) << " " << overflow_safe_stdev(D_min) << " ";
  cout << overflow_safe_average(D_random) << " " << median(D_random) << " " << overflow_safe_stdev(D_random) << " ";
  cout << overflow_safe_average(Omega) << " " << median(Omega) << " " << overflow_safe_stdev(Omega) << " ";
  cout << endl;

}




/**
 * Compute metrics for omega graphs in "classes of grammars" paper: C, D, K2 and Omega (avg and stdev)
 * Includes standard deviation, etc.
 */
void compute_grammar_optimality_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix , bool verbose /*= true*/ )
{

  std::size_t sz = graphs.size();


  //vectors of metrics for each tree:
  std::vector<double> length = std::vector<double>(sz,0.0);
  std::vector<double> D = std::vector<double>(sz,0.0);
  std::vector<double> D_min = std::vector<double>(sz,0.0);
  std::vector<double> D_random = std::vector<double>(sz,0.0);
  std::vector<double> Omega = std::vector<double>(sz,0.0);
  std::vector<double> crossings = std::vector<double>(sz,0.0);
  std::vector<double> K2 = std::vector<double>(sz,0.0);
  
  bool invalid_length_for_omega = false;
  
  for ( std::size_t i = 0 ; i < sz ; i++ ) 
  {

    undirected_graph graph = graphs[i];
    
    //if ( i % 100 == 0 ) cout << i << endl;
	//write_graph_on_ofstream ( graph , fout );
	
	//cout << "Tree " << i << " size " << graph.vertices() << endl;

    //process graph: current_graph
    int n = graph.vertices();
    if ( n < 3 ) invalid_length_for_omega = true;
    length[i] = double(n);

    D[i] = graph.edge_length_sum();
    D_min[i] = graph.minimum_edge_length_sum_of_a_tree();
    D_random[i] = (n-1)*(n+1)/double(3);

    if ( n >= 3 )
    {
        Omega[i] = (D_random[i] - D[i])/(D_random[i] - D_min[i]);
        assert(-8 <= Omega[i]);
        assert(Omega[i] <= 1);
    }
    
    K2[i] = graph.sum_of_squared_degrees();
    crossings[i] = double(graph.crossings());

  }
  cout << setprecision(5);

#ifdef DEBUG_OUTPUT
  for(std::size_t i = 0; i < sz; i++) {
      undirected_graph g = graphs[i];
      cout << Omega[i] << ": ";
	  cout << D[i] << ": ";
	  cout << D_min[i] << ": ";
      cout << g.vertices() << " " << g.edges() << " & ";
      for(int u = 0; u < g.vertices(); ++u) {
	  for(set<int>::const_iterator i = g.adjacent[u].begin(); i != g.adjacent[u].end(); ++i) {
	      if(u < *i)
		  cout << u << " " << *i << " # ";
	  }
      }
      cout << endl;
  }
#endif

  if ( verbose ) 
  {

		if ( !linePrefix.empty() ) cout << linePrefix << endl;
    cout << "   Number of sentences processed: " << sz << endl;
    cout << "   Number of vertices ($n$): " << overflow_safe_average(length) << endl;
    cout << "   Sum of dependency lengths ($D$): " << overflow_safe_average(D) << endl;
    cout << "   Minimum sum of dependency lengths ($D_{min}$): " << overflow_safe_average(D_min) << endl;
    cout << "   Expected sum of dependency lengths in a random linear arrangement ($D_{random}$): " << overflow_safe_average(D_random) << endl;
    cout << "   Optimality measure ($\\Omega$): " << overflow_safe_average(Omega) << endl;
    cout << "   Number of crossings ($C$): " << overflow_safe_average(crossings) << endl;
    cout << "   Degree 2nd moment ($\\left< k^2 \\right>$): " << overflow_safe_average(K2) << endl;

    cout << "CSV format metrics for table:" << endl;

  }

	if ( !linePrefix.empty() ) cout << linePrefix << " ";
  //cout << overflow_safe_average(length) << " " << median(length) << " " << overflow_safe_stdev(length) << " ";
  //cout << overflow_safe_average(D) << " " << median(D) << " " << overflow_safe_stdev(D) << " ";
  //cout << overflow_safe_average(D_min) << " " << median(D_min) << " " << overflow_safe_stdev(D_min) << " ";
  //cout << overflow_safe_average(D_random) << " " << median(D_random) << " " << overflow_safe_stdev(D_random) << " ";
  //cout << overflow_safe_average(Omega) << " " << median(Omega) << " " << overflow_safe_stdev(Omega) << " ";
  cout << overflow_safe_average(crossings) << " " << overflow_safe_stdev(crossings) << " ";
  cout << overflow_safe_average(D) << " " << overflow_safe_stdev(D) << " ";
  cout << overflow_safe_average(K2) << " " << overflow_safe_stdev(K2) << " ";
  if ( invalid_length_for_omega ) cout << "N/A" << " " << "N/A" << " ";
  else cout << overflow_safe_average(Omega) << " " << overflow_safe_stdev(Omega) << " ";
  cout << endl;

}





/**
 * Compute metrics for new project on anti-DLM on short sentences.
 * Includes standard deviation, etc.
 */
void compute_antidlm_detailed_metrics ( std::vector<undirected_graph> graphs , const std::string& linePrefix , bool verbose /*= true*/ )
{

  std::size_t sz = graphs.size();


  //vectors of metrics for each tree:
  std::vector<double> length = std::vector<double>(sz,0.0);
  std::vector<double> K2 = std::vector<double>(sz,0.0);
  std::vector<double> D = std::vector<double>(sz,0.0);
  std::vector<double> D_min = std::vector<double>(sz,0.0);
  
  for ( std::size_t i = 0 ; i < sz ; i++ ) 
  {

    undirected_graph graph = graphs[i];
    
    //if ( i % 100 == 0 ) cout << i << endl;
	//write_graph_on_ofstream ( graph , fout );
	
	//cout << "Tree " << i << " size " << graph.vertices() << endl;

    //process graph: current_graph
    int n = graph.vertices();
    length[i] = double(n);

    D[i] = graph.edge_length_sum();
    D_min[i] = graph.minimum_edge_length_sum_of_a_tree();
    K2[i] = graph.sum_of_squared_degrees();

  }
  cout << setprecision(5);

#ifdef DEBUG_OUTPUT
  for(std::size_t i = 0; i < sz; i++) {
      undirected_graph g = graphs[i];
	  cout << D[i] << ": ";
	  cout << D_min[i] << ": ";
      cout << g.vertices() << " " << g.edges() << " & ";
      for(int u = 0; u < g.vertices(); ++u) {
	  for(set<int>::const_iterator i = g.adjacent[u].begin(); i != g.adjacent[u].end(); ++i) {
	      if(u < *i)
		  cout << u << " " << *i << " # ";
	  }
      }
      cout << endl;
  }
#endif

  if ( verbose ) 
  {

		if ( !linePrefix.empty() ) cout << linePrefix << endl;
    cout << "   Number of sentences processed: " << sz << endl;
    cout << "   Number of vertices ($n$): " << overflow_safe_average(length) << endl;
    cout << "   Sum of dependency lengths ($D$): " << overflow_safe_average(D) << endl;
    cout << "   Minimum sum of dependency lengths ($D_{min}$): " << overflow_safe_average(D_min) << endl;
    cout << "   Sum of squared degrees (K2): " << overflow_safe_average(K2) << endl;

    cout << "CSV format metrics for table:" << endl;

  }

  if ( !linePrefix.empty() ) cout << linePrefix << " ";
  cout << overflow_safe_average(length) << " " << median(length) << " " << overflow_safe_stdev(length) << " ";
  cout << overflow_safe_average(D) << " " << median(D) << " " << overflow_safe_stdev(D) << " ";
  cout << overflow_safe_average(D_min) << " " << median(D_min) << " " << overflow_safe_stdev(D_min) << " ";
  cout << overflow_safe_average(K2) << " " << median(K2) << " " << overflow_safe_stdev(K2) << " ";
  cout << endl;

}


/**
 * Returns the ISO code of a language present in the given string.
 */
std::string get_iso_code ( std::string language_or_filename )
{
    //careful, if a language name contains another (e.g. Ancient Greek contains Greek) the longest name
    //must come before!
    //updated for ud 2.4
    //updated for ud 2.5
    //updated for ud 2.6
    const char * languages [] = {
        "Swiss_German",
        "Hindi_English",
        "Old_French",
        "Abaza",
        "Afrikaans",
        "Akkadian",
        "Albanian",
        "Amharic",
        "Ancient_Greek",
        "Arabic",
        "Armenian",
        "Assyrian",
        "Bambara",
        "Basque",
        "Belarusian",
        "Bhojpuri",
        "Breton",
        "Bulgarian",
        "Buryat",
        "Cantonese",
        "Catalan",
        "Classical_Chinese",
        "Chinese",
        "Coptic",
        "Croatian",
        "Czech",
        "Danish",
        "Dutch",
        "English",
        "Erzya",
        "Estonian",
        "Faroese",
        "Finnish",
        "French",
        "Galician",
        "German",
        "Gothic",
        "Greek",
        "Hebrew",
        "Hindi",
        "Hungarian",
        "Icelandic",
        "Indonesian",
        "Old_Irish",
        "Irish",
        "Italian",
        "Japanese",
        "Karelian",
        "Kazakh",
        "Komi_Permyak",
        "Komi_Zyrian",
        "Korean",
        "Kurmanji",
        "Latin",
        "Latvian",
        "Lithuanian",
        "Livvi",
        "Magahi",
        "Maltese",
        "Marathi",
        "Mbya_Guarani",
        "Moksha",
        "Naija",
        "North_Sami",
        "Norwegian",
        "Old_Church_Slavonic",
        "Persian",
        "Polish",
        "Portuguese",
        "Romanian",
        "Old_Russian",
        "Russian",
        "Sanskrit",
        "Scottish_Gaelic",
        "Serbian",
        "Sindhi",
        "Skolt_Sami",
        "Slovak",
        "Slovenian",
        "Spanish",
        "Swedish_Sign_Language",
        "Swedish",
        "Tagalog",
        "Tamil",
        "Telugu",
        "Thai",
        "Turkish",
        "Ukrainian",
        "Upper_Sorbian",
        "Urdu",
        "Uyghur",
        "Vietnamese",
        "Warlpiri",
        "Welsh",
        "Wolof",
        "Yoruba",        
    };
    const char * codes[] = {
        "gsw",
        "hin_eng",
        "fro",
        "abq",
        "afr",
        "akk",
        "alb",
        "amh",
        "grc",
        "arb",
        "hye",
        "aii",
        "bam",
        "eus",
        "bel",
        "bho",
        "bre",
        "bul",
        "bua",
        "yue",
        "cat",
        "lzh",
        "cmn",
        "cop",
        "hrv",
        "ces",
        "dan",
        "nld",
        "eng",
        "myv",
        "est",
        "fao",
        "fin",
        "fra",
        "glg",
        "deu",
        "got",
        "ell",
        "heb",
        "hin",
        "hun",
        "isl",
        "ind",
        "mga",
        "gle",
        "ita",
        "jpn",
        "krl",
        "kaz",
        "koi",
        "kpv",
        "kor",
        "kmr",
        "lat",
        "lav",
        "lit",
        "olo",
        "mag",
        "mlt",
        "mar",
        "gun",
        "mdf",
        "pcm",
        "sme",
        "nob",
        "chu",
        "pes",
        "pol",
        "por",
        "ron",
        "orv",
        "rus",
        "san",
        "gla",
        "srp",
        "snd",
        "sms",
        "slk",
        "slv",
        "spa",
        "swl",
        "swe",
        "tgl",
        "tam",
        "tel",
        "tha",
        "tur",
        "ukr",
        "hsb",
        "urd",
        "uig",
        "vie",
        "wbp",
        "cym",
        "wol",
        "yor",
    };
    
    //this is for the HamleDT files that have a 2-letter code.
    const char * languages2 [] = {
        "-ar-",
        "-bg-",
        "-bn-",
        "-ca-",
        "-cs-",
        "-da-",
        "-de-",
        "-el-",
        "-en-",
        "-es-",
        "-et-",
        "-eu-",
        "-fa-",
        "-fi-",
        "-grc-",
        "-hi-",
        "-hu-",
        "-it-",
        "-ja-",
        "-la-",
        "-nl-",
        "-pt-",
        "-ro-",
        "-ru-",
        "-sk-",
        "-sl-",
        "-sv-",
        "-ta-",
        "-te-",
        "-tr-",
    };
    
    const char * codes2 [] = {
        "arb",
        "bul",
        "ben", //new
        "cat",
        "ces",
        "dan",
        "deu",
        "ell",
        "eng",
        "spa",
        "est",
        "eus",
        "pes",
        "fin",
        "grc",
        "hin",
        "hun",
        "ita",
        "jpn",
        "lat",
        "nld",
        "por",
        "ron",
        "rus",
        "slk",
        "slv",
        "swe",
        "tam",
        "tel",
        "tur",
    };
    
      int sz = sizeof(languages) / sizeof(const char *);
      int sz2 = sizeof(languages2) / sizeof(const char *);
      for ( int i = 0 ; i < sz ; i++ )
      {
          if ( language_or_filename.find(languages[i]) != std::string::npos ) return codes[i];
      }
      for ( int i = 0 ; i < sz2 ; i++ )
      {
          if ( language_or_filename.find(languages2[i]) != std::string::npos ) return codes2[i];
      }
      cerr << "ISO code not found for " << language_or_filename;
      return "unk";
}

//determine if string ends with given suffix
int ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
  
//get full path to filenames in given directory ending with given extension
std::vector<std::string> get_files ( const char * directory , const char * extension )
{
    std::vector<std::string> filenames;
    DIR * mydir = opendir(directory);
    struct dirent * myfile;
    if ( mydir != NULL )
    {
        while ( (myfile = readdir(mydir)) != NULL )
        {
            std::string name = std::string(myfile->d_name);
            if ( ends_with(name.c_str(),extension) )
                filenames.push_back(std::string(directory)+name);
        }
    }
    return filenames;
}