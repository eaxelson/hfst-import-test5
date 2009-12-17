#ifndef __PARSE_FLAG_DIACRITICS__
#define __PARSE_FLAG_DIACRITICS__
#include <cstring>

static const char SEPARATOR = '.';
static const char AT = '@';
static const char A = 'A';
static const char Z = 'Z';
static const char a = 'a';
static const char z = 'z';

inline void next_index( const char ** symbol_name );
inline void set_index( const char ** symbol_name, 
		       const char * index );
inline bool ended( const char ** symbol_name ); 
inline bool upper_case_char( const char ** symbol_name );
inline bool lower_case_char( const char ** symbol_name );
inline bool alphabetical_char( const char ** symbol_name );
bool at_char(const char ** symbol_name);
bool separator_char(const char ** symbol_name);
const char * next_separator( const char ** symbol_name);
const char * next_at( const char ** symbol_name);
inline bool unequal_indices(const char ** symbol_name,
			    const char * index);
bool parse_NAME(const char ** symbol_name);
bool parse_SUFFIX(const char ** symbol_name);
bool parse_PREFIX(const char ** symbol_name );
bool parse_flag_diacritic( const char * symbol_name);

#ifdef HFST_TESTS
#include <iostream>
using std::cout;
bool _parse_flag_diacritics_C_test( void );
#endif
#endif
