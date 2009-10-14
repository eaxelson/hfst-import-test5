//import java.io.DataInputStream;
import java.io.FileInputStream;

/**
 * On instantiation reads the transducer's header and provides an interface
 * to it.
 */
public class TransducerHeader
{
    private int number_of_input_symbols;
    private int number_of_symbols;
    private long size_of_transition_index_table;
    private long size_of_transition_target_table;
    private long number_of_states;
    private long number_of_transitions;

    private Boolean weighted;
    private Boolean deterministic;
    private Boolean input_deterministic;
    private Boolean minimized;
    private Boolean cyclic;
    private Boolean has_epsilon_epsilon_transitions;
    private Boolean has_input_epsilon_transitions;
    private Boolean has_input_epsilon_cycles;
    private Boolean has_unweighted_input_epsilon_cycles;

    /**
     * Read in the (56 bytes of) header information, which unfortunately
     * is mostly in little-endian unsigned form.
     */
    public TransducerHeader(FileInputStream file) throws java.io.IOException
    {
	ByteArray b = new ByteArray(56);
	file.read(b.bytes);
	
	number_of_input_symbols = b.getUShort();
	number_of_symbols = b.getUShort();
	size_of_transition_index_table = b.getUInt();
	size_of_transition_target_table = b.getUInt();
	number_of_states = b.getUInt();
	number_of_transitions = b.getUInt();

	weighted = b.getBool();
	deterministic = b.getBool();
	input_deterministic = b.getBool();
	minimized = b.getBool();
	cyclic = b.getBool();
	has_epsilon_epsilon_transitions = b.getBool();
	has_input_epsilon_transitions = b.getBool();
	has_input_epsilon_cycles = b.getBool();
	has_unweighted_input_epsilon_cycles = b.getBool();
    }

    public int getInputSymbolCount()
    { return number_of_input_symbols; }
	
    public int getSymbolCount()
    { return number_of_symbols; }

    public long getIndexTableSize()
    { return size_of_transition_index_table; }

    public long getTargetTableSize()
    { return size_of_transition_target_table; }

    public Boolean isWeighted()
    { return weighted; }
}
