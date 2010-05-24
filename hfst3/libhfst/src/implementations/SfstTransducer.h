#include "GlobalSymbolTable.h"
#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include "SFST/src/fst.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
 
namespace hfst { namespace implementations
{
  using namespace SFST;
  using namespace hfst::symbols;
  using namespace hfst::exceptions;
  using std::ostream;
  using std::ostringstream;

  extern GlobalSymbolTable global_symbol_table;

  class SfstInputStream 
  {
  private:
    std::string filename;
    FILE * input_file;
    void add_symbol(StringSymbolMap &string_number_map,
		    Character c,
		    Alphabet &alphabet);
    void populate_key_table(KeyTable &key_table,
			    Alphabet &alphabet,
			    KeyMap &key_map);
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);

  public:
    SfstInputStream(void);
    SfstInputStream(const char * filename);
    void open(void);
    void close(void);
    bool is_open(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    Transducer * read_transducer(KeyTable &key_table);
    Transducer * read_transducer(bool has_header);
  };

  class SfstOutputStream 
  {
  private:
    std::string filename;
    FILE *ofile;
    void write_3_0_library_header(FILE *file);
  public:
    SfstOutputStream(void); 
    SfstOutputStream(const char * filename);
    void open(void);
    void close(void);
    void write_transducer(Transducer * transducer);
  };
  
  class SfstTransitionIterator;
  class SfstStateIndexer;
  class SfstState
  {
    protected:
      Node * state;
      Transducer * t;
      friend class SfstStateIndexer;
    public:
      SfstState(Node * state, Transducer * t);
      SfstState(const SfstState &s);
      bool get_final_weight(void) const;
      bool operator< (const SfstState &another) const;
      bool operator== (const SfstState &another) const;
      bool operator!= (const SfstState &another) const;
      typedef SfstTransitionIterator const_iterator; 
      const_iterator begin(void) const;
      const_iterator end(void) const;
      void print(KeyTable &key_table, ostream &out,
		 SfstStateIndexer &indexer) const;
  };
  
  class SfstStateIterator 
    {
    protected:
      NodeNumbering node_numbering;
      Transducer * t;
      int current_state;
      bool ended;
    public:
      SfstStateIterator(Transducer * t);
      SfstStateIterator(void);
      void operator= (const SfstStateIterator &another);
      bool operator== (const SfstStateIterator &another) const;
      bool operator!= (const SfstStateIterator &another) const;
      const SfstState operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class SfstTransition
    {
    protected:
      Arc * arc;
      Node * source_state;
      Transducer * t;
    public:
      SfstTransition(Arc * arc, Node * n, Transducer * t);
      SfstTransition(const SfstTransition &t);
      Key get_input_key(void) const;
      Key get_output_key(void) const;
      SfstState get_target_state(void) const;
      SfstState get_source_state(void) const;
      /* Kind of weird function, since SFST
	 transitions don't really carry a weight. 
	 Will always return true. */
      bool get_weight(void) const;
      void print(KeyTable &key_table, ostream &out,
		 SfstStateIndexer &indexer) const;
    };

  class SfstTransitionIterator
    {
    protected:
      Node * state;
      ArcsIter arc_iterator;
      Transducer * t;
      bool end_iterator;
    public:
      SfstTransitionIterator(Node * state,Transducer * t);
      SfstTransitionIterator(void);
      void operator=  (const SfstTransitionIterator &another);
      bool operator== (const SfstTransitionIterator &another);
      bool operator!= (const SfstTransitionIterator &another);
      const SfstTransition operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class SfstStateIndexer
    {
    protected:
      Transducer * t;
      NodeNumbering numbering;
    public:
      SfstStateIndexer(Transducer * t);
      unsigned int operator[](const SfstState &state);
      const SfstState operator[](unsigned int number);
    };

  class SfstTransducer
    {
    public:
      static Transducer * create_empty_transducer(void);
      static Transducer * create_epsilon_transducer(void);
      static Transducer * define_transducer(Key k);
      static Transducer * define_transducer(const KeyPair &kp);
      static Transducer * define_transducer(const char *symbol);
      static Transducer * define_transducer(const char *isymbol, const char *osymbol);
      static Transducer * define_transducer(const KeyPairVector &kpv);
      static Transducer * define_transducer(const StringPairVector &spv);
      static Transducer * copy(Transducer * t);
      static Transducer * determinize(Transducer * t);
      static Transducer * minimize(Transducer * t);
      static Transducer * remove_epsilons(Transducer * t);
      static Transducer * repeat_star(Transducer * t);
      static Transducer * repeat_plus(Transducer * t);
      static Transducer * repeat_n(Transducer * t,int n);
      static Transducer * repeat_le_n(Transducer * t,int n);
      static Transducer * optionalize(Transducer * t);
      static Transducer * invert(Transducer * t);
      static Transducer * reverse(Transducer * transducer);
      static Transducer * extract_input_language(Transducer * t);
      static Transducer * extract_output_language(Transducer * t);
      static void extract_strings(Transducer * t, hfst::implementations::WeightedStrings<float>::Set &results);
      static Transducer * substitute(Transducer * t,Key old_key,Key new_key);
      static Transducer * substitute(Transducer * t,
				     KeyPair old_key_pair,
				     KeyPair new_key_pair);  // not implemented

      static Transducer * substitute(Transducer * t, StringSymbol old_symbol, StringSymbol new_symbol);

      static Transducer * compose(Transducer * t1,
			   Transducer * t2);
      static Transducer * concatenate(Transducer * t1,
			       Transducer * t2);
      static Transducer * disjunct(Transducer * t1,
			    Transducer * t2);
      static Transducer * intersect(Transducer * t1,
			     Transducer * t2);
      static Transducer * subtract(Transducer * t1,
			    Transducer * t2);
      typedef SfstStateIterator const_iterator;
      static const_iterator begin(Transducer * t);
      static const_iterator end(Transducer * t);
      //static Transducer * harmonize(Transducer * t,KeyMap &key_map);
      static void print(Transducer * t, KeyTable &key_table, ostream &out);

      static std::pair<Transducer*, Transducer*> harmonize(Transducer *t1, Transducer *t2);

      // remove this
      //static void harmonize(Transducer * t1, Transducer * t2);

      // see if these can be removed
      static KeyMap create_mapping(Transducer * t1, Transducer * t2, StringSymbolSet &unknown2);

      static void print_test(Transducer *t);

    protected:
      static StringSymbolSet get_string_symbol_set(Transducer *t);
      static Transducer * expand_arcs(Transducer * t, StringSymbolSet &unknown);
    };
} }
