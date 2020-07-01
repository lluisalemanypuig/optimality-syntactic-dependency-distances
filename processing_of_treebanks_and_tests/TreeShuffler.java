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
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.StringTokenizer;

/**
 * Class that shuffles trees (generating random linear orders of their nodes).
 * This was extracted from MonteCarloTestOmegaByLength.
 * @author usuario
 *
 */
public class TreeShuffler 
{

	private Random random = new Random();
	
	private static int MAX_SENTENCE_LENGTH = 300;
	
	/*
	 * Permutations[i] is an array of length i containing the first i natural numbers (starting from 1).
	 * Initially ordered, but then we'll shuffle them.
	 */
	int[][] permutations = new int[MAX_SENTENCE_LENGTH+1][];
	
	public TreeShuffler()
	{
		initializePermutations();
	}
	
	/**
	 * Initially the permutations will be straight, of the form 1 2 3 4 5 6 7...
	 * But we will shuffle each one before shuffling a tree.
	 */
	private void initializePermutations ( )
	{
		for ( int i = 1 ; i <= MAX_SENTENCE_LENGTH ; i++ )
		{
			permutations[i] = new int[i];
			for ( int j = 0 ; j < i ; j++ )
			{
				permutations[i][j] = j+1;
			}
		}
	}
	
	//Fisher–Yates shuffle
	private void shuffleArray(int[] ar)
	{
		for (int i = ar.length - 1; i > 0; i--)
		{
			int index = random.nextInt(i + 1);

			//swap ar[index] with ar[i]
			int a = ar[index];
			ar[index] = ar[i];
			ar[i] = a;
		}
	}
	
	public int[] shuffleTree ( int[] treeHeads )
	{
		int n = treeHeads.length;
		//choose a random permutation of n integers
		int[] perm = permutations[n];
		shuffleArray(perm);
		int[] output = new int[n];
		for ( int i = 0 ; i < n ; i++ )
		{
			//in the position assigned to i by the permutation (-1 to correct 1-indexing)...
			output[perm[i]-1] =
					treeHeads[i] > 0?
							//we place the node that was the head of i, but renamed by the permutation.
							perm[treeHeads[i]-1]
					:		//or if the head of i was the dummy root, we place the dummy root.
							0;
		}
		//alternatively, now that we have the output we could copy it to treeHeads, but who cares,
		//we would be throwing away an object anyway.
		return output;
	}

	public static void testShuffle ( String[] args ) throws Throwable
	{
		//shuffleTree works. It shuffles trees. Nice.
		for(;;)
		{
			System.out.print("Tree to shuffle: ");
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			String line = br.readLine();
			List<Integer> tokens = new ArrayList<Integer>();
			StringTokenizer st = new StringTokenizer(line);
			while ( st.hasMoreTokens() )
				tokens.add(Integer.valueOf(st.nextToken()));
			int[] myTree = new int[tokens.size()];
			for ( int i = 0 ; i < myTree.length ; i++ )
			{
				myTree[i] = tokens.get(i);
			}
			TreeShuffler test = new TreeShuffler();
			test.initializePermutations();
			int[] shuffledTree = test.shuffleTree(myTree);
			for ( int i = 0 ; i < shuffledTree.length ; i++ )
				System.out.println(shuffledTree[i] + " ");
		}
	}
	
	public static void main ( String[] args ) throws Throwable
	{
		testShuffle(args);
	}
	
}
