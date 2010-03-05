import java.io.DataInputStream;
import java.util.Vector;
import java.util.Hashtable;

/**
 * On instantiation reads the transducer's alphabet and provides an interface
 * to it. Flag diacritic parsing is also handled here.
 */
public class TransducerAlphabet
{
    public Vector<String> keyTable;
    public Hashtable<Integer, FlagDiacriticOperation> operations;

    public TransducerAlphabet(DataInputStream charstream,
		    int number_of_symbols) throws java.io.IOException
    {
	keyTable = new Vector<String>();
	operations = new Hashtable<Integer, FlagDiacriticOperation>();
	int i = 0;
	int charindex;
	byte[] chars = new byte[1000]; // FIXME magic number
	while (i < number_of_symbols)
	    {
		charindex = 0;
		chars[charindex] = charstream.readByte();
		while (chars[charindex] != 0)
		    {
			++charindex;
			chars[charindex] = charstream.readByte();
		    }
		String ustring = new String(chars, 0, charindex, "UTF-8");
		if (ustring.length() > 5 && ustring.charAt(0) == '@' && ustring.charAt(ustring.length()-1) == '@' && ustring.charAt(2) == '.')
		    { // flag diacritic identified
			String op;
			String feat;
			String val;
			String[] parts = ustring.substring(1,ustring.length()-1).split("\\.");
			op = parts[0];
			feat = parts[1];
			if (parts.length == 3) {
			    val = parts[2];
			} else {
			    val = "";
			}
			operations.put(i, new FlagDiacriticOperation(op, feat, val));
			keyTable.add("#");
			i++;
			continue;
		    }
		keyTable.add(ustring);
		i++;
	    }
    }
}
