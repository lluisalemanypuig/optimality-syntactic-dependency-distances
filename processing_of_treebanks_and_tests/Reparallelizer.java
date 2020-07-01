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
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.StringTokenizer;


/**
 * Filter lines of a .heads* file.
 * @author carlos
 *
 */
interface LineFilter
{
	public boolean valid ( String s ); 
}

class Length3LineFilter implements LineFilter
{
	public boolean valid ( String s )
	{
		int count = 0;
		StringTokenizer st = new StringTokenizer(s);
		while ( st.hasMoreTokens() )
		{
			st.nextToken(); count++;
		}
		return (count >= 3);
	}
	public Length3LineFilter()
	{
		;
	}
}

public class Reparallelizer 
{


	private static String removeDotFromExtension ( String extension )
	{
		if ( extension.startsWith("."))
			return extension.substring(1);
		else
			return extension;
	}
	
	/**
	 * Reparallelize PUD corpora using length-3 filter.
	 * I.e. only keep trees that have length >= 3 in ALL languages.
	 * @param folderName
	 */
	public static void reparallelize ( String folderName ) throws Throwable
	{
		reparallelize ( folderName , new Length3LineFilter() );
	}
	
	public static void reparallelize ( String folderName , LineFilter filter ) throws Throwable
	{
		
		String unfilteredExtension = removeDotFromExtension(ConvertToHeadSequences.UNFILTERED_EXTENSION);
		File[] unfilteredFiles = new File(folderName).listFiles(new FileExtensionFilter(unfilteredExtension));
		BufferedReader[] readers = new BufferedReader[unfilteredFiles.length];
		PrintWriter[] writers = new PrintWriter[unfilteredFiles.length];
		String[] lines = new String[unfilteredFiles.length];
		for ( int i = 0 ; i < unfilteredFiles.length ; i++ )
		{
			readers[i] = new BufferedReader(new InputStreamReader(new FileInputStream(unfilteredFiles[i])));
			writers[i] = new PrintWriter(new OutputStreamWriter(new FileOutputStream(unfilteredFiles[i].getAbsolutePath().replace(unfilteredExtension,removeDotFromExtension(ConvertToHeadSequences.REPARALLELIZED_EXTENSION)))));
			lines[i] = readers[i].readLine();
			if ( lines[i] != null ) lines[i] = lines[i].trim();
		}
		while ( lines[0] != null && !"".equals(lines[0]) )
		{
			//System.out.println("ITER");
			boolean valid = true;
			for ( int i = 0 ; i < unfilteredFiles.length ; i++ )
			{
				assert ( lines[i] != null ); //treebanks should be parallel, so they should have the same number of sentences
				if ( !filter.valid(lines[i]) ) { valid = false; break; }
			}
			if ( valid )
			{
				for ( int i = 0 ; i < unfilteredFiles.length ; i++ )
				{
					writers[i].println(lines[i]);
				}
			}
			for ( int i = 0 ; i < unfilteredFiles.length ; i++ )
			{
				//System.out.println("File: " + unfilteredFiles[i]);
				lines[i] = readers[i].readLine();
				if ( lines[i] != null ) lines[i] = lines[i].trim();
			}
		}
		for ( int i = 0 ; i < unfilteredFiles.length ; i++ )
		{
			readers[i].close();
			writers[i].close();
		}	
	}
	
	
}
