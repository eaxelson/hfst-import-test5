#include "pmatch.h"

namespace hfst_ol {


void PmatchContainer::add_rtn(PmatchTransducer * rtn, std::string & name)
{
    rtns.insert(std::pair<std::string, PmatchTransducer *>(name, rtn));
}

std::string PmatchContainer::match(std::string & input)
{
    std::string ret;
    initialize_input(input.c_str());
    while (has_queued_input()) {
	toplevel->match(input_tape, rtns);
	copy_to_output(toplevel->get_best_result());
	jump_queued_token();
    }
    return stringify_output();
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

void jump_queued_token(void)
{
    if (*input_tape == NO_SYMBOL_NUMBER) {
	return;
    }
    output.push_back(*input_tape++);
    return;
}

void PmatchContainer::tokenize_from_queue(void)
{
    if (input_queue == NULL || *input_queue == '\0') {
	return;
    }
    char * orig_input_queue = input_queue;
    tokenize(&input_queue);
    char * new_input_queue = (char*) malloc(strlen(input_queue));
    strcpy(new_input_queue, input_queue);
    input_queue = new_input_queue;
    free(orig_input_queue);
    return;
}

void PmatchContainer::initialize_input(char * input_str)
{
    input_tape = orig_input_tape;
    int i = 0;
    SymbolNumber key = NO_SYMBOL_NUMBER;
    char ** input_str_pointer = &input_str;
    while (**input_str_ptr != 0) {
	k = encode->find_key(input_str_ptr);
	input_tape[i] = k;
	if (k == NO_SYMBOL_NUMBER) {
	    // Regular tokenization failed
	    int bytes_to_tokenize = nByte_utf8(**input_str_ptr);
	    if (bytes_to_tokenize == 0) {
		// even if it's not utf-8, tokenize a byte at a time
		bytes_to_tokenize = 1;
	    }
	    char new_symbol[bytes_to_tokenize];
	    memcpy(new_symbol, *input_str_ptr, bytes_to_tokenize);
	    *input_str_ptr += bytes_to_tokenize;
	    alphabet.add_symbol(new_symbol);
	    encoder.read_input_symbol(new_symbol, symbol_count);
	    k = symbol_count;
	    ++symbol_count;
	}
	input_tape[i] = k;
	++i;
    }
    input_tape[i] = NO_SYMBOL_NUMBER;
    return;
}

std::string PmatchTransducer::pmatch(NameRtnMap & rtnmap)
{
    
}

}
