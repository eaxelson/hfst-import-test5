//! @file unicode_portability.h
//! @brief portability header for different unicode implementations used for
//!        HFST corpus tools

#ifndef GUARD_hfst_unicode_portability_h
#define GUARD_hfst_unicode_portability_h 1

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>

//! @brief get hfst_utf8 characters in an array
ssize_t hfst_utf8chars(const char* s, char** chars);

//! @brief validate hfst_utf8 string as specified in Unicode standard
bool hfst_utf8validate(const char* s);

//! @brief convert hfst_utf8 character to its uppercase version
char* hfst_utf8toupper(const char* c);

//! @brief convert hfst_utf8 character to its lowercase version
char* hfst_utf8tolower(const char* c);

//! @brief convert hfst_utf8 character to its titlecase version
char* hfst_utf8totitle(const char* c);

//! @brief test whether UTF-8 character at @a c is uppercase unicode character.
bool hfst_utf8isupper(const char* c);
//! @brief test whether UTF-8 character at @a c is lowercase unicode character.
bool hfst_utf8islower(const char* c);
//! @brief test whether UTF-8 character at @a c is titlecase unicode character.
bool hfst_utf8istitle(const char* c);
//! @brief test whether UTF-8 character at @a c is unicode space character.
bool hfst_utf8isspace(const char* c);
//! @brief test whether UTF-8 character at @a c is alphabetic unicode character.
bool hfst_utf8isalpha(const char* c);
//! @brief test whether UTF-8 character at @a c is unicode digit character.
bool hfst_utf8isdigit(const char* c);
//! @brief test whether UTF-8 character at @a c is alphanumeric unicode
//!     character.
bool hfst_utf8isalnum(const char* c);
//! @brief test whether UTF-8 character at @a c is unicode punctuation.
bool hfst_utf8ispunct(const char* c);
#endif

// vim: set ft=cpp.doxygen:
