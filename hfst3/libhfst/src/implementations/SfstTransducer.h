#include "HfstExceptions.h"
#include "SFST/src/fst.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
 
namespace hfst { namespace implementations
{
  using namespace SFST;
  using namespace hfst::exceptions;
  using std::ostream;
  using std::ostringstream;

  class SfstInputStream 
  {
  private:
    std::string filename;
    FILE * input_file;
    void add_symbol(StringNumberMap &string_number_map,
		    Character c,
		    Alphabet &alphabet);
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
  
  class SfstTransducer
    {
    public:
      static Transducer * create_empty_transducer(void);
      static Transducer * create_epsilon_transducer(void);
      static Transducer * define_transducer(const char *symbol);
      static Transducer * define_transducer(const char *isymbol, const char *osymbol);
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
      static void extract_strings(Transducer * t, hfst::WeightedPaths<float>::Set &results);

      static Transducer * substitute(Transducer * t, String old_symbol, String new_symbol);

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
      static std::pair<Transducer*, Transducer*> harmonize(Transducer *t1, Transducer *t2);

      static void print_test(Transducer *t);

    protected:
      static StringSet get_string_set(Transducer *t);
      static Transducer * expand_arcs(Transducer * t, StringSet &unknown);
    };
} }
