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
import java.util.Iterator;
import java.util.LinkedHashMap;


/**
 * Entry for metrics of a treebank by length groups. Contains length group entries.
 * @author carlos_gomez
 *
 */
public class TreebankEntry
{
	
	private String name;
	private LinkedHashMap<Integer,LengthGroupEntry> lengthEntries = new LinkedHashMap<Integer,LengthGroupEntry>();
	
	public void setLengthEntry ( int length , LengthGroupEntry lge )
	{
		lengthEntries.put(length,lge);
	}
	
	public LengthGroupEntry getLengthEntry ( int length )
	{
		return lengthEntries.get(length);
	}
	
	public Iterator<LengthGroupEntry> iterator()
	{
		return lengthEntries.values().iterator();
	}
	
	public String getName()
	{
		return name;
	}
	
	public void setName(String name)
	{
		this.name = name;
	}
	
	public TreebankEntry ( String name )
	{
		setName(name);
	}
	
}