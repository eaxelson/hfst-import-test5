#ifndef _HFST_OL_TRANSDUCER_PMATCH_H_
#define _HFST_OL_TRANSDUCER_PMATCH_H_

#include <map>
#include "transducer.h"

namespace hfst_ol {

class PmatchTransducer;
typedef std::map<std::string, PmatchTransducer *> NameRtnMap;

class PmatchContainer
{
protected:
    TransducerAlphabet alphabet;
    Encoder * encoder;
    SymbolNumber symbol_count;
    PmatchTransducer * toplevel;
    std::string toplevel_name;
    std::map<std::string, PmatchTransducer *> rtns;
    SymbolNumber * input_tape;
    SymbolNumber * orig_input_tape;
    SymbolNumber * output_tape;
    SymbolNumber * orig_output_tape;
    SymbolNumberVector output;

    SymbolNumber entry_marker;
    SymbolNumber exit_marker;
    SymbolNumber LC_marker;
    SymbolNumber RC_marker;
    SymbolNumber NLC_marker;
    SymbolNumber NRC_MARKER;
    SymbolNumber unknown_symbol;
    std::map<SymbolNumber, std::string> end_tag_map;

    void add_special_symbol(const std::string & str, SymbolNumber symbol_number);

public:
    PmatchContainer(std::istream & is);
    ~PmatchContainer(void);

    void initialize_input(const char * input);
    bool has_unsatisfied_rtns(void) const;
    std::string get_unsatisfied_rtn_name(void) const;
    std::string match(std::string & input);
    void add_rtn(PmatchTransducer * rtn, std::string & name);
    bool has_queued_input(void);
    void copy_to_output(SymbolNumberVector & best_result);
    std::string stringify_output(void);

    static std::string parse_name_from_hfst3_header(std::istream & f);
    static bool is_end_tag(const std::string & symbol);
    bool is_end_tag(const SymbolNumber symbol) const;
    std::string end_tag(const SymbolNumber symbol);
    std::string start_tag(const SymbolNumber symbol);

};

struct SimpleTransition
{
    SymbolNumber input;
    SymbolNumber output;
    TransitionTableIndex target;
    static const size_t SIZE = sizeof(input) + sizeof(output) + sizeof(target);
    SimpleTransition(
        SymbolNumber i, SymbolNumber o, TransitionTableIndex t):
    input(i), output(o), target(t) {}
    bool final(void) const {
        return input == NO_SYMBOL_NUMBER &&
            output == NO_SYMBOL_NUMBER &&
            target == 1;
    }
};

struct SimpleIndex
{
    SymbolNumber input;
    TransitionTableIndex target;
    static const size_t SIZE = sizeof(input) + sizeof(target);
    SimpleIndex(
        SymbolNumber i, TransitionTableIndex t):
    input(i), target(t) {}
    bool final(void) const {
        return input == NO_SYMBOL_NUMBER &&
            target != NO_TABLE_INDEX;
    }
};

struct ContextMatchedTrap
{
    bool polarity;
ContextMatchedTrap(bool p): polarity(p) {}
};

class PmatchTransducer
{
protected:
    std::vector<SimpleTransition> transition_table;
    std::vector<SimpleIndex> index_table;

    TransducerAlphabet & alphabet;
    
    SymbolNumberVector best_result;
    SymbolNumber * candidate_input_pos;
    SymbolNumber * output_tape_head;
    hfst::FdState<SymbolNumber> flag_state;
    std::map<std::string, PmatchTransducer *> & rtns;
    SymbolNumber & entry_marker;
    SymbolNumber & exit_marker;

    // The mutually recursive lookup-handling functions

    void try_epsilon_transitions(SymbolNumber * input_tape,
                                 SymbolNumber * output_tape,
                                 TransitionTableIndex i);
  
    void try_epsilon_indices(SymbolNumber * input_tape,
                             SymbolNumber * output_tape,
                             TransitionTableIndex i);

    void find_transitions(SymbolNumber input,
                          SymbolNumber * input_tape,
                          SymbolNumber * output_tape,
                          TransitionTableIndex i);

    void find_index(SymbolNumber input,
                    SymbolNumber * input_tape,
                    SymbolNumber * output_tape,
                    TransitionTableIndex i);

    void get_analyses(SymbolNumber * input_tape,
                      SymbolNumber * output_tape,
                      TransitionTableIndex index);

    // A similar but slightly different set of mutually recursive functions
    // for checking contexts

    void check_context_try_epsilon_transitions(SymbolNumber * input_tape,
                                               TransitionTableIndex i,
                                               int step,
                                               bool polarity);
  
    void check_context_try_epsilon_indices(SymbolNumber * input_tape,
                                           TransitionTableIndex i,
                                           int step,
                                           bool polarity);

    void check_context_find_transitions(SymbolNumber input,
                                        SymbolNumber * input_tape,
                                        TransitionTableIndex i,
                                        int step,
                                        bool polarity);

    void check_context_find_index(SymbolNumber input,
                                  SymbolNumber * input_tape,
                                  TransitionTableIndex i,
                                  int step,
                                  bool polarity);

    void check_context(SymbolNumber * input_tape,
                       TransitionTableIndex index,
                       int step,
                       bool polarity);



public:
    PmatchTransducer(std::istream& is,
                     TransitionTableIndex index_table_size,
                     TransitionTableIndex transition_table_size,
                     TransducerAlphabet & alphabet,
                     std::map<std::string, PmatchTransducer *> & rtns,
                     SymbolNumber & entry_marker,
                     SymbolNumber & exit_marker);
        
    SymbolNumberVector & get_best_result(void)
    { return best_result; }

    void display() const;

    // const SimpleIndex& get_index(TransitionTableIndex i) const
    // { return index_table[i] }
    // const Transition& get_transition(TransitionTableIndex i) const
    // { return tables->get_transition(i); }
    bool final_index(TransitionTableIndex i) const
    {
        if (indexes_transition_table(i)) {
	    return transition_table[i].final();
        } else {
	    return index_table[i].final();
        }
    }

    static bool indexes_transition_table(TransitionTableIndex i)
	{ return  i >= TRANSITION_TARGET_TABLE_START; }
    
    void match(SymbolNumber ** input_tape_entry, SymbolNumber ** output_tape_entry);
    void note_analysis(SymbolNumber * input_tape, SymbolNumber * output_tape);

};


}

#endif //_HFST_OL_TRANSDUCER_PMATCH_H_
