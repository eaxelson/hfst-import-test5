#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include <stdbool.h>  // foma uses _Bool
#include "foma/fomalib.h"
#include "zlib.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

namespace hfst {
  namespace implementations
{
  using namespace hfst::exceptions;
  using std::ostream;
  using std::ostringstream;

  class FomaInputStream 
  {
  private:
    std::string filename;
    FILE * input_file;
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);
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
    fsm * read_transducer(bool has_header);
  };

  class FomaOutputStream 
  {
  private:
    std::string filename;
    FILE *ofile;
    void write_3_0_library_header(FILE *file);
  public:
    FomaOutputStream(void); 
    FomaOutputStream(const char * filename);
    void open(void);
    void close(void);
    void write_transducer(fsm * transducer);
  };

  class FomaTransducer
    {
    public:
      static fsm * create_empty_transducer(void);
      static fsm * create_epsilon_transducer(void);
      static fsm * define_transducer(const hfst::StringPairVector &spv);
      static fsm * define_transducer(char *symbol);                     
      static fsm * define_transducer(char *isymbol, char *osymbol);       
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

      static bool are_equivalent(fsm *t1, fsm *t2);

      static fsm * substitute(fsm * t,hfst::String old_symbol,hfst::String new_symbol);

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
      static void delete_foma(fsm * net);
      static void print_test(fsm * t);
    };

} }
