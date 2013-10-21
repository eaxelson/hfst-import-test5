#include "pmatch.h"
#include "hfst.h"

namespace hfst_ol {

PmatchAlphabet::PmatchAlphabet(std::istream & inputstream,
                               SymbolNumber symbol_count):
    TransducerAlphabet(inputstream, symbol_count),
    verbose(false)
{
    special_symbols[entry] = NO_SYMBOL_NUMBER;
    special_symbols[exit] = NO_SYMBOL_NUMBER;
    special_symbols[LC_entry] = NO_SYMBOL_NUMBER;
    special_symbols[LC_exit] = NO_SYMBOL_NUMBER;
    special_symbols[RC_entry] = NO_SYMBOL_NUMBER;
    special_symbols[RC_exit] = NO_SYMBOL_NUMBER;
    special_symbols[NLC_entry] = NO_SYMBOL_NUMBER;
    special_symbols[NLC_exit] = NO_SYMBOL_NUMBER;
    special_symbols[NRC_entry] = NO_SYMBOL_NUMBER;
    special_symbols[NRC_exit] = NO_SYMBOL_NUMBER;
    special_symbols[Pmatch_passthrough] = NO_SYMBOL_NUMBER;
    special_symbols[boundary] = NO_SYMBOL_NUMBER;
    for (SymbolNumber i = 1; i < symbol_table.size(); ++i) {
        add_special_symbol(symbol_table[i], i);
    }
    
}

PmatchAlphabet::PmatchAlphabet(void):
    TransducerAlphabet()
{}

void PmatchAlphabet::add_special_symbol(const std::string & str,
                                         SymbolNumber symbol_number)
{
    if (str == "@PMATCH_ENTRY@") {
        special_symbols[entry] = symbol_number;
    } else if (str == "@PMATCH_EXIT@") {
        special_symbols[exit] = symbol_number;
    } else if (str == "@PMATCH_LC_ENTRY@") {
        special_symbols[LC_entry] = symbol_number;
    } else if (str == "@PMATCH_RC_ENTRY@") {
        special_symbols[RC_entry] = symbol_number;
    } else if (str == "@PMATCH_LC_EXIT@") {
        special_symbols[LC_exit] = symbol_number;
    } else if (str == "@PMATCH_RC_EXIT@") {
        special_symbols[RC_exit] = symbol_number;
    } else if (str == "@PMATCH_NLC_ENTRY@") {
        special_symbols[NLC_entry] = symbol_number;
    } else if (str == "@PMATCH_NRC_ENTRY@") {
        special_symbols[NRC_entry] = symbol_number;
    } else if (str == "@PMATCH_NLC_EXIT@") {
        special_symbols[NLC_exit] = symbol_number;
    } else if (str == "@PMATCH_NRC_EXIT@") {
        special_symbols[NRC_exit] = symbol_number;
    } else if (str == "@PMATCH_PASSTHROUGH@") {
        special_symbols[Pmatch_passthrough] = symbol_number;
    } else if (str == "@BOUNDARY@") {
        special_symbols[boundary] = symbol_number;
    } else if (is_end_tag(str)) {
        // Fetch the part between @PMATCH_ENDTAG_ and @
        end_tag_map[symbol_number] = str.substr(
            sizeof("@PMATCH_ENDTAG_") - 1,
            str.size() - (sizeof("@PMATCH_ENDTAG_@") - 1));
    } else if (is_insertion(str)) {
        rtn_names[name_from_insertion(str)] = symbol_number;
    }
        
}



PmatchContainer::PmatchContainer(std::istream & inputstream)
{
    std::string transducer_name;
    transducer_name = parse_name_from_hfst3_header(inputstream);
    // the first transducer should be called eg. "TOP", this could be tested
    // for once more established

    TransducerHeader header(inputstream);
    orig_symbol_count = symbol_count = header.symbol_count();
    alphabet = PmatchAlphabet(inputstream, header.symbol_count());

    io_size = MAX_IO_LEN;
    orig_input_tape = ((SymbolNumber*)
                       (malloc(sizeof(SymbolNumber)*io_size)));
    orig_output_tape = ((SymbolNumber*)
                        (malloc(sizeof(SymbolNumber)*io_size)));
    input_tape = orig_input_tape;
    output_tape = orig_output_tape;
    
    encoder = new Encoder(alphabet.get_symbol_table(), header.input_symbol_count());
    toplevel = new hfst_ol::PmatchTransducer(
        inputstream,
        header.index_table_size(),
        header.target_table_size(),
        alphabet);
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
                                          alphabet);
        if (!alphabet.has_rtn(transducer_name)) {
            alphabet.add_rtn(rtn, transducer_name);
        } else {
            delete rtn;
        }
    }
}

bool PmatchAlphabet::is_end_tag(const std::string & symbol)
{
    return symbol.find("@PMATCH_ENDTAG_") == 0 &&
        symbol.rfind("@") == symbol.size() - 1;
}

bool PmatchAlphabet::is_end_tag(const SymbolNumber symbol) const
{
    return end_tag_map.count(symbol) == 1;
}

bool PmatchAlphabet::is_insertion(const std::string & symbol)
{
    return symbol.find("@I.") == 0 && symbol.rfind("@") == symbol.size() - 1;
}

std::string PmatchAlphabet::name_from_insertion(const std::string & symbol)
{
    return symbol.substr(sizeof("@I.") - 1, symbol.size() - (sizeof("@I.@") - 1));
}

std::string PmatchAlphabet::end_tag(const SymbolNumber symbol)
{
    if (end_tag_map.count(symbol) == 0) {
        return "";
    } else {
        return "</" + end_tag_map[symbol] + ">";
    }
}

std::string PmatchAlphabet::start_tag(const SymbolNumber symbol)
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
}

PmatchAlphabet::~PmatchAlphabet(void)
{
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
        bool type_defined = false;
        char name[remaining_header_len];
        bool name_defined = false;
        int i = 0;
        while (i < remaining_header_len) {
            if (!type_defined && strstr(headervalue + i, "type")) {
                strcpy(type, headervalue + i + strlen("type") + 1);
                type_defined = true;
            } else if (!name_defined && strstr(headervalue + i, "name")) {
                strcpy(name, headervalue + i + strlen("name") + 1);
                name_defined = true;
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

void PmatchAlphabet::add_rtn(PmatchTransducer * rtn, std::string const & name)
{
    SymbolNumber symbol = rtn_names[name];
    rtns.insert(std::pair<SymbolNumber, PmatchTransducer *>(symbol, rtn));
}

bool PmatchAlphabet::has_rtn(std::string const & name) const
{
    return rtns.count(rtn_names.at(name)) != 0;
}

bool PmatchAlphabet::has_rtn(SymbolNumber symbol) const
{
    return rtns.count(symbol) != 0;
}

PmatchTransducer * PmatchAlphabet::get_rtn(SymbolNumber symbol)
{
    return rtns[symbol];
}

SymbolNumber PmatchAlphabet::get_special(SpecialSymbol special) const
{
    return special_symbols.at(special);
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
    return alphabet.stringify(output);
}

std::string PmatchAlphabet::stringify(const SymbolNumberVector & str)
{
    std::string retval;
    std::stack<unsigned int> start_tag_pos;
    for (SymbolNumberVector::const_iterator it = str.begin();
         it != str.end(); ++it) {
        if (*it == special_symbols[entry]) {
            start_tag_pos.push(retval.size());
        } else if (*it == special_symbols[exit]) {
            if (start_tag_pos.size() != 0) {
                start_tag_pos.pop();
            }
        } else if (is_end_tag(*it)) {
            unsigned int pos;
            if (start_tag_pos.size() == 0) {
                std::cerr << "Warning: end tag without start tag\n";
                pos = 0;
            } else {
                pos = start_tag_pos.top();
            }
            retval.insert(pos, start_tag(*it));
            retval.append(end_tag(*it));
        } else if (*it == special_symbols[boundary]) {
            continue;
        } else {
            retval.append(string_from_symbol(*it));
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
                                   PmatchAlphabet & alpha):
    alphabet(alpha)
{
    orig_symbol_count = alphabet.get_symbol_table().size();
    // initialize the stack for local variables
    LocalVariables locals_front;
    locals_front.flag_state = alphabet.get_fd_table();
    locals_front.tape_step = 1;
    locals_front.context = none;
    locals_front.context_placeholder = NULL;
    locals_front.default_symbol_trap = false;
    locals_front.negative_context_success = false;
    locals_front.pending_passthrough = false;
    local_stack.push(locals_front);
    RtnVariables rtn_front;
    rtn_front.candidate_input_pos = NULL;
    rtn_front.output_tape_head = NULL;
    rtn_stack.push(rtn_front);


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
    if (strlen(input) > io_size - 4) {
        free(orig_input_tape);
        free(orig_output_tape);
        orig_input_tape = ((SymbolNumber*)(malloc(10*sizeof(SymbolNumber)*strlen(input))));
        orig_output_tape = ((SymbolNumber*)(malloc(10*sizeof(SymbolNumber)*strlen(input))));
        if (orig_input_tape == NULL || orig_output_tape == NULL) {
            std::cerr << "Failed to allocate input buffer of length " << strlen(input) << std::endl;
            free(orig_input_tape);
            free(orig_output_tape);
            orig_input_tape = ((SymbolNumber*)(malloc(sizeof(SymbolNumber)*io_size)));
            orig_output_tape = ((SymbolNumber*)(malloc(sizeof(SymbolNumber)*io_size)));
        } else {
            io_size = 10*strlen(input);
        }
        output_tape = orig_output_tape;
    }
    input_tape = orig_input_tape;
    char * input_str = const_cast<char *>(input);
    char ** input_str_ptr = &input_str;
    SymbolNumber k = NO_SYMBOL_NUMBER;
    input_tape[0] = NO_SYMBOL_NUMBER;
    input_tape[1] = alphabet.get_special(boundary);
    int i = 2;
    while (**input_str_ptr != 0 && i < io_size - 4 ) {
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
    input_tape[i] = alphabet.get_special(boundary);
    input_tape[i+1] = NO_SYMBOL_NUMBER;
    // Place input_tape beyond the opening NO_SYMBOL
    ++input_tape;
    // If the boundary marker is NO_SYMBOL (unused), skip it too
    if (*input_tape == NO_SYMBOL_NUMBER) {
        ++input_tape;
    }
    return;
}

void PmatchTransducer::match(SymbolNumber ** input_tape_entry,
                             SymbolNumber ** output_tape_entry)
{
    rtn_stack.top().best_result.clear();
    rtn_stack.top().candidate_input_pos = *input_tape_entry;
    rtn_stack.top().output_tape_head = *output_tape_entry;
    local_stack.top().context = none;
    local_stack.top().tape_step = 1;
    local_stack.top().context_placeholder = NULL;
    local_stack.top().default_symbol_trap = false;
    get_analyses(*input_tape_entry, *output_tape_entry, 0);
    *input_tape_entry = rtn_stack.top().candidate_input_pos;
}

void PmatchTransducer::rtn_call(SymbolNumber * input_tape_entry,
                                SymbolNumber * output_tape_entry)
{
    rtn_stack.push(rtn_stack.top());
    rtn_stack.top().candidate_input_pos = input_tape_entry;
    rtn_stack.top().output_tape_head = output_tape_entry;
    local_stack.push(local_stack.top());
    local_stack.top().flag_state = alphabet.get_fd_table();
    local_stack.top().tape_step = 1;
    local_stack.top().context = none;
    local_stack.top().context_placeholder = NULL;
    local_stack.top().default_symbol_trap = false;
    get_analyses(input_tape_entry, output_tape_entry, 0);
}

void PmatchTransducer::rtn_exit(void)
{
    rtn_stack.pop();
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
    if (input_tape > rtn_stack.top().candidate_input_pos) {
        rtn_stack.top().best_result.assign(
            rtn_stack.top().output_tape_head, output_tape);
        rtn_stack.top().candidate_input_pos = input_tape;
    } else if (alphabet.is_verbose() &&
               input_tape == rtn_stack.top().candidate_input_pos) {
        SymbolNumberVector discarded(rtn_stack.top().output_tape_head,
                                     output_tape);
        std::cerr << "\n\tWarning: conflicting matches found, discarding:\n\t"
                  << alphabet.stringify(discarded) << std::endl << std::endl;
            }
}

// void PmatchTransducer::get_initial_input_symbols(
//     std::set<SymbolNumber> & initials,
//     TransitionTableIndex i)
// {
    
    
// }

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
            if (alphabet.get_unknown_symbol() != NO_SYMBOL_NUMBER &&
                output == alphabet.get_unknown_symbol()) {
                output = *(input_tape - 1);
            }
            if (!checking_context()) {
                if (!try_entering_context(output)) {
                    *output_tape = output;
                    get_analyses(input_tape,
                                 output_tape + 1,
                                 transition_table[i].target);
                    ++i;

                } else {
                    // We're going to do some context checking
//                    std::cerr << "Entered context, stack is " << local_stack.size() << std::endl;
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
                    if (local_stack.top().context == LC ||
                        local_stack.top().context == NLC) {
                        input_tape += 1; // Undo what we did - this is very inelegant
                    }
                    // In case we have a negative context, we check to see if the context matched.
                    // If it did, we schedule a passthrough arc after we've processed epsilons.
                    bool schedule_passthrough = false;
                    if((local_stack.top().context == NLC || local_stack.top().context == NRC)
                       && !local_stack.top().negative_context_success) {
                        schedule_passthrough = true;
                    }
                    local_stack.pop();
                    if (schedule_passthrough) {
                        local_stack.top().pending_passthrough = true;
                    }
                    ++i;
                }
            } else {
                // We *are* checking context and may be done
                if (try_exiting_context(output)) {
//                    std::cerr << "Exited context, stack is " << local_stack.size() << std::endl;
                    // We've successfully completed a context check
                    input_tape = local_stack.top().context_placeholder;
//                    std::cerr << "input tape restored to " << *input_tape << std::endl;
                    get_analyses(input_tape,
                                 output_tape,
                                 transition_table[i].target);
                    local_stack.pop();
                    ++i;
                } else {
                    if (local_stack.top().negative_context_success == true) {
                        ++i;
                        continue;
                    }
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
        }
        else if (alphabet.has_rtn(transition_table[i].input)) {
            SymbolNumber * original_output = output_tape;
            SymbolNumber * original_input = input_tape;
            // Pass control
            PmatchTransducer * rtn_target =
                alphabet.get_rtn(transition_table[i].input);
            rtn_target->rtn_call(input_tape, output_tape);
            // Fetch the result
            for(SymbolNumberVector::const_iterator it =
                    rtn_target->rtn_stack.top().best_result.begin();
                it != rtn_target->rtn_stack.top().best_result.end();
                ++it) {
                *output_tape = *it;
                ++output_tape;
            }
            input_tape = rtn_target->rtn_stack.top().candidate_input_pos;
            rtn_target->rtn_exit();
            // We're back in this transducer and continue where we left off
            if (input_tape != original_input) {
                // the rtn advanced the tape
            get_analyses(input_tape,
                         output_tape,
                         transition_table[i].target);
            // Finally we go back to where we were so the
            // other possible paths get a shot
            input_tape = original_input;
            output_tape = original_output;
            }
            ++i;
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
                SymbolNumber output = transition_table[i].output;
                if (input == alphabet.get_identity_symbol()) {
                // we got here via identity, so look back in the
                // input tape to find the symbol we want to write
                    output = *(input_tape - 1);
                } else if (alphabet.get_unknown_symbol() != NO_SYMBOL_NUMBER &&
                           output == alphabet.get_unknown_symbol()) {
                    output = *(input_tape - 1);
                }
                *output_tape = output;
                get_analyses(input_tape,
                             output_tape + 1,
                             transition_table[i].target);
            } else {
                // Checking context so don't touch output
                get_analyses(input_tape,
                             output_tape,
                             transition_table[i].target);
            }
            local_stack.top().default_symbol_trap = false;
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
        local_stack.top().default_symbol_trap = false;
    }
}

void PmatchTransducer::get_analyses(SymbolNumber * input_tape,
                                    SymbolNumber * output_tape,
                                    TransitionTableIndex i)
{
    if (indexes_transition_table(i))
    {
        local_stack.top().default_symbol_trap = true;
        i -= TRANSITION_TARGET_TABLE_START;
        
        try_epsilon_transitions(input_tape,
                                output_tape,
                                i+1);

        if (local_stack.top().pending_passthrough) {
            find_transitions(alphabet.get_special(Pmatch_passthrough),
                             input_tape, output_tape, i+1);
            local_stack.top().pending_passthrough = false;
        }
        
        // Check for finality even if the input string hasn't ended
        if (transition_table[i].final()) {
            note_analysis(input_tape, output_tape);
        }

        if (*input_tape == NO_SYMBOL_NUMBER) {
            return;
        }

        SymbolNumber input = *input_tape;
        input_tape += local_stack.top().tape_step;

        if (input < orig_symbol_count) {
            find_transitions(input,
                             input_tape,
                             output_tape,
                             i+1);
        } else if (alphabet.get_identity_symbol() != NO_SYMBOL_NUMBER) {
            find_transitions(alphabet.get_identity_symbol(),
                             input_tape,
                             output_tape,
                             i+1);
        }
        if (alphabet.get_unknown_symbol() != NO_SYMBOL_NUMBER) {
            find_transitions(alphabet.get_unknown_symbol(),
                             input_tape,
                             output_tape,
                             i+1);
        }
            
        // if (alphabet.get_identity_symbol() != NO_SYMBOL_NUMBER &&
        //     local_stack.top().default_symbol_trap == true) {
        //     find_transitions(alphabet.get_identity_symbol(),
        //                      input_tape, output_tape, i+1);
        // }
    } else {
        local_stack.top().default_symbol_trap = true;
        try_epsilon_indices(input_tape,
                            output_tape,
                            i+1);

        if (local_stack.top().pending_passthrough) {
            find_index(alphabet.get_special(Pmatch_passthrough),
                       input_tape, output_tape, i+1);
            local_stack.top().pending_passthrough = false;
        }
        
        if (index_table[i].final()) {
            note_analysis(input_tape, output_tape);
        }
        
        if (*input_tape == NO_SYMBOL_NUMBER) {
            return;
        }


        SymbolNumber input = *input_tape;
        input_tape += local_stack.top().tape_step;

        if (input < orig_symbol_count) {
            find_index(input,
                       input_tape,
                       output_tape,
                       i+1);
        } else if (alphabet.get_identity_symbol() != NO_SYMBOL_NUMBER) {
            find_index(alphabet.get_identity_symbol(),
                       input_tape,
                       output_tape,
                       i+1);
        }
        if (alphabet.get_unknown_symbol() != NO_SYMBOL_NUMBER) {
            find_index(alphabet.get_unknown_symbol(),
                       input_tape,
                       output_tape,
                       i+1);
        }
        // if (alphabet.get_identity_symbol() != NO_SYMBOL_NUMBER &&
        //     local_stack.top().default_symbol_trap == true) {
        //     find_index(alphabet.get_identity_symbol(),
        //                input_tape, output_tape, i+1);
        // }

    }
}

bool PmatchTransducer::checking_context(void) const
{
    return local_stack.top().context != none;
}

bool PmatchTransducer::try_entering_context(SymbolNumber symbol)
{
    if (symbol == alphabet.get_special(LC_entry)) {
        local_stack.push(local_stack.top());
        local_stack.top().context = LC;
        local_stack.top().tape_step = -1;
        return true;
    } else if (symbol == alphabet.get_special(RC_entry)) {
        local_stack.push(local_stack.top());
        local_stack.top().context = RC;
        local_stack.top().tape_step = 1;
        return true;
    } else if (symbol == alphabet.get_special(NLC_entry)) {
        local_stack.push(local_stack.top());
        local_stack.top().context = NLC;
        local_stack.top().tape_step = -1;
        return true;
    } else if (symbol == alphabet.get_special(NRC_entry)) {
        local_stack.push(local_stack.top());
        local_stack.top().context = NRC;
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
        if (symbol == alphabet.get_special(LC_exit)) {
            exit_context();
            return true;
        } else {
            return false;
        }
    case RC:
        if (symbol == alphabet.get_special(RC_exit)) {
            exit_context();
            return true;
        } else {
            return false;
        }
    case NRC:
        if (symbol == alphabet.get_special(NRC_exit)) {
            local_stack.top().negative_context_success = true;
            return false;
        }
    case NLC:
        if (symbol == alphabet.get_special(NLC_exit)) {
            local_stack.top().negative_context_success = true;
            return false;
        }
    default:
        return false;
    }
}

void PmatchTransducer::exit_context(void)
{
    local_stack.push(local_stack.top());
    local_stack.top().context = none;
    local_stack.top().negative_context_success = false;
    local_stack.top().tape_step = 1;
}

}
