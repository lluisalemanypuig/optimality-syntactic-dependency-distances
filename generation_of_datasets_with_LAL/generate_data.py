"""
This script is part of the "Optimality of Syntactic Dependencies" repository
which can be found at

	https://github.com/lluisalemanypuig/optimality-syntactic-dependency-distances

and is part of the code used for the paper entitled

	The optimality of syntactic dependency distances
	https://arxiv.org/abs/2007.15342

NOTE:
=====
* This script requires a version of the Linear Arrangement Library
newer than 21.07

* Users who want to execute this should update the paths to the each
collection's main files at the bottom of this script.
"""

import lal

def process_treebank(main_file, treebank_collection_name, output_directory):
	"""
	Process a treebank using the Linear Arrangement Library
	
	Parameters
	----------
	- main_file : name of the collection's main file.
	- treebank_collection_name : identifier of the treebank collection;
		used only to create the output filename.
	- output_directory : directory where to store the results.
	"""
	
	# Initialise the processor. Make sure the input data exists.
	tbcolproc = lal.io.treebank_collection_processor()
	err = tbcolproc.init(main_file, output_directory)
	if err != lal.io.treebank_error_type.no_error:
		print(err)
		return False
	
	# set the exact metrics (or, in LAL-ian, features) that are to be computed
	tbcolproc.clear_features()
	tbcolproc.add_feature(lal.io.treebank_feature.num_nodes)
	tbcolproc.add_feature(lal.io.treebank_feature.sum_squared_degrees)
	tbcolproc.add_feature(lal.io.treebank_feature.sum_edge_lengths)
	tbcolproc.add_feature(lal.io.treebank_feature.min_sum_edge_lengths)
	
	# finish setting up the behaviour of the processor
	tbcolproc.set_number_threads(4)
	tbcolproc.set_separator(' ')
	
	# customize header
	tbcolproc.set_column_name(lal.io.treebank_feature.num_nodes, "n")
	tbcolproc.set_column_name(lal.io.treebank_feature.sum_squared_degrees, "K2")
	tbcolproc.set_column_name(lal.io.treebank_feature.sum_edge_lengths, "D")
	tbcolproc.set_column_name(lal.io.treebank_feature.min_sum_edge_lengths, "D_min")
	tbcolproc.set_treebank_column_name("language")
	
	# set output file name
	tbcolproc.set_join_to_file_name(output_directory + "/ddmetrics_trimmed-" + treebank_collection_name + ".txt")
	
	# process the treebank collection. This may take a while...
	err = tbcolproc.process()
	
	# check for errors
	if err != lal.io.treebank_error_type.no_error:
		print(err)
		
		if err == lal.io.treebank_error_type.some_treebank_file_failed:
			nerrs = tbcolproc.get_num_errors()
			for i in range(0,nerrs):
				print(tbcolproc.get_error_type(i))
				print("    Treebank name:", tbcolproc.get_error_treebank_name(i))
				print("    Treebank filename:", tbcolproc.get_error_treebank_filename(i))
		return False
	
	return True

process_treebank("prague_list.txt",   "prague",   "out/")
process_treebank("stanford_list.txt", "stanford", "out/")
process_treebank("psud26_list.txt",   "psud26",   "out/")
process_treebank("pud26_list.txt",    "pud26",    "out/")
process_treebank("sud26_list.txt",    "sud26",    "out/")
process_treebank("ud26_list.txt",     "ud26",     "out/")
