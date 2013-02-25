#include "pmatch.h"
#include "hfst.h"

namespace hfst_ol {

PmatchContainer::PmatchContainer(std::istream & inputstream)
{
    orig_input_tape = ((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN)));
    orig_output_tape = ((SymbolNumber*)(malloc(sizeof(SymbolNumber)*MAX_IO_LEN)));
    input_tape = orig_input_tape;
    output_tape = orig_output_tape;
            
    std::string transducer_name;
    transducer_name = parse_name_from_hfst3_header(inputstream);
    // the first transducer should be called eg. "TOP", this could be tested
    // for once more established
    
    TransducerHeader header(inputstream);
    symbol_count = header.symbol_count();
    alphabet = TransducerAlphabet(inputstream, header.symbol_count());

    // Retrieve extra special symbols from the alphabet

    special_symbols[entry] = NO_SYMBOL_NUMBER;
    special_symbols[exit] = NO_SYMBOL_NUMBER;
    special_symbols[LC_entry] = NO_SYMBOL_NUMBER;
    special_symbols[LC_exit] = NO_SYMBOL_NUMBER;
    special_symbols[RC_entry] = NO_SYMBOL_NUMBER;
    special_symbols[RC_exit] = NO_SYMBOL_NUMBER;
    special_symbols[boundary] = NO_SYMBOL_NUMBER;

    const SymbolTable & symbol_table = alphabet.get_symbol_table();
    for (SymbolNumber i = 1; i < symbol_table.size(); ++i) {
        add_special_symbol(symbol_table[i], i);
    }
    
    encoder = new Encoder(alphabet.get_symbol_table(), header.input_symbol_count());
    toplevel = new hfst_ol::PmatchTransducer(
        inputstream,
        header.index_table_size(),
        header.target_table_size(),
        alphabet,
        rtns,
        special_symbols);
    while (inputstream.good()) {
        try {
            transducer_name = parse_name_from_hfst3_header(inputstream);
        } catch (TransducerHeaderException & e) {
            break;
        }
        header = TransducerHeader(inputstream);
        TransducerAlphabet dummy = TransducerAlphabet(
            inputstream, header.symbol_count());
        hfst_ol::PmatchTransducer * rtn =
            new hfst_ol::PmatchTransducer(inputstream,
                                          header.index_table_size(),
                                          header.target_table_size(),
                                          alphabet,
                                          rtns,
                                          special_symbols);
        if (rtn_names.count(transducer_name) != 0) {
            add_rtn(rtn, rtn_names[transducer_name]);
        } else {
            delete rtn;
        }
    }
}

void PmatchContainer::add_special_symbol(const std::string & str,
                                         SymbolNumber symbol_number)
{
    if (str == "@_PMATCH_ENTRY_@") {
        special_symbols[entry] = symbol_number;
    } else if (str == "@_PMATCH_EXIT_@") {
        special_symbols[exit] = symbol_number;
    } else if (str == "@_PMATCH_LC_ENTRY_@") {
        special_symbols[LC_entry] = symbol_number;
    } else if (str == "@_PMATCH_RC_ENTRY_@") {
        special_symbols[RC_entry] = symbol_number;
    } else if (str == "@_PMATCH_LC_EXIT_@") {
        special_symbols[LC_exit] = symbol_number;
    } else if (str == "@_PMATCH_RC_EXIT_@") {
        special_symbols[RC_exit] = symbol_number;
    } else if (str == "@_BOUNDARY_@") {
        special_symbols[boundary] = symbol_number;
    } else if (is_end_tag(str)) {
        end_tag_map[symbol_number] = str.substr(16, str.size() - 18);
    } else if (is_insertion(str)) {
        rtn_names[name_from_insertion(str)] = symbol_number;
    }
        
}

bool PmatchContainer::is_end_tag(const std::string & symbol)
{
    return symbol.find("@_PMATCH_ENDTAG_") == 0 &&
        symbol.rfind("_@") == symbol.size() - 2;
}

bool PmatchContainer::is_end_tag(const SymbolNumber symbol) const
{
    return end_tag_map.count(symbol) == 1;
}

bool PmatchContainer::is_insertion(const std::string & symbol)
{
    return symbol.find("@I.") == 0 && symbol.rfind("@") == symbol.size() - 2;
}

std::string PmatchContainer::name_from_insertion(const std::string & symbol)
{
    return symbol.substr(2, symbol.size() - 3);
}

std::string PmatchContainer::end_tag(const SymbolNumber symbol)
{
    if (end_tag_map.count(symbol) == 0) {
        return "";
    } else {
        return "</" + end_tag_map[symbol] + ">";
    }
}

std::string PmatchContainer::start_tag(const SymbolNumber symbol)
{
    if (end_tag_map.count(symbol) == 0) {
        return "";
    } else {
        return "<" + end_tag_map[symbol] + ">";
    }
    
}

PmatchContainer::~PmatchContainer(void)
{
    free(orig_input_tape);
    free(orig_output_tape);
    delete encoder;
    delete toplevel;
    for (RtnMap::iterator it = rtns.begin();
         it != rtns.end(); ++it) {
        delete it->second;
    }
}

std::string PmatchContainer::parse_name_from_hfst3_header(std::istream & f)
{
    const char* header1 = "HFST";
    unsigned int header_loc = 0; // how much of the header has been found
    int c;
    for(header_loc = 0; header_loc < strlen(header1) + 1; header_loc++)
    {
        c = f.get();
        if(c != header1[header_loc]) {
            break;
        }
    }
    if(header_loc == strlen(header1) + 1)
    {
        unsigned short remaining_header_len;
        f.read((char*) &remaining_header_len, sizeof(remaining_header_len));
        if (f.get() != '\0') {
            HFST_THROW(TransducerHeaderException);
        }
        char * headervalue = new char[remaining_header_len];
        f.read(headervalue, remaining_header_len);
        if (headervalue[remaining_header_len - 1] != '\0') {
            HFST_THROW(TransducerHeaderException);
        }
        char type[remaining_header_len];
        char name[remaining_header_len];
        int i = 0;
        while (i < remaining_header_len) {
            if (strstr(headervalue + i, "type")) {
                strcpy(type, headervalue + i + strlen("type") + 1);
            } else if (strstr(headervalue + i, "name")) {
                strcpy(name, headervalue + i + strlen("name") + 1);
            }
            while (i < remaining_header_len &&
                   headervalue[i] != '\0') {
                ++i;
            }
            ++i;
        }
        delete[] headervalue;
        if (strcmp(type, "HFST_OL") && strcmp(type, "HFST_OLW")) {
            HFST_THROW(TransducerHeaderException);
        }
        return std::string(name);
    } else // nope. put back what we've taken
    {
        f.unget(); // first the non-matching character
        for(int i = header_loc - 1; i>=0; i--) {
            // then the characters that did match (if any)
            f.unget();
        }
        HFST_THROW(TransducerHeaderException);
    }
}



void PmatchContainer::add_rtn(PmatchTransducer * rtn, SymbolNumber s)
{
    rtns.insert(std::pair<SymbolNumber, PmatchTransducer *>(s, rtn));
}

std::string PmatchContainer::match(std::string & input)
{
    std::string ret;
    initialize_input(input.c_str());
    output.clear();
    while (has_queued_input()) {
        SymbolNumber * input_entry = input_tape;
        toplevel->match(&input_tape, &output_tape);
        copy_to_output(toplevel->get_best_result());
        if (input_entry == input_tape) {
            output.push_back(*input_tape++);
        }
    }
    return stringify_output();
}

void PmatchContainer::copy_to_output(const SymbolNumberVector & best_result)
{
    for (SymbolNumberVector::const_iterator it = best_result.begin();
         it != best_result.end(); ++it) {
        output.push_back(*it);
    }
}

std::string PmatchContainer::stringify_output(void)
{
    std::string retval;
    std::stack<unsigned int> start_tag_pos;
    for (SymbolNumberVector::const_iterator it = output.begin();
         it != output.end(); ++it) {
        if (*it == special_symbols[entry]) {
            start_tag_pos.push(retval.size());
        } else if (*it == special_symbols[exit]) {
            start_tag_pos.pop();
        } else if (is_end_tag(*it)) {
            retval.insert(start_tag_pos.top(), start_tag(*it));
            retval.append(end_tag(*it));
        } else if (*it == special_symbols[boundary]) {
            continue;
        } else {
            retval.append(alphabet.string_from_symbol(*it));
        }
    }
    return retval;
}

bool PmatchContainer::has_unsatisfied_rtns(void) const
{
    return false;
}

std::string PmatchContainer::get_unsatisfied_rtn_name(void) const
{
    return "";
}

bool PmatchContainer::has_queued_input(void)
{
    return *input_tape != NO_SYMBOL_NUMBER;
}

// void PmatchContainer::tokenize_from_queue(void)
// {
//     if (input_queue == NULL || *input_queue == '\0') {
//     return;
//     }
//     char * orig_input_queue = input_queue;
//     tokenize(&input_queue);
//     char * new_input_queue = (char*) malloc(strlen(input_queue));
//     strcpy(new_input_queue, input_queue);
//     input_queue = new_input_queue;
//     free(orig_input_queue);
//     return;
// }

PmatchTransducer::PmatchTransducer(std::istream & is,
                                   TransitionTableIndex index_table_size,
                                   TransitionTableIndex transition_table_size,
                                   TransducerAlphabet & alpha,
                                   RtnMap & rtn_map,
                                   std::map<SpecialSymbol, SymbolNumber> & marker_symbols):
    alphabet(alpha),
    rtns(rtn_map),
    markers(marker_symbols)
{
    // initialize the stack for local variables
    Locals front;
    front.candidate_input_pos = NULL;
    front.output_tape_head = NULL;
    front.flag_state = alphabet.get_fd_table();
    front.tape_step = 1;
    front.context = none;
    front.context_placeholder = NULL;
    local_stack.push(front);

    // Allocate and read tables
    char * indextab = (char*) malloc(SimpleIndex::SIZE * index_table_size);
    char * transitiontab = (char*) malloc(SimpleTransition::SIZE * transition_table_size);
    is.read(indextab, SimpleIndex::SIZE * index_table_size);
    is.read(transitiontab, SimpleTransition::SIZE * transition_table_size);
    char * orig_p = indextab;
    while(index_table_size) {
        index_table.push_back(
            SimpleIndex(*(SymbolNumber *) indextab,
                        *(TransitionTableIndex *) (indextab + sizeof(SymbolNumber))));
        --index_table_size;
        indextab += SimpleIndex::SIZE;
    }
    free(orig_p);
    orig_p = transitiontab;
    while(transition_table_size) {
        transition_table.push_back(
            SimpleTransition(*(SymbolNumber *) transitiontab,
                             *(SymbolNumber *) (transitiontab + sizeof(SymbolNumber)),
                             *(TransitionTableIndex *) (transitiontab + 2*sizeof(SymbolNumber))));
        --transition_table_size;
        transitiontab += SimpleTransition::SIZE;
    }
    free(orig_p);
}

void PmatchContainer::initialize_input(const char * input)
{
    input_tape = orig_input_tape;
    char * input_str = const_cast<char *>(input);
    char ** input_str_ptr = &input_str;
    SymbolNumber k = NO_SYMBOL_NUMBER;
    input_tape[0] = NO_SYMBOL_NUMBER;
    input_tape[1] = special_symbols[boundary];
    int i = 2;
    while (**input_str_ptr != 0) {
        char * original_input_loc = *input_str_ptr;
        k = encoder->find_key(input_str_ptr);
        if (k == NO_SYMBOL_NUMBER) {
            // the encoder moves as far as it can during tokenization,
            // we want to go back to be in position to add one utf-8 char
            *input_str_ptr = original_input_loc;
            // Regular tokenization failed
            int bytes_to_tokenize = nByte_utf8(**input_str_ptr);
            if (bytes_to_tokenize == 0) {
                // even if it's not utf-8, tokenize a byte at a time
                bytes_to_tokenize = 1;
            }
            char new_symbol[bytes_to_tokenize + 1];
            memcpy(new_symbol, *input_str_ptr, bytes_to_tokenize);
            new_symbol[bytes_to_tokenize] = '\0';
            (*input_str_ptr) += bytes_to_tokenize;
            alphabet.add_symbol(new_symbol);
            encoder->read_input_symbol(new_symbol, symbol_count);
            k = symbol_count;
            ++symbol_count;
        }
        input_tape[i] = k;
        ++i;
    }
    input_tape[i] = special_symbols[boundary];
    input_tape[i+1] = NO_SYMBOL_NUMBER;
    // Place input_tape beyond the opening boundary
    input_tape += 2;
    return;
}

void PmatchTransducer::match(SymbolNumber ** input_tape_entry,
                             SymbolNumber ** output_tape_entry)
{
    local_stack.top().best_result.clear();
    local_stack.top().candidate_input_pos = *input_tape_entry;
    local_stack.top().output_tape_head = *output_tape_entry;
    local_stack.top().context = none;
    local_stack.top().tape_step = 1;
    local_stack.top().context_placeholder = NULL;
    get_analyses(*input_tape_entry, *output_tape_entry, 0);
    *input_tape_entry = local_stack.top().candidate_input_pos;
}

void PmatchTransducer::rtn_call(SymbolNumber * input_tape_entry,
                                SymbolNumber * output_tape_entry)
{
    Locals locals;
    locals.candidate_input_pos = input_tape_entry;
    locals.output_tape_head = output_tape_entry;
    locals.flag_state = alphabet.get_fd_table();
    locals.tape_step = 1;
    locals.context = none;
    locals.context_placeholder = NULL;
    local_stack.push(locals);
    get_analyses(input_tape_entry, output_tape_entry, 0);
}

void PmatchTransducer::rtn_exit(void)
{
    local_stack.pop();
}

// void PmatchTransducer::take_best_path(void)
// {
//     if(candidate_path.size() == 0) {
//         jump_queued_token();
//     } else {
//         output.insert(output.end(), candidate_path.begin(), candidate_path.end());
//         input_tape = candidate_input_pos;
//     }
// }

void PmatchTransducer::note_analysis(SymbolNumber * input_tape,
                                     SymbolNumber * output_tape)
{
    if (input_tape > local_stack.top().candidate_input_pos) {
        local_stack.top().best_result.assign(local_stack.top().output_tape_head, output_tape);
        local_stack.top().candidate_input_pos = input_tape;
    }
}

void PmatchTransducer::try_epsilon_transitions(SymbolNumber * input_tape,
                                               SymbolNumber * output_tape,
                                               TransitionTableIndex i)
{
    while (true) {
        // First we handle actual input epsilons.
        // There is a special case when the output side
        // is a context-checking marker.
        if (transition_table[i].input == 0) {
            SymbolNumber output = transition_table[i].output;
            if (!checking_context()) {
                if (!try_entering_context(output)) {
                    *output_tape = output;
                    get_analyses(input_tape,
                                 output_tape + 1,
                                 transition_table[i].target);
                    ++i;

                } else {
                    // We're going to do some context checking
                    local_stack.top().context_placeholder = input_tape;
                    if (local_stack.top().context == LC ||
                        local_stack.top().context == NLC) {
                        // When entering a left context,
                        // we begin checking not at the current symbol
                        // but the previous one. This should be safe
                        // because the tape is NO_SYMBOL padded.
                        input_tape -= 1;
                    }
                    get_analyses(input_tape,
                                 output_tape,
                                 transition_table[i].target);
                    exit_context();
                    ++i;
                }
            } else {
                // We *are* checking context and may be done
                if (try_exiting_context(output)) {
                    // We've successfully completed a context check
                    input_tape = local_stack.top().context_placeholder;
                    get_analyses(input_tape,
                                 output_tape,
                                 transition_table[i].target);
                    ++i;
                } else {
                    // Don't touch output when checking context
                    get_analyses(input_tape,
                                 output_tape,
                                 transition_table[i].target);
                    ++i;
                }
            }
        } else if (alphabet.is_flag_diacritic(
                       transition_table[i].input)) {
            std::vector<short> old_values(local_stack.top().flag_state.get_values());
            if (local_stack.top().flag_state.apply_operation(
                    *(alphabet.get_operation(
                          transition_table[i].input)))) {
                // flag diacritic allowed
                *output_tape = transition_table[i].output;
                get_analyses(input_tape,
                             output_tape + 1,
                             transition_table[i].target);
            }
            local_stack.top().flag_state.assign_values(old_values);
            ++i;
            // It could be an insert statement... (gee, this function should
            // probably be refactored
//        }
//        else if (rtns.count(transition_table[i].input) != 0) {
//            SymbolNumberVector old_best = locals_stack.top().best_result;
//            SymbolNumber * old_candidate = locals_stack.top().candidate_input_pos;
//            rtns->operator[](transition_table[i].input).rtn_call(input_tape, output_tape);
            
        } else { // it's not epsilon and it's not a flag or Ins, so nothing to do
            return;
        }
    }

}

void PmatchTransducer::try_epsilon_indices(SymbolNumber * input_tape,
                                           SymbolNumber * output_tape,
                                           TransitionTableIndex i)
{
    if (index_table[i].input == 0) {
        try_epsilon_transitions(input_tape,
                                output_tape,
                                index_table[i].target -
                                TRANSITION_TARGET_TABLE_START);
    }

}

void PmatchTransducer::find_transitions(SymbolNumber input,
                                        SymbolNumber * input_tape,
                                        SymbolNumber * output_tape,
                                        TransitionTableIndex i)
{
    while (transition_table[i].input != NO_SYMBOL_NUMBER) {
        if (transition_table[i].input == input) {
            if (!checking_context()) {
                *output_tape = transition_table[i].output;
                get_analyses(input_tape,
                             output_tape + 1,
                             transition_table[i].target);
            } else {
                // Checking context so don't touch output
                get_analyses(input_tape,
                             output_tape,
                             transition_table[i].target);
            }
        } else {
            return;
        }
        ++i;
    }
}

void PmatchTransducer::find_index(SymbolNumber input,
                                  SymbolNumber * input_tape,
                                  SymbolNumber * output_tape,
                                  TransitionTableIndex i)
{
    if (index_table[i+input].input == input) {
        find_transitions(input,
                         input_tape,
                         output_tape,
                         index_table[i+input].target - TRANSITION_TARGET_TABLE_START);
    }
}

void PmatchTransducer::get_analyses(SymbolNumber * input_tape,
                                    SymbolNumber * output_tape,
                                    TransitionTableIndex i)
{
    if (indexes_transition_table(i))
    {
        i -= TRANSITION_TARGET_TABLE_START;
        
        try_epsilon_transitions(input_tape,
                                output_tape,
                                i+1);
        
        // Check for finality even if the input string hasn't ended
        if (transition_table[i].final()) {
            note_analysis(input_tape, output_tape);
        }

        if (*input_tape == NO_SYMBOL_NUMBER) {
            return;
        }

        SymbolNumber input = *input_tape;
        input_tape += local_stack.top().tape_step;

        find_transitions(input,
                         input_tape,
                         output_tape,
                         i+1);
    } else {

        try_epsilon_indices(input_tape,
                            output_tape,
                            i+1);
        
        if (index_table[i].final()) {
            note_analysis(input_tape, output_tape);
        }
        
        if (*input_tape == NO_SYMBOL_NUMBER) {
            return;
        }


        SymbolNumber input = *input_tape;
        input_tape += local_stack.top().tape_step;

        find_index(input,
                   input_tape,
                   output_tape,
                   i+1);
        
    }
}

bool PmatchTransducer::checking_context(void) const
{
    return local_stack.top().context != none;
}

bool PmatchTransducer::try_entering_context(SymbolNumber symbol)
{
    if (symbol == markers[LC_entry]) {
        local_stack.top().context = LC;
        local_stack.top().tape_step = -1;
        return true;
    } else if (symbol == markers[RC_entry]) {
        local_stack.top().context = RC;
        local_stack.top().tape_step = 1;
        return true;
    } else {
        return false;
    }
}

bool PmatchTransducer::try_exiting_context(SymbolNumber symbol)
{
    switch (local_stack.top().context) {
    case LC:
        if (symbol == markers[LC_exit]) {
            exit_context();
            return true;
        } else {
            return false;
        }
    case RC:
        if (symbol == markers[RC_exit]) {
            exit_context();
            return true;
        } else {
            return false;
        }
    default:
        return false;
    }
}

void PmatchTransducer::exit_context(void)
{
    local_stack.top().context = none;
    local_stack.top().tape_step = 1;
}

}