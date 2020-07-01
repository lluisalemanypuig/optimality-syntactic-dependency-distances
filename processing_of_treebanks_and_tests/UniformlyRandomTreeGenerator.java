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
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Random;



/**
 * 
 * @author carlos
 *
 */
public class UniformlyRandomTreeGenerator 
{
	
	private Random rand = new Random();
	
	public static final int ALDOUS_BRODER = 0;
	public static final int PRUFER_CODES = 1;

	
	/**
	 * Uses the Aldous-Broder algorithm to generate a uniformly random labelled undirected tree
	 * with the provided number of nodes.
	 * Returns the adjacency lists of said tree (an array of lists of integers)
	 * No considerations about roots, dummy roots, etc. are made here. It just returns a tree with
	 * that number of nodes, that's all. Nodes are numbered from 0 to nNodes-1.
	 * 
	 * @param nNodes
	 * @return
	 */
	public List[] generateRandomLabelledUndirectedTreeUsingAldousBroder ( int nNodes )
	{
		//array that holds whether a node has been visited
		boolean[] visited = new boolean[nNodes];
		
		//adjacency lists for the returned tree
		List[] adjacencyLists = new List[nNodes];
		for ( int i = 0 ; i < adjacencyLists.length ; i++ )
		{
			adjacencyLists[i] = new ArrayList();
		}
		
		//visit a first random node
		int currentNode = rand.nextInt(nNodes);
		visited[currentNode] = true;
		
		//this variable will hold the previously-visited node
		int previousNode;
		
		//number of edges created: at the moment, none
		int nEdgesCreated = 0;
		
		while ( nEdgesCreated < nNodes-1 ) //a tree must have n-1 edges
		{
			//update current and previous node
			previousNode = currentNode;
			currentNode = rand.nextInt(nNodes);
			
			if ( !visited[currentNode] )
			{
				visited[currentNode] = true;
				adjacencyLists[previousNode].add(currentNode);
				adjacencyLists[currentNode].add(previousNode);
				nEdgesCreated++;
			}	
		}
		
		return adjacencyLists;
	}
	
	/**
	 * Uses Prufer codes to generate a uniformly random labelled undirected tree with the provided
	 * number of nodes.
	 * Returns the adjacency lists of said tree (an array of lists of integers).
	 * No considerations about roots, dummy roots, etc. are made here. It just returns a tree with
	 * that number of nodes, that's all. Nodes are numbered from 0 to nNodes-1.
	 * @param nNodes
	 * @return
	 */
	public List[] generateRandomLabelledUndirectedTreeUsingPruferCodes ( int nNodes )
	{
		int[] prufer = new int[nNodes-2];
		for ( int i = 0 ; i < prufer.length ; i++ )
			prufer[i] = rand.nextInt(nNodes);
		
		int[] degree = new int[nNodes];
		for (int i = 0; i < nNodes; i++)
			degree[i] = 1;
		for (int i = 0; i < nNodes-2; i++)
			degree[prufer[i]]++;
		
		//adjacency lists for the returned tree
		List[] adjacencyLists = new List[nNodes];
		for ( int i = 0 ; i < adjacencyLists.length ; i++ )
		{
			adjacencyLists[i] = new ArrayList();
		}

		for (int i = 0; i < nNodes-2; i++) 
		{
			int j;
			for (j = 0; degree[j] != 1; j++); //goes to first node with degree 1.
			adjacencyLists[j].add(prufer[i]);
			adjacencyLists[prufer[i]].add(j);
			degree[j]--;
			degree[prufer[i]]--;
		}
		
		int j,k;
		for (j = 0; degree[j] != 1; j++); //goes to first node with degree 1.
		for (k = j+1; degree[k] != 1; k++); //goes to next node with degree 1.
		
		adjacencyLists[j].add(k);
		adjacencyLists[k].add(j);
		
		return adjacencyLists;
	}
	
	
	public List[] generateRandomLabelledUndirectedTree ( int nNodes )
	{
		return generateRandomLabelledUndirectedTree(nNodes,ALDOUS_BRODER);
	}
	
	public List[] generateRandomLabelledUndirectedTree ( int nNodes , int algo )
	{
		if ( algo == ALDOUS_BRODER )
			return generateRandomLabelledUndirectedTreeUsingAldousBroder(nNodes);
		else /*if ( algo == PRUFER_CODES )*/
			return generateRandomLabelledUndirectedTreeUsingPruferCodes(nNodes);
	}
	
	
	//temporary variables for conversion of undirected tree to directed dependency structures
	private List[] adjacencyLists;
	private int[] dependencyStructureArray;
	private boolean[] visitedDuringConversion;
	
	private void undirectedTreeToDependencyForestTraversal ( int currentNode )
	{
		visitedDuringConversion[currentNode] = true;
		List adjacentNodes = adjacencyLists[currentNode];
		for (Iterator iterator = adjacentNodes.iterator(); iterator.hasNext();) 
		{
			int nextNode = (Integer) iterator.next();
			if ( !visitedDuringConversion[nextNode] ) //this check is needed bc. graph is undirected, so has edges a->b, b->a
			{
				dependencyStructureArray[nextNode-1]=currentNode;
				undirectedTreeToDependencyForestTraversal(nextNode);
			}
		}
	}
	
	private void undirectedTreeToDependencyTreeTraversal ( int currentNode )
	{
		visitedDuringConversion[currentNode] = true;
		List adjacentNodes = adjacencyLists[currentNode];
		for (Iterator iterator = adjacentNodes.iterator(); iterator.hasNext();) 
		{
			int nextNode = (Integer) iterator.next();
			if ( !visitedDuringConversion[nextNode] ) //this check is needed bc. graph is undirected, so has edges a->b, b->a
			{
				dependencyStructureArray[nextNode]=currentNode+1; //in the undirected tree we have nodes 0..n-1, but in the dependency tree we index them 1..n with 0 the dummy root
				undirectedTreeToDependencyTreeTraversal(nextNode);
			}
		}
	}
	
	/**
	 * Generates a uniformly random dependency forest (trees linked to the dummy root), with nNodes nodes PLUS the dummy root.
	 * @param nNodes
	 * @return
	 */
	public DependencyStructure generateUniformlyRandomDependencyForest ( int nNodes , int algo )
	{
		
		adjacencyLists = generateRandomLabelledUndirectedTree(nNodes+1,algo);
		dependencyStructureArray = new int[nNodes];
		visitedDuringConversion = new boolean[nNodes+1];
		
		//this modifies the previous two variables
		undirectedTreeToDependencyForestTraversal(0);
		
		DependencyStructure ds = new DependencyStructure();
		ds.init(dependencyStructureArray);
		return ds;
	}
	
	/**
	 * Generates a uniformly random dependency forest (trees linked to the dummy root), with nNodes nodes PLUS the dummy root.
	 * @param nNodes
	 * @return
	 */
	public DependencyStructure generateUniformlyRandomDependencyForest ( int nNodes )
	{
		return generateUniformlyRandomDependencyForest ( nNodes , ALDOUS_BRODER );
	}
	
	/**
	 * Generates a uniformly random dependency tree, with nNodes nodes PLUS the dummy root.
	 * @param nNodes
	 * @return
	 */
	public DependencyStructure generateUniformlyRandomDependencyTree ( int nNodes , int algo )
	{
		
		adjacencyLists = generateRandomLabelledUndirectedTree(nNodes,algo);
		dependencyStructureArray = new int[nNodes];
		visitedDuringConversion = new boolean[nNodes+1];
		
		int root = rand.nextInt(nNodes); //indexed 0..n-1
		
		//this modifies the previous two variables
		dependencyStructureArray[root] = 0; //link root as dependent of dummy root
		undirectedTreeToDependencyTreeTraversal(root);
		
		DependencyStructure ds = new DependencyStructure();
		ds.init(dependencyStructureArray);
		return ds;
	}
	
	/**
	 * Generates a uniformly random dependency tree, with nNodes nodes PLUS the dummy root.
	 * @param nNodes
	 * @return
	 */
	public DependencyStructure generateUniformlyRandomDependencyTree ( int nNodes )
	{
		return generateUniformlyRandomDependencyTree ( nNodes , ALDOUS_BRODER );
	}
	
	
	/**
	 * If numberOfTrees is zero then the iterator is infinite.
	 * @param nNodes
	 * @param numberOfTrees
	 * @return
	 */
	public Iterator getUniformlyRandomDependencyTreeIterator( final int nNodes , final int numberOfTrees )
	{
		return new Iterator()
		{
			int returned = 0;
			public boolean hasNext() 
			{
				if ( numberOfTrees < 1 ) return true;
				return (returned<numberOfTrees);
			}

			public Object next() 
			{
				returned++;
				return generateUniformlyRandomDependencyTree(nNodes);
			}

			public void remove() 
			{
				throw new UnsupportedOperationException();
			}
		};
	}
	
	public Iterator getUniformlyRandomDependencyTreeIterator ( final int nNodes )
	{
		return getUniformlyRandomDependencyTreeIterator ( nNodes , 0 );
	}
	
	
	private static void testUndirectedTreeGeneration()
	{
		int n = 50;
		for ( int i = 0 ; i < 3 ; i++ )
		{
			System.out.println(Arrays.toString(new UniformlyRandomTreeGenerator().generateRandomLabelledUndirectedTree(n,ALDOUS_BRODER)));
		}
		for ( int i = 0 ; i < 3 ; i++ )
		{
			System.out.println(Arrays.toString(new UniformlyRandomTreeGenerator().generateRandomLabelledUndirectedTree(n,PRUFER_CODES)));
		}
	}
	
	private static void testDependencyStructureGeneration()
	{
		int n = 50;
		for ( int i = 0 ; i < 15 ; i++ )
		{
			System.out.println(new UniformlyRandomTreeGenerator().generateUniformlyRandomDependencyTree(n,ALDOUS_BRODER));
		}
		for ( int i = 0 ; i < 15 ; i++ )
		{
			System.out.println(new UniformlyRandomTreeGenerator().generateUniformlyRandomDependencyTree(n,PRUFER_CODES));
		}
	}
	
	public static void main ( String[] args )
	{
		testUndirectedTreeGeneration();
		testDependencyStructureGeneration();
	}
	
	

	
	
	
}
