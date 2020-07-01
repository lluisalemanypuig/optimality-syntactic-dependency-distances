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
