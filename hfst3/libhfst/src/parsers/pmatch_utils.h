/**
 * @file pmatch_utils.h
 *
 * @brief auxiliary functions for handling Xerox compatible regular expressions
 * (PMATCH).
 */

#ifndef GUARD_pmatch_utils_h
#define GUARD_pmatch_utils_h

#include <map>
#include <set>
#include "HfstTransducer.h"

namespace hfst { namespace pmatch {

extern char* data;
extern char* startptr;
extern size_t len;
extern std::map<std::string,hfst::HfstTransducer*> definitions;
extern std::set<std::string> inserted_transducers;
extern std::set<std::string> unsatisfied_insertions;
extern HfstTransducer* last_compiled;
extern ImplementationType format;

struct PmatchUtilityTransducers;
extern PmatchUtilityTransducers utils;
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
std::map<std::string, HfstTransducer*>
    compile(const std::string& pmatch,
            std::map<std::string,hfst::HfstTransducer*>& defs,
            hfst::ImplementationType type,
            bool verbose);

void print_size_info(HfstTransducer * net);

/**
 * @brief Given a text file, read it line by line and return an acceptor
 * of a disjunction of the lines
 */
HfstTransducer * read_text(char * filename,
                           ImplementationType type = TROPICAL_OPENFST_TYPE);

/** @brief Return a transducer that accepts a single string from an array of
 *  char *. 
 */

/* First some magic templates for compile-time length checking */

template<typename T, size_t N>
    HfstTransducer * acceptor_from_cstr(
        T (&strings)[N],
        ImplementationType type) {
    HfstTokenizer tok;
    HfstTransducer * retval = new HfstTransducer(type);
    for (size_t i = 0; i < N; ++i) {
        retval->disjunct(HfstTransducer(strings[i], tok, type));
    }
    return retval;
}

template<typename T, size_t N>
    size_t array_len(T(&strings)[N]) {
    return N;
}

    static const char * latin1_upper[] =
    {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
        "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "À", "Á",
        "Â", "Ã", "Ä", "Å", "Æ", "Ç", "È", "É", "Ê", "Ë", "Ì", "Í", "Î", "Ï",
        "Ð", "Ñ", "Ò", "Ó", "Ô", "Õ", "Ö", "Ø", "Ù", "Ú", "Û", "Ü", "Ý", "Þ",
        "ß"
    };
    
    static const char * latin1_lower[] =
    {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n",
        "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "à", "á",
        "â", "ã", "ä", "å", "æ", "ç", "è", "é", "ê", "ë", "ì", "í", "î", "ï",
        "ð", "ñ", "ò", "ó", "ô", "õ", "ö", "ø", "ù", "ú", "û", "ü", "ý", "þ",
        "ß"
    };

    static const char * combining_accents[] =
    {
        // Combining accents: grave, acute, circumflex, tilde, overline,
        // diaresis, cedilla
        "\u0300", "\u0301", "\u0302", "\u0303", "\u0305", "\u0308", "\u0327",
        // Small solidus and large combining solidus
        "\u0337", "\u0338"
    };
    
    static const char * latin1_punct[] =
    {
        "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".",
        "/", ":", ";", "<", "=", ">", "?", "@", "[", "\\", "]", "^", "_",
        "{", "|", "}", "~", "`", "´", "¡", "«", "»", "¿"
    };
    
    static const char * latin1_whitespace[] =
    {
        " ", "\n", "\t",
        // Non-breaking space, CR
        "\u00A0",
        "\r",
        // punctuation space, thin space, line separator, par separator
        "\u2008", "\u2009", "\u2028", "\u2029"
    };


struct PmatchUtilityTransducers
{
    PmatchUtilityTransducers();
    ~PmatchUtilityTransducers();
    /**
     * Character class acceptors
     */

    const HfstTransducer * latin1_acceptor;
    const HfstTransducer * latin1_alpha_acceptor;
    const HfstTransducer * latin1_lowercase_acceptor;
    const HfstTransducer * latin1_uppercase_acceptor;
    const HfstTransducer * combining_accent_acceptor;
    const HfstTransducer * latin1_numeral_acceptor;
    const HfstTransducer * latin1_punct_acceptor;
    const HfstTransducer * latin1_whitespace_acceptor;
    const HfstTransducer * capify;
    const HfstTransducer * lowerfy;
    
    HfstTransducer * make_latin1_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
    HfstTransducer * make_latin1_alpha_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
    HfstTransducer * make_latin1_lowercase_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
    HfstTransducer * make_latin1_uppercase_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
    HfstTransducer * make_combining_accent_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
/** @brief Return a transducer that accepts one arabic numeral character. 
 */
    HfstTransducer * make_latin1_numeral_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
/** @brief Return a transducer that accepts one utf-8 symbol that is also a
 *  latin-1 punctuation character.
 */
    HfstTransducer * make_latin1_punct_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
/** @brief Return a transducer that accepts one utf-8 symbol that is also a
 *  latin-1 whitespace character.
 */

    HfstTransducer * make_latin1_whitespace_acceptor(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
    HfstTransducer * make_lowerfy(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
    HfstTransducer * make_capify(
        ImplementationType type = TROPICAL_OPENFST_TYPE);
    
    HfstTransducer * optcap(HfstTransducer & t);
    HfstTransducer * tolower(HfstTransducer & t);
    HfstTransducer * toupper(HfstTransducer & t);
};


} } // namespaces
#endif
// vim: set ft=cpp.doxygen:
