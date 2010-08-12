//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include "../../libhfst/src/HfstTransducer.h"
#include "implementations/SFST/src/interface.h"

// global variables and datatypes specific to SFST-PL parser
// that are defined in file 'SFST/src/interface.C'
namespace SFST {
  //extern bool Verbose;
  //extern bool UTF8;
  //extern char *FileName;
  //extern SFST::Alphabet TheAlphabet;
  extern int Alphabet_Defined;
  extern std::set<char*> RS;
  extern std::set<char*> RSS;
}

namespace hfst
{

  /** A library class that forms a bridge between the SFST programming language parser
      and the HFST library and contains some extra functions needed by the parser.
      Most of these functions either directly use the SFST interface or are generalized
      versions of corresponding SFST functions that work on all types of HfstTransducers. */
  class HfstCompiler {

  public:
    HfstCompiler();
 
    ImplementationType compiler_type;
   
    typedef SFST::Twol_Type Twol_Type;
    typedef SFST::Repl_Type Repl_Type;
    typedef SFST::Range Range;
    typedef SFST::Ranges Ranges;
    typedef SFST::Character Character;
    
    typedef struct contexts_t {
      HfstTransducer *left, *right;
      struct contexts_t *next;
    } Contexts;

    /* For storing transducer variables */
    typedef map<char*, HfstTransducer*, SFST::ltstr> VarMap;

  private:
    static HfstTransducer * make_transducer(Range *r1, Range *r2, ImplementationType type);
    static VarMap VM; /* Transducer variables */

  public:
    static void warn(const char *msg);
    static HfstTransducer *new_transducer( Range*, Range*, ImplementationType );
    HfstTransducer *read_words( char *filename );
    static HfstTransducer *read_transducer( char *filename );
    static HfstTransducer *var_value( char *name );
    static HfstTransducer *rvar_value( char *name, ImplementationType );
    static Range *svar_value( char *name );
    Range *complement_range( Range* );
    static Range *rsvar_value( char *name );
    static Character character_code( unsigned int uc );
    static Character symbol_code( char *s );
    static unsigned int utf8toint( char *s );

    static void write_to_file(HfstTransducer *t, char* filename);

    static Range *add_value( Character, Range*);
    static Range *add_var_values( char *name, Range*);
    static Range *add_values( unsigned int, unsigned int, Range*);
    static Range *append_values( Range *r2, Range *r );
    static void add_alphabet( HfstTransducer* );  // ?

    static HfstCompiler::Contexts *make_context( HfstTransducer *l, HfstTransducer *r );
    static HfstCompiler::Contexts *add_context( HfstCompiler::Contexts *nc, HfstCompiler::Contexts *c );    

    static HfstTransducer * insert_freely(HfstTransducer *t, Character input, Character output);
    static HfstTransducer * negation( HfstTransducer *t );

    // expand agreement variable markers
    static HfstTransducer * explode( HfstTransducer *t );
    
    // rule operators
    static HfstTransducer * replace_in_context(HfstTransducer * mapping, Repl_Type repl_type, Contexts *contexts, bool optional);
    static HfstTransducer * replace(HfstTransducer * mapping, Repl_Type repl_type, bool optional);
    static HfstTransducer * restriction( HfstTransducer * t, Twol_Type type, Contexts *c, int direction );
    static HfstTransducer * make_rule( HfstTransducer * lc, Range * lower_range, Twol_Type type, 
				       Range * upper_range, HfstTransducer * rc );

    static void def_alphabet( HfstTransducer *a );
    static bool def_var( char *name, HfstTransducer *a );
    static bool def_rvar( char *name, HfstTransducer *a );
    static bool def_svar( char *name, Range *r );
    
    static HfstTransducer *make_mapping( Ranges*, Ranges*, ImplementationType );
    static Ranges *add_range( Range*, Ranges* );
    static HfstTransducer *result( HfstTransducer*, bool );

  };
}
//#endif
  

