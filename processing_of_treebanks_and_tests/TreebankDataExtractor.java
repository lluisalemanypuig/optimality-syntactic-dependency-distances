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
 *******************************************************************************/
import java.io.File;
import java.io.IOException;
import java.util.StringTokenizer;

/**
 * This class calls several other classes to perform treebank merging, data extraction, shuffling and conversion to head sequences (.heads2 files),
 * making the process easier to use.
 * 
 * Usage: java TreebankDataExtractor <UD/PUD/UDcontent/PUDcontent/HamleDT> <path/to/corpora>
 * 
 * Will create folders with same name as the path to the corpora, but with
 * the suffixes "-merged", "-merged-bylanguage" and "-merged-bylanguage-shuffled".
 * 
 * With the UDcontent/PUDcontent options (remove UD function words), folders will
 * be named "-content-merged", "-content-merged-bylanguage" and "-content-merged-bylanguage-shuffled".
 * 
 * @author carlos
 * @date 2020-06-26
 */
public class TreebankDataExtractor 
{
	
	private static void printUsage()
	{
		System.err.println("Usage: java TreebankDataExtractor <UD/UDcontent/PUD/PUDcontent/HamleDT> <path/to/corpora>");
	}
	
	private static String separator = "-";

	
	private static void deleteDirectoryRecursively(File file) throws IOException 
	{
		if (file.isDirectory()) 
		{
			File[] entries = file.listFiles();
			if (entries != null) 
			{
				for (File entry : entries) 
				{
					deleteDirectoryRecursively(entry);
				}
			}
		}
		if (!file.delete()) 
		{
			throw new IOException("Failed to delete " + file);
		}
	}
	
	
	
	public static void processHamleDT ( String folderName ) throws Throwable
	{
		HamleDTTreebankJoiner.pathToHamleDT = folderName;
		HamleDTTreebankJoiner.pathToOutput = folderName + separator + "merged-bylanguage";
		System.out.println("Merging HamleDT CoNLL files to one per treebank and annotation style...");
		HamleDTTreebankJoiner.main(new String[0]);
		System.out.println("Done. Written to " + HamleDTTreebankJoiner.pathToOutput);
		System.out.println("Deleting excess files...");
		File[] myFiles = new File(HamleDTTreebankJoiner.pathToOutput).listFiles();
		for ( int i = 0 ; i < myFiles.length ; i++ )
		{
			if ( myFiles[i].isFile() && myFiles[i].getName().endsWith("test.conll") || myFiles[i].isFile() && myFiles[i].getName().endsWith("train.conll") )
			{
				myFiles[i].delete();
			}
		}
		System.out.println("Done.");
		Conll2006TreebankShuffler.directory = HamleDTTreebankJoiner.pathToOutput;
		Conll2006TreebankShuffler.extension = ".conll";
		System.out.println("Creating shuffled version of HamleDT CoNLL files...");
		Conll2006TreebankShuffler.main(new String[0]);
		System.out.println("Done. Written to " + HamleDTTreebankJoiner.pathToOutput);
		System.out.println("Moving shuffled files to their own directory...");
		File shuffledPath = new File(folderName + separator + "merged-bylanguage-shuffled");
		shuffledPath.mkdirs();
		myFiles = new File(Conll2006TreebankShuffler.directory).listFiles();
		for ( int i = 0 ; i < myFiles.length ; i++ )
		{
			if ( myFiles[i].isFile() && myFiles[i].getName().contains("shuffled") )
			{
				myFiles[i].renameTo(new File(myFiles[i].getAbsolutePath().replace("merged-bylanguage","merged-bylanguage-shuffled")));
			}
		}
		System.out.println("Done. Written to " + shuffledPath);
		
		ConvertToHeadSequences.E2_FILTER = false;
		ConvertToHeadSequences.OMEGA_FILTER = true;
		ConvertToHeadSequences.punctuationCriterion = LabelledDependencyStructure.HAMLEDT_PUNCTUATION_CRITERION_PLUS_NULL_ELEMENTS;
		ConvertToHeadSequences.replaceInvalidWithPlaceholders = false;
		ConvertToHeadSequences.removeFunctionWords = false;
		ConvertToHeadSequences.directory=HamleDTTreebankJoiner.pathToOutput;
		System.out.println("Generating " + ConvertToHeadSequences.OMEGA_FILTER_EXTENSION + " files for regular files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + ConvertToHeadSequences.directory);
		ConvertToHeadSequences.directory=shuffledPath.getAbsolutePath();
		System.out.println("Generating " + ConvertToHeadSequences.OMEGA_FILTER_EXTENSION + " files for shuffled files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + shuffledPath.getAbsolutePath());
		ConvertToHeadSequences.OMEGA_FILTER=false;
		ConvertToHeadSequences.directory=HamleDTTreebankJoiner.pathToOutput;
		System.out.println("Generating " + ConvertToHeadSequences.UNFILTERED_EXTENSION + " files for regular files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + ConvertToHeadSequences.directory);
		ConvertToHeadSequences.directory=shuffledPath.getAbsolutePath();
		System.out.println("Generating " + ConvertToHeadSequences.UNFILTERED_EXTENSION + " files for shuffled files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + shuffledPath.getAbsolutePath());
		
		File stanfordRegularPath = new File(folderName + separator + "stanford-merged-bylanguage");
		File pragueRegularPath = new File(folderName + separator + "prague-merged-bylanguage");
		File stanfordShuffledPath = new File(folderName + separator + "stanford-merged-bylanguage-shuffled");
		File pragueShuffledPath = new File(folderName + separator + "prague-merged-bylanguage-shuffled");
		stanfordRegularPath.mkdirs();
		pragueRegularPath.mkdirs();
		stanfordShuffledPath.mkdirs();
		pragueShuffledPath.mkdirs();
		System.out.println("Separating Prague from Stanford regular files...");
		myFiles = new File(HamleDTTreebankJoiner.pathToOutput).listFiles();
		for ( int i = 0 ; i < myFiles.length ; i++ )
		{
			if ( myFiles[i].isFile() && myFiles[i].getName().startsWith("conll") )
			{
				myFiles[i].renameTo(new File(myFiles[i].getAbsolutePath().replace("merged-bylanguage","prague-merged-bylanguage")));
			}
			else if ( myFiles[i].isFile() && myFiles[i].getName().startsWith("stanford") )
			{
				myFiles[i].renameTo(new File(myFiles[i].getAbsolutePath().replace("merged-bylanguage","stanford-merged-bylanguage")));
			}
		}
		System.out.println("Done. Written to " + stanfordRegularPath + " and " + pragueRegularPath);
		System.out.println("Separating Prague from Stanford shuffled files...");
		myFiles = shuffledPath.listFiles();
		for ( int i = 0 ; i < myFiles.length ; i++ )
		{
			if ( myFiles[i].isFile() && myFiles[i].getName().startsWith("conll") )
			{
				myFiles[i].renameTo(new File(myFiles[i].getAbsolutePath().replace("merged-bylanguage","prague-merged-bylanguage")));
			}
			else if ( myFiles[i].isFile() && myFiles[i].getName().startsWith("stanford") )
			{
				myFiles[i].renameTo(new File(myFiles[i].getAbsolutePath().replace("merged-bylanguage","stanford-merged-bylanguage")));
			}
		}
		System.out.println("Done. Written to " + stanfordShuffledPath + " and " + pragueShuffledPath);
		System.out.println("Deleting " + HamleDTTreebankJoiner.pathToOutput + "...");
		deleteDirectoryRecursively(new File(HamleDTTreebankJoiner.pathToOutput));
		System.out.println("Deleting " + shuffledPath + "...");
		deleteDirectoryRecursively(shuffledPath);
		System.out.println("Done.");
		
	}
	
	public static void processUD ( String folderName , boolean parallel , boolean removeFunctionWords ) throws Throwable
	{
		UniversalDependenciesTreebankJoiner.pathToUD = folderName;
		UniversalDependenciesTreebankJoiner.pathToOutput = folderName + separator + (removeFunctionWords?"content-":"") + "merged";
		System.out.println("Merging UD CoNLLU files to one per treebank...");
		UniversalDependenciesTreebankJoiner.main(new String[0]);
		System.out.println("Done. Written to " + UniversalDependenciesTreebankJoiner.pathToOutput);
		UniversalDependenciesJoinerByLanguage.inputPath = UniversalDependenciesTreebankJoiner.pathToOutput;
		UniversalDependenciesJoinerByLanguage.outputPath = folderName + separator + (removeFunctionWords?"content-":"") + "merged-bylanguage";
		System.out.println("Merging UD CoNLLU files to one per language...");
		UniversalDependenciesJoinerByLanguage.main(new String[0]);
		System.out.println("Done. Written to " + UniversalDependenciesJoinerByLanguage.outputPath);
		Conll2006TreebankShuffler.directory = UniversalDependenciesJoinerByLanguage.outputPath;
		Conll2006TreebankShuffler.extension = ".conllu";
		System.out.println("Creating shuffled version of UD CoNLLU files...");
		Conll2006TreebankShuffler.main(new String[0]);
		System.out.println("Done. Written to " + Conll2006TreebankShuffler.directory);
		System.out.println("Moving shuffled files to their own directory...");
		File shuffledPath = new File(folderName + separator + (removeFunctionWords?"content-":"") + "merged-bylanguage-shuffled");
		shuffledPath.mkdirs();
		File[] myFiles = new File(Conll2006TreebankShuffler.directory).listFiles();
		for ( int i = 0 ; i < myFiles.length ; i++ )
		{
			if ( myFiles[i].isFile() && myFiles[i].getName().contains("shuffled") )
			{
				myFiles[i].renameTo(new File(myFiles[i].getAbsolutePath().replace("merged-bylanguage","merged-bylanguage-shuffled")));
			}
		}
		System.out.println("Done. Written to " + shuffledPath);
		
		ConvertToHeadSequences.E2_FILTER = false;
		ConvertToHeadSequences.OMEGA_FILTER = true;
		ConvertToHeadSequences.punctuationCriterion = LabelledDependencyStructure.UNIVERSAL_DEPENDENCIES_PUNCTUATION_CRITERION;
		ConvertToHeadSequences.removeFunctionWords = removeFunctionWords;
		ConvertToHeadSequences.replaceInvalidWithPlaceholders = false;
		if ( removeFunctionWords )
			ConvertToHeadSequences.functionWordCriterion = LabelledDependencyStructure.UNIVERSAL_DEPENDENCIES_FUNCTION_WORD_CRITERION;
		ConvertToHeadSequences.directory=UniversalDependenciesJoinerByLanguage.outputPath;
		System.out.println("Generating " + ConvertToHeadSequences.OMEGA_FILTER_EXTENSION + " files for regular files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + ConvertToHeadSequences.directory);
		ConvertToHeadSequences.directory=shuffledPath.getAbsolutePath();
		System.out.println("Generating " + ConvertToHeadSequences.OMEGA_FILTER_EXTENSION + " files for shuffled files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + shuffledPath.getAbsolutePath());
		ConvertToHeadSequences.OMEGA_FILTER=false;
		ConvertToHeadSequences.replaceInvalidWithPlaceholders = removeFunctionWords; //in the unfiltered files for reparallelization, we replace invalid trees with placeholders.
		ConvertToHeadSequences.directory=UniversalDependenciesJoinerByLanguage.outputPath;
		System.out.println("Generating " + ConvertToHeadSequences.UNFILTERED_EXTENSION + " files for regular files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + ConvertToHeadSequences.directory);
		ConvertToHeadSequences.directory=shuffledPath.getAbsolutePath();
		System.out.println("Generating " + ConvertToHeadSequences.UNFILTERED_EXTENSION + " files for shuffled files...");
		ConvertToHeadSequences.main(new String[0]);
		System.out.println("Done. Written to " + shuffledPath.getAbsolutePath());
		System.out.println("Deleting " + UniversalDependenciesJoinerByLanguage.inputPath + "...");
		deleteDirectoryRecursively(new File(UniversalDependenciesJoinerByLanguage.inputPath));
		System.out.println("Done.");
		
		//reparallelize treebanks if they are parallel (PUD).
		if ( parallel )
		{
			System.out.println("Reparallelizing regular files into " + ConvertToHeadSequences.REPARALLELIZED_EXTENSION + " files...");
			Reparallelizer.reparallelize(UniversalDependenciesJoinerByLanguage.outputPath);
			System.out.println("Done.");
			System.out.println("Reparallelizing shuffled files into " + ConvertToHeadSequences.REPARALLELIZED_EXTENSION + " files...");
			Reparallelizer.reparallelize(shuffledPath.getAbsolutePath());
			System.out.println("Done.");
		}
		
		ConvertToHeadSequences.replaceInvalidWithPlaceholders = false;
		ConvertToHeadSequences.removeFunctionWords = false;
		
	}
	
	public static void main ( String[] args ) throws Throwable
	{
		if ( args.length < 2 )
		{
			printUsage(); 
			System.exit(1);
		}
		if ( "UD".equalsIgnoreCase(args[0]) )
		{
			processUD ( args[1] , false , false );
		}
		else if ( "PUD".equalsIgnoreCase(args[0]) )
		{
			processUD ( args[1] , true , false  );
		}
		else if ( "UDcontent".equalsIgnoreCase(args[0]) )
		{
			processUD ( args[1] , false , true  );
		}
		else if ( "PUDcontent".equalsIgnoreCase(args[0]) )
		{
			processUD ( args[1] , true , true  );
		}
		else if ( "HamleDT".equalsIgnoreCase(args[0]) )
		{
			processHamleDT ( args[1] );
		}
		else
		{
			printUsage();
			System.exit(1);
		}
	}

}
