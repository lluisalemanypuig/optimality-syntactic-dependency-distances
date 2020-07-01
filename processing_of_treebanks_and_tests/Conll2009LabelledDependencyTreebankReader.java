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
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;

import javax.xml.transform.TransformerException;


/*
 *
 Created 23/09/2008 2:06:19
 */

public class Conll2009LabelledDependencyTreebankReader 
{

	File f;
	
	public Conll2009LabelledDependencyTreebankReader ( String s )
	{
		f = new File(s);
	}
	
	public static List<LabelledDependencyStructure> readFile ( File f ) throws IOException
	{
		
		String line;
		BufferedReader br = new BufferedReader ( new InputStreamReader ( new FileInputStream(f) ) );
		List<LabelledDependencyStructure> result = new ArrayList<LabelledDependencyStructure>();
		
		List<Integer> currentStructure = new ArrayList<Integer>();
		List<String> currentLinkLabels = new ArrayList<String>();
		List<String> currentWords = new ArrayList<String>();
		List<String> currentPosTags = new ArrayList<String>();

		while ( (line=br.readLine()) != null )
		{
			
			if ( line.trim().length() < 1 )
			{
				//end of sentence
				int[] dsArray = new int[currentStructure.size()];
				for ( int i = 0 ; i < currentStructure.size() ; i++ )
				{
					dsArray[i] = currentStructure.get(i).intValue();
				}
				LabelledDependencyStructure ds = new LabelledDependencyStructure();
				ds.init(dsArray);
				for ( int i = 0 ; i < currentLinkLabels.size(); i++ )
				{
					ds.setLinkLabel(i+1, currentLinkLabels.get(i));
				}
				for ( int i = 0 ; i < currentWords.size(); i++ )
				{
					ds.setWord(i+1, currentWords.get(i));
				}
				for ( int i = 0 ; i < currentPosTags.size(); i++ )
				{
					ds.setPosTag(i+1, currentPosTags.get(i));
				}
				ds.setDisplayName("From " + f);
				result.add(ds);
				currentStructure.clear();
				currentLinkLabels.clear();
				currentWords.clear();
				currentPosTags.clear();
			}
			else
			{
				StringTokenizer st = new StringTokenizer(line.trim());
				//2nd token is word
				for ( int i = 0 ; i < 1 ; i++ ) st.nextToken();
				String word = new String(st.nextToken().trim());
				//5th token is POS
				for ( int i = 0 ; i < 2 ; i++ ) st.nextToken();
				String posTag = new String(st.nextToken().trim());
				//9th token is head position
				for ( int i = 0 ; i < 3 ; i++ ) st.nextToken();
				Integer headPos = new Integer(Integer.parseInt(st.nextToken().trim()));
				//and 11th is dependency label
				for ( int i = 0 ; i < 1 ; i++ ) st.nextToken();
				String label = new String(st.nextToken().trim());
				currentStructure.add(headPos);
				currentLinkLabels.add(label);
				currentWords.add(word);
				currentPosTags.add(posTag);
			}
		
		}
		
		br.close();
		
		return result;
		
	}
	
	public Iterator<LabelledDependencyStructure> getIterator() throws IOException
	{
		List<LabelledDependencyStructure> structures = readFile(f);
		return structures.iterator();
	}
	
}
