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
import java.util.Arrays;


/**
 * Simplification of the class DependencyStructure in project DependencyParsingTests,
 * to avoid unnecessary dependencies to other classes and bloat.
 * 
 * This class isn't, in general, optimized. Since this is typically only use to compute
 * metrics on treebanks, many methods are implemented in a quick naive way.
 * @author Carlos
 *
 */
public class DependencyStructure 
{
	
	private static int[] EMPTY_ARRAY = new int[0];
	
	/**
	 * This is the array that actually stores the dependencies.
	 * contents[i] = j means that node i+1 has j as its parent.
	 * Note that contents[0] contains the parent of node 1 (not of node 0 i.e. the dummy root).
	 * A tree with dummy root + n nodes will be represented by an array of length n.
	 */
	public int[] contents = EMPTY_ARRAY;
	
	/**
	 * Node 0 always represents the dummy root node, with the rest of the nodes indexed from 1.
	 */
	protected static int DUMMY_ROOT = 0; 
	
	public int getNumberOfNodes() //not including the dummy root
	{
		return contents.length;
	}
	
	public int getParentForNode ( int nodeIndex )
	{
		return contents[nodeIndex-1];
	}
	
	public boolean existsArrow ( int fromNode , int toNode )
	{
		if ( fromNode == DUMMY_ROOT ) return false;
 		return contents[fromNode-1] == toNode;
	}
	
	public boolean isLeaf ( int node )
	{
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			if ( contents[i] == node ) return false;
 		}
		return true;
	}
	
	/**
	 * @return The size WITHOUT including dummy root
	 */
	public int size()
	{
		return contents.length;
	}
	
	public boolean areLinked ( int node1 , int node2 )
	{
	    	return existsArrow ( node1 , node2 ) || existsArrow ( node2 , node1 );
	}
	
	/**
	 * Initializes a dependency structure with a given array of int.
	 * @param ar
	 */
	public void init ( int[] ar )
	{
		contents = new int[ar.length];
		for ( int i = 0 ; i < contents.length ; i++ )
			contents[i] = ar[i];
	}
	
	public boolean equals ( Object obj )
	{
		if ( !(obj instanceof DependencyStructure) ) return false;
		DependencyStructure other = (DependencyStructure) obj;
		return Arrays.equals(this.contents, other.contents);
	}
	
	public int hashCode()
	{
		return Arrays.hashCode(contents);
	}
	
	private boolean detectCycle ( boolean[] visited , int currentNode )
	{
		if ( currentNode == DUMMY_ROOT ) return false; //root can't be linked to anything
		if ( visited[currentNode-1] )
			return true;
		else
		{
			visited[currentNode-1] = true;
			for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
			{
				if ( existsArrow ( currentNode , i ) )
				{
					if ( detectCycle(visited,i) )
						return true;
				}
			}
			return false;
		}
	}
	
	/**
	 * @return true if the structure is a forest without counting the dummy root
	 * (or, equivalently, a tree, if we do count the dummy root)
	 */
	public boolean isForest ( )
	{
		//detects illegal links.
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			if ( contents[i] < 0 || contents[i] > getNumberOfNodes() ) return false;
		}
		//detects cycles.
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			boolean[] visited = new boolean[contents.length];
			if ( detectCycle ( visited , i ) )
				return false;
		}
		return true;
	}

	/**
	 * @return true if the structure is a tree, i.e., connected, acyclic and has only
	 * one "true" root (i.e. one dependent of the dummy root)
	 */
	public boolean isTree()
	{
		if ( !isForest() ) return false;
		//detects multiple roots.
		boolean foundRoot = false;
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			if ( getParentForNode(i) == DUMMY_ROOT )
			{
				if ( foundRoot ) return false;
				else foundRoot = true;
			}
		}
		return true;
	}
	
	//true iff node1 transitively governs node2
	public boolean dominates ( int node1 , int node2 )
	{
		int current = node2;
		while ( current != DUMMY_ROOT && current != node1 )
		{
			current = contents[current-1];
		}
		return ( current == node1 );
	}
	
	
	//true iff the arc to dependent from its head is projective
	public boolean isProjectiveArc ( int dependent )
	{
		int head = contents[dependent-1];
		int min = Math.min(head,dependent);
		int max = Math.max(head,dependent);
		for ( int i = min+1 ; i < max ; i++ )
		{
			if ( !dominates(head,i ) ) return false;
		}
		return true;
	}
	
	
	public boolean isProjective ()
	{
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			if ( !isProjectiveArc(i+1) ) return false;
 		}
		return true;
	}
	
	
	public String toString()
	{
		StringBuffer sb = new StringBuffer();
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			sb.append(contents[i]);
			sb.append(" ");
		}
		sb.append("[forest: ");
		boolean tree = isForest();
		sb.append(isForest());
		sb.append("]");		
		
		return new String(sb);
	}
	
	/**
	 * Contracts the dependency structure using this head->dependent link.
	 * The dependent disappears and its children go to the head.
	 * @param head
	 * @param dependent
	 */
	public DependencyStructure contract ( int head , int dependent )
	{
		if ( !existsArrow ( dependent,head ) )
			return null;
		
		//link children of dependent as children of head
		int[] updatedContents = new int[contents.length];
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			if ( contents[i] == dependent )
				updatedContents[i] = head;
			else
				updatedContents[i] = contents[i];
		}
		
		int[] contractedContents = new int[contents.length - 1];
		for ( int i = 0 ; i < updatedContents.length ; i++ )
		{
			if ( i+1 < dependent && updatedContents[i] < dependent )
				contractedContents[i] = updatedContents[i];
			if ( i+1 < dependent && updatedContents[i] > dependent )
				contractedContents[i] = updatedContents[i]-1;
			if ( i+1 > dependent && updatedContents[i] < dependent )
				contractedContents[i-1] = updatedContents[i];
			if ( i+1 > dependent && updatedContents[i] > dependent )
				contractedContents[i-1] = updatedContents[i]-1;
		}
		
		DependencyStructure newStruct = new DependencyStructure();
		newStruct.contents = contractedContents;
		//System.out.println("After: " + newStruct); 
		return newStruct;
	}
	
	/**
	 * Returns the number of arc crossings in this dependency structure.
	 * @param includeArcsFromRoot Whether arcs from the dummy root are to be considered.
	 * @return
	 */
	public int getNumberOfCrossings ( boolean includeArcsFromRoot )
	{
		int nCrossings = 0;
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			for ( int j = i+1 ; j <= getNumberOfNodes() ; j++ )
			{
				//see whether the unique arc going to i and the unique arc going to j cross
				int parentOfI = getParentForNode(i);
				int parentOfJ = getParentForNode(j);
				if ( (parentOfI==0 || parentOfJ==0) && !includeArcsFromRoot ) continue; //in this case we're not considering arcs from the root
				int min1 = Math.min(i,parentOfI);
				int max1 = Math.max(i,parentOfI);
				int min2 = Math.min(j,parentOfJ);
				int max2 = Math.max(j,parentOfJ);
				if ( min1 < min2 && min2 < max1 && max1 < max2 ) nCrossings++;
				if ( min2 < min1 && min1 < max2 && max2 < max1 ) nCrossings++;
			}
		}
		return nCrossings;
	}
	
	/**
	 * Returns true if there is a single true root (i.e. node that depends on the dummy root) and all the rest of the nodes are direct dependents of it.
	 * @return
	 */
	public boolean isDirectedStarTree()
	{
		int possibleTrueRoot = -1;
		int nRoots = 0;
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			if ( getParentForNode(i) == DUMMY_ROOT ) 
			{ 
				nRoots++; 
				if ( nRoots > 1 ) return false; //not a tree
			}
			else
			{
				if ( possibleTrueRoot == -1 ) possibleTrueRoot = getParentForNode(i);
				else if ( getParentForNode(i) != possibleTrueRoot ) return false; //two nodes had distinct non-dummy-root dependents
			}
		}
		return true;
	}
	
	/**
	 * Returns true if the tree is a undirecte star tree (a node has in+out degree n-1, being directly connected to all others).
	 * @return
	 */
	public boolean isStarTree()
	{
		int[] degrees = new int[contents.length];
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			int parent = getParentForNode(i);
			if ( parent != DUMMY_ROOT )
			{
				degrees[i-1] ++;
				degrees[parent-1] ++;
			}
		}
		for ( int i = 0 ; i < degrees.length ; i++ )
		{
			if ( degrees[i] == getNumberOfNodes() - 1 )
				return true;
		}
		return false;
	}
	
	public Object clone()
	{
		DependencyStructure ds = new DependencyStructure();
		ds.contents = this.contents.clone(); //shallow, but doesn't matter, it's an array of int
		return ds;
	}
	
	
	/**
	 * The parameter should really always be false.
	 * @param includeArcsFromRoot
	 * @return
	 */
	public double getDegree2ndMoment ( boolean includeArcsFromRoot )
	{
		double sum = 0.0;
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			//see how many adjacent nodes there are.
			int adjacent = 0;
			int startCheck = (includeArcsFromRoot)?0:1;
			for ( int j = startCheck ; j <= getNumberOfNodes() ; j++ )
			{
				if ( j!= i && this.areLinked(i, j)) adjacent++;
			}
			sum+= (((double)adjacent)*((double)adjacent));
		}
		return sum/((double)getNumberOfNodes());
	}
	
	public double getHubiness ( )
	{
		double numerator = getDegree2ndMoment(false);
		numerator = numerator - 4.0;
		numerator = numerator * ((double)getNumberOfNodes());
		numerator = numerator + 6.0;
		double denominator = (((double)getNumberOfNodes())-2.0) * (((double)getNumberOfNodes())-3.0);
		return numerator/denominator;
	}
	
	public int getSumOfDependencyLengths ( boolean includeDummyRoot )
	{
		int result = 0;
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			int p = getParentForNode(i);
			if ( p!=0 || includeDummyRoot )
			{
				result += Math.abs(p-i);
			}
		}
		return result;
	}
		
	
}