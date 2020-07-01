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
import java.io.FilenameFilter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.StringTokenizer;

class FileExtensionFilter implements FilenameFilter
{
	
	private String extension;
	private String[] extensions;
	
	public FileExtensionFilter ( final String extension )
	{
		this.extension = extension;
		if ( !this.extension.startsWith(".") ) this.extension = "."+(this.extension);
	}
	
	public FileExtensionFilter ( final String[] extensions )
	{
		this.extensions = extensions;
		for ( int i = 0 ; i < this.extensions.length ; i++ )
		{
			if ( !this.extensions[i].startsWith(".") ) this.extensions[i] = "."+(this.extensions[i]);
		}
	}
	
	public boolean accept(File dir, String name) 
	{
		if ( extension != null )
			return name.toLowerCase().endsWith(extension);
		else if ( extensions != null )
		{
			for ( int i = 0 ; i < extensions.length ; i++ )
			{
				if ( name.toLowerCase().endsWith(extensions[i]) )
					return true;
			}
		}
		return false;
	}

}

/**
 * 
 * Reads a directory that can contain .conllu, .heads, .heads2 and/or .omegas files for
 * Universal Dependencies treebanks.
 * 
 * Joins these files by language (i.e. joins Spanish with Spanish-Ancora, Galician with 
 * Galician-TreeGal, etc.) assuming that the hyphen (-) in a file name separates the language
 * from the rest of the name.
 *
 */
public class UniversalDependenciesJoinerByLanguage 
{

	//static String inputPath = "F:\\Treebanks\\ud-2.0-concatenated-shuffled";
	//static String outputPath = "F:\\Treebanks\\ud-2.0-concatenated-shuffled-bylanguage";
	
	static String inputPath = "D:\\Treebanks\\ud-treebanks-v2.6\\merged";
	static String outputPath = "D:\\Treebanks\\ud-treebanks-v2.6\\merged-bylanguage";
	
	private static void merge ( File[] files , File outputPath, boolean blankLineBetweenFiles , String fileSuffix , boolean forceUnixNewlines ) throws Throwable
	{
		Arrays.sort(files);
		String currentLanguage=null;
		PrintWriter currentWriter = null;
		FileOutputStream fos = null;
		
		for ( int i = 0 ; i < files.length ; i++ )
		{
			
			String fName = files[i].getName();
			//remove extension
			fName = fName.substring(0, fName.lastIndexOf('.'));
			//get language name
			StringTokenizer st = new StringTokenizer(fName,"-");
			String languageName = st.nextToken();
			
			
			//open input file
			BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(files[i]),"UTF-8"));
			
			if ( !languageName.equals(currentLanguage) )
			{
				
				System.out.println(languageName);
				
				//close file for last language, if any
				if ( currentWriter != null ) currentWriter.close();
				if ( fos != null ) fos.close();
				
				//open file for new language
				File outputFile = new File(outputPath,languageName+fileSuffix);
				//open stream and writer for the output
				fos = new FileOutputStream(outputFile);
				OutputStreamWriter osw = new OutputStreamWriter(fos,"UTF-8");
				currentWriter = new PrintWriter(osw);
				
			}
			/*
			else
			{
				if ( blankLineBetweenFiles ) currentWriter.print("\n"); //unix format newline
			}
			*/
			
			currentLanguage = languageName;
			
			String lastLine = null;
			String line = null;
			
			boolean fileFinished = false;
			while ( !fileFinished )
			{
				lastLine = line;
				line=br.readLine();
				if ( line != null )
				{
					if ( forceUnixNewlines )
						currentWriter.print(line+"\n"); //unix format newline
					else
						currentWriter.println(line);
				}
				else
					fileFinished=true;
			}

			//unneeded in practice in ud because all treebanks have the newline, but just in case...
			if ( lastLine.trim().length() > 0 && blankLineBetweenFiles ) //no empty line at end of file in CoNLL or similar. Place one.
			{
				if ( forceUnixNewlines )
					currentWriter.print("\n"); //unix format newline
				else
					currentWriter.println();
				System.out.println("term");
			}
			
			
			br.close();
			
		}
		
		//close file for last language, if any
		if ( currentWriter != null ) currentWriter.close();
		if ( fos != null ) fos.close();
		
	}
	
	private static String removeDotFromExtension ( String extension )
	{
		if ( extension.startsWith("."))
			return extension.substring(1);
		else
			return extension;
	}
	
	public static void main ( String[] args ) throws Throwable
	{
		File inputPathFile = new File(inputPath);
		File outputPathFile = new File(outputPath);
		if ( !outputPathFile.mkdirs() ) //create directory for output
		{
			System.err.println("Could not create output directory " + outputPathFile);
			System.exit(1);
		}
		
		File[] conlluFiles = inputPathFile.listFiles(new FileExtensionFilter("conllu"));
		Arrays.sort(conlluFiles);
		merge ( conlluFiles , outputPathFile , true , "-all.conllu", true );
		
		String headsExtension = removeDotFromExtension(ConvertToHeadSequences.E2_FILTER_EXTENSION);
		File[] headsFiles = inputPathFile.listFiles(new FileExtensionFilter(headsExtension));
		Arrays.sort(headsFiles);
		merge ( headsFiles , outputPathFile , false , "-all."+headsExtension, false );
		
		String heads2Extension = removeDotFromExtension(ConvertToHeadSequences.OMEGA_FILTER_EXTENSION);
		File[] heads2Files = inputPathFile.listFiles(new FileExtensionFilter(heads2Extension));
		Arrays.sort(heads2Files);
		merge ( heads2Files , outputPathFile , false , "-all."+heads2Extension, false );
		
		String unfilteredExtension = removeDotFromExtension(ConvertToHeadSequences.UNFILTERED_EXTENSION);
		File[] unfilteredFiles = inputPathFile.listFiles(new FileExtensionFilter(unfilteredExtension));
		Arrays.sort(unfilteredFiles);
		merge ( unfilteredFiles , outputPathFile , false , "-all."+unfilteredExtension, false );
		
		String reparallelExtension = removeDotFromExtension(ConvertToHeadSequences.REPARALLELIZED_EXTENSION);
		File[] reparallelFiles = inputPathFile.listFiles(new FileExtensionFilter(reparallelExtension));
		Arrays.sort(reparallelFiles);
		merge ( reparallelFiles , outputPathFile , false , "-all."+reparallelExtension, false );
		
		File[] omegasFiles = inputPathFile.listFiles(new FileExtensionFilter("omegas"));
		Arrays.sort(omegasFiles);
		merge ( omegasFiles , outputPathFile , false , "-all.omegas", false );
		
		

	}
	
	
}
