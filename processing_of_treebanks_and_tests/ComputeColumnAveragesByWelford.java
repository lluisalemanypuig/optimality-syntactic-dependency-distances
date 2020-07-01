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
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;


/**
 * Computes averages and st. devs. of columns of a file.
 * @author carlos
 *
 */
public class ComputeColumnAveragesByWelford 
{

	
	/**
	 * Use Welford's algorithm to incrementally calculate mean and variance
	 */
	private boolean USE_WELFORD_ALGORITHM = true;
	
	/**
	 * If this is set to true, we use the biased variance estimator (variance) instead of the unbiased estimator (quasivariance)
	 */
	private boolean USE_BIASED_VARIANCE_ESTIMATOR = false;
	
	private Map<String,List<Double>> metricsLists = new LinkedHashMap<String,List<Double>>();
	
	/**
	 * Values of n, mean, M2 for Welford - https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
	 */
	private Map<String,Long> metricN = new LinkedHashMap<String,Long>();
	private Map<String,Double> metricMean = new LinkedHashMap<String,Double>();
	private Map<String,Double> metricM2 = new LinkedHashMap<String,Double>();
	
	/**
	 * Registers a given value of a metric on a tree. This will update the lists used to compute the average, standard deviation, etc. for that metric.
	 * @param metric
	 * @param setOfTrees
	 * @param ivalue
	 */
	private void registerMetricValue ( String metric , String setOfTrees , double value )
	{
		String id = metric + "$" + setOfTrees;
		//double value = (double) ivalue;
		if ( !USE_WELFORD_ALGORITHM ) //just add the value to the list. Mean and variance will be calculated afterwards
		{
			if ( metricsLists.get(id) == null ) metricsLists.put(id,new ArrayList<Double>());
			List<Double> l = metricsLists.get(id);
			l.add(value);
		}
		else
		{
			if ( metricN.get(id) == null )
			{
				//initialise values
				metricN.put(id, 0L);
				metricMean.put(id,0.0);
				metricM2.put(id,0.0);
			}
				//incremental calculation
				long n = metricN.get(id);
				n = n+1;
				metricN.put(id,n);
				Double mean = metricMean.get(id);
				double delta = value - mean;
				mean += (delta/((double)n));
				metricMean.put(id,mean);
				double m2 = metricM2.get(id);
				m2 += (delta*(value-mean));
				metricM2.put(id,m2);
				//System.out.println("n= " + n + " mean= " + mean + " id " + id + " value " + value + " delta " + delta);
		}
	}
	
	private List<Double> getMetricsList ( String metric , String setOfTrees )
	{
		String id = metric + "$" + setOfTrees;
		return metricsLists.get(id);
	}
	
	private double getAverage ( String metric , String setOfTrees )
	{
		if ( !USE_WELFORD_ALGORITHM )
		{
			List<Double> theList = getMetricsList ( metric , setOfTrees );
			return Statistics.overflowSafeAverage(theList);
		}
		else
		{
			return metricMean.get(metric+"$"+setOfTrees);
		}
	}
	
	private double getStDev ( String metric , String setOfTrees )
	{
		if ( !USE_WELFORD_ALGORITHM )
		{
			List<Double> theList = getMetricsList ( metric , setOfTrees );
			return Statistics.overflowSafeStDev(theList);
		}
		else
		{
			double m2 = metricM2.get(metric+"$"+setOfTrees);
			long n = metricN.get(metric+"$"+setOfTrees);
			double divisor = USE_BIASED_VARIANCE_ESTIMATOR?n:(n-1);
			return Math.sqrt( m2/divisor ); //this is quasi-stdev (unbiased estimator). Change n-1 to n for stdev (biased estimator).
		}
	}
	
	private long getN ( String metric , String setOfTrees )
	{
		if ( !USE_WELFORD_ALGORITHM )
		{
			List<Double> theList = getMetricsList ( metric , setOfTrees );
			return theList.size();
		}
		else
		{
			long n = metricN.get(metric+"$"+setOfTrees);
			return n;
		}
	}
	
	public void compute ( File inputFile , File outFile , String setName , String[] metricNames ) throws FileNotFoundException, IOException
	{
		//open input file
		BufferedReader br = new BufferedReader ( new InputStreamReader ( new FileInputStream(inputFile.getAbsolutePath()) /*, "UTF-8"*/ ) ); //file generated by c++
		
		//read lines from input file
		String line;
		while ( (line=br.readLine()) != null && line.length() != 0 )
		{
			
			StringTokenizer stMetrics = new StringTokenizer(line);
			double[] myMetrics = new double[metricNames.length];
			for ( int i = 0 ; i < myMetrics.length ; i++ )
			{
				myMetrics[i] = Double.valueOf(stMetrics.nextToken()).doubleValue();
				registerMetricValue ( metricNames[i] , setName , myMetrics[i] );
			}
			
		}
		
		//output to output file
		PrintWriter pw = new PrintWriter( new OutputStreamWriter ( new FileOutputStream(outFile) ) );
		String output=setName;
		long n = getN(metricNames[0],setName);
		output += " ";
		output += n;
		output += " ";
		for ( int i = 0 ; i < metricNames.length ; i++ )
		{
			if ( n >= 1 )
				output += getAverage( metricNames[i] , setName );
			else
				output += "N/A";
			output += " ";
			if ( n >= 2 )
				output += getStDev( metricNames[i] , setName );
			else
				output += "N/A";
			if ( i != metricNames.length - 1 ) output+= " ";
		}
		pw.println(output);
		
		br.close();
		pw.close();	
	}
	
	
	public void processDirectory ( String directory ) throws Throwable
	{
		
		File f = new File(directory);
		ArrayList<File> files = new ArrayList<File>(Arrays.asList(f.listFiles()));
		
		for ( Iterator<File> it = files.iterator() ; it.hasNext() ; )
		{
			File currentInputFile = it.next();
			if ( currentInputFile.getName().endsWith(".omegas2") )
			{
				String inputAbsPath = currentInputFile.getAbsolutePath();
				String extension = ".average";
				String outputAbsPath;
				outputAbsPath = inputAbsPath.substring(0, inputAbsPath.length()-8) + extension;
				File outputFile = new File(outputAbsPath);
				if ( outputFile.exists() )
				{
					System.err.println("Output file " + outputFile + " already exists, skipping.");
					continue;
				}
				try 
				{
					outputFile.createNewFile();
				} 
				catch (IOException e) 
				{
					e.printStackTrace();
					continue;
				}
				String setName = currentInputFile.getName().substring(0,currentInputFile.getName().indexOf(".")).replace("_"," ");  //.substring(0,3); //.replace("_"," ");
				System.out.println("=====================");
				System.out.println("Processing input file: " + currentInputFile);
				System.out.println("Output file: " + outputFile);
				compute(currentInputFile,outputFile,setName,new String[] {"length","crossings","D","k2","omega"});
			}
		}
		
	}
	
	public static void main (String[] args) throws Throwable
	{
		new ComputeColumnAveragesByWelford().processDirectory("D:\\OmegasClassesOfTrees\\exhaustive-1-9");
	}
	
	
	
	
}
