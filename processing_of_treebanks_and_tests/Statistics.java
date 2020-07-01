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
import java.util.Collections;
import java.util.List;


public class Statistics 
{
	
	public static Double overflowSafeAverage ( List<Double> myList )
	{
	  double avg = 0.0;
	  int sz = myList.size();
	  for ( int i = 0 ; i < sz ; i++ ) 
	  {
	    avg += myList.get(i) / ((double)sz);
	  }
	  return avg;
	}

	public static Double overflowSafeStDev ( List<Double> myList )
	{
	  double avg = overflowSafeAverage(myList);
	  double variance = 0.0;
	  int sz = myList.size();
	  for ( int i = 0 ; i < sz ; i++ ) 
	  {
	    variance += ( ((myList.get(i)-avg)*(myList.get(i)-avg)) / ((double)sz) );
	  }
	  return Math.sqrt(variance);
	}

	public static Double median ( List<Double> myList )
	{
	  double median;
	  int sz = myList.size();
	  Collections.sort(myList);
	  if (sz  % 2 == 0) 
	  {
	    median = (myList.get(sz/2-1) + myList.get(sz/2)) / 2.0;
	  } 
	  else 
	  {
	    median = myList.get(sz/2);
	  }
	  return median;
	}

}
