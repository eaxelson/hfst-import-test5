package net.sf.hfst;

import java.io.FileInputStream;
import java.io.DataInputStream;
import java.io.Console;
import java.util.Collection;

import net.sf.hfst.Transducer;
import net.sf.hfst.UnweightedTransducer;
import net.sf.hfst.WeightedTransducer;
import net.sf.hfst.NoTokenizationException;

/**
 * HfstRuntimeReader takes a transducer (the name of which should
 * be the first argument) of its own format (these can be generated with
 * eg. hfst-runtime-convert) and reads one word at a time from standard
 * input; output is a newline-separated list of analyses.
 *
 * This is essentially a Java port of hfst-runtime-reader
 * written by Miikka Silfverberg in C++.
 *
 * @author sam.hardwick@iki.fi
 *
 */
public class HfstOptimizedLookup {
    public final static long TRANSITION_TARGET_TABLE_START = 2147483648l; // 2^31 or UINT_MAX/2 rounded up
    
    public final static float INFINITE_WEIGHT = (float) 4294967295l; // this is hopefully the same as
    // static_cast<float>(UINT_MAX) in C++
    public final static int NO_SYMBOL_NUMBER = 65535; // this is USHRT_MAX

    public static enum FlagDiacriticOperator {P, N, R, D, C, U};

    public static void runTransducer(Transducer t)
    {
	System.out.println("Ready for input.");
	Console console = System.console();
	String str = console.readLine();
	while (str != null)
	    {
		try{
		    for ( String analysis : t.analyze(str) )
			{
			    System.out.println( analysis );
			}
		} catch (NoTokenizationException e) {
		    System.out.println(e.message());
		}
    		str = console.readLine();
	    }
    }
    
    
    public static void main(String[] argv) throws java.io.IOException
    {
	if (argv.length != 1)
	    {
		System.err.println("Usage: java HfstRuntimeReader FILE");
		System.exit(1);
	    }
	FileInputStream transducerfile = null;
	try
	    { transducerfile = new FileInputStream(argv[0]); }
	catch (java.io.FileNotFoundException e)
	    {
		System.err.println("File not found: couldn't read transducer file " + argv[0] + ".");
		System.exit(1);
	    }
	System.out.println("Reading header...");
	TransducerHeader h = new TransducerHeader(transducerfile);
	DataInputStream charstream = new DataInputStream(transducerfile);
	System.out.println("Reading alphabet...");
	TransducerAlphabet a = new TransducerAlphabet(charstream, h.getSymbolCount());
	System.out.println("Reading transition and index tables...");
	if (h.isWeighted())
	    {
		Transducer transducer = new WeightedTransducer(transducerfile, h, a);
		runTransducer(transducer);
	    } else
	    {
		Transducer transducer = new UnweightedTransducer(transducerfile, h, a);
		runTransducer(transducer);
	    }
    }
}