#ifndef STRING_MANIPULATION_H
#define STRING_MANIPULATION_H
#include "../config.h"
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream> 

#ifdef TEST
#include <iostream>
#endif

#define ESCAPE_CHAR '%'

// Return new char * of size lgth + 1.
// Ensure that the string is '\0' terminated.
char * new_string(size_t lgth);

// This replaces strdup, which is dangerous
// since it allocates in different ways in linux
// and windows. string_copy always allocates
// using malloc.
char * string_copy(const char * str);

// This replaces strcmp, since percent signs need to
// be skipped
int strcmp_skip_0(const char * str1, 
		  const char * str2);

// Whether str is empty.
bool empty_string(char * str);

// Transform string of form (-)[0-9]+ to an integer.
int str2int(const char * string);

// Precondition text = "...".
//
// Return new char * representing text
// unquoted.
char * unquote(const char * text);

// Return string with unquoted whitespace removed
// from beginning and end.
char * remove_white_space(const char * str);

// Remove white space and unquote.
char * unescape_and_remove_white_space(const char * str);

// Perform equivalent of
// perl s/%(%?)/$1/g'. The % sign
// is the escape character in twolc.
char * unescape(const char * text);

void print_kill_symbol(void);

struct str_cmp {
  bool operator() (const char * str1,
		   const char * str2) const
  {
    return strcmp(str1,str2) < 0;
  }
};

struct lax_str_cmp {
  bool operator() (const char * str1,
		   const char * str2) const
  {
    return strcmp_skip_0(str1,str2) < 0;
  }
};


#endif
