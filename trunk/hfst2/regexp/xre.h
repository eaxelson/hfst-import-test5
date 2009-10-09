/**
 * @file xre.h
 *
 * @brief auxiliary functions for handlin Xerox compatible regular expressions
 * (XRE).
 */

#ifndef GUARD_xre_h
#define GUARD_xre_h 1

#include <map>
#include "../hfst.h"

#define EPSILON_KEY 0
#define ANY_KEY 65535

extern char *_xre_data;
extern size_t _xre_len;
extern HFST::TransducerHandle _xre_transducer_;
extern HFST::KeySet* _xre_keys;
extern HFST::KeyPairSet* _xre_negation_pi;
extern HFST::KeyPairSet* _xre_creation_pi;
extern HFST::KeyTable* _xre_key_table;
extern bool _xre_initialised;
extern std::map<HFST::Key,HFST::TransducerHandle> _xre_definitions;


//! @brief weighted must be defined for this to work
extern bool _xre_weighted;

/**
 * @brief input handling function for flex that parses strings.
 */
int xre_getinput(char *buf, int maxlen);

/**
 * @brief remove percent escaping from given string @a s.
 */
char* xre_strip_percents(const char *s);

/**
 * @brief find first quoted segment from strign @a s.
 */
char* xre_get_quoted(const char *s);

//! @brief sets string for next regular expression.
void _xre_new_data(const char* xre);

//! @brief initialises XRE data structures with existing data
void _xre_initialise(const char* xre, HFST::KeyPairSet* negation_pi,
					 HFST::KeyPairSet* creation_pi, HFST::KeyTable* keytable,
					 bool weighted);
//! @brief initialises XRE data structures empty
void _xre_initialise();

HFST::Key _xre_string_to_key(char *s);
void _xre_new_key(HFST::Key k);
void _xre_new_transducer(HFST::TransducerHandle t);
HFST::TransducerHandle _xre_make_key_pair(HFST::Key k1, HFST::Key k2);
#endif
// vim: set ft=cpp.doxygen:
