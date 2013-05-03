#include "hfst-ol-lib.h"

namespace hfst_ol {

void TransducerHeader::skip_hfst3_header(FILE * f)
{
    const char* header1 = "HFST";
    unsigned int header_loc = 0; // how much of the header has been found
    int c;
    for(header_loc = 0; header_loc < strlen(header1) + 1; header_loc++)
    {
        c = getc(f);
        if(c != header1[header_loc]) {
            break;
        }
    }
    if(header_loc == strlen(header1) + 1) // we found it
    {
        unsigned short remaining_header_len;
        if (fread(&remaining_header_len,
                  sizeof(remaining_header_len), 1, f) != 1 ||
            getc(f) != '\0') {
            throw HeaderParsingException();
        }
        char * headervalue = new char[remaining_header_len];
        if (fread(headervalue, remaining_header_len, 1, f) != 1)
        {
            throw HeaderParsingException();
        }
        if (headervalue[remaining_header_len - 1] != '\0') {
            throw HeaderParsingException();
        }
        std::string header_tail(headervalue, remaining_header_len);
        size_t type_field = header_tail.find("type");
        if (type_field != std::string::npos) {
            if (header_tail.find("HFST_OL") != type_field + 5 &&
                header_tail.find("HFST_OLW") != type_field + 5) {
                delete headervalue;
                throw HeaderParsingException();
            }
        }
    } else // nope. put back what we've taken
    {
        ungetc(c, f); // first the non-matching character
        for(int i = header_loc - 1; i>=0; i--) {
// then the characters that did match (if any)
            ungetc(header1[i], f);
        }
    }
}

void TransducerAlphabet::get_next_symbol(FILE * f, SymbolNumber k)
{
    int byte;
    char * sym = line;
    while ( (byte = fgetc(f)) != 0 )
    {
        if (byte == EOF)
        { throw AlphabetParsingException(); }
        *sym = byte;
        ++sym;
    }
    *sym = 0;
    if (strlen(line) >= 5 && line[0] == '@' && line[strlen(line) - 1] == '@' && line[2] == '.')
    { // a flag diacritic needs to be parsed
        std::string feat;
        std::string val;
        FlagDiacriticOperator op = P; // g++ worries about this falling through uninitialized
        switch (line[1]) {
        case 'P': op = P; break;
        case 'N': op = N; break;
        case 'R': op = R; break;
        case 'D': op = D; break;
        case 'C': op = C; break;
        case 'U': op = U; break;
        }
        char * c = line;
// as long as we're working with utf-8, this should be ok
        for (c +=3; *c != '.' && *c != '@'; c++) { feat.append(c,1); }
        if (*c == '.')
        {
            for (++c; *c != '@'; c++) { val.append(c,1); }
        }
        if (feature_bucket.count(feat) == 0)
        {
            feature_bucket[feat] = feat_num;
            ++feat_num;
        }
        if (value_bucket.count(val) == 0)
        {
            value_bucket[val] = val_num;
            ++val_num;
        }
        operations.push_back(FlagDiacriticOperation(op, feature_bucket[feat], value_bucket[val]));
        operation_peek.push_back(k);
        kt->operator[](k) = "";
        return;
    }
    operations.push_back(FlagDiacriticOperation()); // dummy flag

  
    kt->operator[](k) = strdup(line);
}

void LetterTrie::add_string(const char * p, SymbolNumber symbol_key)
{
    if (*(p+1) == 0)
    {
        symbols[(unsigned char)(*p)] = symbol_key;
        return;
    }
    if (letters[(unsigned char)(*p)] == NULL)
    {
        letters[(unsigned char)(*p)] = new LetterTrie();
    }
    letters[(unsigned char)(*p)]->add_string(p+1,symbol_key);
}

SymbolNumber LetterTrie::find_key(char ** p)
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

void Encoder::read_input_symbols(KeyTable * kt)
{
    for (SymbolNumber k = 0; k < number_of_input_symbols; ++k)
    {
        const char * p = kt->operator[](k);
        if ((strlen(p) == 1) && (unsigned char)(*p) <= 127)
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

/**
 * BEGIN old transducer.cc
 */

bool TransducerFd::PushState(FlagDiacriticOperation op)
{ // try to alter the flag diacritic state stack
    switch (op.Operation()) {
    case P: // positive set
        statestack.push_back(statestack.back());
        statestack.back()[op.Feature()] = op.Value();
        return true;
    case N: // negative set (literally, in this implementation)
        statestack.push_back(statestack.back());
        statestack.back()[op.Feature()] = -1*op.Value();
        return true;
    case R: // require
        if (op.Value() == 0) // empty require
        {
            if (statestack.back()[op.Feature()] == 0)
            {
                return false;
            }
            else
            {
                statestack.push_back(statestack.back());
                return true;
            }
        }
        if (statestack.back()[op.Feature()] == op.Value())
        {
            statestack.push_back(statestack.back());
            return true;
        }
        return false;
    case D: // disallow
        if (op.Value() == 0) // empty disallow
        {
            if (statestack.back()[op.Feature()] != 0)
            {
                return false;
            }
            else
            {
                statestack.push_back(statestack.back());
                return true;
            }
        }
        if (statestack.back()[op.Feature()] == op.Value()) // nonempty disallow
        {
            return false;
        }
        statestack.push_back(statestack.back());
        return true;
    case C: // clear
        statestack.push_back(statestack.back());
        statestack.back()[op.Feature()] = 0;
        return true;
    case U: // unification
        if (statestack.back()[op.Feature()] == 0 || // if the feature is unset or
            statestack.back()[op.Feature()] == op.Value() || // the feature is at this value already or
            (statestack.back()[op.Feature()] < 0 &&
             (statestack.back()[op.Feature()] * -1 != op.Value())) // the feature is negatively set to something else
            )
        {
            statestack.push_back(statestack.back());
            statestack.back()[op.Feature()] = op.Value();
            return true;
        }
        return false;
    }
    throw; // for the compiler's peace of mind
}

bool TransitionIndex::matches(SymbolNumber s)
{
  
    if (input_symbol == NO_SYMBOL_NUMBER)
    {
        return false;
    }
    if (s == NO_SYMBOL_NUMBER)
    {
        return true;
    }
    return input_symbol == s;
}

bool Transition::matches(SymbolNumber s)
{
  
    if (input_symbol == NO_SYMBOL_NUMBER)
    {
        return false;
    }
    if (s == NO_SYMBOL_NUMBER)
    {
        return true;
    }
    return input_symbol == s;
}


void IndexTableReader::get_index_vector(void)
{
    for (size_t i = 0;
         i < number_of_table_entries;
         ++i)
    {
        size_t j = i * TransitionIndex::SIZE;
        SymbolNumber * input = (SymbolNumber*)(TableIndices + j);
        TransitionTableIndex * index = 
            (TransitionTableIndex*)(TableIndices + j + sizeof(SymbolNumber));
        indices.push_back(new TransitionIndex(*input,*index));
    }
}

void TransitionTableReader::Set(TransitionTableIndex pos)
{
    if (pos >= TRANSITION_TARGET_TABLE_START)
    {
        position = pos - TRANSITION_TARGET_TABLE_START;
    }
    else
    {
        position = pos;
    }
}

void TransitionTableReader::get_transition_vector(void)
{
    for (size_t i = 0; i < number_of_table_entries; ++i)
    {
        size_t j = i * Transition::SIZE;
        SymbolNumber * input = (SymbolNumber*)(TableTransitions + j);
        SymbolNumber * output = 
            (SymbolNumber*)(TableTransitions + j + sizeof(SymbolNumber));
        TransitionTableIndex * target = 
            (TransitionTableIndex*)(TableTransitions + j + 2 * sizeof(SymbolNumber));
        transitions.push_back(new Transition(*input,
                                             *output,
                                             *target));
      
    }
}

bool TransitionTableReader::Matches(SymbolNumber s)
{
    Transition * t = transitions[position];
    return t->matches(s);
}

bool TransitionTableReader::get_finality(TransitionTableIndex i)
{
    if (i >= TRANSITION_TARGET_TABLE_START) 
    {
        return transitions[i - TRANSITION_TARGET_TABLE_START]->final();
    }
    else
    {
        return transitions[i]->final();
    }
}

void Transducer::set_symbol_table(void)
{
    for(KeyTable::iterator it = keys->begin();
        it != keys->end();
        ++it)
    {
        const char * key_name = it->second;
        symbol_table.push_back(std::string(key_name));
    }
}

bool Transducer::set_input(char * str)
{
    int i = 0;
    SymbolNumber k = NO_SYMBOL_NUMBER;
    for ( char ** Str = &str; **Str != 0; )
    {
        k = find_next_key(Str);
        if (k == NO_SYMBOL_NUMBER)
        {
            return false; // tokenization failed
        }
        input_tape[i] = k;
        ++i;
    }
    input_tape[i] = NO_SYMBOL_NUMBER;
    return true;
}

OutputVector Transducer::returnAnalyses(void)
{
    output_vector.clear();
    analyze();
    return output_vector;
}

bool TransducerW::set_input(char * str)
{
    int i = 0;
    SymbolNumber k = NO_SYMBOL_NUMBER;
    for ( char ** Str = &str; **Str != 0; )
    {
        k = find_next_key(Str);
        if (k == NO_SYMBOL_NUMBER)
        {
            return false; // tokenization failed
        }
        input_tape[i] = k;
        ++i;
    }
    input_tape[i] = NO_SYMBOL_NUMBER;
    return true;
}

OutputVector TransducerW::returnAnalyses(void)
{
    output_vector.clear();
    analyze();
    return output_vector;
}


  
void Transducer::try_epsilon_transitions(SymbolNumber * input_symbol,
                                         SymbolNumber * output_symbol,
                                         SymbolNumber * original_output_tape,
                                         TransitionTableIndex i)
{
    while (transitions[i]->get_input() == 0)
    {
        *output_symbol = transitions[i]->get_output();
        get_analyses(input_symbol,
                     output_symbol+1,
                     original_output_tape,
                     transitions[i]->target());
        ++i;
    }
}

void TransducerFd::try_epsilon_transitions(SymbolNumber * input_symbol,
                                           SymbolNumber * output_symbol,
                                           SymbolNumber * original_output_tape,
                                           TransitionTableIndex i)
{
  
    while (true)
    {
        if (transitions[i]->get_input() == 0) // epsilon
        {
            *output_symbol = transitions[i]->get_output();
            get_analyses(input_symbol,
                         output_symbol+1,
                         original_output_tape,
                         transitions[i]->target());
            ++i;
        } else if (transitions[i]->get_input() != NO_SYMBOL_NUMBER &&
                   operations[transitions[i]->get_input()].isFlag())
        {
            if (PushState(operations[transitions[i]->get_input()]))
            {
// flag diacritic allowed
                *output_symbol = transitions[i]->get_output();
                get_analyses(input_symbol,
                             output_symbol+1,
                             original_output_tape,
                             transitions[i]->target());
                statestack.pop_back();
            }
            else
            {
            }
            ++i;
        } else
        {
            return;
        }
    }
}

void Transducer::try_epsilon_indices(SymbolNumber * input_symbol,
                                     SymbolNumber * output_symbol,
                                     SymbolNumber * original_output_tape,
                                     TransitionTableIndex i)
{
    if (indices[i]->get_input() == 0)
    {
        try_epsilon_transitions(input_symbol,
                                output_symbol,
                                original_output_tape,
                                indices[i]->target() - 
                                TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::find_transitions(SymbolNumber input,
                                  SymbolNumber * input_symbol,
                                  SymbolNumber * output_symbol,
                                  SymbolNumber * original_output_tape,
                                  TransitionTableIndex i)
{

    while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
        if (transitions[i]->get_input() == input)
        {
  
            *output_symbol = transitions[i]->get_output();
            get_analyses(input_symbol,
                         output_symbol+1,
                         original_output_tape,
                         transitions[i]->target());
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
    if (indices[i+input]->get_input() == input)
    {
        find_transitions(input,
                         input_symbol,
                         output_symbol,
                         original_output_tape,
                         indices[i+input]->target() - 
                         TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::note_analysis(SymbolNumber * whole_output_tape)
{
    std::string str = "";
    for (SymbolNumber * num = whole_output_tape; *num != NO_SYMBOL_NUMBER; ++num)
    {
        str.append(symbol_table[*num]);
    }
    output_vector.push_back(OutputPair(str, 0.0));
}

void Transducer::get_analyses(SymbolNumber * input_symbol,
                              SymbolNumber * output_symbol,
                              SymbolNumber * original_output_tape,
                              TransitionTableIndex i)
{
    if (i >= TRANSITION_TARGET_TABLE_START )
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
            if (final_transition(i))
            {
                note_analysis(original_output_tape);
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
            if (final_index(i))
            {
                note_analysis(original_output_tape);
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

/**
 * BEGIN old transducer-weighted.cc
 */

bool TransitionWIndex::matches(SymbolNumber s)
{
  
    if (input_symbol == NO_SYMBOL_NUMBER)
    {
        return false;
    }
    if (s == NO_SYMBOL_NUMBER)
    {
        return true;
    }
    return input_symbol == s;
}

bool TransitionW::matches(SymbolNumber s)
{
  
    if (input_symbol == NO_SYMBOL_NUMBER)
    {
        return false;
    }
    if (s == NO_SYMBOL_NUMBER)
    {
        return true;
    }
    return input_symbol == s;
}

bool TransducerWFd::PushState(FlagDiacriticOperation op)
{
    switch (op.Operation()) {
    case P: // positive set
        statestack.push_back(statestack.back());
        statestack.back()[op.Feature()] = op.Value();
        return true;
    case N: // negative set (literally, in this implementation)
        statestack.push_back(statestack.back());
        statestack.back()[op.Feature()] = -1*op.Value();
        return true;
    case R: // require
        if (op.Value() == 0) // empty require
        {
            if (statestack.back()[op.Feature()] == 0)
            {
                return false;
            }
            statestack.push_back(statestack.back());
            return true;
        }
        if (statestack.back()[op.Feature()] == op.Value())
        {
            statestack.push_back(statestack.back());
            return true;
        }
        return false;
    case D: // disallow
        if (op.Value() == 0) // empty disallow
        {
            if (statestack.back()[op.Feature()] != 0)
            {
                return false;
            }
            else
            {
                statestack.push_back(statestack.back());
                return true;
            }
        }
        if (statestack.back()[op.Feature()] == op.Value()) // nonempty disallow
        {
            return false;
        }
        statestack.push_back(statestack.back());
        return true;
    case C: // clear
        statestack.push_back(statestack.back());
        statestack.back()[op.Feature()] = 0;
        return true;
    case U: // unification
        if (statestack.back()[op.Feature()] == 0 || // if the feature is unset or
            statestack.back()[op.Feature()] == op.Value() || // the feature is at this value already or
            (statestack.back()[op.Feature()] < 0 &&
             (statestack.back()[op.Feature()] * -1 != op.Value())) // the feature is negatively set to something else
            )
        {
            statestack.push_back(statestack.back());
            statestack.back()[op.Feature()] = op.Value();
            return true;
        }
        return false;
    }
    throw; // for the compiler's peace of mind
}

void IndexTableReaderW::get_index_vector(void)
{
    for (size_t i = 0;
         i < number_of_table_entries;
         ++i)
    {
        size_t j = i * TransitionWIndex::SIZE;
        SymbolNumber * input = (SymbolNumber*)(TableIndices + j);
        TransitionTableIndex * index = 
            (TransitionTableIndex*)(TableIndices + j + sizeof(SymbolNumber));
        indices.push_back(new TransitionWIndex(*input,*index));
    }
}

void TransitionTableReaderW::Set(TransitionTableIndex pos)
{
    if (pos >= TRANSITION_TARGET_TABLE_START)
    {
        position = pos - TRANSITION_TARGET_TABLE_START;
    }
    else
    {
        position = pos;
    }
}

void TransitionTableReaderW::get_transition_vector(void)
{
    for (size_t i = 0; i < number_of_table_entries; ++i)
    {
        size_t j = i * TransitionW::SIZE;
        SymbolNumber * input = (SymbolNumber*)(TableTransitions + j);
        SymbolNumber * output = 
            (SymbolNumber*)(TableTransitions + j + sizeof(SymbolNumber));
        TransitionTableIndex * target = 
            (TransitionTableIndex*)(TableTransitions + j + 2 * sizeof(SymbolNumber));
        Weight * weight =
            (Weight*)(TableTransitions + j + 2 * sizeof(SymbolNumber) + sizeof(TransitionTableIndex));
        transitions.push_back(new TransitionW(*input,
                                              *output,
                                              *target,
                                              *weight));
      
    }
    transitions.push_back(new TransitionW());
    transitions.push_back(new TransitionW());
}

bool TransitionTableReaderW::Matches(SymbolNumber s)
{
    TransitionW * t = transitions[position];
    return t->matches(s);
}

bool TransitionTableReaderW::get_finality(TransitionTableIndex i)
{
    if (i >= TRANSITION_TARGET_TABLE_START) 
    {
        return transitions[i - TRANSITION_TARGET_TABLE_START]->final();
    }
    else
    {
        return transitions[i]->final();
    }
}


void TransducerW::set_symbol_table(void)
{
    for(KeyTable::iterator it = keys->begin();
        it != keys->end();
        ++it)
    {
        const char * key_name =
            it->second;

        symbol_table.push_back(std::string(key_name));
    }
}

void TransducerW::try_epsilon_transitions(SymbolNumber * input_symbol,
                                          SymbolNumber * output_symbol,
                                          SymbolNumber * 
                                          original_output_tape,
                                          TransitionTableIndex i)
{

    if (transitions.size() <= i) 
    {
        return;
    }

    while ((transitions[i] != NULL) and (transitions[i]->get_input() == 0))
    {
        *output_symbol = transitions[i]->get_output();
        current_weight += transitions[i]->get_weight();
        get_analyses(input_symbol,
                     output_symbol+1,
                     original_output_tape,
                     transitions[i]->target());
        current_weight -= transitions[i]->get_weight();
        ++i;
    }
    *output_symbol = NO_SYMBOL_NUMBER;
}

void TransducerWFd::try_epsilon_transitions(SymbolNumber * input_symbol,
                                            SymbolNumber * output_symbol,
                                            SymbolNumber * 
                                            original_output_tape,
                                            TransitionTableIndex i)
{
    if (transitions.size() <= i)
    { return; }
  
    while (true)
    {
        if (transitions[i]->get_input() == 0) // epsilon
        {
            *output_symbol = transitions[i]->get_output();
            current_weight += transitions[i]->get_weight();
            get_analyses(input_symbol,
                         output_symbol+1,
                         original_output_tape,
                         transitions[i]->target());
            current_weight += transitions[i]->get_weight();
            ++i;
        } else if (transitions[i]->get_input() != NO_SYMBOL_NUMBER &&
                   operations[transitions[i]->get_input()].isFlag())
        {
            if (PushState(operations[transitions[i]->get_input()]))
            {
// flag diacritic allowed
                *output_symbol = transitions[i]->get_output();
                current_weight += transitions[i]->get_weight();
                get_analyses(input_symbol,
                             output_symbol+1,
                             original_output_tape,
                             transitions[i]->target());
                current_weight -= transitions[i]->get_weight();
                statestack.pop_back();
            }
            ++i;
        } else
        {
            return;
        }
    }
}

void TransducerW::try_epsilon_indices(SymbolNumber * input_symbol,
                                      SymbolNumber * output_symbol,
                                      SymbolNumber * original_output_tape,
                                      TransitionTableIndex i)
{
    if (indices[i]->get_input() == 0)
    {
        try_epsilon_transitions(input_symbol,
                                output_symbol,
                                original_output_tape,
                                indices[i]->target() - 
                                TRANSITION_TARGET_TABLE_START);
    }
}

void TransducerW::find_transitions(SymbolNumber input,
                                   SymbolNumber * input_symbol,
                                   SymbolNumber * output_symbol,
                                   SymbolNumber * original_output_tape,
                                   TransitionTableIndex i)
{

    if (transitions.size() <= i) 
    {
        return;
    }
    while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
      
        if (transitions[i]->get_input() == input)
        {
            current_weight += transitions[i]->get_weight();
            *output_symbol = transitions[i]->get_output();
            get_analyses(input_symbol,
                         output_symbol+1,
                         original_output_tape,
                         transitions[i]->target());
            current_weight -= transitions[i]->get_weight();
        }
        else
        {
            return;
        }
        ++i;
    }
  
}

void TransducerW::find_index(SymbolNumber input,
                             SymbolNumber * input_symbol,
                             SymbolNumber * output_symbol,
                             SymbolNumber * original_output_tape,
                             TransitionTableIndex i)
{
    if (indices.size() <= i) 
    {
        return;
    }
  
    if (indices[i+input]->get_input() == input)
    {
      
        find_transitions(input,
                         input_symbol,
                         output_symbol,
                         original_output_tape,
                         indices[i+input]->target() - 
                         TRANSITION_TARGET_TABLE_START);
    }
}

void TransducerW::note_analysis(SymbolNumber * whole_output_tape)
{
    std::string str = "";
    for (SymbolNumber * num = whole_output_tape;
         *num != NO_SYMBOL_NUMBER;
         ++num)
    {
        str.append(symbol_table[*num]);
    }
    output_vector.push_back(OutputPair(str, current_weight));
}


void TransducerW::get_analyses(SymbolNumber * input_symbol,
                               SymbolNumber * output_symbol,
                               SymbolNumber * original_output_tape,
                               TransitionTableIndex i)
{
    if (i >= TRANSITION_TARGET_TABLE_START )
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
            if (transitions.size() <= i) 
            {
                return;
            }
            if (final_transition(i))
            {
                current_weight += get_final_transition_weight(i);
                note_analysis(original_output_tape);
                current_weight -= get_final_transition_weight(i);
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
// input-string ended.
        if (*input_symbol == NO_SYMBOL_NUMBER)
        {
            *output_symbol = NO_SYMBOL_NUMBER;
            if (final_index(i))
            {
                current_weight += get_final_index_weight(i);
                note_analysis(original_output_tape);
                current_weight -= get_final_index_weight(i);
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
}

}
