import java.util.Hashtable;

public class LetterTrie
{
    public class LetterTrieNode
    {
	private Hashtable<Character, Integer> symbols;
	private Hashtable<Character, LetterTrieNode> children;

	public void addString(String str, Integer symbolNumber)
	{
	    if (str.length() > 1)
		{
		    if (children.containsKey(str.charAt(0)) == false)
			{
			    children.put(str.charAt(0), new LetterTrieNode());
			}
		    children.get(str.charAt(0)).addString(str.substring(1,str.length()), symbolNumber);
		} else if (str.length() == 1)
		{
		    symbols.put(str.charAt(0), symbolNumber);
		}
	}
	    
	public int findKey(IndexString string)
	{
	    if (string.index >= string.str.length())
		{
		    return HfstOptimizedLookup.NO_SYMBOL_NUMBER;
		}
	    ++string.index;
	    if (children.containsKey(string.str.charAt(string.index - 1)) == false)
		{
		    if (symbols.get(string.str.charAt(string.index - 1)) == null)
			{
			    return HfstOptimizedLookup.NO_SYMBOL_NUMBER;
			}
		    return symbols.get(string.str.charAt(string.index - 1));
		}
	    int oldindex = string.index;
	    int temp = children.get(string.str.charAt(string.index - 1)).findKey(string);
	    if (temp == HfstOptimizedLookup.NO_SYMBOL_NUMBER)
		{
		    string.index = oldindex;
		    return symbols.get(string.str.charAt(string.index - 1));
		}
	    return temp;
	}
	
	public LetterTrieNode()
	{
	    symbols = new Hashtable<Character, Integer>();
	    children = new Hashtable<Character, LetterTrieNode>();
	}
    }
	
    private LetterTrieNode root;

    public LetterTrie()
    {
	root = new LetterTrieNode();
    }

    public void addString(String str, Integer symbolNumber)
    {
	root.addString(str, symbolNumber);
    }

    Integer findKey(IndexString str)
    {
	return root.findKey(str);
    }
}
    
