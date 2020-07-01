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
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Dependency structure that includes (optionally) form, postag, cpostag, lemma. Also feats as of 2015-04-12.
 * and labelling information.
 * @author Carlos
 *
 */
public class LabelledDependencyStructure extends DependencyStructure implements Cloneable
{

	private String[] linkLabels;
	
	private String[] lemmata;
	private String[] cPosTags;
	private String[] posTags;
	private String[] feats;
	
	private String[] words;
	
	private String displayName = "Unnamed";
	
	/**
	 * A node is considered punctuation if Java deems it's formed by Unicode punctuation symbols (with a tweak to exclude the section symbol) 
	 */
	public static int UNICODE_PUNCTUATION_CRITERION = 0;
	
	/**
	 * A node is considered punctuation if its feats field contains "pos=punc"
	 */
	public static int HAMLEDT_PUNCTUATION_CRITERION = 1;
	
	/**
	 * A node is considered punctuation if its feats field contains "pos=punc". Additionally, nodes with a NULL form are also considered "punctuation" (not that they are really punctuation,
	 * but we want to remove them as well)
	 */
	public static int HAMLEDT_PUNCTUATION_CRITERION_PLUS_NULL_ELEMENTS = 2;
	
	/**
	 * A node is considered punctuation if it has universal postag (UPOSTAG field, equivalent to legacy CPOSTAG) "PUNCT" and dependency type "punct"
	 */
	public static int UNIVERSAL_DEPENDENCIES_PUNCTUATION_CRITERION_STRICT = 3;
	
	/**
	 * A node is considered punctuation if it has universal postag (UPOSTAG field, equivalent to legacy CPOSTAG) "PUNCT", even if it does not have dependency type "punct".
	 * This seems more reasonable that requiring the dependency type too. If something has UPOSTAG "PUNCT", it is always punctuation. There are cases where the dependency
	 * type is not "punct" e.g. because it is "root" (as it's marked as root of the sentence), but it's still effectively punctuation.
	 */
	public static int UNIVERSAL_DEPENDENCIES_PUNCTUATION_CRITERION = 4;
	
	/**
	 * Criterion used to determine whether a node is punctuation.
	 */
	private static int punctuationCriterion = UNICODE_PUNCTUATION_CRITERION;
	

	
	/**
	 * Obtains the criterion used for distinguishing punctuation nodes.
	 * @return the punctuationCriterion
	 */
	public static int getPunctuationCriterion() 
	{
		return punctuationCriterion;
	}

	/**
	 * Sets the criterion used for distinguishing punctuation nodes.
	 * @param punctuationCriterion the punctuationCriterion to set
	 */
	public static void setPunctuationCriterion(int punctuationCriterion) 
	{
		LabelledDependencyStructure.punctuationCriterion = punctuationCriterion;
	}

	/** 
	 * Set the label of dependency coming from node i.
	 * contents must be initialised.
	 * @param i
	 * @param label
	 */
	public void setLinkLabel ( int i , String label )
	{
		if ( linkLabels == null )
			linkLabels = new String[contents.length];
		linkLabels[i-1] = label;
	}
	
	public String getLinkLabel ( int i )
	{
		if ( size() == 0 ) { return null; }
		if ( linkLabels == null )
			linkLabels = new String[contents.length];
		return linkLabels[i-1];
	}
	
	/**
	 * @deprecated Use {@link #setForm(int,String)} instead
	 */
	public void setWord ( int i , String word )
	{
		setForm(i, word);
	}

	public void setForm ( int i , String word )
	{
		if ( words == null )
			words = new String[contents.length];
		words[i-1] = word;
	}
	
	/**
	 * @deprecated Use {@link #getForm(int)} instead
	 */
	public String getWord ( int i )
	{
		return getForm(i);
	}

	public String getForm ( int i )
	{
		if ( size() == 0 ) return null;
		if ( words == null )
			words = new String[contents.length];
		return words[i-1];
	}
	
	public void setPosTag ( int i , String posTag )
	{
		if ( posTags == null )
			posTags = new String[contents.length];
		posTags[i-1] = posTag;
	}
	
	public String getPosTag ( int i )
	{
		if ( size() == 0 ) return null;
		if ( posTags == null )
			posTags = new String[contents.length];
		if ( i == 0 ) return null;
		return posTags[i-1];
	}
	
	public void setCPosTag ( int i , String cPosTag )
	{
		if ( cPosTags == null )
			cPosTags = new String[contents.length];
		cPosTags[i-1] = cPosTag;
	}
	
	public String getCPosTag ( int i )
	{
		if ( size() == 0 ) return null;
		if ( cPosTags == null )
			cPosTags = new String[contents.length];
		return cPosTags[i-1];
	}
	
	public void setLemma( int i , String lemma )
	{
		if ( lemmata == null )
			lemmata = new String[contents.length];
		lemmata[i-1] = lemma;
	}
	
	public String getLemma ( int i )
	{
		if ( size() == 0 ) return null;
		if ( lemmata == null )
			lemmata = new String[contents.length];
		return lemmata[i-1];
	}
	
	public void setFeats( int i , String feat )
	{
		if ( feats == null )
			feats = new String[contents.length];
		feats[i-1] = feat;
	}
	
	public String getFeats ( int i )
	{
		if ( size() == 0 ) return null;
		if ( feats == null )
			feats = new String[contents.length];
		return feats[i-1];
	}
	
	
	/**
	 * For the use of isPunctuation (String s)
	 */
	private static Pattern punctuationPattern = Pattern.compile("\\p{Punct}*",Pattern.UNICODE_CHARACTER_CLASS); //requires java 1.7
	
	/**
	 * Returns whether a given string is Unicode punctuation.
	 * There is a discrepancy between Java 1.7 and Java 1.8 in the treatment of the section symbol (§).
	 * Java 1.7 doesn't count it as punctuation, while 1.8 does.
	 * We unify the behavior by hand. We won't consider it to be punctuation.
	 * @param s
	 * @return
	 */
	public static boolean isUnicodePunctuation ( String s )
	{
		if ( s.contains("§") ) return false;
		Matcher m = punctuationPattern.matcher(s);
		return m.matches();
	}
	
	public boolean isPunctuation ( int i )
	{
		if ( punctuationCriterion == UNICODE_PUNCTUATION_CRITERION )
		{
			String form = getForm(i);
			if ( form == null ) return false;
			return isUnicodePunctuation(form);
		}
		else if ( punctuationCriterion == HAMLEDT_PUNCTUATION_CRITERION )
		{
			String featString = getFeats(i);
			if ( featString == null ) return false;
			else return featString.contains("pos=punc");
		}
		else if ( punctuationCriterion == HAMLEDT_PUNCTUATION_CRITERION_PLUS_NULL_ELEMENTS )
		{
			String form = getForm(i);
			String featString = getFeats(i);
			if ( form == null ) return false;
			if ( featString == null ) return false;
			if ( "NULL".equals(form) ) return true;
			if ( featString.contains("pos=punc") ) return true;
			return false;
		}
		else if ( punctuationCriterion == UNIVERSAL_DEPENDENCIES_PUNCTUATION_CRITERION_STRICT )
		{
			String uPosTag = getCPosTag(i);
			String depLabel = getLinkLabel(i);
			if ( "PUNCT".equals(uPosTag) && "punct".equals(depLabel) ) return true;
			if ( "PUNCT".equals(uPosTag) || "punct".equals(depLabel) ) System.err.println("UPOSTAG/LABEL mismatch in punctuation: " + this.getSentence() + ": " + this);
			return false;
		}
		else if ( punctuationCriterion == UNIVERSAL_DEPENDENCIES_PUNCTUATION_CRITERION )
		{
			String uPosTag = getCPosTag(i);
			if ( "PUNCT".equals(uPosTag) ) return true;
			return false;
		}
		else
			return false;
	}
	
	public String getSentence()
	{
		StringBuffer sb = new StringBuffer("");
		for ( int i = 0 ; i < words.length ; i++ )
		{
			sb.append(words[i]);
			if ( i < words.length - 1 )
				sb.append(" ");
		}
		return new String(sb);
	}
	
	public void setDisplayName( String displayName )
	{
		this.displayName = displayName;
	}
	
	public String getDisplayName()
	{
		return displayName;
	}
	
	public boolean hasPunctuation()
	{
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			if ( isPunctuation(i) ) return true;
		}
		return false;
	}
	
	/**
	 * Returns true if the tree has a punctuation node which is the head of a non-punctuation node.
	 * @return
	 */
	public boolean hasSyntacticallyRelevantPunctuation()
	{
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			int head = getParentForNode(i);
			if ( head != DUMMY_ROOT && isPunctuation(head) && !isPunctuation(i) ) return true;
		}
		return false;
	}
	
	/**
	 * Returns the result of removing all punctuation from the tree. If a punctuation node has children, then the children are attached to the punctuation node's parent.
	 */
	public LabelledDependencyStructure removePunctuation()
	{
		LabelledDependencyStructure result = (LabelledDependencyStructure) this.clone();
		boolean done = false;
		while ( !done )
		{
			done = true;
			for ( int i = 1 ; i <= result.getNumberOfNodes() ; i++ )
			{
				if ( result.isPunctuation(i) )
				{
					int head = result.getParentForNode(i);
					//System.out.println("Contract " + head + "," + i);
					result = result.contract(head,i);
					done = false;
					break;
				}
			}
		}
		return result;
	}
	
	
	/**
	 * Copies all the information in a node of a dependency structure to another node of another dependency structure.
	 * @param target
	 * @param targetNode
	 * @param source
	 * @param sourceNode
	 */
	private void assignNode ( LabelledDependencyStructure target , int targetNode , LabelledDependencyStructure source , int sourceNode )
	{
		target.contents[targetNode-1] = source.contents[sourceNode-1];
		target.setLinkLabel(targetNode,source.getLinkLabel(sourceNode));
		target.setLemma(targetNode,source.getLemma(sourceNode));
		target.setCPosTag(targetNode,source.getCPosTag(sourceNode));
		target.setPosTag(targetNode,source.getPosTag(sourceNode));
		target.setForm(targetNode,source.getForm(sourceNode));
		target.setFeats(targetNode,source.getFeats(sourceNode));
	}
	
	/**
	 * Returns the result of removing a given node from this dependency structure.
	 * Consequences:
	 * - The given node is removed (duh!)
	 * - All indices > that one have to be updated in the contents array
	 * Precondition: the removed node has no dependents
	 * @param node
	 * @return
	 */
	private LabelledDependencyStructure removeNode ( int node )
	{
		//translate indices
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			if ( contents[i] > node )
				contents[i]--;
		}
		
		//actually remove the node
		int[] newContents = new int[contents.length-1];
		LabelledDependencyStructure result = new LabelledDependencyStructure();
		result.init(newContents);
		for ( int i = 1 ; i <= getNumberOfNodes() ; i++ )
		{
			if ( i < node )
			{
				assignNode(result,i,this,i);
			}
			else if ( i > node )
			{
				assignNode(result,i-1,this,i);
			}
		}
		
		return result;
		
	}
	
	
	
	/**
	 * Contracts the dependency structure using this head->dependent link.
	 * The dependent disappears and its children go to the head.
	 * @param head
	 * @param dependent
	 */
	public LabelledDependencyStructure contract ( int head , int dependent )
	{
		if ( !existsArrow ( dependent,head ) )
			return null;
		
		//link children of dependent as children of head
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			if ( contents[i] == dependent )
				contents[i] = head;
		}
		
		return removeNode(dependent);
	}
	
	public Object clone()
	{
		LabelledDependencyStructure ds = new LabelledDependencyStructure();
		ds.contents = this.contents.clone(); //shallow, but doesn't matter, it's an array of int
		ds.linkLabels = this.linkLabels.clone();
		ds.lemmata = this.lemmata.clone();
		ds.cPosTags = this.cPosTags.clone();
		ds.posTags = this.posTags.clone();
		ds.words = this.words.clone();
		ds.feats = this.feats.clone();
		ds.displayName = this.displayName;
		return ds;
	}
	
	public String toString()
	{
		StringBuffer sb = new StringBuffer();
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			sb.append(contents[i]);
			sb.append(" ");
		}
		if ( contents.length == 0 ) sb.append("Zero-length sentence!");
		else
		{
			for ( int i = 0 ; i < words.length ; i++ )
			{
				sb.append(words[i]);
				sb.append(" ");
			}
		}
		sb.append("[forest: ");
		boolean tree = isForest();
		sb.append(tree);
		sb.append("]");		
		
		return new String(sb);
	}
	
	/**
	 * Returns a string with the sequence of heads (i.e. the contents array) and nothing else.
	 * @return
	 */
	public String getHeadSequence()
	{
		StringBuffer sb = new StringBuffer();
		for ( int i = 0 ; i < contents.length ; i++ )
		{
			sb.append(contents[i]);
			if ( i < contents.length-1 ) sb.append(" ");
		}
		if ( contents.length == 0 ) sb.append("Zero-length sentence!");
		return new String(sb);
	}
	
	/**
	 * This is just a test of isUnicodePunctuation(String s)
	 * @param args
	 */
	public static void main ( String[] args )
	{
		String[] myStrings = { "a" , "barca" , "写字帖" , "." , "," , "：.", "a.", ".a", "??" };
		for ( int i = 0 ; i < myStrings.length ; i++ )
		{
			System.out.print(myStrings[i] + " is punctuation? ");
			System.out.println(isUnicodePunctuation(myStrings[i]));
		}
	}
	
}
