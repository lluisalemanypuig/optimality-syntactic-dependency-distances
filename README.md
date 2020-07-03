# optimality-syntactic-dependency-distances

## **Processing of treebanks**

The first step of the analysis involves downloading collections of syntactic corpora and processing them into a compact, uniform format that facilitates analysis.

 1. Download and uncompress the [UD](https://lindat.mff.cuni.cz/repository/xmlui/handle/11234/1-3226) and [SUD](http://www.grew.fr/download/sud-treebanks-v2.6.tgz) collections (currently v2.6), each will produce a folder named (s)ud-treebanks-vX.Y. If you wish to perform separate analysis on the PUD treebanks (parallel subset of UD and SUD) as well, create another two folders (they can be named, for example, p(s)ud-treebanks-vX.Y) and copy every folder whose name ends with PUD into those.
 2. Download and unzip the free version of HamleDT that contains stripped treebanks for those corpora with licensing problems from https://lindat.mff.cuni.cz/repository/xmlui/handle/11858/00-097C-0000-0023-9551-4?show=full . It will produce a folder named 2.0 (you may wish to rename it to something more specific).
Ensure that the directory with the UD, SUD and HamleDT downloads are in a drive with at least 20 GB free disk space (large files will be generated) and that the parent directory is writable (new directories will be created for processed treebanks).
3. Change into the processing_of_treebanks_and_tests directory of this project, and run `make` to compile all classes (they work on Java 8 or greater).
4. Still in said directory, run:
```
java TreebankDataExtractor UD <path to UD> 
java TreebankDataExtractor UD <path to SUD> 
java TreebankDataExtractor HamleDT <path to HamleDT>
java TreebankDataExtractor PUD <path to PUD> 
java TreebankDataExtractor PUD <path to PSUD> 
```
The following directories will be created:
```
<path to UD>-merged-bylanguage
<path to UD>-merged-bylanguage-shuffled
<path to SUD>-merged-bylanguage
<path to SUD>-merged-bylanguage-shuffled
<path to PUD>-merged-bylanguage
<path to PUD>-merged-bylanguage-shuffled
<path to PSUD>-merged-bylanguage
<path to PSUD>-merged-bylanguage-shuffled
<path to HamleDT>-stanford-merged-bylanguage
<path to HamleDT>-stanford-merged-bylanguage-shuffled
<path to HamleDT>-prague-merged-bylanguage
<path to HamleDT>-prague-merged-bylanguage-shuffled
```
Each generated directory contains one .conll or .conllu file, one .headsf and one .headsu file per language. The folders for the parallel treebanks (PUD and PSUD) will also contain .headsr files. 

The .conll(u) file is a merging of all corpora of the language present in the collection and with a given annotation style, in the original format. The .headsu files contain compact representations of trees (after removing punctuation and null elements). The .headsf files are filtered versions of .headsu files that exclude trees with less than 3 nodes (useful, e.g. to feed to the program that calculates D_min). The .headsr files for parallel collections of treebanks are filtered versions that exclude sentences that have less than 3 words in *any* of the languages in the parallel treebanks - this ensures that the treebanks stay parallel after the filtering.

The directories suffixed by "-shuffled" contain versions of the same where each tree has been randomly shuffled. These can be used as sanity check or to obtain metrics in case where the linear arrangements of trees are random.

Of course, to process subsets of the collections (e.g. if one is interested in corpora from a single language, family, etc.), one can just create a folder with the relevant corpora (with the same subdirectory structure that comes in the UD and HamleDT downloads, i.e., for example, by just making a copy of the downloaded directory and deleting whatever languages/corpora are unneeded) and call java TreebankDataExtractor <UD/PUD/HamleDT> <relevant path>.

## **Generation of datasets**

Now, we will calculate D_min for the trees in each dataset, as well as other metrics (n, K2, D) useful for our analysis. To do so:

1. Enter the directory generation_of_datasets and run `make` to compile the C++ files.
2. For each non-parallel treebank collection, run:
```analyze_treebanks individual_table <path-to-files-including-final-slash/> .headsu name-of-output-table.txt```

Where path-to-files-including-final-slash is the path to the directory containing .headsu files (generated previously) that is being analyzed.

3. For each parallel treebank collection, run:
```analyze_treebanks individual_table <path-to-files-including-final-slash/> .headsr name-of-output-table.txt```

Note that the only difference is that for parallel treebanks, we use .headsr files for our analysis, to guarantee that they are not skewed by different number of sentences of length <= 3 across treebanks.

The generated tables will be text files with a header that explains the columns:
```language n K2 D D_min```

where language is an ISO 639-3 language code. The header is followed by one row per sentence, with the language and metrics associated with said sentence.

## **Analysis**

In this section, we will run the one-sided Monte Carlo tests for significance of each score, both globally and by sentence length groups.

To do so, first we need to generate .omegas files for each of the .headsf files generated in the "Processing of treebanks" step. .omegas files contain one line per tree with relevant metrics (n, D, D_min, D_random and Omega) and will be used as input by the Java class that performs the Monte Carlo tests. Note that sentences of length smaller than 3 do not have an influence on the test's p-value, hence .headsf files are sufficient, and there is no need to use .headsu files.

To generate the .omegas files:

1. Enter the directory generation_of_datasets, containing C++ files that should already be compiled (see previous section).
2. For each treebank collection, run:
```analyze_treebanks omega_files <path-to-files-including-final-slash/> .headsf```

Where path-to-files-including-final-slash is the path to the directory containing .headsf files (generated previously) that is being analyzed. The .omegas files will be generated in the same directory.

Now, we are ready to run the Monte Carlo tests. We will output the results into a single file for all collections and languages (columns will be used to identify collections, languages and sentence length for each test). To do so:
3. Change to the processing_of_treebanks_and_tests directory. The Java classes there have been compiled in a previous step.
4. To generate the header of the data file, run:
```
java MonteCarloTestOmegaByLength header >> path/to/test-output-file.txt
``` 

This will write a header line into the file with the information about the columns that we are going to populate next:
```
annotation language length sentences optimality optimality_random right_p_value left_p_value meanD meanD_random meanD_right_p_value meanD_left_p_value delta delta_random delta_right_p_value delta_left_p_value gamma gamma_random gamma_right_p_value gamma_left_p_value
``` 

5. For each of the collections, run:
```
java MonteCarloTestOmegaByLength all <iters> <collection-name> <path-to-omegas-files> >> path/to/test-output-file.txt
```
Where <iters> is the number of iterations for the Monte Carlo tests (100000 in the article), <collection-name> is an alphanumeric name to identify the collection (we suggest Stanford, Prague, UD26 and SUD26 for the respective collections) and <path-to-omegas-files> is the full path to the directory containing the omegas files for that collection.
 
This program (which may take a while, depending on number of iterations - starting with small numbers is recommended for testing - will perform Monte Carlo tests for each .omegas file in the given folder. Tests will be performed both for groups of sentences of a given length (rows where the length column has a value >= 3) and mixing sentences of all lengths (rows where the length column has the value 0).
