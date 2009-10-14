import java.io.FileInputStream;
import java.io.DataInputStream;
import java.io.Console;

/**
 * HfstRuntimeReader takes a transducer (the name of which should
 * be the first argument) of its own format (these can be generated with
 * eg. hfst-runtime-convert) and reads one word at a time from standard
 * input; output is a newline-separated list of analyses.
 *
 * This is essentially a Java port of hfst-runtime-reader
 * written by Miikka Silfverberg in C++.
 *
 * Last changed: 5.8.2009
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

    public interface transducer
    {
	Boolean analyze(String str);
	void printAnalyses();
    }

    public static void runTransducer(transducer t)
    {
	Console console = System.console();
	String str = console.readLine();
	while (str != null)
	    {
		if (t.analyze(str))
		    {
			t.printAnalyses();
			System.out.println();
		    } else
		    {
			//System.out.println("tokenization failed");
			// tokenization failed
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
	TransducerHeader h = new TransducerHeader(transducerfile);
	DataInputStream charstream = new DataInputStream(transducerfile);
	TransducerAlphabet a = new TransducerAlphabet(charstream, h.getSymbolCount());
	if (h.isWeighted())
	    {
		TransducerWeighted transducer = new TransducerWeighted(transducerfile, h, a);
		runTransducer(transducer);
	    } else
	    {
		Transducer transducer = new Transducer(transducerfile, h, a);
		runTransducer(transducer);
	    }
    }
}