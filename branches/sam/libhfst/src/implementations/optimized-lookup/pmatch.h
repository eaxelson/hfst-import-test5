#ifndef _HFST_OL_TRANSDUCER_PMATCH_H_
#define _HFST_OL_TRANSDUCER_PMATCH_H_

#include <map>
#include "transducer.h"

namespace hfst_ol {

class PmatchTransducer;
typedef std::map<std::string, PmatchTransducer *> NameRtnMap;

class PmatchContainer
{
    TransducerAlphabet alphabet;
    PmatchTransducer * toplevel;
    std::string toplevel_name;
    std::map<std::string, PmatchTransducer *> rtns;
    SymbolNumber * input_tape;
    SymbolNumber * orig_input_tape;
    SymbolNumber symbol_count;
    SymbolNumberVector output;

public:
PmatchContainer(TransducerAlphabet alpha,
		PmatchTransducer * tl,
		std::string tl_name):
    alphabet(alpha),
	toplevel(tl),
	toplevel_name(tl_name),
        rtns(std::map<std::string, PmatchTransducer *>()),
	input_tape((SymbolNumber*)(malloc(sizeof(SymbolNumber) * MAX_IO_LEN))),
	output()
        { }

    ~PmatchContainer()
    {
	delete toplevel;
	for (std::map<std::string, PmatchTransducer *>::iterator it = rtns.begin();
	     it != rtns.end(); ++it) {
	    delete it->second;
	}
    }

    initialize_input(const char * input_str);
    bool has_unsatisfied_rtns(void) const;
    std::string get_unsatisfied_rtn_name(void) const;
    std::string match(std::string & input);
    void add_rtn(PmatchTransducer * rtn, std::string & name);
    bool has_queued_input(void);

};

class PmatchTransducer
{
protected:
    TransducerTablesInterface* tables;
    void load_tables(std::istream& is);

    Weight current_weight;
    SymbolNumber * best_result;
    SymbolNumber * output_tape;
    SymbolNumber * input_pointer;
    hfst::FdState<SymbolNumber> flag_state;

    void try_epsilon_transitions(SymbolNumber * input_symbol,
                 SymbolNumber * output_symbol,
                 SymbolNumber * original_output_tape,
                 TransitionTableIndex i);
  
    void try_epsilon_indices(SymbolNumber * input_symbol,
                 SymbolNumber * output_symbol,
                 SymbolNumber * original_output_tape,
                 TransitionTableIndex i);

    void find_transitions(SymbolNumber input,
              SymbolNumber * input_symbol,
              SymbolNumber * output_symbol,
              SymbolNumber * original_output_tape,
              TransitionTableIndex i);

    void find_index(SymbolNumber input,
            SymbolNumber * input_symbol,
            SymbolNumber * output_symbol,
            SymbolNumber * original_output_tape,
            TransitionTableIndex i);

    void get_analyses(SymbolNumber * input_symbol,
              SymbolNumber * output_symbol,
              SymbolNumber * original_output_tape,
              TransitionTableIndex i);



public:
    PmatchTransducer(std::istream& is);
    SymbolNumber * get_best_result(void)
    { return best_result; }

    void write(std::ostream& os) const;
    Transducer * copy(Transducer * t, bool weighted = false);
    void display() const;

    const TransducerHeader& get_header() const
    { return *header; }
    const TransducerAlphabet& get_alphabet() const
    { return *alphabet; }
    const Encoder& get_encoder(void) const
	{ return *encoder; }
    const hfst::FdTable<SymbolNumber>& get_fd_table() const
        { return alphabet->get_fd_table(); }
    const SymbolTable& get_symbol_table() const
        { return alphabet->get_symbol_table(); }


    const TransitionIndex& get_index(TransitionTableIndex i) const
    { return tables->get_index(i); }
    const Transition& get_transition(TransitionTableIndex i) const
    { return tables->get_transition(i); }
    bool final_index(TransitionTableIndex i) const
    {
        if (indexes_transition_table(i)) {
        return tables->get_transition_finality(i);
        } else {
        return tables->get_index_finality(i);
        }
    }
    TransducerTable<TransitionWIndex> & copy_windex_table();
    TransducerTable<TransitionW> & copy_transitionw_table();
    TransducerTable<TransitionIndex> & copy_index_table();
    TransducerTable<Transition> & copy_transition_table();

    // state_index must be an index to a state which is defined as either:
    // (1) the start of a set of entries in the transition index table, or
    // (2) the boundary before a set of entries in the transition table, in
    //     which case the following entries will all have the same input symbol
    // This function will return a vector of indices to the transition table,
    // i.e. the arcs from the given state
    TransitionTableIndexSet get_transitions_from_state(
    TransitionTableIndex state_index) const;


    HfstOneLevelPaths * lookup_fd(const StringVector & s);
    /* Tokenize and lookup, accounting for flag diacritics, the surface string
       \a s. The return value, a pointer to HfstOneLevelPaths
       (which is a set) of analyses, is newly allocated.
     */
    HfstOneLevelPaths * lookup_fd(const std::string & s);
    HfstOneLevelPaths * lookup_fd(const char * s);
    std::string pmatch(const std::string & s);
    void note_analysis(SymbolNumber * whole_output_tape);

    // Methods for supporting ospell
    SymbolNumber get_unknown_symbol(void) const
        { return alphabet->get_unknown_symbol(); }
    StringSymbolMap get_string_symbol_map(void) const
        { return alphabet->build_string_symbol_map(); }
    STransition take_epsilons(const TransitionTableIndex i) const;
    STransition take_epsilons_and_flags(const TransitionTableIndex i);
    STransition take_non_epsilons(const TransitionTableIndex i,
				  const SymbolNumber symbol) const;
    TransitionTableIndex next(const TransitionTableIndex i,
			      const SymbolNumber symbol) const;
    TransitionTableIndex next_e(const TransitionTableIndex i) const;
    bool has_transitions(const TransitionTableIndex i,
			 const SymbolNumber symbol) const;
    bool has_epsilons_or_flags(const TransitionTableIndex i);
    Weight final_weight(const TransitionTableIndex i) const;
    bool is_flag(const SymbolNumber symbol)
	{ return alphabet->is_flag_diacritic(symbol); }
    bool is_weighted(void)
	{ return header->probe_flag(Weighted);}

    
};


}

#endif //_HFST_OL_TRANSDUCER_PMATCH_H_
