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
public class LanguageCodeConversion 
{

    //careful, if a language name contains another (e.g. Ancient Greek contains Greek) the longest name
    //must come before!
    //updated for ud 2.4
    //updated for ud 2.5
	//updated for ud 2.6
	static String[] languages =  {
	        "Swiss_German",
	        "Hindi_English",
	        "Old_French",
	        "Abaza",
	        "Afrikaans",
	        "Akkadian",
	        "Albanian",
	        "Amharic",
	        "Ancient_Greek",
	        "Arabic",
	        "Armenian",
	        "Assyrian",
	        "Bambara",
	        "Basque",
	        "Belarusian",
	        "Bhojpuri",
	        "Breton",
	        "Bulgarian",
	        "Buryat",
	        "Cantonese",
	        "Catalan",
	        "Classical_Chinese",
	        "Chinese",
	        "Coptic",
	        "Croatian",
	        "Czech",
	        "Danish",
	        "Dutch",
	        "English",
	        "Erzya",
	        "Estonian",
	        "Faroese",
	        "Finnish",
	        "French",
	        "Galician",
	        "German",
	        "Gothic",
	        "Greek",
	        "Hebrew",
	        "Hindi",
	        "Hungarian",
	        "Icelandic",
	        "Indonesian",
	        "Old_Irish",
	        "Irish",
	        "Italian",
	        "Japanese",
	        "Karelian",
	        "Kazakh",
	        "Komi_Permyak",
	        "Komi_Zyrian",
	        "Korean",
	        "Kurmanji",
	        "Latin",
	        "Latvian",
	        "Lithuanian",
	        "Livvi",
	        "Magahi",
	        "Maltese",
	        "Marathi",
	        "Mbya_Guarani",
	        "Moksha",
	        "Naija",
	        "North_Sami",
	        "Norwegian",
	        "Old_Church_Slavonic",
	        "Persian",
	        "Polish",
	        "Portuguese",
	        "Romanian",
	        "Old_Russian",
	        "Russian",
	        "Sanskrit",
	        "Scottish_Gaelic",
	        "Serbian",
	        "Sindhi",
	        "Skolt_Sami",
	        "Slovak",
	        "Slovenian",
	        "Spanish",
	        "Swedish_Sign_Language",
	        "Swedish",
	        "Tagalog",
	        "Tamil",
	        "Telugu",
	        "Thai",
	        "Turkish",
	        "Ukrainian",
	        "Upper_Sorbian",
	        "Urdu",
	        "Uyghur",
	        "Vietnamese",
	        "Warlpiri",
	        "Welsh",
	        "Wolof",
	        "Yoruba",        
	    };
	    static String[] codes = {
	        "gsw",
	        "hin_eng",
	        "fro",
	        "abq",
	        "afr",
	        "akk",
	        "alb",
	        "amh",
	        "grc",
	        "arb",
	        "hye",
	        "aii",
	        "bam",
	        "eus",
	        "bel",
	        "bho",
	        "bre",
	        "bul",
	        "bua",
	        "yue",
	        "cat",
	        "lzh",
	        "cmn",
	        "cop",
	        "hrv",
	        "ces",
	        "dan",
	        "nld",
	        "eng",
	        "myv",
	        "est",
	        "fao",
	        "fin",
	        "fra",
	        "glg",
	        "deu",
	        "got",
	        "ell",
	        "heb",
	        "hin",
	        "hun",
	        "isl",
	        "ind",
	        "mga",
	        "gle",
	        "ita",
	        "jpn",
	        "krl",
	        "kaz",
	        "koi",
	        "kpv",
	        "kor",
	        "kmr",
	        "lat",
	        "lav",
	        "lit",
	        "olo",
	        "mag",
	        "mlt",
	        "mar",
	        "gun",
	        "mdf",
	        "pcm",
	        "sme",
	        "nob",
	        "chu",
	        "pes",
	        "pol",
	        "por",
	        "ron",
	        "orv",
	        "rus",
	        "san",
	        "gla",
	        "srp",
	        "snd",
	        "sms",
	        "slk",
	        "slv",
	        "spa",
	        "swl",
	        "swe",
	        "tgl",
	        "tam",
	        "tel",
	        "tha",
	        "tur",
	        "ukr",
	        "hsb",
	        "urd",
	        "uig",
	        "vie",
	        "wbp",
	        "cym",
	        "wol",
	        "yor",
	    };
	    
	    //this is for the HamleDT files that have a 2-letter code.
	    static String[] languages2 = {
	        "-ar",
	        "-bg",
	        "-bn",
	        "-ca",
	        "-cs",
	        "-da",
	        "-de",
	        "-el",
	        "-en",
	        "-es",
	        "-et",
	        "-eu",
	        "-fa",
	        "-fi",
	        "-grc",
	        "-hi",
	        "-hu",
	        "-it",
	        "-ja",
	        "-la",
	        "-nl",
	        "-pt",
	        "-ro",
	        "-ru",
	        "-sk",
	        "-sl",
	        "-sv",
	        "-ta",
	        "-te",
	        "-tr",
	    };
	    
	    static String[] codes2 = {
	        "arb",
	        "bul",
	        "ben", //new
	        "cat",
	        "ces",
	        "dan",
	        "deu",
	        "ell",
	        "eng",
	        "spa",
	        "est",
	        "eus",
	        "pes",
	        "fin",
	        "grc",
	        "hin",
	        "hun",
	        "ita",
	        "jpn",
	        "lat",
	        "nld",
	        "por",
	        "ron",
	        "rus",
	        "slk",
	        "slv",
	        "swe",
	        "tam",
	        "tel",
	        "tur",
	    };
	    
	public static String convertToISO3LetterCode ( String input )
	{
		int sz = languages.length;
		int sz2 = languages2.length;
	    for ( int i = 0 ; i < sz ; i++ )
	    {
	          if ( input.contains(languages[i]) ) return codes[i];
	    }
	    for ( int i = 0 ; i < sz2 ; i++ )
	    {
	        if ( input.contains(languages2[i]) ) return codes2[i];
	    }
	    System.err.println("ISO code not found for " + input);
	    return "unk";
	}
	    

}
