import java.io.DataInputStream;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.Enumeration;
import java.util.Vector;
import java.util.Iterator;
import java.util.Hashtable;
import java.util.Stack;

/**
 * Reads the header, alphabet, index table and transition table and provides
 * interfaces to them.
 */
public class TransducerWeighted implements HfstOptimizedLookup.transducer
{

    public class TransitionIndex
    {
	protected int inputSymbol;
	protected long firstTransitionIndex;
	
	public TransitionIndex(int input, long firstTransition)
	    {
		inputSymbol = input;
		firstTransitionIndex = firstTransition;
	    }
	
	public Boolean matches(int s)
	{
	    if (inputSymbol == HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		{ return false; }
	    if (s == HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		{ return true; }
	    return (s == inputSymbol);
	}
	
	public Boolean isFinal()
	{
	    if (inputSymbol != HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		{ return false; }
	    float w = (float) firstTransitionIndex;
	    return (w != HfstOptimizedLookup.INFINITE_WEIGHT);
	}

	public float getFinalWeight()
	{ return (float) firstTransitionIndex; }
	
	    public long target()
	{ return firstTransitionIndex; }
	
	public int getInput()
	{ return inputSymbol; }
    }
    
    
    
    /**
     * On instantiation reads the transducer's index table and provides an interface
     * to it.
     */
    public class IndexTable
    {
	private Hashtable<Long, TransitionIndex> indices;
	
	public IndexTable(FileInputStream filestream,
			  long indicesCount) throws java.io.IOException
	{
	    ByteArray b = new ByteArray((int) indicesCount*6);
	    filestream.read(b.bytes);
	    // each index entry is a unsigned short followed by an unsigned int
	    indices = new Hashtable<Long, TransitionIndex>();

	    long i = 0;
	    while (i < indicesCount)
		{
		    indices.put(i, new TransitionIndex(b.getUShort(), b.getUInt()));
		    i++;
		}
	}

	public Boolean isFinal(long index)
	{ return indices.get(index).isFinal(); }

	public TransitionIndex at(long index)
	{ return indices.get(index); }

	public Hashtable<Long, TransitionIndex> getIndices()
	{ return indices; }
    }
    
    public class Transition
    {
	protected int inputSymbol;
	protected int outputSymbol;
	protected long targetIndex;
	protected float weight;
	
	public Transition(int input, int output, long target, float w)
	{
	    inputSymbol = input;
	    outputSymbol = output;
	    targetIndex = target;
	    weight = w;
	}

	public Transition()
	{
	    inputSymbol = HfstOptimizedLookup.NO_SYMBOL_NUMBER;
	    outputSymbol = HfstOptimizedLookup.NO_SYMBOL_NUMBER;
	    targetIndex = Long.MAX_VALUE;
	    weight = HfstOptimizedLookup.INFINITE_WEIGHT;
	}
	
	public Boolean matches(int symbol)
	{
	    if (inputSymbol == HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		{ return false; }
	    if (symbol == HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		{ return true; }
	    return (inputSymbol == symbol);
	}
	public long target()
	{ return targetIndex; }
	
	public int getOutput()
	{ return outputSymbol; }
	
	public int getInput()
	{ return inputSymbol; }
	
	public Boolean isFinal()
	{
	    if (inputSymbol != HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		return false;
	    if (outputSymbol != HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		return false;
	    return (weight != HfstOptimizedLookup.INFINITE_WEIGHT);
	}

	public float getWeight()
	{ return weight; }
    }
    
    /**
     * On instantiation reads the transducer's transition table and provides an
     * interface to it.
     */
    public class TransitionTable
    {
	private Hashtable<Long, Transition> transitions;
	private long position;

	public TransitionTable(FileInputStream filestream,
			       long transitionCount) throws java.io.IOException
	{
	    ByteArray b = new ByteArray((int) transitionCount*12);
	    // 12 bytes per transition
	    // each transition entry is two unsigned shorts, an unsigned int and a float
	    filestream.read(b.bytes);
	    transitions = new Hashtable<Long, Transition>();
	    position = 0;
	    long i = 0;
	    while (i < transitionCount)
		{
		    transitions.put(i, new Transition(b.getUShort(), b.getUShort(), b.getUInt(), b.getFloat()));
		    i++;
		}
	}

	public void set(long pos)
	{
	    if (pos >= HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START)
		{
		    position = pos - HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START;
		} else
		{
		    position = pos;
		}
	}

	public Transition at(long pos)
	{ return transitions.get((pos-HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START)); }

	public void next()
	{ ++position; }

	public Boolean matches(int symbol)
	{ return transitions.get(position).matches(symbol); }

	public long getTarget()
	{ return transitions.get(position).target(); }

	public int getOutput()
	{ return transitions.get(position).getOutput(); }

	public int getInput()
	{ return transitions.get(position).getInput(); }

	public Boolean isFinal(long pos)
	{
	    if (pos >= HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START)
		{ return transitions.get((pos - HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START)).isFinal(); }
	    else { return transitions.get((pos)).isFinal(); }
	}

	public Hashtable<Long, Transition> getTransitions()
	{ return transitions; }
    }

    private class FlagState
    {
	public String value;
	public Boolean polarity;
	
	public FlagState(String val, Boolean pol)
	{
	    value = val;
	    polarity = pol;
	}
    }

    protected TransducerHeader header;
    protected TransducerAlphabet alphabet;
    protected Stack< Hashtable <String, FlagState> > stateStack;
    protected Hashtable<Integer, FlagDiacriticOperation> operations;
    protected LetterTrie letterTrie;
    protected IndexTable indexTable;
    protected Hashtable<Long, TransitionIndex> indices;
    protected TransitionTable transitionTable;
    protected Hashtable<Long, Transition> transitions;
    protected Vector<String> displayVector;
    protected int[] outputString;
    protected Vector<Integer> inputString;
    protected int outputPointer;
    protected int inputPointer;
    protected float current_weight;
    
    public TransducerWeighted(FileInputStream file, TransducerHeader h, TransducerAlphabet a) throws java.io.IOException
    {
	header = h;
	alphabet = a;
	stateStack = new Stack< Hashtable <String, FlagState> >();
	stateStack.push(new Hashtable <String, FlagState>());
	operations = alphabet.operations;
	letterTrie = new LetterTrie();
	int i = 0;
	while (i < header.getSymbolCount())
	    {
		letterTrie.addString(alphabet.keyTable.get(i), i);
		i++;
	    }
	indexTable = new IndexTable(file, header.getIndexTableSize());
	indices = indexTable.getIndices();
	transitionTable = new TransitionTable(file, header.getTargetTableSize());
	transitions = transitionTable.getTransitions();
	displayVector = new Vector<String>();
	outputString = new int[1000];
	for (i = 0; i < 1000; i++)
	    { outputString[i] = HfstOptimizedLookup.NO_SYMBOL_NUMBER; }
	inputString = new Vector<Integer>();
	outputPointer = 0;
	inputPointer = 0;
	current_weight = 0.0f;
    }
    
    private void tryEpsilonIndices(long index)
    {
	if (indices.get(index).getInput() == 0)
	    {
		tryEpsilonTransitions(indices.get(index).target() - HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START);
	    }
    }

    private void tryEpsilonTransitions(long index)
    {
	while (true)
	    {
		// first test for flag
		if (operations.containsKey(transitions.get(index).getInput())) {
		    if (!pushState(operations.get(transitions.get(index).getInput())))
			{
			    ++index;
			    continue;
			} else {
			outputString[outputPointer] = transitions.get(index).getOutput();
			++outputPointer;
			current_weight += transitions.get(index).getWeight();
			getAnalyses(transitions.get(index).target());
			current_weight -= transitions.get(index).getWeight();
			--outputPointer;
			++index;
			stateStack.pop();
			continue;
		    }
		} else if (transitions.get(index).getInput() == 0)
		    { // epsilon transitions
			outputString[outputPointer] = transitions.get(index).getOutput();
			++outputPointer;
			current_weight += transitions.get(index).getWeight();
			getAnalyses(transitions.get(index).target());
			current_weight -= transitions.get(index).getWeight();
			--outputPointer;
			++index;
		    }
		else
		    {
			break;
		    }
	    }
    }

    private void findIndex(long index)
    {
	if (indices.get(index + (inputString.get(inputPointer - 1))).getInput() == inputString.get(inputPointer - 1))
	    {
		findTransitions(indices.get(index + (inputString.get(inputPointer - 1))).target() - HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START);
	    }
    }

    private void findTransitions(long index)
    {
	while (transitions.get(index).getInput() != HfstOptimizedLookup.NO_SYMBOL_NUMBER)
	    {
		if (transitions.get(index).getInput() == inputString.get(inputPointer - 1))
		    {
			outputString[outputPointer] = transitions.get(index).getOutput();
			++outputPointer;
			current_weight += transitions.get(index).getWeight();
			getAnalyses(transitions.get(index).target());
			current_weight -= transitions.get(index).getWeight();
			--outputPointer;
		    } else
		    {
			return;
		    }
		++index;
	    }
    }

    private void getAnalyses(long index)
    {
	if (index >= HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START)
	    {
		index -= HfstOptimizedLookup.TRANSITION_TARGET_TABLE_START;
		tryEpsilonTransitions(index + 1);
		if (inputString.get(inputPointer) == HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		    { // end of input string
			if (transitions.size() <= index)
			    { return; }
			if (transitionTable.isFinal(index))
			    {
				current_weight += transitions.get(index).getWeight();
				noteAnalysis();
				current_weight -= transitions.get(index).getWeight();
			    }
			outputString[outputPointer] = HfstOptimizedLookup.NO_SYMBOL_NUMBER;
			return;
		    }
		++inputPointer;
		findTransitions(index + 1);
	    } else
	    {
		tryEpsilonIndices(index + 1);
		if (inputString.get(inputPointer) == HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		    { // end of input string
			if (indexTable.isFinal(index))
			    {
				current_weight += indices.get(index).getFinalWeight();
				noteAnalysis();
				current_weight -= indices.get(index).getFinalWeight();
			    }
			outputString[outputPointer] = HfstOptimizedLookup.NO_SYMBOL_NUMBER;
			return;
		    }
		++inputPointer;
		findIndex(index + 1);
	    }
	--inputPointer;
	outputString[outputPointer] = HfstOptimizedLookup.NO_SYMBOL_NUMBER;
    }

    private void noteAnalysis()
    {
	int i = 0;
	displayVector.add("");
	while (outputString[i] != HfstOptimizedLookup.NO_SYMBOL_NUMBER)
	    {
		displayVector.set(displayVector.size() - 1, displayVector.lastElement() + alphabet.keyTable.get(outputString[i]));
		++i;
	    }
	displayVector.set(displayVector.size() - 1, displayVector.lastElement() + "\t" + current_weight);
    }

    public Boolean analyze(String input)
    {
	inputString.clear();
	displayVector.clear();
	outputPointer = 0;
	outputString[0] = HfstOptimizedLookup.NO_SYMBOL_NUMBER;
	inputPointer = 0;
	
	IndexString inputLine = new IndexString(input);
	while (inputLine.index < input.length())
	    {
		inputString.add(letterTrie.findKey(inputLine));
	    }
	if ( (inputString.size() == 0) || (inputString.lastElement() == HfstOptimizedLookup.NO_SYMBOL_NUMBER) )
	    {
		return false;
	    }
	inputString.add(HfstOptimizedLookup.NO_SYMBOL_NUMBER);
	getAnalyses(0);
	return true;
    }

    public void printAnalyses()
    {
	Iterator it = displayVector.iterator();
	while (it.hasNext())
	    { System.out.println(it.next()); }
    }

        private Boolean pushState(FlagDiacriticOperation flag)
    {
	if (flag.operation.equals("P")) { // positive set
	    stateStack.push(stateStack.peek());
	    stateStack.peek().put(flag.feature, new FlagState(flag.value, true));
	    return true;
	} else if (flag.operation.equals("N")) { // negative set
	    stateStack.push(stateStack.peek());
	    stateStack.peek().put(flag.feature, new FlagState(flag.value, false));
	    return true;
	} else if (flag.operation.equals("R")) { // require
	    if (flag.value.equals("")) // empty require
		{
		    if (stateStack.peek().get(flag.feature).value.equals(""))
			{
			    return false;
			}
		    else
			{
			    stateStack.push(stateStack.peek());
			    return true;
			}
		}
	    if (stateStack.peek().containsKey(flag.feature))
		{
		    if (stateStack.peek().get(flag.feature).value == flag.value &&
			stateStack.peek().get(flag.feature).polarity == true)
			{
			    stateStack.push(stateStack.peek());
			    return true;
			}
		}
	    return false;
	} else if (flag.operation.equals("D")) { // disallow
	    if (flag.value.equals("")) // empty disallow
		{
		    if (stateStack.peek().get(flag.feature).value.equals(""))
			{
			    return false;
			}
		    else
			{
			    stateStack.push(stateStack.peek());
			    return true;
			}
		}
	    if (stateStack.peek().containsKey(flag.feature))
		{
		    if (stateStack.peek().get(flag.feature).value == flag.value &&
			stateStack.peek().get(flag.feature).polarity == true)
			{
			    return false;
			}
		}
	    stateStack.push(stateStack.peek());
	    return true;
	} else if (flag.operation.equals("C")) { // clear
	    stateStack.push(stateStack.peek());
	    stateStack.peek().remove(flag.feature);
	} else if (flag.operation.equals("U")) { // unification
	    if (!(stateStack.peek().containsKey(flag.feature)) ||
		(stateStack.peek().get(flag.feature).value == flag.value &&
		 stateStack.peek().get(flag.feature).polarity == true) ||
		(stateStack.peek().get(flag.feature).value != flag.value &&
		 stateStack.peek().get(flag.feature).polarity == false))
		{
		    stateStack.push(stateStack.peek());
		    stateStack.peek().put(flag.feature, new FlagState(flag.value, true));
		    return true;
		}
	    return false;
	}
	return false; // compiler sanity
    }
}