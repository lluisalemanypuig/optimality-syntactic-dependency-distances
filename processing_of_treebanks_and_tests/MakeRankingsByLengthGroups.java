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
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Comparator;

/*
 * This class reads a file containing treebanks metrics for various treebanks ordered by length groups.
 * It computes rankings of the omega metric among each length for which there is at least one sentence in each treebank.
 * Format of the file:
 * Processing file: path to treebank 1
 * line
 * line
 * line
 * Processing file: path to treebank 2
 * line
 * line
 * line
 * ...
 * Format of each of the above lines (space-separated):
 * irrelevant length #sentences length length irrelevant avg(D) median(D) stdev(D) avg(D_min) median(D_min) stdev(D_min) avg(D_random) median(D_random) stdev(D_random) avg(omega) median(omega) stdev(omega)
 */

class TreebankComparatorByOmegaAtGivenLength implements Comparator<TreebankEntry>
{
	
	private int length;
	
	public TreebankComparatorByOmegaAtGivenLength ( int length )
	{
		this.length = length;
	}

	public int compare(TreebankEntry o1, TreebankEntry o2) 
	{
		double omega1 = o1.getLengthEntry(length).metrics[9];
		double omega2 = o2.getLengthEntry(length).metrics[9];
		return -Double.compare(omega1,omega2); //sort descending
	}
	
}

public class MakeRankingsByLengthGroups 
{
	
	//simplify names of the treebanks removing common prefix/suffix, just for pretty-printing, not important
	private static void simplifyNames ( List<TreebankEntry> treebanks )
	{
		boolean removeFirstChar = true;
		boolean removeLastChar = true;
		String charString = null;
		for ( Iterator<TreebankEntry> it = treebanks.iterator() ; it.hasNext() ; )
		{
			TreebankEntry treebank = it.next();
			if ( charString == null )
				charString = treebank.getName().substring(0,1);
			else if ( !treebank.getName().substring(0,1).equals(charString) )
			{
				removeFirstChar = false;
				break;
			}
		}
		charString = null;
		for ( Iterator<TreebankEntry> it = treebanks.iterator() ; it.hasNext() ; )
		{
			TreebankEntry treebank = it.next();
			if ( charString == null )
				charString = treebank.getName().substring(treebank.getName().length()-1,treebank.getName().length());
			else if ( !treebank.getName().substring(treebank.getName().length()-1,treebank.getName().length()).equals(charString) )
			{
				removeLastChar = false;
				break;
			}
		}
		if ( removeFirstChar )
		{
			for ( Iterator<TreebankEntry> it = treebanks.iterator() ; it.hasNext() ; )
			{
				TreebankEntry treebank = it.next();
				treebank.setName(treebank.getName().substring(1));
			}
		}
		if ( removeLastChar )
		{
			for ( Iterator<TreebankEntry> it = treebanks.iterator() ; it.hasNext() ; )
			{
				TreebankEntry treebank = it.next();
				treebank.setName(treebank.getName().substring(0,treebank.getName().length()-1));
			}
		}
		if (removeFirstChar || removeLastChar) //keep doing it recursively
			simplifyNames(treebanks);
	}
	
	
	public static ArrayList<TreebankEntry> readTreebanks ( BufferedReader br ) throws IOException
	{
		return readTreebanks(br,12); /*12 is the value required for lines generated by compute_optimality_detailed_metrics*/
		/*33 for compute_detailed_metrics*/
	}
	
	public static ArrayList<TreebankEntry> readTreebanks ( BufferedReader br , int numMetrics ) throws IOException
	{
		String line;
		ArrayList<TreebankEntry> treebanks = new ArrayList<TreebankEntry>();
		TreebankEntry currentTreebank = null;
		
		while ( (line=br.readLine()) != null && line.length() != 0 )
		{
			if ( !Character.isDigit(line.charAt(0)) ) //we have a corpus name 
			{
				currentTreebank = new TreebankEntry(line);
				treebanks.add(currentTreebank);
			}
			else //we have a length entry for the current treebank
			{
				LengthGroupEntry lge = new LengthGroupEntry(line,numMetrics);
				currentTreebank.setLengthEntry(lge.getLength(), lge);
			}
		}
		br.close();
		simplifyNames(treebanks);
		return(treebanks);
	}

	
	public static void process ( File inputFile , File outputFile ) throws Throwable
	{
		
		BufferedReader br = new BufferedReader ( new InputStreamReader ( new FileInputStream(inputFile.getAbsolutePath()) /*, "UTF-8"*/ ) ); //file generated by c++
		//PrintWriter pw = new PrintWriter( new OutputStreamWriter( new FileOutputStream(outputFile) ) );
		ArrayList<TreebankEntry> treebanks = readTreebanks(br);
		
		System.out.println("I got the following: ");
		for ( Iterator<TreebankEntry> it = treebanks.iterator() ; it.hasNext() ; )
		{
			TreebankEntry treebank = it.next();
			System.out.println(treebank.getName());
			for ( Iterator<LengthGroupEntry> it2 = treebank.iterator() ; it2.hasNext() ; )
			{
				LengthGroupEntry lge = it2.next();
				System.out.println(lge);
			}
		}
		
		//count treebanks that have a given length
		int[] treebanksThatHaveLength = new int[300]; 
		for ( int i = 0 ; i < treebanksThatHaveLength.length ; i++ )
		{
			treebanksThatHaveLength[i] = 0;
		}
		for ( Iterator<TreebankEntry> it = treebanks.iterator() ; it.hasNext() ; )
		{
			TreebankEntry treebank = it.next();
			for ( int i = 0 ; i < treebanksThatHaveLength.length ; i++ )
			{
				if ( treebank.getLengthEntry(i) != null )
				{
					treebanksThatHaveLength[i]++;
				}
			}
		}
		for ( int i = 0 ; i < treebanksThatHaveLength.length ; i++ )
		{
			System.out.println("Length "+i+": "+treebanksThatHaveLength[i]);
		}
		
		
		//each element is a list with the corresponding treebank's ranks
		Map<String,ArrayList<Integer>> treebankRanks = new LinkedHashMap<String,ArrayList<Integer>>();
		
		//process lengths that appear in all the treebanks:
		for ( int i = 0 ; i < treebanksThatHaveLength.length ; i++ )
		{
			if ( treebanksThatHaveLength[i] == treebanks.size() )
			{
				//process length i
				List<TreebankEntry> clonedTreebanks = (List<TreebankEntry>) treebanks.clone();
				Collections.sort(clonedTreebanks,new TreebankComparatorByOmegaAtGivenLength(i));
				System.out.println("For sentences of length " + i);
				for ( int t = 0 ; t < clonedTreebanks.size() ; t++ )
				{
					System.out.println((t+1)+". "+clonedTreebanks.get(t).getName() + "(" + clonedTreebanks.get(t).getLengthEntry(i).metrics[9] +")");
					if ( treebankRanks.get(clonedTreebanks.get(t).getName()) == null )
						treebankRanks.put(clonedTreebanks.get(t).getName(), new ArrayList<Integer>());
					treebankRanks.get(clonedTreebanks.get(t).getName()).add(t+1);
				}
			}
		}
		
		//output ranks
		for ( int i = 0 ; i < treebanks.size() ; i++ )
		{
			System.out.println(treebanks.get(i).getName());
			System.out.println(treebankRanks.get(treebanks.get(i).getName()));
		}
		
		//output rank stats
		for ( int i = 0 ; i < treebanks.size() ; i++ )
		{
			System.out.print(treebanks.get(i).getName()+" ");
			List<Integer> ranks = (treebankRanks.get(treebanks.get(i).getName()));
			
			//convert ranks to double... java generics, duh
			List<Double> doubleRanks = new ArrayList<Double>();
			for ( int j = 0 ; j < ranks.size() ; j++ )
			{
				doubleRanks.add((double)ranks.get(j));
			}
			
			System.out.print(Collections.min(doubleRanks));
			System.out.print(" ");
			System.out.print(Collections.max(doubleRanks));
			System.out.print(" ");
			System.out.print(Statistics.overflowSafeAverage(doubleRanks));
			System.out.print(" ");
			System.out.print(Statistics.median(doubleRanks));
			System.out.print(" ");
			System.out.print(Statistics.overflowSafeStDev(doubleRanks));
			System.out.println();
		}
		
	}
	
	//test
	public static void main ( String[] args ) throws Throwable
	{
		//File inputFile = new File("C:\\CPlusPlusProjects\\crossing-counts\\hamledt_stanford_omega_by_length.txt");
		//File inputFile = new File("F:\\Dropbox\\crossing_counts\\len_groups_omega_csv_shiloach_hamledt_prague_only.txt");
		File inputFile = new File("C:\\Users\\Carlos\\Dropbox\\crossing_counts\\len_groups_ud_omega_csv_shiloach.txt");
		File outputFile = null;
		process ( inputFile , outputFile );
	}
	
}
