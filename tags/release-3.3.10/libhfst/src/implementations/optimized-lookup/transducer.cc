//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "./transducer.h"

#ifndef MAIN_TEST

namespace hfst_ol {

bool should_ascii_tokenize(unsigned char c)
{
    return ((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z'));
}

TransducerAlphabet::TransducerAlphabet(std::istream& is,
                       SymbolNumber symbol_count)
{
    unknown_symbol = NO_SYMBOL_NUMBER;
    for(SymbolNumber i=0; i<symbol_count; i++)
    {
    std::string str;
    std::getline(is, str, '\0');
    symbol_table.push_back(str.c_str());
    if(hfst::FdOperation::is_diacritic(str)) {
        fd_table.define_diacritic(i, str);
    } else if (hfst::is_unknown(str)) {
        unknown_symbol = i;
    }
    if(!is) {
      HFST_THROW(TransducerHasWrongTypeException);
    }
    }
}

TransducerAlphabet::TransducerAlphabet(const SymbolTable& st):
    symbol_table(st)
{
    unknown_symbol = NO_SYMBOL_NUMBER;
    for(SymbolNumber i=0; i<symbol_table.size(); i++)
    {
        if(hfst::FdOperation::is_diacritic(symbol_table[i])) {
            fd_table.define_diacritic(i, symbol_table[i]);
        } else if (hfst::is_unknown(symbol_table[i])) {
            unknown_symbol = i;
        }
    }
}

StringSymbolMap TransducerAlphabet::build_string_symbol_map(void) const
{
    StringSymbolMap ss_map;
    for(SymbolNumber i = 0; i < symbol_table.size(); ++i) {
        ss_map[symbol_table[i]] = i;
    }
    return ss_map;
}

void TransducerAlphabet::display() const
{
    std::cout << "Transducer alphabet:" << std::endl;
    for(size_t i=0;i<symbol_table.size();i++)
    std::cout << " Symbol " << i << ": " << symbol_table[i] << std::endl;
}

void TransitionIndex::display() const
{
    std::cout << "input_symbol: " << input_symbol
          << ", target: " << first_transition_index
          << (final()?" (final)":"") << std::endl;
}
void Transition::display() const
{
    std::cout << "input_symbol: " << input_symbol << ", output_symbol: "
          << output_symbol << ", target: " << target_index
          << (final()?" (final)":"") << std::endl;
}
void TransitionW::display() const
{
    std::cout << "input_symbol: " << input_symbol << ", output_symbol: "
          << output_symbol << ", target: " << target_index
          << ", weight: " << transition_weight << (final()?" (final)":"")
          << std::endl;
}

bool TransitionIndex::matches(SymbolNumber s) const
{
    return input_symbol != NO_SYMBOL_NUMBER && input_symbol == s;
}
bool Transition::matches(SymbolNumber s) const
{
    return input_symbol != NO_SYMBOL_NUMBER && input_symbol == s;
}

bool TransitionIndex::final() const
{
    return input_symbol == NO_SYMBOL_NUMBER
    && first_transition_index != NO_TABLE_INDEX;
}
bool Transition::final() const
{
    return input_symbol == NO_SYMBOL_NUMBER
    && output_symbol == NO_SYMBOL_NUMBER && target_index == 1;
}

Weight TransitionWIndex::final_weight(void) const
{
    union to_weight
    {
    TransitionTableIndex i;
    Weight w;
    } weight;
    weight.i = first_transition_index;
    return weight.w;
}

void OlLetterTrie::add_string(const char * p, SymbolNumber symbol_key)
{
    if (*(p+1) == 0)
    {
    symbols[(unsigned char)(*p)] = symbol_key;
    return;
    }
    if (letters[(unsigned char)(*p)] == NULL)
    {
    letters[(unsigned char)(*p)] = new OlLetterTrie();
    }
    letters[(unsigned char)(*p)]->add_string(p+1,symbol_key);
}

SymbolNumber OlLetterTrie::find_key(char ** p)
{
    const char * old_p = *p;
    ++(*p);
    if (letters[(unsigned char)(*old_p)] == NULL)
    {
    return symbols[(unsigned char)(*old_p)];
    }
    SymbolNumber s = letters[(unsigned char)(*old_p)]->find_key(p);
    if (s == NO_SYMBOL_NUMBER)
    {
    --(*p);
    return symbols[(unsigned char)(*old_p)];
    }
    return s;
}

void Encoder::read_input_symbols(const SymbolTable & kt)
{
    for (SymbolNumber k = 0; k < number_of_input_symbols; ++k)
    {
    const char * p = kt[k].c_str();
    if ((strlen(p) == 1) && should_ascii_tokenize((unsigned char)(*p)))
    {
        ascii_symbols[(unsigned char)(*p)] = k;
    }
    letters.add_string(p,k);
    }
}

SymbolNumber Encoder::find_key(char ** p)
{
    if (ascii_symbols[(unsigned char)(**p)] == NO_SYMBOL_NUMBER)
    {
    return letters.find_key(p);
    }
    SymbolNumber s = ascii_symbols[(unsigned char)(**p)];
    ++(*p);
    return s;
}

bool Transducer::initialize_input(const char * input_str)
{
    char * c = strdup(input_str);
    char * c_orig = c;
    int i = 0;
    SymbolNumber k = NO_SYMBOL_NUMBER;
    for ( char ** Str = &c; **Str != 0; )
    {
    k = encoder->find_key(Str);
    if (k == NO_SYMBOL_NUMBER)
    {
        free(c_orig);
        return false; // tokenization failed
    }
    input_tape[i] = k;
    ++i;
    }
    input_tape[i] = NO_SYMBOL_NUMBER;
    free(c_orig);
    return true;
}

HfstOneLevelPaths * Transducer::lookup_fd(const StringVector & s)

{
    std::string input_str;
    for (StringVector::const_iterator it = s.begin(); it != s.end(); ++it) {
    input_str.append(*it);
    }
    return lookup_fd(input_str);
}

HfstOneLevelPaths * Transducer::lookup_fd(const std::string & s)
{
    return lookup_fd(s.c_str());
}

HfstOneLevelPaths * Transducer::lookup_fd(const char * s)
{
    HfstOneLevelPaths * results = new HfstOneLevelPaths;
    lookup_paths = results;
    if (!initialize_input(s)) {
        lookup_paths = NULL;
        return results;
    }
    //current_weight += s.second;
    get_analyses(input_tape, output_tape, output_tape, 0);
    //current_weight -= s.second;
    lookup_paths = NULL;
    return results;
}

void Transducer::try_epsilon_transitions(SymbolNumber * input_symbol,
                     SymbolNumber * output_symbol,
                     SymbolNumber * original_output_tape,
                     TransitionTableIndex i)
{
//        std::cerr << "try_epsilon_transitions, index " << i << std::endl;
    while (true)
    {
    if (tables->get_transition_input(i) == 0) // epsilon
    {
        *output_symbol = tables->get_transition_output(i);
        current_weight += tables->get_weight(i);
        get_analyses(input_symbol,
             output_symbol+1,
             original_output_tape,
             tables->get_transition_target(i));
        current_weight -= tables->get_weight(i);
        ++i;
    } else if (alphabet->is_flag_diacritic(
               tables->get_transition_input(i))) {
        std::vector<short> old_values(flag_state.get_values());
        if (flag_state.apply_operation(
            *(alphabet->get_operation(
              tables->get_transition_input(i))))) {
        // flag diacritic allowed
        *output_symbol = tables->get_transition_output(i);
        current_weight += tables->get_weight(i);
        get_analyses(input_symbol,
                 output_symbol+1,
                 original_output_tape,
                 tables->get_transition_target(i));
        current_weight -= tables->get_weight(i);
        }
        flag_state.assign_values(old_values);
        ++i;
    } else { // it's not epsilon and it's not a flag, so nothing to do
        return;
    }
    }
}

void Transducer::try_epsilon_indices(SymbolNumber * input_symbol,
                     SymbolNumber * output_symbol,
                     SymbolNumber * original_output_tape,
                     TransitionTableIndex i)
{
//    std::cerr << "try_epsilon_indices, index " << i << std::endl;
    if (tables->get_index_input(i) == 0)
    {
    try_epsilon_transitions(input_symbol,
                output_symbol,
                original_output_tape,
                tables->get_index_target(i) - 
                TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::find_transitions(SymbolNumber input,
                  SymbolNumber * input_symbol,
                  SymbolNumber * output_symbol,
                  SymbolNumber * original_output_tape,
                  TransitionTableIndex i)
{

    while (tables->get_transition_input(i) != NO_SYMBOL_NUMBER)
    {
    if (tables->get_transition_input(i) == input)
    {
      
        *output_symbol = tables->get_transition_output(i);
        current_weight += tables->get_weight(i);
        get_analyses(input_symbol,
             output_symbol+1,
             original_output_tape,
             tables->get_transition_target(i));
        current_weight -= tables->get_weight(i);
    }
    else
    {
        return;
    }
    ++i;
    }
}

void Transducer::find_index(SymbolNumber input,
                SymbolNumber * input_symbol,
                SymbolNumber * output_symbol,
                SymbolNumber * original_output_tape,
                TransitionTableIndex i)
{
    if (tables->get_index_input(i+input) == input)
    {
    find_transitions(input,
             input_symbol,
             output_symbol,
             original_output_tape,
             tables->get_index_target(i+input) - 
             TRANSITION_TARGET_TABLE_START);
    }
}




void Transducer::get_analyses(SymbolNumber * input_symbol,
                  SymbolNumber * output_symbol,
                  SymbolNumber * original_output_tape,
                  TransitionTableIndex i)
{
    if (indexes_transition_table(i))
    {
    i -= TRANSITION_TARGET_TABLE_START;

    try_epsilon_transitions(input_symbol,
                output_symbol,
                original_output_tape,
                i+1);
    
    
    // input-string ended.
    if (*input_symbol == NO_SYMBOL_NUMBER)
    {
        *output_symbol = NO_SYMBOL_NUMBER;
        if (tables->get_transition_finality(i))
        {
        current_weight += tables->get_weight(i);
        note_analysis(original_output_tape);
        current_weight -= tables->get_weight(i);
        }
        return;
    }
      
    SymbolNumber input = *input_symbol;
    ++input_symbol;

    find_transitions(input,
             input_symbol,
             output_symbol,
             original_output_tape,
             i+1);
    }
    else
    {
    try_epsilon_indices(input_symbol,
                output_symbol,
                original_output_tape,
                i+1);
      
      
    if (*input_symbol == NO_SYMBOL_NUMBER)
    { // input-string ended.
        *output_symbol = NO_SYMBOL_NUMBER;
        if (tables->get_index_finality(i))
        {
        current_weight += tables->get_final_weight(i);
        note_analysis(original_output_tape);
        current_weight -= tables->get_final_weight(i);
        }
        return;
    }
      
    SymbolNumber input = *input_symbol;
    ++input_symbol;

    find_index(input,
           input_symbol,
           output_symbol,
           original_output_tape,
           i+1);
    }
    *output_symbol = NO_SYMBOL_NUMBER;
}

void Transducer::note_analysis(SymbolNumber * whole_output_tape)
{
    HfstOneLevelPath result;
    for (SymbolNumber * num = whole_output_tape; *num != NO_SYMBOL_NUMBER; ++num)
    {
    result.second.push_back(alphabet->string_from_symbol(*num));
    }
    result.first = current_weight;
    lookup_paths->insert(result);
}



Transducer::Transducer(): header(NULL), alphabet(NULL), tables(NULL),
              current_weight(0.0), lookup_paths(NULL), encoder(NULL),
	      input_tape(NULL), output_tape(NULL), flag_state() {}

Transducer::Transducer(std::istream& is):
    header(new TransducerHeader(is)),
    alphabet(new TransducerAlphabet(is, header->symbol_count())),
    tables(NULL), current_weight(0.0), lookup_paths(NULL),
    encoder(new Encoder(alphabet->get_symbol_table(),
            header->input_symbol_count())),
    input_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    output_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    flag_state(alphabet->get_fd_table())
{
    load_tables(is);
}


Transducer::Transducer(bool weighted):
    header(new TransducerHeader(weighted)),
    alphabet(new TransducerAlphabet()),
    current_weight(0.0),
    lookup_paths(NULL),
    encoder(new Encoder(alphabet->get_symbol_table(),
            header->input_symbol_count())),
    input_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    output_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    flag_state(alphabet->get_fd_table())
{
    if(weighted)
    tables = new TransducerTables<TransitionWIndex,TransitionW>();
    else
    tables = new TransducerTables<TransitionIndex,Transition>();
}

Transducer::Transducer(const TransducerHeader& header,
               const TransducerAlphabet& alphabet,
               const TransducerTable<TransitionIndex>& index_table,
               const TransducerTable<Transition>& transition_table):
    header(new TransducerHeader(header)),
    alphabet(new TransducerAlphabet(alphabet)),
    tables(new TransducerTables<TransitionIndex,Transition>(
           index_table, transition_table)),
    current_weight(0.0),
    lookup_paths(NULL),
    encoder(new Encoder(alphabet.get_symbol_table(),
            header.input_symbol_count())),
    input_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    output_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    flag_state(alphabet.get_fd_table())
{}

Transducer::Transducer(const TransducerHeader& header,
               const TransducerAlphabet& alphabet,
               const TransducerTable<TransitionWIndex>& index_table,
               const TransducerTable<TransitionW>& transition_table):
    header(new TransducerHeader(header)),
    alphabet(new TransducerAlphabet(alphabet)),
    tables(new TransducerTables<TransitionWIndex,TransitionW>(
           index_table, transition_table)),
    current_weight(0.0),
    lookup_paths(NULL),
    encoder(new Encoder(alphabet.get_symbol_table(),
            header.input_symbol_count())),
    input_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    output_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN))),
    flag_state(alphabet.get_fd_table())
{}

Transducer::~Transducer()
{
    delete header;
    delete alphabet;
    delete tables;
    delete encoder;
    free(input_tape);
    free(output_tape);
}

TransducerTable<TransitionWIndex> & Transducer::copy_windex_table()
{
    if (!header->probe_flag(Weighted)) {
      HFST_THROW(TransducerHasWrongTypeException);
    }
    TransducerTable<TransitionWIndex> * another =
    new TransducerTable<TransitionWIndex>;
    for (unsigned int i = 0; i < header->index_table_size(); ++i) {
    another->append(TransitionWIndex(tables->get_index_input(i),
                    tables->get_index_target(i)));
    }
    return *another;
}
TransducerTable<TransitionW> & Transducer::copy_transitionw_table()
{
    if (!header->probe_flag(Weighted)) {
      HFST_THROW(TransducerHasWrongTypeException);
    }
    TransducerTable<TransitionW> * another = new TransducerTable<TransitionW>;
    for (unsigned int i = 0; i < header->target_table_size(); ++i) {
    another->append(TransitionW(tables->get_transition_input(i),
                   tables->get_transition_output(i),
                   tables->get_transition_target(i),
                   tables->get_weight(i)));
    }
    return *another;
}
TransducerTable<TransitionIndex> & Transducer::copy_index_table()
{
    if (header->probe_flag(Weighted)) {
      HFST_THROW(TransducerHasWrongTypeException);
    }
    TransducerTable<TransitionIndex> * another =
    new TransducerTable<TransitionIndex>;
    for (unsigned int i = 0; i < header->index_table_size(); ++i) {
    another->append(tables->get_index(i));
    }
    return *another;
}
TransducerTable<Transition> & Transducer::copy_transition_table()
{
    if (header->probe_flag(Weighted)) {
      HFST_THROW(TransducerHasWrongTypeException);
    }
    TransducerTable<Transition> * another = new TransducerTable<Transition>();
    for (unsigned int i = 0; i < header->target_table_size(); ++i) {
    another->append(tables->get_transition(i));
    }
    return *another;
}


void Transducer::load_tables(std::istream& is)
{
    if(header->probe_flag(Weighted))
    tables = new TransducerTables<TransitionWIndex,TransitionW>(
        is, header->index_table_size(),header->target_table_size());
    else
    tables = new TransducerTables<TransitionIndex,Transition>(
        is, header->index_table_size(),header->target_table_size());
    if(!is) {
      HFST_THROW(TransducerHasWrongTypeException);
    }
}

void Transducer::write(std::ostream& os) const
{
    header->write(os);
    alphabet->write(os);
    for(size_t i=0;i<header->index_table_size();i++)
    tables->get_index(i).write(os, header->probe_flag(Weighted));
    for(size_t i=0;i<header->target_table_size();i++)
    tables->get_transition(i).write(os, header->probe_flag(Weighted));
}

Transducer * Transducer::copy(Transducer * t, bool weighted)
{
    Transducer * another;
    if (weighted) {
    another = new Transducer(
        t->get_header(), t->get_alphabet(),
        t->copy_windex_table(), t->copy_transitionw_table());
    } else {
    another = new Transducer(
        t->get_header(), t->get_alphabet(),
        t->copy_index_table(), t->copy_transition_table());
    }
    return another;
}

void Transducer::display() const
{
    std::cout << "-----Displaying optimized-lookup transducer------"
          << std::endl;
    header->display();
    alphabet->display();
    tables->display();
    std::cout << "-------------------------------------------------"
          << std::endl;
}

TransitionTableIndexSet Transducer::get_transitions_from_state(
    TransitionTableIndex state_index) const
{
    TransitionTableIndexSet transitions;
  
    if(indexes_transition_index_table(state_index)) {
    // for each input symbol that has a transition from this state
    for(SymbolNumber symbol=0; symbol < header->symbol_count(); symbol++) {
	// There may be flags at index 0 even if there aren't
	// any epsilons, so those have to be checked for
 	if (alphabet->is_flag_diacritic(symbol)) {
	    TransitionTableIndex transition_i =
		get_index(state_index+1).get_target();
	    if (!get_index(state_index+1).matches(0)) {
		continue;
	    }
	    while(true) {
		// First skip any epsilons
		if(get_transition(transition_i).matches(0)) {
		    ++transition_i;
		    continue;
		} else if (get_transition(transition_i).matches(symbol)) {
		    transitions.insert(transition_i);
		    ++transition_i;
		    continue;
		} else {
		    break;
		}
	    }
	} else { // not a flag
	    const TransitionIndex& test_transition_index =
		get_index(state_index+1+symbol);
	    if(test_transition_index.matches(symbol)) {
		// there are one or more transitions with this input symbol,
		// starting at test_transition_index.get_target()
		TransitionTableIndex transition_i =
		    test_transition_index.get_target();
		while(true)
		{
		    if(get_transition(transition_i).matches(
			   test_transition_index.get_input_symbol())) {
			transitions.insert(transition_i);
		    } else {
			break;
		    }
		    ++transition_i;
		}
	    }
	}
    }
    } else { // indexes transition table
    const Transition& transition = get_transition(state_index);
    if(transition.get_input_symbol() != NO_SYMBOL_NUMBER ||
       transition.get_output_symbol() != NO_SYMBOL_NUMBER)
    {
        std::cerr << "Oops" << std::endl;
        throw;
    }
    
    TransitionTableIndex transition_i = state_index+1;
    while(true)
    {
        if(get_transition(transition_i)
	   .get_input_symbol() != NO_SYMBOL_NUMBER)
        transitions.insert(transition_i);
        else
        break;
        transition_i++;
    }
    }
    return transitions;
}

TransitionTableIndex Transducer::next(const TransitionTableIndex i,
				      const SymbolNumber symbol) const
{
    if (i >= TRANSITION_TARGET_TABLE_START) {
	return i - TRANSITION_TARGET_TABLE_START + 1;
    } else {
	return get_index(i+1+symbol).get_target() - TRANSITION_TARGET_TABLE_START;
    }
}

bool Transducer::has_transitions(const TransitionTableIndex i,
				 const SymbolNumber symbol) const
{
    if (i >= TRANSITION_TARGET_TABLE_START) {
	return (get_transition(i - TRANSITION_TARGET_TABLE_START).get_input_symbol() == symbol);
    } else {
	return (get_index(i+symbol).get_input_symbol() == symbol);
    }
}

bool Transducer::has_epsilons_or_flags(const TransitionTableIndex i)
{
    if (i >= TRANSITION_TARGET_TABLE_START) {
	return(get_transition(i - TRANSITION_TARGET_TABLE_START)
               .get_input_symbol() == 0 ||
	       is_flag(get_transition(i - TRANSITION_TARGET_TABLE_START)
                       .get_input_symbol()));
    } else {
	return (get_index(i).get_input_symbol() == 0);
    }
}

STransition Transducer::take_epsilons(const TransitionTableIndex i) const
{
    if (get_transition(i).get_input_symbol() != 0) {
	return STransition(0, NO_SYMBOL_NUMBER);
    }
    return STransition(get_transition(i).get_target(),
		       get_transition(i).get_output_symbol(),
		       get_transition(i).get_weight());
}

STransition Transducer::take_epsilons_and_flags(const TransitionTableIndex i)
{
    if (get_transition(i).get_input_symbol() != 0&&
	!is_flag(get_transition(i).get_input_symbol())) {
	return STransition(0, NO_SYMBOL_NUMBER);
    }
    return STransition(get_transition(i).get_target(),
		       get_transition(i).get_output_symbol(),
		       get_transition(i).get_weight());
}

STransition Transducer::take_non_epsilons(const TransitionTableIndex i,
					  const SymbolNumber symbol) const
{
    if (get_transition(i).get_input_symbol() != symbol) {
	return STransition(0, NO_SYMBOL_NUMBER);
    }
    return STransition(get_transition(i).get_target(),
		       get_transition(i).get_output_symbol(),
		       get_transition(i).get_weight());
}

Weight Transducer::final_weight(const TransitionTableIndex i) const
{
    if (i >= TRANSITION_TARGET_TABLE_START) {
	return get_transition(i - TRANSITION_TARGET_TABLE_START).get_weight();
    } else {
	return get_index(i).final_weight();
    }
}


}

#else // MAIN_TEST was defined

#include <iostream>

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST