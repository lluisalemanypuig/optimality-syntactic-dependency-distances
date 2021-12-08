# Preprocessed data and code for the article ["Optimality of syntactic dependency distances"](https://arxiv.org/abs/2007.15342)

## Preprocessed data

Data is available in two levels of preprocessing. The first level consist of the syntactic dependency structures of every sentence in head vector format 
for each collection and treebank: [UD-based collections](https://cqllab.upc.edu/lal/universal-dependencies/) and [Prague and Stanford collections](https://cqllab.upc.edu/lal/others/). 

The second level of preprocessing consists of just the essential information of every sentence, for each collection and treebank:
language (the iso code of the language), `n` (the number of vertices), `K2` (the sum of squared degrees), `D` (the sum of dependency distances), `D_min` (the minimum sum of dependency distances). The information is available as [one file for each combination of collection and annotation style](https://mydisk.cs.upc.edu/s/ByX3w4KFqFNjE7w).

The data resulting from the experiments on function word removal is available for [level 1 and level 2 all together](https://mydisk.cs.upc.edu/s/9X9Akx3M8MkFQqy).

The information about every language employed in our analyses (e.g. family, popular name) is available [here](https://mydisk.cs.upc.edu/s/odKZBqDeatoiNdt).

## Code

### Theory

We wanted to find empirically a lower bound of the value of Omega. Such a lower bound is obtained by calculating the value of modified Omega for every tree and keeping the minimum value among all _n_-vertex trees. The modified Omega is:

             E[D] - D_max
    Omega = --------------
             E[D] - D_min
             
We denote said minimum value as `O_n`. However, the actual strategy used is that explained in the paper. This strategy prevents calculating the value of modified Omega for all trees, thus greatly reducing the number of times that maximum D has to be calculated. This is important since there are not, to the best of our knowledge, any polynomial time algorithms to calculate the maximum value of D of a given tree. In order to calculate it we use two methods:
- the well-known technique of Constraint Programming; for this we use a [MiniZinc](https://www.minizinc.org/) model, and
- a linear-time computable formula to calculate the maximum D of specific kinds of trees for which the model takes too much time to calculate their maximum D.

The [theory](https://github.com/lluisalemanypuig/optimality-syntactic-dependency-distances/tree/master/theory) folder contains all code necessary to calculate the minimum values of Omega. The MiniZinc model can be found within the [theory/MiniZinc folder](https://github.com/lluisalemanypuig/optimality-syntactic-dependency-distances/tree/master/theory/MiniZinc). The startegy used to calculate `O_n` is  implemented in the [main `.cpp` file](https://github.com/lluisalemanypuig/optimality-syntactic-dependency-distances/blob/master/theory/table_Omin.cpp?ts=4). The code to calculate the minimum value of D can be found in the [theory/Dmin folder](https://github.com/lluisalemanypuig/optimality-syntactic-dependency-distances/tree/master/theory/Dmin); said code implements Shiloach's algorithm. The other folders contain code that implement the `tree` class, code to generate all trees of a given number of vertices, and a class that wraps the GMP `m̀pq_t` data structure into a C++ class that is useful to keep track of `O_n`.

Compiling the code is as simple as issuing the command `make` on a command line terminal (assuming a Unix environment) at the root of the theory folder. Compilation was tested on `gcc` version 9.3.0. Upon execution without parameters, as in

    ./table_Omin

you will get the following error message:

    Error:
        You did not set the path to MiniZinc's executable file.
        Change the value of the variable 'exe_file'.

Open the source file and change the value of the variable `exe_file` at line `82` with the appropriate path to the MiniZinc executable within your system. One might also want to change the value of the `out_dir` variable at line `80` with a more suitable path to a temporary directory. Executing again (and without parameters) should output anoher error message:

    Error: wrong number of parameters.
        ./table_Omin n

    Find the trees of 'n' vertices minimises Omega_min

which means that we must indicate the number of vertices of the trees for which we want to find `O_n`. Therefore, one can execute it like this:

    ./table_Omin 5

The program will crash for any value of `n` less than or equal to 2.

The results will be stored in a directory called `data/`. The most important results are in two files:
- `data/table_file.tsv`: contains a list of tabulator-separated columns with the following data

        n num_trees Omega_min trees_min D_min D_max
   
  where
  - `n` is the number of vertices of the tree,
  - `num_trees` is the number of trees of `n` vertices,
  - `Omega_min` is the value `O_n`,
  - `trees_min` is the amount of trees whose value of modified Omega is `O_n`, and
  - `D_min`, `D_max` are the minimum and maximum values of D of the tree(s) that yield the value `O_n`.

- `data/tree_file.txt`: this file contains all the trees that yield the value `O_n`. Each tree is described with the following format:
    
        0: 1 3
        1: 0 2
        2: 1
        3: 0

    which, as it is easy to see, each row indicates the set of neighbours of the vertex with index given before the colon `:`. Each set of _n_-vertex trees are separated by a row of asterisks `*`, and headed by the string `n= ` followed by the number of vertices of the trees. For example, by running the executable first with parameter `3` and then with parameter `4` the result is:
    
        ******************************************
        n= 3
        0: 1 2
        1: 0
        2: 0
        ******************************************
        ******************************************
        n= 4
        0: 1 3
        1: 0 2
        2: 1
        3: 0
        0: 1 2 3
        1: 0
        2: 0
        3: 0
        ******************************************

In a given execution, a third file is also produced. Such file is a log file where one can see a more verbose progress of the program, which also displays on standard output the percentage of trees processed.

### Preprocessing and Analysis

#### Processing of treebanks

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
```
If you wish to perform separate analysis on the parallel UD and SUD treebanks, also run:
```
java TreebankDataExtractor PUD <path to PUD> 
java TreebankDataExtractor PUD <path to PSUD> 
```
Finally, if you wish to perform analysis on UD and SUD (and their parallel versions) with function words removed, then you also need to run:
```
java TreebankDataExtractor UDcontent <path to UD> 
java TreebankDataExtractor UDcontent <path to SUD> 
java TreebankDataExtractor PUDcontent <path to PUD> 
java TreebankDataExtractor PUDcontent <path to PSUD> 
```
Depending on which of the above you run, the following directories will be created: for the first mentioned runs,
```
<path to UD>-merged-bylanguage
<path to UD>-merged-bylanguage-shuffled
<path to SUD>-merged-bylanguage
<path to SUD>-merged-bylanguage-shuffled
<path to HamleDT>-stanford-merged-bylanguage
<path to HamleDT>-stanford-merged-bylanguage-shuffled
<path to HamleDT>-prague-merged-bylanguage
<path to HamleDT>-prague-merged-bylanguage-shuffled
```
For the parallel treebanks,
```
<path to PUD>-merged-bylanguage
<path to PUD>-merged-bylanguage-shuffled
<path to PSUD>-merged-bylanguage
<path to PSUD>-merged-bylanguage-shuffled
```
And for the analysis without function words,
```
<path to UD>-content-merged-bylanguage
<path to UD>-content-merged-bylanguage-shuffled
<path to SUD>-content-merged-bylanguage
<path to SUD>-content-merged-bylanguage-shuffled
<path to PUD>-content-merged-bylanguage
<path to PUD>-content-merged-bylanguage-shuffled
<path to PSUD>-content-merged-bylanguage
<path to PSUD>-content-merged-bylanguage-shuffled
```
Each generated directory contains one .conll or .conllu file, one .headsf and one .headsu file per language. The folders for the parallel treebanks (PUD and PSUD) will also contain .headsr files. 

The .conll(u) file is a merging of all corpora of the language present in the collection and with a given annotation style, in the original format. The .headsu files contain compact representations of trees (after removing punctuation and null elements). The .headsf files are filtered versions of .headsu files that exclude trees with less than 3 nodes (useful, e.g. to feed to the program that calculates D_min). The .headsr files for parallel collections of treebanks are filtered versions that exclude sentences that have less than 3 words in *any* of the languages in the parallel treebanks - this ensures that the treebanks stay parallel after the filtering.

The directories containing the string "-content-" contain versions where UD function words have been removed, thus leaving only content words.

The directories suffixed by "-shuffled" contain versions of the same where each tree has been randomly shuffled. These can be used as sanity check or to obtain metrics in case where the linear arrangements of trees are random.

Of course, to process subsets of the collections (e.g. if one is interested in corpora from a single language, family, etc.), one can just create a folder with the relevant corpora (with the same subdirectory structure that comes in the UD and HamleDT downloads, i.e., for example, by just making a copy of the downloaded directory and deleting whatever languages/corpora are unneeded) and call java TreebankDataExtractor <UD/PUD/HamleDT> <relevant path>.

#### Generation of datasets

##### Procedure A

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

##### Procedure B

The steps above can be reproduced with a single [script](https://github.com/lluisalemanypuig/optimality-syntactic-dependency-distances/blob/master/generation_of_datasets_with_LAL/generate_data.py).

#### Analysis

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

The meaning of the columns is:
- Annotation: the treebank collection.
- Language: ISO 639-3 code for the language.
- Length: length of the sentences taken into consideration in this test. Rows with length=0 means that all lengths were included.
- Optimality: value of Omega in the attested sentences.
- Optimality_random: value of Omega in the randomized sentences.
- Right_p_value: right p-value of the Monte Carlo test for significance of Omega.
- Left_p_value: left p-value of the Monte Carlo test for significance of Omega.
- Rest of the columns: same as the last four but using other metrics instead of Omega: D, <d>=D/(n-1), Delta and Gamma.

5. For each of the collections, run:
```
java MonteCarloTestOmegaByLength all <iters> <collection-name> <path-to-omegas-files> >> path/to/test-output-file.txt
```
Where <iters> is the number of iterations for the Monte Carlo tests (100000 in the article), <collection-name> is an alphanumeric name to identify the collection (we suggest Stanford, Prague, UD26 and SUD26 for the respective collections) and <path-to-omegas-files> is the full path to the directory containing the omegas files for that collection.
 
This program (which may take a while, depending on number of iterations - starting with small numbers is recommended for testing - will perform Monte Carlo tests for each .omegas file in the given folder. Tests will be performed both for groups of sentences of a given length (rows where the length column has a value >= 3) and mixing sentences of all lengths (rows where the length column has the value 0).
