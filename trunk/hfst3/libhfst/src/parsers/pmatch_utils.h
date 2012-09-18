/**
 * @file pmatch_utils.h
 *
 * @brief auxiliary functions for handling Xerox compatible regular expressions
 * (PMATCH).
 */

#ifndef GUARD_pmatch_utils_h
#define GUARD_pmatch_utils_h

#include <map>
#include "HfstTransducer.h"

namespace hfst { namespace pmatch {

extern char* data;
extern char* startptr;
extern size_t len;
extern std::map<std::string,hfst::HfstTransducer*> definitions;
extern HfstTransducer* last_compiled;
extern ImplementationType format;

extern std::map<std::string, hfst::HfstTransducer> named_transducers;
const std::string RC_ENTRY_SYMBOL = "@_PMATCH_RC_ENTRY_@";
const std::string RC_EXIT_SYMBOL = "@_PMATCH_RC_EXIT_@";
const std::string LC_ENTRY_SYMBOL = "@_PMATCH_LC_ENTRY_@";
const std::string LC_EXIT_SYMBOL = "@_PMATCH_LC_EXIT_@";

/**
 * @brief input handling function for flex that parses strings.
 */
int getinput(char *buf, int maxlen);

/**
 * @brief remove percent escaping from given string @a s.
 */
char* strip_percents(const char *s);

/**
 * @brief add percents to string to form valid PMATCH symbol.
 */
char* add_percents(const char* s);

/**
 * @brief get a transition name for use in Ins, RC and LC statements
 */
char* get_Ins_transition(const char *s);
char* get_RC_transition(const char *s);
char* get_LC_transition(const char *s);

/**
 * @brief add special beginning and ending arcs for pmatch compatibility
 */
HfstTransducer * add_pmatch_delimiters(HfstTransducer * regex);

/**
 * @brief concatenate with an appropriate end tag transducer
 */
void add_end_tag(HfstTransducer * regex, std::string tag);

/**
 * @brief find first quoted segment from strign @a s.
 */
char* get_quoted(const char *s);

char* parse_quoted(const char *s);

int* get_n_to_k(const char* s);

double get_weight(const char* s);
/**
 * @brief compile new transducer
 */
HfstTransducer* compile(const std::string& pmatch,
                        std::map<std::string,hfst::HfstTransducer*>& defs,
                        hfst::ImplementationType type);

/** @brief Return a transducer that accepts one utf-8 symbol that is also a
    latin-1 alphabetical character. 
*/
HfstTransducer latin1_alpha_acceptor(
    ImplementationType type = TROPICAL_OPENFST_TYPE);

/** @brief Return a transducer that accepts one arabic numeral character. 
 */
HfstTransducer latin1_numeral_acceptor(
    ImplementationType type = TROPICAL_OPENFST_TYPE);

/** @brief Return a transducer that accepts one utf-8 symbol that is also a
    latin-1 punctuation character.
*/
HfstTransducer latin1_punct_acceptor(
    ImplementationType type = TROPICAL_OPENFST_TYPE);

/** @brief Return a transducer that accepts one utf-8 symbol that is also a
    latin-1 whitespace character.
*/
HfstTransducer latin1_whitespace_acceptor(
    ImplementationType type = TROPICAL_OPENFST_TYPE);

} } // namespaces
#endif
// vim: set ft=cpp.doxygen:
