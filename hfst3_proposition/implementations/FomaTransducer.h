#include "GlobalSymbolTable.h"
#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include <stdbool.h>  // foma uses _Bool
#include "foma/fomalib.h"
// Tropical: #include "ExtractStrings.h" NOT NEEDED
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

/*
int io_buf_is_empty(void);
int io_buf_is_end(void);
void io_free(void);
size_t io_gz_file_to_mem (char *filename);
struct fsm *io_net_read(char **net_name);
*/

// dummy definitions
typedef int FomaNode;
typedef int FomaArc;

// FomaNode and FomaArc must be replaced with foma's types
 
namespace hfst {
  namespace implementations
{
  //using namespace FOO;
  using namespace hfst::exceptions;
  // Tropical: typedef StdArc::StateId StateId;
  using namespace hfst::symbols;
  using std::ostream;
  using std::ostringstream;
  // Tropical: using std::stringstream;

  extern GlobalSymbolTable global_symbol_table;

  class FomaInputStream 
  {
    private:
    std::string filename;
    bool is_open_;
    // Sfst: FILE * input_file;
    // Tropical: ifstream i_stream;
    // Tropical: istream &input_stream;
    // Sfst: void add_symbol(StringSymbolMap &string_number_map,
    // Character c,
    // Alphabet &alphabet);
    /*void populate_key_table(KeyTable &key_table,
			    Sfst: Alphabet &alphabet,
			    Tropical: const SymbolTable * i_symbol_table,
			    Tropical: const SymbolTable * o_symbol_table,
			    KeyMap &key_map);
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);*/

  public:
    FomaInputStream(void);
    FomaInputStream(const char * filename);
    void open(void);
    void close(void);
    bool is_open(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    // Tropical: bool operator() (void);
    fsm * read_transducer(KeyTable &key_table);
    fsm * read_transducer(void);
  };

  class FomaTransitionIterator;
  class FomaStateIndexer;
  class FomaState
  {
    /*protected:
      FomaNode state;
      fsm * t;
      friend class FomaStateIndexer;*/
    public:
      FomaState(FomaNode state, fsm * t);
      FomaState(const FomaState &s);
      bool get_final_weight(void) const;
      bool operator< (const FomaState &another) const;
      bool operator== (const FomaState &another) const;
      bool operator!= (const FomaState &another) const;
      typedef FomaTransitionIterator const_iterator; 
      const_iterator begin(void) const;
      const_iterator end(void) const;
      void print(KeyTable &key_table, ostream &out,
		 FomaStateIndexer &indexer) const;
  };

  // dummy
  typedef int NodeNumbering;

  class FomaStateIterator 
    {
    protected:
      fsm * t;
      NodeNumbering node_numbering; // Sfst:
      // Tropical: StateIterator<fsm> * iterator;
      int current_state;
      bool ended;
    public:
      FomaStateIterator(fsm * t);
      FomaStateIterator(void);
      // Tropical: ~FomaStateIterator(void);
      void operator= (const FomaStateIterator &another);
      bool operator== (const FomaStateIterator &another) const;
      bool operator!= (const FomaStateIterator &another) const;
      const FomaState operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class FomaTransition
    {
      /*protected:
      FomaArc arc;
      FomaNode source_state;
      fsm * t;*/
    public:
      FomaTransition(FomaArc arc, FomaNode n, fsm * t); // Sfst:
      // Tropical: FomaTransition(const FomaArc &arc, 
      //		       FomaNode source_state, 
      //		       fsm * t);
      FomaTransition(const FomaTransition &t);  // Sfst:
      Key get_input_key(void) const;
      Key get_output_key(void) const;
      FomaState get_target_state(void) const;
      FomaState get_source_state(void) const;
      bool get_weight(void) const;
      void print(KeyTable &key_table, ostream &out,
		 FomaStateIndexer &indexer) const;
    };

  // dummy
  typedef int FomaArcsIter;

  class FomaTransitionIterator
    {
    protected:
      FomaNode state;
      FomaArcsIter arc_iterator;
      fsm * t;
      bool end_iterator;
    public:
      FomaTransitionIterator(FomaNode state,fsm * t);
      FomaTransitionIterator(void);
      //Sfst: ~FomaTransitionIterator(void);
      void operator=  (const FomaTransitionIterator &another);
      bool operator== (const FomaTransitionIterator &another);
      bool operator!= (const FomaTransitionIterator &another);
      const FomaTransition operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class FomaStateIndexer
    {
    protected:
      fsm * t;
      NodeNumbering numbering; // Sfst:
    public:
      FomaStateIndexer(fsm * t);
      unsigned int operator[](const FomaState &state);
      const FomaState operator[](unsigned int number);
    };

  class FomaTransducer
    {
    public:
      static fsm * create_empty_transducer(void);
      static fsm * create_epsilon_transducer(void);
      static fsm * define_transducer(Key k);                     // not done
      static fsm * define_transducer(const KeyPair &kp);         // not done
      static fsm * define_transducer(const KeyPairVector &kpv);  // not done
      static fsm * copy(fsm * t);
      static fsm * determinize(fsm * t);
      static fsm * minimize(fsm * t);
      static fsm * remove_epsilons(fsm * t);
      static fsm * repeat_star(fsm * t);
      static fsm * repeat_plus(fsm * t);
      static fsm * repeat_n(fsm * t,int n);
      static fsm * repeat_le_n(fsm * t,int n);
      static fsm * optionalize(fsm * t);
      static fsm * invert(fsm * t);
      static fsm * reverse(fsm * t);
      static fsm * extract_input_language(fsm * t);
      static fsm * extract_output_language(fsm * t);
      static fsm * substitute(fsm * t,Key old_key,Key new_key);  // not done
      static fsm * substitute(fsm * t,                           // not done
			      KeyPair old_key_pair,
			      KeyPair new_key_pair);
      static fsm * compose(fsm * t1,
			   fsm * t2);
      static fsm * concatenate(fsm * t1,
			       fsm * t2);
      static fsm * disjunct(fsm * t1,
			    fsm * t2);
      static fsm * intersect(fsm * t1,
			     fsm * t2);
      static fsm * subtract(fsm * t1,
			    fsm * t2);
      typedef FomaStateIterator const_iterator;
      static const_iterator begin(fsm * t);
      static const_iterator end(fsm * t);
      static fsm * harmonize(fsm * t,KeyMap &key_map);                // not done
      static void print(fsm * t, KeyTable &key_table, ostream &out);  // not done
    };

} }
