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
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Reader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;
import java.util.zip.GZIPInputStream;

/**
 * Reads a directory with UD treebanks. 
 * Joins all the files into a single file per (language, train/test) pair or just per language.
 * @author carlos_gomez
 *
 */
public class UniversalDependenciesTreebankJoiner 
{

	static String pathToUD = "D:\\Treebanks\\ud-treebanks-v2.6";
	static String pathToOutput = "D:\\Treebanks\\ud-treebanks-v2.6\\merged";
	
	public static void main ( String[] args ) throws Throwable
	{
		File generalPath = new File(pathToUD);
		File outputPath = new File(pathToOutput);
		if ( !outputPath.mkdirs() ) //create directory for output
		{
			System.err.println("Could not create output directory " + pathToOutput);
			System.exit(1);
		}
		ArrayList<File> files = new ArrayList<File>(Arrays.asList(generalPath.listFiles()));
		for ( Iterator<File> it = files.iterator() ; it.hasNext() ; )
		{
			File currentLanguageDirectory = it.next();
			if ( currentLanguageDirectory.isDirectory() && !"merged".equals(currentLanguageDirectory.getName()) && !currentLanguageDirectory.getName().startsWith("merged") )
			{
				//work with a given language
				processTreebankDirectory(currentLanguageDirectory,outputPath,currentLanguageDirectory.getName().substring(3).replaceAll("^_", "")+"-all"); 
				//substring(3) eliminates the UD_... prefix in the directory name, which is not too useful
				//replaceAll is for SUD (in that case the prefix has one more letter, so substring(3) leaves an underscore at the beginning)
			}
		}
	}
	
	
	/**
	 * Processes the directory corresponding to a given treebank. 
	 * @param currentTreebankDirectory Directory corresponding to a given UD treebank. 
	 * @param outputPath General path where we are placing all our merged output files.
	 * @param outputFileName Desired name (without extension) of the output file.
	 */
	public static void processTreebankDirectory(File currentTreebankDirectory, File outputPath, String outputFileName) throws Throwable
	{
		//do a full merge of training+test set
		System.out.println(outputFileName.substring(0,outputFileName.length()-4));
		String fullOutputFileName = outputFileName+".conllu";
		File fullOutputFile = createFileOrDie(outputPath,fullOutputFileName);
		mergeAllConllFiles(currentTreebankDirectory,fullOutputFile,"conllu");
	}
	
	
	private static File createFileOrDie(File pathToDirectory, String fileName) throws IOException
	{
		File f = new File(pathToDirectory,fileName);
		if ( !f.createNewFile() )
		{
			System.err.println("File already exists: " + f);
			throw new IOException();
		}
		return f;
	}
	
	
	public static void mergeAllConllFiles(File baseDirectory, File outputFile, String extension) throws IOException
	{
		//open stream and writer for the output
		FileOutputStream fos = new FileOutputStream(outputFile);
		OutputStreamWriter osw = new OutputStreamWriter(fos,"UTF-8");
		PrintWriter pw = new PrintWriter(osw);
		mergeAllConllFiles(baseDirectory,pw,extension);
		pw.close();
	}
	
	public static void mergeAllConllFiles(File baseDirectory, PrintWriter writer,String extension) throws IOException
	{
		ArrayList<File> files = new ArrayList<File>(Arrays.asList(baseDirectory.listFiles()));	
		for ( Iterator<File> it = files.iterator() ; it.hasNext() ; )
		{
			File currentEntry = it.next();
			if ( currentEntry.isDirectory() )
			{
				mergeAllConllFiles(currentEntry,writer,extension);
			}
			else if ( currentEntry.getName().endsWith(extension) )
			{
				//open input stream
				FileInputStream is = new FileInputStream(currentEntry);
				Reader reader = new InputStreamReader(is, "UTF-8");
				BufferedReader br = new BufferedReader(reader);
				copyTrees(br,writer);
				br.close();
			}
			else if ( currentEntry.getName().endsWith(extension +".gz") )
			{
				//open input stream incl. gz extraction
				FileInputStream is = new FileInputStream(currentEntry);
				InputStream gzipStream = new GZIPInputStream(is);
				Reader reader = new InputStreamReader(gzipStream, "UTF-8");
				BufferedReader br = new BufferedReader(reader);
				copyTrees(br,writer);
				br.close();
			}
		}
	}
		
	public static void copyTrees(BufferedReader br, PrintWriter pw) throws IOException
	{
		//read CoNLL trees from a reader and write them to a writer.
		/*A newline is printed after the last tree (apart from the newline corresponding to the last line in the tree) so that this method can be invoked several times on the same writer
		and produce a valid CoNLL file*/
		
		String line;
		List<String> sentenceLines = new ArrayList<String>();
		
		do
		{
			//read one sentence, adding the lines to list sentenceLines
			while ( (line=br.readLine()) != null && line.trim().length() > 0 )
			{
				sentenceLines.add(line);
			}
			
			//output the lines
			for ( int i = 0 ; i < sentenceLines.size() ; i++ )
			{
				pw.print(sentenceLines.get(i)+"\n"); //unix format newline
			}
			if ( sentenceLines.size() > 0 ) pw.print("\n"); //unix format newline

			//clear array for next sentence
			sentenceLines.clear();
			
		} while (line!=null);
		
	}
	

	
}
