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
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Random;
import java.util.StringTokenizer;

/**
 * Encapsulates a tree, together with some associated metrics, needed for the Monte Carlo test of 
 * omega (MonteCarloTestOmegaByLength class).
 * @author Carlos
 *
 */
class TreeWithMetrics
{
	
	public int[] getTree() 
	{
		return tree;
	}
	public void setTree(int[] tree) 
	{
		this.tree = tree;
	}
	public int getDMin() 
	{
		return dMin;
	}
	public void setDMin(int dMin) 
	{
		this.dMin = dMin;
	}
	public int getD() 
	{
		return d;
	}
	public void setD(int d) 
	{
		this.d = d;
	}
	public double getDRandom() 
	{
		return dRandom;
	}
	public void setDRandom(double dRandom) 
	{
		this.dRandom = dRandom;
	}
	public double getOmega() 
	{
		return omega;
	}
	public void setOmega(double omega) 
	{
		this.omega = omega;
	}
	
	/**
	 * Updates d as the edge length sum of this tree (will change and need to be updated with shuffling)
	 */
	public void updateD()
	{
		d = 0;
		for ( int i = 0 ; i < tree.length ; i++ )
		{
			if ( tree[i] != 0 ) //not root (we have an arc)
			{
				d += Math.abs( (i+1)-tree[i] );
			}
		}
	}
	
	/**
	 * Calculates dRandom (only uses length of the tree, wont change with shuffling)
	 */
	public void updateDRandom()
	{
		int n = tree.length;
		dRandom = (n-1)*(n+1)/((double)(3.0));
	}
	
	/**
	 * Updates omega using dRandom, dMin and d. Take care to update d.
	 */
	public void updateOmega()
	{
		omega = (dRandom-(double)d) / (dRandom-(double)dMin);
	}
	
	public TreeWithMetrics( int d , int dMin , double dRandom , double omega , int[] tree )
	{
		this.d=d;
		this.dMin=dMin;
		this.dRandom=dRandom;
		this.omega=omega;
		this.tree=tree;
	}
	
	
	private int[] tree;
	private int d;
	private int dMin;
	private double dRandom;
	private double omega;
	
	
	//extended metrics
	
	
	//auxiliary
	private double degree2ndMoment;
	private double stdevOfDRandom;
	//final
	private double meanD;
	private double delta;
	private double gamma;
	private double dZeta;
	
	
	/**
	 * Updates degree 2nd moment and stdev of dRandom, which don't change with shuffling.
	 */
	public void updateDegree2ndMoment()
	{
		double n = (double)tree.length;
		DependencyStructure ds = new DependencyStructure();
		ds.contents = tree;
		degree2ndMoment = ds.getDegree2ndMoment(false);
	    double varianceOfDRandom = ((n+1.0)/45.0) * ( (n-1.0)*(n-1.0) + (n/4.0-1) * n * degree2ndMoment );
		stdevOfDRandom = Math.sqrt(varianceOfDRandom);
	}
	
	/**
	 * Updates dZeta using dRandom, d and degree2ndMoment (and associated stdev).
	 */
	private void updateDZeta()
	{
		dZeta = ( d - dRandom ) / stdevOfDRandom;
	}
	
	/**
	 * Updates the extended metrics using dRandom, dMin and d. Take care to update d.
	 */
	public void updateExtendedMetrics()
	{
		meanD = ((double)d) / ((double)tree.length-1.0);
		gamma = ((double)d) / ((double)dMin);
		delta = ((double)d) - ((double)dMin);
		updateDZeta();
	}
	
	public double getMeanD()
	{
		return meanD;
	}
	
	public double getDelta()
	{
		return delta;
	}
	
	public double getGamma()
	{
		return gamma;
	}
	
	public double getDZeta()
	{
		return dZeta;
	}
	
	
}



/**
 * Performs a Monte Carlo hypothesis test on the value of omega (dependency optimality parameter)
 * by dependency length.
 * Reads .heads2 files and .omega files.
 * .heads2 files contain the trees, .omega files contain the relevant stats of each tree.
 * @author Carlos
 *
 */
public class MonteCarloTestOmegaByLength 
{
	
	/**
	 * The code that shuffles trees has been copied to the class TreeShuffler for reuse in other tests.
	 */
	
	private Random random = new Random();
	private static int MAX_SENTENCE_LENGTH = 600;
	
	private static int OUTPUT_FORMAT_NORMAL = 0;
	private static int OUTPUT_FORMAT_VERBOSE = 1;
	private static int OUTPUT_FORMAT_TABLE = 2;
	
	private static boolean SHOW_RANDOMIZED_OMEGA_AVERAGE = true; //also applies to the extended metrics, if included
	
	private static boolean INCLUDE_EXTENDED_METRICS = true;
	
	/*
	 * Permutations[i] is an array of length i containing the first i natural numbers (starting from 1).
	 * Initially ordered, but then we'll shuffle them.
	 */
	int[][] permutations = new int[MAX_SENTENCE_LENGTH+1][];
	

	public MonteCarloTestOmegaByLength()
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
	
	private int[] shuffleTree ( int[] treeHeads )
	{
		int n = treeHeads.length;
		//choose a random permutation of n integers
		int[] perm = permutations[n];
		shuffleArray(perm);
		int[] output = new int[n];
		for ( int i = 0 ; i < n ; i++ )
		{
			//in the possition assigned to i by the permutation (-1 to correct 1-indexing)...
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
			MonteCarloTestOmegaByLength test = new MonteCarloTestOmegaByLength();
			test.initializePermutations();
			int[] shuffledTree = test.shuffleTree(myTree);
			for ( int i = 0 ; i < shuffledTree.length ; i++ )
				System.out.println(shuffledTree[i] + " ");
		}
	}
	
	
	@SuppressWarnings("unchecked")
	public List<TreeWithMetrics>[] data = new List[MAX_SENTENCE_LENGTH+1];
	
	public void readData ( File headsFile , File omegasFile , boolean includeExtendedMetrics ) throws Throwable
	{
		
		BufferedReader headsReader = new BufferedReader(new InputStreamReader(new FileInputStream(headsFile)));
		BufferedReader omegasReader = new BufferedReader(new InputStreamReader(new FileInputStream(omegasFile)));
		
		String headsLine;
		
		//initialize the arrays. The 0th one won't be used
		for ( int i = 0 ; i <= MAX_SENTENCE_LENGTH ; i++ )
		{
			data[i] = new ArrayList<TreeWithMetrics>();
		}
		
		while ( ((headsLine=headsReader.readLine()) != null) && headsLine.trim().length() > 0 )
		{
			
			String omegasLine = omegasReader.readLine();
			StringTokenizer stOmegas = new StringTokenizer(omegasLine);
			int length = Integer.valueOf(stOmegas.nextToken()).intValue();
			int d = Integer.valueOf(stOmegas.nextToken()).intValue();
			int dMin = Integer.valueOf(stOmegas.nextToken()).intValue();
			double dRandom = Double.valueOf(stOmegas.nextToken()).doubleValue();
			double omega = Double.valueOf(stOmegas.nextToken()).doubleValue();
			int[] tree = new int[length];
			StringTokenizer stHeads = new StringTokenizer(headsLine);
			int i = 0;
			while ( stHeads.hasMoreTokens() )
			{
				tree[i] = Integer.valueOf(stHeads.nextToken()).intValue();
				i++;
			}
			assert(i==length); //length of heads is same as length in omegas
			TreeWithMetrics twm = new TreeWithMetrics(d,dMin,dRandom,omega,tree);
			//sanity check d
			twm.updateD();
			assert(d==twm.getD());
			//sanity check dRandom
			twm.updateDRandom();
			assert(Math.abs(dRandom-twm.getDRandom())<0.000000001);
			//sanity check omega (using Dmin)
			twm.updateOmega();
			assert(Math.abs(omega-twm.getOmega())<0.000000001);
			if ( includeExtendedMetrics )
			{
				twm.updateDegree2ndMoment(); //needed for d_zeta updates, but this one is only called once
				twm.updateExtendedMetrics(); //and these will be called on shufflings as well
			}
			data[length].add(twm);
		}
		
		headsReader.close();
		omegasReader.close();
		
	}
	
	

	
	/**
	 * readData should have been called before this, so that the data arrays are filled.
	 * @param length. A length value of 0 means to mix all lengths.
	 * @return the p-value for omega (but really, most of the value is in the tables printed with OUTPUT_FORMAT_TABLE).
	 */
	public double doMonteCarloTest ( int length , int iters , int outputFormat , String treebankName , String linePrefix , boolean includeExtendedMetrics )
	{
		
		if ( outputFormat == OUTPUT_FORMAT_VERBOSE ) System.out.println("Running test for length " + length);
		List<TreeWithMetrics> treeList;
		if ( length > 0 )
			treeList = data[length];
		else //if length is given as 0, we concatenate lists for all lengths
		{
			treeList = new ArrayList<TreeWithMetrics>();
			for ( int i = 0 ; i < data.length ; i++ )
			{
				treeList.addAll(data[i]);
			}
		}
		if ( treeList.isEmpty() )
		{
			if ( outputFormat == OUTPUT_FORMAT_VERBOSE ) System.out.println("No sentences of this length");
			return Double.NaN;
		}
		
		double initialOmegaSum = 0.0;
		double[] initialExtendedMetricSum = new double[5];
		for ( Iterator<TreeWithMetrics> it = treeList.iterator() ; it.hasNext() ; )
		{
			TreeWithMetrics t = it.next();
			initialOmegaSum += t.getOmega();
			if ( includeExtendedMetrics )
			{
				initialExtendedMetricSum[0] += t.getMeanD();
				initialExtendedMetricSum[1] += t.getDelta();
				initialExtendedMetricSum[2] += t.getGamma();
				initialExtendedMetricSum[3] += ((double)t.getD());
				initialExtendedMetricSum[4] += t.getDZeta();
			}
			
		}
		//System.out.println("Initial: " + initialOmegaSum);
		
		int greaterOmegaSums = 0;
		int smallerOmegaSums = 0;
		int[] greaterExtendedMetricSums = new int[5];
		int[] smallerExtendedMetricSums = new int[5];
		
		double sumOfRandomizedOmegaAverages = 0.0;
		double[] sumOfRandomizedExtendedMetricAverages = new double[5];
		
		for ( int i = 0 ; i < iters ; i++ )
		{
			//do one simulation
			double thisIterationOmegaSum = 0.0;
			double[] thisIterationExtendedMetricSum = new double[5];
			
			for ( Iterator<TreeWithMetrics> it = treeList.iterator() ; it.hasNext() ; )
			{
				TreeWithMetrics currentTree = it.next();
				
				/*
				if ( i == 0 )
				{
					System.out.println("Initial tree " + Arrays.toString(currentTree.getTree()));
					System.out.println("Initial d " + currentTree.getD() + " omega " + currentTree.getOmega());
				}
				*/
				
				currentTree.setTree(shuffleTree(currentTree.getTree()));
				currentTree.updateD();
				currentTree.updateOmega();
				if ( includeExtendedMetrics ) currentTree.updateExtendedMetrics();
				
				/*
				if ( i == 0 )
				{
					System.out.println("Final tree " + Arrays.toString(currentTree.getTree()));
					System.out.println("Final d " + currentTree.getD() + " omega " + currentTree.getOmega());
				}
				*/
				
				thisIterationOmegaSum += currentTree.getOmega();
				if ( includeExtendedMetrics )
				{
					thisIterationExtendedMetricSum[0] += currentTree.getMeanD();
					thisIterationExtendedMetricSum[1] += currentTree.getDelta();
					thisIterationExtendedMetricSum[2] += currentTree.getGamma();
					thisIterationExtendedMetricSum[3] += ((double)currentTree.getD());
					thisIterationExtendedMetricSum[4] += currentTree.getDZeta();
				}
			}
			//System.out.println("Iter " + i + ": " + thisIterationOmegaSum);
			if ( thisIterationOmegaSum >= initialOmegaSum )
				greaterOmegaSums++;
			else if ( thisIterationOmegaSum <= initialOmegaSum )
				smallerOmegaSums++;
			
			if ( includeExtendedMetrics )
			{
				for ( int metric = 0 ; metric < 5 ; metric++ )
				{
					if ( thisIterationExtendedMetricSum[metric] >= initialExtendedMetricSum[metric] )
						greaterExtendedMetricSums[metric]++;
					else if ( thisIterationExtendedMetricSum[metric] <= initialExtendedMetricSum[metric] )
						smallerExtendedMetricSums[metric]++;
				}
			}
			
			if ( SHOW_RANDOMIZED_OMEGA_AVERAGE ) 
			{
				sumOfRandomizedOmegaAverages += thisIterationOmegaSum/((double)treeList.size());
				if ( includeExtendedMetrics )
				{
					for ( int metric = 0 ; metric < 5 ; metric++ )
					{
						sumOfRandomizedExtendedMetricAverages[metric] += thisIterationExtendedMetricSum[metric]/((double)treeList.size());
					}
				}
			}
		}
		
		double rightPValue = ((double)greaterOmegaSums) / ((double)iters);
		double leftPValue = ((double)smallerOmegaSums) / ((double)iters);
		//System.out.println("Length " + length + " p-value: " + pValue);
		
		double extendedMetricRightPValue[] = new double[5];
		double extendedMetricLeftPValue[] = new double[5];
		
		if ( includeExtendedMetrics )
		{
			for ( int metric = 0 ; metric < 5 ; metric++ )
			{
				extendedMetricRightPValue[metric] = ((double)greaterExtendedMetricSums[metric]) / ((double)iters);
				extendedMetricLeftPValue[metric] = ((double)smallerExtendedMetricSums[metric]) / ((double)iters);
			}
		}
		
		
		if (outputFormat == OUTPUT_FORMAT_TABLE )
		{
			double avgOmega = initialOmegaSum / ((double)treeList.size());
			double avgExtendedMetric[] = new double[5];
			if ( includeExtendedMetrics )
			{
				for ( int metric = 0 ; metric < 5 ; metric++ )
				{
					avgExtendedMetric[metric] = initialExtendedMetricSum[metric] / ((double)treeList.size());
				}
			}
			double avgRandomizedOmega = 0.0;
			double avgRandomizedExtendedMetric[] = new double[5];
			System.out.print(linePrefix);
			if ( SHOW_RANDOMIZED_OMEGA_AVERAGE )
			{
				avgRandomizedOmega = sumOfRandomizedOmegaAverages / ((double)iters); //not very important value. Should approach 0.
				if ( includeExtendedMetrics )
				{
					for ( int metric = 0 ; metric < 5 ; metric++ )
					{
						avgRandomizedExtendedMetric[metric] = sumOfRandomizedExtendedMetricAverages[metric] / ((double)iters);
					}
				}
			}
			System.out.print(LanguageCodeConversion.convertToISO3LetterCode(treebankName) + " " + length + " " + treeList.size() + " " + avgOmega);
			if ( SHOW_RANDOMIZED_OMEGA_AVERAGE ) System.out.print(" " + avgRandomizedOmega);
			System.out.print(" " + rightPValue + " " + leftPValue);
			if ( includeExtendedMetrics )
			{
				for ( int metric = 0 ; metric < 5 ; metric++ )
				{
					System.out.print(" " + avgExtendedMetric[metric]);
					if ( SHOW_RANDOMIZED_OMEGA_AVERAGE ) System.out.print(" " + avgRandomizedExtendedMetric[metric]);
					System.out.print(" " + extendedMetricRightPValue[metric] + " " + extendedMetricLeftPValue[metric]);
				}
			}
			System.out.println();
		}
		
		return rightPValue;
		
	}
	
	/**
	 * Parameters are just used for output
	 * @param outputFormat
	 * @param treebankName
	 */
	public void doMonteCarloTest( int outputFormat , String treebankName , int minLength , int maxLength , int iters , String linePrefix , boolean includeExtendedMetrics )
	{
		//perform the test
		for ( int i = minLength ; i <= maxLength ; i++ )
		{
			if ( (i == 0) || !data[i].isEmpty() )
			{
				double rightPValue = doMonteCarloTest(i,iters,outputFormat,treebankName,linePrefix,includeExtendedMetrics);
				if ( outputFormat != OUTPUT_FORMAT_TABLE ) System.out.println("Length " + i + " p-value: " + rightPValue);
				//if ( rightPValue == 0.0 ) break;
			}
		}
	}
	
	
	public static void main ( String[] args ) throws Throwable
	{
		
		String stanford = "D:\\Treebanks\\hamledt-2.0-stanford-concatenated";
		String prague = "D:\\Treebanks\\hamledt-2.0-conll-concatenated";
		String ud = "D:\\Treebanks\\ud-treebanks-v2.6\\merged-bylanguage";
		String sud = "D:\\Treebanks\\sud-treebanks-v2.6\\merged-bylanguage";
				//"C:\\Treebanks\\ud-2.0-concatenated";
				//"D:\\Treebanks\\ud-treebanks-v2.4\\merged-bylanguage";
				//"D:\\Treebanks\\sud-treebanks-v2.5\\merged-bylanguage";
		
		if ( args.length < 1 )
		{
			//default processing of hardwired folders.
		
			//by length:
			processDirectory(stanford,3,MAX_SENTENCE_LENGTH,10000,"Stanford ");
			processDirectory(prague,3,MAX_SENTENCE_LENGTH,10000,"Prague ");
			processDirectory(ud,3,MAX_SENTENCE_LENGTH,10000,"UD26 ");
			processDirectory(sud,3,MAX_SENTENCE_LENGTH,10000,"SUD26 ");
			//not by length:
			processDirectory(stanford,0,0,10000,"Stanford ");
			processDirectory(prague,0,0,10000,"Prague ");
			processDirectory(ud,0,0,10000,"UD26 ");
			processDirectory(sud,0,0,10000,"SUD26 ");
			
		}
		else if ( args.length == 1 )
		{
			if ( "header".equalsIgnoreCase(args[0]) ) 
				System.out.println("annotation language length sentences optimality optimality_random right_p_value left_p_value meanD meanD_random meanD_right_p_value meanD_left_p_value delta delta_random delta_right_p_value delta_left_p_value gamma gamma_random gamma_right_p_value gamma_left_p_value");
			else
				printUsage();
		}
		else if (args.length >= 4)
		{
			int maxLength;
			if ( "all".equalsIgnoreCase(args[0]) ) maxLength = -1; //"all" means do both test by length and not by length.
			else maxLength = Integer.valueOf(args[0]).intValue();
			int iters = Integer.valueOf(args[1]).intValue();
			String name = args[2];
			String directory = args[3];
			if ( maxLength == -1 )
			{
				processDirectory(directory,3,MAX_SENTENCE_LENGTH,iters,name+" ");
				processDirectory(directory,0,0,iters,name+" ");
			}
			else if ( maxLength > 0 )
				processDirectory(directory,3,maxLength,iters,name+" ");
			else
				processDirectory(directory,0,0,iters,name+" ");
		}
		else
		{
			printUsage();
		}
		
	}
	
	public static void printUsage()
	{
		System.err.println("Usage: java MonteCarloTestOmegaByLength header | java MonteCarloTestOmegaByLength <maxlength/\"all\"> <iters> <collection-name> <full-path-to-folder-with-omegas-files>");
	}
	
	public static void processDirectory ( String directory , int minLength , int maxLength , int iters , String linePrefix ) throws Throwable
	{
				
		int outputFormat = OUTPUT_FORMAT_TABLE;

		File f = new File(directory);
		
		ArrayList<File> files = new ArrayList<File>(Arrays.asList(f.listFiles()));
		//Collections.sort(files);
		
		for ( Iterator<File> it = files.iterator() ; it.hasNext() ; )
		{
			File currentHeadsFile = it.next();
			if ( currentHeadsFile.getName().endsWith(ConvertToHeadSequences.OMEGA_FILTER_EXTENSION) )
			{
				String headsAbsPath = currentHeadsFile.getAbsolutePath();
				String omegasAbsPath = headsAbsPath.replaceAll(ConvertToHeadSequences.OMEGA_FILTER_EXTENSION,".omegas");
				
				File currentOmegasFile = new File(omegasAbsPath);
				if ( !currentOmegasFile.exists() )
				{
					System.err.println("Omegas file " + currentOmegasFile + " does not exist, skipping.");
					continue;
				}
				
				String treebankName = currentHeadsFile.getName().substring(0,currentHeadsFile.getName().lastIndexOf("."));
				treebankName = treebankName.replaceAll("-all","");
				
				if ( outputFormat != OUTPUT_FORMAT_TABLE )
				{
					System.out.println("=====================");
					System.out.println("Processing heads file: " + currentHeadsFile);
					System.out.println("Omegas file: " + currentOmegasFile);
				}
				
				MonteCarloTestOmegaByLength test = new MonteCarloTestOmegaByLength();
				
				//initialize the permutation arrays that are used internally from shuffling
				test.initializePermutations();
				
				
				//read the data from the heads and omegas file
				test.readData(currentHeadsFile, currentOmegasFile, INCLUDE_EXTENDED_METRICS);
				
				//perform the test
				
				//by length:
				//test.doMonteCarloTest(outputFormat,treebankName,3,MAX_SENTENCE_LENGTH,10000);
				//not by length:
				//test.doMonteCarloTest(outputFormat, treebankName, 0, 0, 10000);
				
				test.doMonteCarloTest(outputFormat,treebankName,minLength,maxLength,iters,linePrefix,INCLUDE_EXTENDED_METRICS);
				
			}
		}
		
	}
	
	
}
