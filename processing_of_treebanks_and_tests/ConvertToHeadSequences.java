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
import java.io.PrintWriter;
import java.io.OutputStreamWriter;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

/**
 * Convert a directory of .conll treebanks into head sequences.
 * @author Carlos
 *
 */
public class ConvertToHeadSequences 
{

	//Right now, works in CoNLL-X format. Add formats.
	
	
	static String E2_FILTER_EXTENSION = ".heads";
	static String OMEGA_FILTER_EXTENSION = ".headsf";
	static String UNFILTERED_EXTENSION = ".headsu";
	static String REPARALLELIZED_EXTENSION = ".headsr";
	
	
	
	//filter to calculate E2 of the number of crossings: remove trees of length < 4, non-trees and star trees.
	static boolean E2_FILTER = false;
	
	//filter to calculate omega (optimality): remove trees of length < 3 and non-trees.
	static boolean OMEGA_FILTER = true;
	
	
	private static double quotient ( int a , int b )
	{
		return (double)a / (double)b;
	}
	
	/*conversion stats*/
	private static int nTotal = 0;
	private static int nSyntacticPunct = 0;
	private static int nShort = 0;
	private static int nNonTree = 0;
	private static int nStarTree = 0;
	private static int nAfterFilters = 0;
	
	private static int sumInitialLengths = 0;
	private static int sumInitialC = 0;
	private static int sumMiddleC = 0;
	private static int sumMiddleLengths = 0;
	private static int sumFinalLengths = 0;
	private static int sumFinalC = 0;
	
	/**
	 * Returns null if lds doesn't pass the filters, the preprocessed structure if it does.
	 * Also, stats are updated.
	 * @param lds
	 */
	public static LabelledDependencyStructure preprocess ( LabelledDependencyStructure lds , boolean removeFunctionWords )
	{
		lds = lds.removePunctuation();
		if ( removeFunctionWords ) lds = lds.removeFunctionWords();
		
		sumMiddleLengths += lds.getNumberOfNodes();
		sumMiddleC += lds.getNumberOfCrossings(false);
		
		if ( E2_FILTER && lds.getNumberOfNodes() < 4 )
		{
			nShort++;
			return null;
		}
		
		if ( OMEGA_FILTER && lds.getNumberOfNodes() < 3 )
		{
			nShort++;
			return null;
		}
		
		if ( !lds.isTree() )
		{
			nNonTree++;
			return null;
		}
		
		if ( E2_FILTER && lds.isStarTree() )
		{
			nStarTree++;
			return null;
		}
		
		//retained
		return lds;
	}
	
	/**
	 * Right now, input file has to be in CoNLL-X format. Add formats.
	 * @param inputFile
	 * @param outputFile
	 * @return Array of doubles to make exporting stats to Excel easier.
	 */
	public static double[] processFiles ( File inputFile , File outputFile , boolean removeFunctionWords ) throws Throwable
	{
		Conll2006LabelledDependencyTreebankReader myReader = 
				new Conll2006LabelledDependencyTreebankReader(
						inputFile.getAbsolutePath()
						);
		
		PrintWriter pw = new PrintWriter( new OutputStreamWriter( new FileOutputStream(outputFile) ) );
		
		Iterator<LabelledDependencyStructure> iter;
		
		nTotal = 0;
		nSyntacticPunct = 0;
		nShort = 0;
		nNonTree = 0;
		nStarTree = 0;
		nAfterFilters = 0;
		
		sumInitialLengths = 0;
		sumInitialC = 0;
		sumMiddleC = 0;
		sumMiddleLengths = 0;
		sumFinalLengths = 0;
		sumFinalC = 0;
		
		for ( iter = myReader.getIterator() ; iter.hasNext() ; )
		{
			
			LabelledDependencyStructure lds = iter.next();
			nTotal++;
			
			//System.out.println("Processing " + lds);
			
			if ( lds.hasSyntacticallyRelevantPunctuation() )
			{
				nSyntacticPunct++;
				//System.out.println("Relevant syntactic punctuation: " + lds);
				//System.out.println("Removing it: " + lds.removePunctuation());
			}
				
			sumInitialLengths += lds.getNumberOfNodes();
			sumInitialC += lds.getNumberOfCrossings(false);
			
			lds = preprocess(lds,removeFunctionWords);
			
			if ( lds != null && lds.contents.length > 0 ) //we don't output sentences that end up being length 0 after removing punctuation and such.
			{
			
				//System.out.println("Retained");
				
				nAfterFilters++;
				
				sumFinalLengths += lds.getNumberOfNodes();
				sumFinalC += lds.getNumberOfCrossings(false);
				
				pw.println(lds.getHeadSequence());
			
			}
			
		}
		
		System.out.println("Total structures: " + nTotal);
		System.out.println("With syntactic punctuation: " + nSyntacticPunct);
		System.out.println("Too short: " + nShort);
		System.out.println("Non-trees: " + nNonTree);
		System.out.println("Star trees: "+ nStarTree);
		System.out.println("Retained after filters: " + nAfterFilters);
		System.out.println("Mean length of raw sentences: " + quotient(sumInitialLengths,nTotal));
		System.out.println("Mean length after punctuation removal: " + quotient(sumMiddleLengths,nTotal));
		System.out.println("Mean length after filters: " + quotient(sumFinalLengths,nAfterFilters));
		System.out.println("Mean crossings of raw sentences: " + quotient(sumInitialC,nTotal));	
		System.out.println("Mean crossings after punctuation removal: " + quotient(sumMiddleC,nTotal));	
		System.out.println("Mean crossings after filters: " + quotient(sumFinalC,nAfterFilters));	
		
		double[] csvStats = null;
		
		if ( E2_FILTER )
		{
			csvStats  = new double[7];
			csvStats[0] = nTotal;
			csvStats[1] = quotient(sumInitialLengths,nTotal);
			csvStats[2] = quotient(sumInitialC,nTotal);
			csvStats[3] = nShort;
			csvStats[4] = nNonTree;
			csvStats[5] = nStarTree;
			csvStats[6] = nAfterFilters;
		
		}
		else if ( OMEGA_FILTER )
		{
			csvStats = new double[5];
			csvStats[0] = nTotal;
			csvStats[1] = quotient(sumInitialLengths,nTotal);
			csvStats[2] = nShort;
			csvStats[3] = nNonTree;
			csvStats[4] = nAfterFilters;
		}
		else //no filter
		{
			csvStats = new double[5];
			csvStats[0] = nTotal;
			csvStats[1] = quotient(sumInitialLengths,nTotal);
			csvStats[2] = nShort;
			csvStats[3] = nNonTree;
			csvStats[4] = nAfterFilters;
		}
		
		pw.close();
		
		return csvStats;
		
	}

	static String directory = //"C:\\Treebanks\\conll-x-concatenated";
			//"C:\\Treebanks\\universal-2.0-std-concatenated";
			//"C:\\Treebanks\\conll-x-concatenated-shuffled";
			//"C:\\Treebanks\\universal-2.0-std-concatenated-shuffled";
			//"I:\\Treebanks\\hamledt-2.0-stanford-concatenated-testcopy";
			//"I:\\Treebanks\\ud-treebanks-conll2017-flat";
			//"F:\\Treebanks\\ud-2.0-concatenated-bylanguage";
			//"D:\\Treebanks\\ud-treebanks-v2.5\\merged-bylanguage";
			"D:\\Treebanks\\ud-treebanks-v2.6\\merged-bylanguage";
	
	//for UD we use this criterion:
	static int punctuationCriterion = LabelledDependencyStructure.UNIVERSAL_DEPENDENCIES_PUNCTUATION_CRITERION;
	
	//for HamleDT we use this one:
	//static int punctuationCriterion = LabelledDependencyStructure.HAMLEDT_PUNCTUATION_CRITERION_PLUS_NULL_ELEMENTS;
	
	//for UD we use this criterion:
	static int functionWordCriterion = LabelledDependencyStructure.UNIVERSAL_DEPENDENCIES_FUNCTION_WORD_CRITERION;
	
	//set to true to remove function words
	static boolean removeFunctionWords = false;
	
	public static void main ( String[] args ) throws Throwable
	{
		
		LabelledDependencyStructure.setPunctuationCriterion(punctuationCriterion);
		
		File f = new File(directory);
		
		ArrayList<File> files = new ArrayList<File>(Arrays.asList(f.listFiles()));
		
		List<double[]> statsToPrintOut = new ArrayList<double[]>();
		
		for ( Iterator<File> it = files.iterator() ; it.hasNext() ; )
		{
			File currentInputFile = it.next();
			if ( currentInputFile.getName().endsWith(".conll") || currentInputFile.getName().endsWith(".conllu") )
			{
				String inputAbsPath = currentInputFile.getAbsolutePath();
				String extension = OMEGA_FILTER?OMEGA_FILTER_EXTENSION:E2_FILTER?E2_FILTER_EXTENSION:UNFILTERED_EXTENSION; //legacy extensions were heads for e2filter, heads2 for omega filter, heads 3 for no filter
																	
				String outputAbsPath;
				if ( currentInputFile.getName().endsWith(".conll") )
					outputAbsPath = inputAbsPath.substring(0, inputAbsPath.length()-6) + extension;
				else //conllu: 7 chars
					outputAbsPath = inputAbsPath.substring(0, inputAbsPath.length()-7) + extension;
				File outputFile = new File(outputAbsPath);
				if ( outputFile.exists() )
				{
					System.err.println("Output file " + outputFile + " already exists, skipping.");
					continue;
				}
				try 
				{
					outputFile.createNewFile();
				} 
				catch (IOException e) 
				{
					e.printStackTrace();
					continue;
				}
				System.out.println("=====================");
				System.out.println("Processing input file: " + currentInputFile);
				System.out.println("Output file: " + outputFile);
				double[] csvStats = processFiles(currentInputFile,outputFile,removeFunctionWords);
				statsToPrintOut.add(csvStats);
			}
		}
		
		System.out.println("CSV STATS");
		for ( Iterator<double[]> it = statsToPrintOut.iterator(); it.hasNext() ; )
		{
			double[] rowOfStats = it.next();
			for ( int i = 0 ; i < rowOfStats.length ; i++ )
			{
				System.out.print(rowOfStats[i]);
				if ( i < rowOfStats.length - 1 ) System.out.print(" ");
			}
			System.out.println();
		}
		
	}
	
}
