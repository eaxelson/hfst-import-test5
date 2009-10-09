#include "parse_flag_diacritics.h"

inline void next_index( const char ** symbol_name ) {
  ++(*symbol_name);
}

inline void set_index( const char ** symbol_name, 
		       const char * index ) {
  *symbol_name = index;
}

inline bool ended( const char ** symbol_name ) {
  return 0 == **symbol_name;
}
 
inline bool upper_case_char( const char ** symbol_name ) {
  return (A <= **symbol_name) and (**symbol_name <= Z);
}

inline bool lower_case_char( const char ** symbol_name ) {
  return (a <= **symbol_name) and (**symbol_name <= z);
}

inline bool alphabetical_char( const char ** symbol_name ) {
  return upper_case_char(symbol_name) or lower_case_char(symbol_name);
}

bool at_char(const char ** symbol_name) {
  return AT == **symbol_name;
}

bool separator_char(const char ** symbol_name) {
  return SEPARATOR == **symbol_name;
}


const char * next_separator( const char ** symbol_name) {
  return strchr(*symbol_name,SEPARATOR);
}

const char * next_at( const char ** symbol_name) {
  return strchr(*symbol_name,AT);
}

inline bool unequal_indices(const char ** symbol_name,
			    const char * index) {
  return *symbol_name != index;
}

// A valid name matches / [A-Za-z]+ \./
bool parse_NAME(const char ** symbol_name) {

  const char * start = *symbol_name;
  const char * separator_index = 
    next_separator(symbol_name);

  if ( NULL == separator_index )
    return false;

  if ( start == separator_index )
    return false;


  while ( unequal_indices(symbol_name,separator_index) ) {
    if ( alphabetical_char(symbol_name) )
      next_index(symbol_name);
    else {
      set_index(symbol_name,start);
      return false;
    }
  }
  next_index(symbol_name);
  return true;
}

// A valid suffix matches / [A-Za-z]+ @ \0/
bool parse_SUFFIX(const char ** symbol_name) {

  const char * start = *symbol_name;
  const char * at_char_index = 
    next_at(symbol_name);

  // No separator means no name
  if ( NULL == at_char_index )
    return false;

  // Empty names are not allowed
  if ( start == at_char_index )
    return false;

  // Valid names consist of one or more characters from A-Z and a-z
  while ( unequal_indices(symbol_name,at_char_index) ) {
    if ( alphabetical_char(symbol_name) )
      next_index(symbol_name);
    else {
      set_index(symbol_name,start);
      return false;
    }
  }
  next_index(symbol_name);

  if ( not ended(symbol_name) )
    return false;
  return true;

}

// A valid prefix matches /[A-Z] \./
bool parse_PREFIX(const char ** symbol_name ) {

  if ( not at_char(symbol_name) ) {
    return false;
  }
  next_index(symbol_name);

  if ( not upper_case_char(symbol_name) ) {
    return false;
  }
  next_index(symbol_name);

  if ( not separator_char(symbol_name)) {
    return false;
  }
  next_index(symbol_name);

  return true;
}

// A valid flag diacritic matches 
// /@ [A-Z] \. ([A-Za-z]+ \.)? [A-Za-z]+ @/
bool parse_flag_diacritic( const char * symbol_name) {
  
  const char ** symbol_name_address = &symbol_name;
  
  if (not parse_PREFIX(symbol_name_address)) {
    return false;
  }

  parse_NAME(symbol_name_address);

  if (not parse_SUFFIX(symbol_name_address)) {
    return false;
  }

  return true;
}

// Unit test
#ifdef HFST_TESTS
bool _parse_flag_diacritics_C_test( void ) { 

  bool failed = false;

  cout << "Positive tests:\n";

  const char * symbol_name = "@D.NeedNoun.ON@";  
  if ( parse_flag_diacritic(symbol_name) )
    cout << "DIACRITIC: " << symbol_name << "\n";
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }

  symbol_name = "@D.NeedNoun@";  
  if ( parse_flag_diacritic(symbol_name) )
    cout << "DIACRITIC: " << symbol_name << "\n";
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }

  symbol_name = "@A.Z@";  
  if ( parse_flag_diacritic(symbol_name) )
    cout << "DIACRITIC: " << symbol_name << "\n";
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }

  symbol_name = "@A.Z.Z@";  
  if ( parse_flag_diacritic(symbol_name) )
    cout << "DIACRITIC: " << symbol_name << "\n";
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  
  cout << "\nNegative tests:\n";

  symbol_name = "D.NeedNoun.ON@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "D.NeedNoun@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@D.NeedNoun.ON";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@D.NeedNoun";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "D.NeedNoun.ON";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "D.NeedNoun";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@?.NeedNoun.ON@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@D.?.ON@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@D.NeedNoun.?@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@DNeedNoun.ON@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@DD.NeedNoun.ON@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@D..@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n";
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@D..NeedNounON@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    cout << "DIACRITIC: " << symbol_name << "\n"; 
    failed = true;
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "@D.NeedNoun.ON@a";  
  if ( parse_flag_diacritic(symbol_name) ) {
    failed = true;
    cout << "DIACRITIC: " << symbol_name << "\n";
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  symbol_name = "a@D.NeedNoun.ON@";  
  if ( parse_flag_diacritic(symbol_name) ) {
    failed = true;
    cout << "DIACRITIC: " << symbol_name << "\n";
  }
  else {
    cout << "NOT A DIACRITIC: " << symbol_name << "\n";
  }

  return not failed;
}
#endif
