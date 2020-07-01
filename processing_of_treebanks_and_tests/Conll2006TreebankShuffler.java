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
import java.util.Arrays;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;

/**
 * Class used to shuffle the sentences in a CoNLL-X format treebank. 
 * Words are shuffled into random order, with the dependency structure kept.
 * @author carlos_gomez
 *
 */
public class Conll2006TreebankShuffler 
{

	
	public static boolean isInteger( String input ) 
	{
	    try 
	    {
	        Integer.parseInt( input );
	        return true;
	    }
	    catch( NumberFormatException e ) 
	    {
	        return false;
	    }
	}
	
	private static boolean mweWarningMessageShown = false;
	private static boolean conlluemptyWarningMessageShown = false;
	
	/**
	 * Input file has to be in CoNLL-X format.
	 * @param inputFile
	 * @param outputFile
	 */
	public static void shuffle ( File inputFile , File outputFile ) throws Throwable
	{
		
		BufferedReader br = new BufferedReader ( new InputStreamReader ( new FileInputStream(inputFile.getAbsolutePath()) , "UTF-8" ) );
		PrintWriter pw = new PrintWriter( new OutputStreamWriter( new FileOutputStream(outputFile) ) );
		String line;
		List<String> sentenceLines = new ArrayList<String>();
		List<String> outputLines = new ArrayList<String>();
		
		do
		{
			//read one sentence, adding the lines to list sentenceLines
			while ( (line=br.readLine()) != null && line.trim().length() > 0 )
			{
				//ignore CoNLL-U comments
				if ( line.trim().startsWith("#") ) continue; //CoNLL-U format comment line
				
				//ignore index ranges and empty indexes
				boolean ignoreThisLine=false;
				StringTokenizer st = new StringTokenizer(line.trim(),"\t");
				//1st token is index
				for ( int i = 0 ; i < 1 ; i++ )
				{
					String index = st.nextToken();
					if ( index.contains("-") )
					{
						if ( !mweWarningMessageShown )
							System.err.println("Warning: index range found in CoNLL-U file. Not prepared to deal with this, will ignore it. (Will only show this once per treebank even if it appears repeatedly).");
						mweWarningMessageShown = true;
						ignoreThisLine = true;
					}
					if ( index.contains(".") )
					{
						if ( !conlluemptyWarningMessageShown )
							System.err.println("Warning: empty element index found in CoNLL-U file. Not prepared to deal with this, will ignore it. (Will only show this once per treebank even if it appears repeatedly).");
						conlluemptyWarningMessageShown = true;
						ignoreThisLine = true;
					}
				}
				if ( ignoreThisLine ) continue; //and flag is set to false at the beginning of next iteration
				
				sentenceLines.add(line);
			}
			
			//System.out.println( sentenceLines.size() + " lines " );
			
			//choose a random permutation of n integers
			List<Integer> perm = new ArrayList<Integer>();
			for ( int i = 0 ; i < sentenceLines.size() ; i++ )
			{
				perm.add(i+1);
			}
			Collections.shuffle(perm);
			
			//initialize outputLines to be a list of the same size as sentenceLines (this is just so that we can later use List.set)
			for ( int i = 0 ; i < sentenceLines.size() ; i++ )
			{
				outputLines.add(null);
			}
			
			//take each line, change the head and phead columns according to permutation, and move resulting line to output position
			for ( int i = 0 ; i < sentenceLines.size() ; i++ )
			{
				StringTokenizer st = new StringTokenizer(sentenceLines.get(i),"\t");
				StringBuilder outputLine = new StringBuilder();
				st.nextToken(); outputLine.append(perm.get(i)); outputLine.append("\t"); //index column (now becoming the target index in the permutation)
				outputLine.append(st.nextToken()); outputLine.append("\t"); //form column
				outputLine.append(st.nextToken()); outputLine.append("\t"); //lemma column
				outputLine.append(st.nextToken()); outputLine.append("\t"); //cpostag column
				outputLine.append(st.nextToken()); outputLine.append("\t"); //postag column
				outputLine.append(st.nextToken()); outputLine.append("\t"); //feats column
				String head = st.nextToken(); //head column (changed according to permutation)
				if ( head.trim().equals("0") ) outputLine.append("0"); //dummy root doesn't change
				else outputLine.append(perm.get(Integer.valueOf(head)-1));
				outputLine.append("\t");
				outputLine.append(st.nextToken());  outputLine.append("\t"); //deprel column
				String phead; //phead or deps column (if phead, changed according to permutation) or nothing (hamledt treebanks have nothing here)
				if ( st.hasMoreTokens() )
					phead = st.nextToken(); 
				else
					phead = "_";
				if ( isInteger(phead) ) //in CoNLL-U maybe it's not phead but deps field, so we need to check this
				{
					if ( phead.trim().equals("0") ) outputLine.append("0");
					else outputLine.append(perm.get(Integer.valueOf(phead)-1));
				}
				else outputLine.append(phead);
				
				outputLine.append("\t");
				if ( st.hasMoreTokens() ) outputLine.append(st.nextToken()); //pdeprel or misc column
				else outputLine.append("_");
				outputLines.set(perm.get(i)-1,outputLine.toString()); //put line into position
			}
			
			//actually output the lines
			for ( int i = 0 ; i < outputLines.size() ; i++ )
			{
				pw.print(outputLines.get(i)+"\n"); //unix format newline
			}
			if ( outputLines.size() > 0 ) pw.print("\n"); //unix format newline

			//clear arrays for next sentence
			sentenceLines.clear();
			outputLines.clear();
			
		} while (line!=null);
		
		br.close();
		pw.close();
		
	}
	
	static String directory = //"C:\\Treebanks\\conll-x-concatenated";
			//"D:\\Treebanks\\hamledt-2.0-conll-concatenated";
			//"C:\\Treebanks\\ud-2.0-concatenated";
						//"C:\\Treebanks\\universal-2.0-std-concatenated";
			"D:\\Treebanks\\ud-treebanks-v2.6\\merged-bylanguage";
	
	static String extension = ".conllu"; //change between .conll and .conllu as needed
	
	public static void main ( String[] args ) throws Throwable
	{
		
		File f = new File(directory);
		
		ArrayList<File> files = new ArrayList<File>(Arrays.asList(f.listFiles()));
		
		for ( Iterator<File> it = files.iterator() ; it.hasNext() ; )
		{
			File currentInputFile = it.next();
			if ( currentInputFile.getName().endsWith(extension) && !currentInputFile.getName().endsWith("shuffled"+extension) )
			{
				String inputAbsPath = currentInputFile.getAbsolutePath();
				String outputAbsPath = inputAbsPath.substring(0, inputAbsPath.length()-extension.length()) + "-shuffled"+extension;
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
				shuffle(currentInputFile,outputFile);
			}
		}
		
	}
	
	
}
