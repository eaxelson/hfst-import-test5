#ifndef _HFST_OL_TRANSDUCER_PMATCH_H_
#define _HFST_OL_TRANSDUCER_PMATCH_H_

#include <map>
#include <stack>
#include "transducer.h"

namespace hfst_ol {

    class PmatchTransducer;
    typedef std::map<std::string, PmatchTransducer *> NameRtnMap;
    enum SpecialSymbol{entry, exit, LC_entry, LC_exit, RC_entry, RC_exit};

    class PmatchContainer
    {
    protected:
        TransducerAlphabet alphabet;
        Encoder * encoder;
        SymbolNumber symbol_count;
        PmatchTransducer * toplevel;
        std::string toplevel_name;
        std::map<std::string, PmatchTransducer *> rtns;
        SymbolNumberVector best_result;
        SymbolNumber * input_tape;
        SymbolNumber * orig_input_tape;
        SymbolNumber * output_tape;
        SymbolNumber * orig_output_tape;
        SymbolNumber * candidate_input_pos;
        SymbolNumberVector output;

        std::map<SpecialSymbol, SymbolNumber> special_symbols;
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
        static const size_t SIZE = 
            sizeof(SymbolNumber) + 
            sizeof(SymbolNumber) + 
            sizeof(TransitionTableIndex);
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
        static const size_t SIZE = 
            sizeof(SymbolNumber) + 
            sizeof(TransitionTableIndex);
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
        enum ContextChecking{none, LC, NLC, RC, NRC};

// Transducers have static data, ie. tables for describing the states and
// transitions, and dynamic data, which is altered during lookup.
// In pmatch several instances of the same transducer may be operating
// in a stack, so this dynamic data is put in a class of its own.
        struct Locals
        {
            SymbolNumber * candidate_input_pos;
            SymbolNumber * output_tape_head;
            hfst::FdState<SymbolNumber> flag_state;
            char tape_step;
            SymbolNumber * context_placeholder;
            ContextChecking context;
        };

        std::stack<Locals> local_stack;
    
        std::vector<SimpleTransition> transition_table;
        std::vector<SimpleIndex> index_table;

        TransducerAlphabet & alphabet;
    
        std::map<std::string, PmatchTransducer *> & rtns;
        std::map<SpecialSymbol, SymbolNumber> & markers;
        SymbolNumberVector & best_result;

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

        bool checking_context(void) const;
        bool try_entering_context(SymbolNumber symbol);
        bool try_exiting_context(SymbolNumber symbol);
        void exit_context(void);


    public:
        PmatchTransducer(std::istream& is,
                         TransitionTableIndex index_table_size,
                         TransitionTableIndex transition_table_size,
                         TransducerAlphabet & alphabet,
                         std::map<std::string, PmatchTransducer *> & rtns,
                         std::map<SpecialSymbol, SymbolNumber> & markers,
                         SymbolNumberVector & best_result);

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
