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

#include "HfstTransducer.h"
#include <ext/hash_map>


namespace hfst
{

  typedef unsigned int Character;

    typedef struct range_t {
      Character character;
      struct range_t *next;
    } Range;
    
    typedef struct ranges_t {
      Range  *range;
      struct ranges_t *next;
    } Ranges;
    
    typedef struct contexts_t {
      HfstTransducer *left, *right;
      struct contexts_t *next;
    } Contexts;

    typedef enum {twol_left,twol_right,twol_both} Twol_Type;
    typedef enum {repl_left,repl_right,repl_up,repl_down} Repl_Type;


    namespace HfstUtf8 {

      const unsigned char get3LSbits=7;
      const unsigned char get4LSbits=15;
      const unsigned char get5LSbits=31;
      const unsigned char get6LSbits=63;
      
      const unsigned char set1MSbits=128;
      const unsigned char set2MSbits=192;
      const unsigned char set3MSbits=224;
      const unsigned char set4MSbits=240;

      char *int2utf8( unsigned int sym );
      unsigned int utf8toint( char **s );
      unsigned int utf8toint( char *s );
    }

    namespace HfstBasic {
      
      //bool Switch_Bytes=false;

      char* fst_strdup(const char* pString);
      int read_string( char *buffer, int size, FILE *file );
      size_t read_num( void *p, size_t n, FILE *file );

    }


    /* copied from SFST's alphabet.h|C */

    class HfstAlphabet {
      
    public:
      typedef std::pair<Character,Character> NumberPair;
      
    private:
      // string comparison operators needed to stored strings in a hash table
      struct eqstr {
	bool operator()(const char* s1, const char* s2) const {
	  return strcmp(s1, s2) == 0;
	}
      };
      
      // hash table used to map the symbols to their codes
      typedef __gnu_cxx::hash_map<const char*, Character, __gnu_cxx::hash<const char*>,eqstr> SymbolMap;
      // hash table used to map the codes back to the symbols
      typedef __gnu_cxx::hash_map<Character, char*> CharMap;
      // set of symbol pairs
      typedef std::set<NumberPair> NumberPairSet;
      
      SymbolMap sm; // maps symbols to codes
      CharMap  cm; // maps codes to symbols
      
      
      // The set of string pairs
      NumberPairSet pairs;
      
    public:
      HfstAlphabet() {
	add("@_EPSILON_SYMBOL_@",0);
	add("@_UNKNOWN_SYMBOL_@",1);
	add("@_IDENTITY_SYMBOL_@",2);
      }

      typedef NumberPairSet::const_iterator const_iterator;
      const_iterator begin() const { return pairs.begin(); }
      const_iterator end() const { return pairs.end(); };
      size_t size() const { return pairs.size(); };
      
      //bool contains_special_symbols(StringPair sp);  TODO!
      
      void insert(NumberPair sp) { /* check special symbols */ pairs.insert(sp); };  // TODO!
      void clear_pairs() { pairs.clear(); };
      
      void add( const char *symbol, Character c ) {
	char *s = HfstBasic::fst_strdup(symbol);
	cm[c] = s;
	sm[s] = c;
      }

      int symbol2code( const char * s ) const {
	SymbolMap::const_iterator p = sm.find(s);
	if (p != sm.end()) return p->second;
	return EOF;	
      }

      const char *code2symbol( Character c ) const {
	CharMap::const_iterator p=cm.find(c);
	if (p == cm.end())
	  return NULL;
	else
	  return p->second;
      }

      unsigned int add_symbol(const char * symbol) {
	if (sm.find(symbol) != sm.end())
	  return sm[symbol];
	
	// assign the symbol to some unused character
	for(Character i=1; i!=0; i++)
	  if (cm.find(i) == cm.end()) {
	    add(symbol, i);
	    return i;
	  }
	
	throw "Error: too many symbols in transducer definition";
      }
      
      void complement( vector<Character> &sym ) {
	vector<Character> result;
	for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
	  Character c = it->first;
	  if (c != 0) { // Label::epsilon
	    size_t i;
	    for( i=0; i<sym.size(); i++ )
	      if (sym[i] == c)
		break;
	    if (i == sym.size())
	      result.push_back(c);
	  }
	}
	sym.swap(result);
      }
      
    };
    
    
    /** A library class that forms a bridge between the SFST programming language parser
	and the HFST library and contains some extra functions needed by the parser.
	Most of these functions either directly use the SFST interface or are generalized
	versions of corresponding SFST functions that work on all types of HfstTransducers. */
    class HfstCompiler {
      
      struct ltstr {
	bool operator()(const char* s1, const char* s2) const
	{ return strcmp(s1, s2) < 0; }
      };
      
      struct eqstr {
	bool operator()(const char* s1, const char* s2) const
	{ return strcmp(s1, s2) == 0; }
      };
      
      typedef set<char*, ltstr> RVarSet;
      typedef __gnu_cxx::hash_map<char*, HfstTransducer*, __gnu_cxx::hash<const char*>, eqstr> VarMap;
      typedef __gnu_cxx::hash_map<char*, Range*, __gnu_cxx::hash<const char*>, eqstr> SVarMap;
      
    private:
      VarMap VM;
      SVarMap SVM;
      RVarSet RS;
      RVarSet RSS;
      
    public:
      bool Verbose;
      bool Alphabet_Defined;
      HfstAlphabet TheAlphabet;
      ImplementationType compiler_type;
#ifdef SFST
      
#endif
      
    HfstCompiler( ImplementationType type, bool verbose=false ) :
      Verbose(verbose), Alphabet_Defined(false), compiler_type(type)
      {
	
      }
      
    public:
      HfstTransducer * make_transducer(Range *r1, Range *r2, ImplementationType type);
      static void warn(const char *msg);
      HfstTransducer *new_transducer( Range*, Range*, ImplementationType );
      HfstTransducer *read_words( char *filename );  // TODO
      HfstTransducer *read_transducer( char *filename );
      HfstTransducer *var_value( char *name );
      HfstTransducer *rvar_value( char *name, ImplementationType );
      Range *svar_value( char *name );
      Range *complement_range( Range* );
      Range *rsvar_value( char *name );
      Character character_code( unsigned int uc );
      Character symbol_code( char *s );
      //static unsigned int utf8toint( char *s );
      
      void write_to_file(HfstTransducer *t, char* filename);
      
      static Range *add_value( Character, Range*);
      static Range *add_var_values( char *name, Range*);
      Range *add_values( unsigned int, unsigned int, Range*);
      static Range *append_values( Range *r2, Range *r );
      static Ranges *add_range( Range*, Ranges* );    
      //static void add_alphabet( HfstTransducer* );
      
      static Contexts *make_context( HfstTransducer *l, HfstTransducer *r );
      static Contexts *add_context( Contexts *nc, Contexts *c );    
      
      HfstTransducer * insert_freely(HfstTransducer *t, Character input, Character output);
      HfstTransducer * negation( HfstTransducer *t );
      
      // expand agreement variable markers
      HfstTransducer * explode( HfstTransducer *t );
      
      // rule operators
      HfstTransducer * replace_in_context(HfstTransducer * mapping, Repl_Type repl_type, Contexts *contexts, bool optional);
      HfstTransducer * replace(HfstTransducer * mapping, Repl_Type repl_type, bool optional);
      HfstTransducer * restriction( HfstTransducer * t, Twol_Type type, Contexts *c, int direction );
      HfstTransducer * make_rule( HfstTransducer * lc, Range * lower_range, Twol_Type type, 
				  Range * upper_range, HfstTransducer * rc );
      
      void def_alphabet( HfstTransducer *a );
      bool def_var( char *name, HfstTransducer *a );
      bool def_rvar( char *name, HfstTransducer *a );
      bool def_svar( char *name, Range *r );
      
      HfstTransducer *make_mapping( Ranges*, Ranges*, ImplementationType );
      HfstTransducer *result( HfstTransducer*, bool );
      
      // from SFST's interface.C
      static void free_values( Range * );
      static void free_values( Ranges * );
      static bool in_range( unsigned int c, Range *r );
      static unsigned int utf8toint( char * );

      static Range *copy_values( const Range *r );
      

      static void error( const char *message );
      static void error2( const char *message, char *input );
    };    
}

  

