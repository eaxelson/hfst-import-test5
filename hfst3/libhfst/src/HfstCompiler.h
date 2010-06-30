#include "../../libhfst/src/HfstTransducer.h"
#include "implementations/SFST/src/interface.h"
//#include methods in interface.C not declared in interface.h
namespace SFST {
  void free_values( Range *r );
}

//#ifndef _INTERFACE_H_
//#define _INTERFACE_H_

//#include "utf8.h"
//#include "fst.h"

namespace SFST {
  extern bool Verbose;
  extern bool UTF8;
  extern char *FileName;
  extern SFST::Alphabet TheAlphabet;
}

namespace hfst
{
  class HfstCompiler {

  public:
    HfstCompiler();

    /*
    typedef enum {twol_left,twol_right,twol_both} Twol_Type;
    typedef enum {repl_left,repl_right,repl_up,repl_down} Repl_Type;
    */
    /*
    typedef struct range_t {
      Character character;
      struct range_t *next;
    } Range;
    
    typedef struct ranges_t {
      Range  *range;
      struct ranges_t *next;
    } Ranges;
    */    
    
    typedef SFST::Twol_Type Twol_Type;
    typedef SFST::Repl_Type Repl_Type;
    typedef SFST::Range Range;
    typedef SFST::Ranges Ranges;
    typedef SFST::Character Character;
    
    typedef struct contexts_t {
      HfstTransducer *left, *right;
      struct contexts_t *next;
    } Contexts;

  private:
    static HfstTransducer * make_transducer(Range *r1, Range *r2, ImplementationType type);
    
  public:
    void error2( const char *message, char *input );
    static HfstTransducer *new_transducer( Range*, Range*, ImplementationType );
    HfstTransducer *read_words( char *filename );
    HfstTransducer *read_transducer( char *filename );
    HfstTransducer *var_value( char *name );
    HfstTransducer *rvar_value( char *name );
    Range *svar_value( char *name );
    Range *complement_range( Range* );
    Range *rsvar_value( char *name );
    static Character character_code( unsigned int uc );
    static Character symbol_code( char *s );
    static unsigned int utf8toint( char *s );
    //static unsigned int utf8toint( char **s );

    static Range *add_value( Character, Range*);
    Range *add_var_values( char *name, Range*);
    static Range *add_values( unsigned int, unsigned int, Range*);
    static Range *append_values( Range *r2, Range *r );
    void add_alphabet( HfstTransducer* );  // ?
    
    // These functions delete their argument automata
    
    static void def_alphabet( HfstTransducer *a );
    bool def_var( char *name, HfstTransducer *a );
    bool def_rvar( char *name, HfstTransducer *a );
    bool def_svar( char *name, Range *r );
    HfstTransducer *explode( HfstTransducer *a );

    /*
    Transducer *catenate( Transducer *a1, Transducer *a2 );
    Transducer *disjunction( Transducer *a1, Transducer *a2 );
    Transducer *conjunction( Transducer *a1, Transducer *a2 );
    Transducer *subtraction( Transducer *a1, Transducer *a2 );
    Transducer *composition( Transducer *a1, Transducer *a2 );
    Transducer *restriction( Transducer *a, Twol_Type type, Contexts *c, int );
    Transducer *replace( Transducer *a, Repl_Type type, bool optional );
    Transducer *replace_in_context( Transducer *a, Repl_Type type, Contexts *c, bool optional );
    Transducer *negation( Transducer *a );
    Transducer *upper_level( Transducer *a );
    Transducer *lower_level( Transducer *a );
    Transducer *minimise( Transducer *a );
    Transducer *switch_levels( Transducer *a );
    Transducer *repetition( Transducer *a );
    Transducer *repetition2( Transducer *a );
    Transducer *optional( Transducer *a );
    Transducer *make_rule( Transducer *lc, Range *r1, Twol_Type type,
			   Range *r2, Transducer *rc );
    Transducer *freely_insert( Transducer *a, Character lc, Character uc );
    */
    
    static HfstTransducer *make_mapping( Ranges*, Ranges*, ImplementationType );
    Ranges *add_range( Range*, Ranges* );
    Contexts *make_context( HfstTransducer *l, HfstTransducer *r );
    Contexts *add_context( Contexts *nc, Contexts *c );
    static HfstTransducer *result( HfstTransducer*, bool );

    //void write_to_file( Transducer*, char *filename);
  };
}
//#endif
  

