%module libhfst
%include "std_string.i"
%include "std_set.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%{
#define SWIG_FILE_WITH_INIT
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "HfstDataTypes.h"
#include "HfstFlagDiacritics.h"
#include "hfst_swig_extensions.h"
    %}

namespace std {
%template(StringVector) vector<string>;
%template(OneLevelPath) pair<float, vector<string> >;
%template(OneLevelPathVector) vector<pair<float, vector<string> > >;
%template(OneLevelPaths) set<pair<float, vector<string> > >;
%template(StringFloatVector) vector<pair<string, float> >;
}

namespace hfst
{

/*
 * One of the (apparent) peculiarities of swig is that things break in the
 * wrapper very easily if things aren't defined in precisely the right order,
 * and often it's necessary to forward define things. For that reason there's
 * some overkill in the forward definitions in this file.
 */

enum ImplementationType;
enum PushType;
class HfstInputStream;
class HfstOneLevelPaths;
class HfstTwoLevelPaths;
class HfstOutputStream;
class HfstTransducer;
class FdOperation;
class HfstTokenizer;
class HfstTransducerVector;
class StringSet;
class StringPair;
class StringPairSet;
class StringVector;

std::vector<std::pair <float, std::vector<std::string> > > vectorize(HfstOneLevelPaths * olps);
std::vector<std::pair <float, std::vector<std::string> > > purge_flags(std::vector<std::pair<float, std::vector<std::string> > > olpv);
std::vector<std::pair <std::string, float> > detokenize_vector(OneLevelPathVector olpv);
std::vector<std::pair <std::string, float > > detokenize_paths(HfstOneLevelPaths * olps);

class HfstInputStream{
public:
    HfstInputStream(const std::string & filename);
    HfstInputStream(void);
    void close(void);
    bool is_bad(void);
    bool is_eof(void);
    bool is_good(void);
    ~HfstInputStream(void);
};

class HfstTransducer {
public:
    // First all the constructors
    HfstTransducer();
    HfstTransducer(HfstInputStream & in);
    HfstTransducer(const HfstTransducer &another);
    HfstTransducer(const std::string &input_utf8_str, const std::string &output_utf8_str, const HfstTokenizer &multichar_symbol_tokenizer, ImplementationType type);
    HfstTransducer(const hfst::implementations::HfstBasicTransducer &t, ImplementationType type);
    HfstTransducer(ImplementationType type);
    HfstTransducer(const std::string &symbol, ImplementationType type);
    HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type);
    HfstTransducer(FILE *ifile, ImplementationType type, const std::string &epsilon_symbol);
    
    // Then everything else, in the (alphabetic) order in the API manual
    bool compare(const HfstTransducer &another) const;
    HfstTransducer & compose(const HfstTransducer &another);
    HfstTransducer & compose_intersect(const HfstTransducerVector &v);
    HfstTransducer & concatenate(const HfstTransducer &another);
    HfstTransducer & convert(ImplementationType type, std::string options="");
    HfstTransducer & determinize(void);
    HfstTransducer & disjunct(const HfstTransducer &another);
    void extract_paths(HfstTwoLevelPaths &results, int max_num=-1, int cycles=-1) const;
    void extract_paths_fd(HfstTwoLevelPaths &results, int max_num=-1, int cycles=-1, bool filter_fd=true) const;
    StringSet get_alphabet(void) const;
    std::string get_name(void) const;
    ImplementationType get_type(void) const;
    HfstTransducer & input_project(void);
    HfstTransducer & insert_freely(const StringPair &symbol_pair);
    HfstTransducer & insert_freely(const HfstTransducer &tr);
    void insert_to_alphabet(const std::string &symbol);
    HfstTransducer & intersect(const HfstTransducer &another);
    HfstTransducer & invert(void);
    bool is_cyclic(void) const;
    bool is_lookdown_infinitely_ambiguous(const StringVector &s) const;
    bool is_lookup_infinitely_ambiguous (const StringVector &s) const;
    HfstOneLevelPaths *	lookdown(const StringVector &s, ssize_t limit=-1) const;
    HfstOneLevelPaths *	lookdown_fd(StringVector &s, ssize_t limit=-1) const;
    HfstOneLevelPaths * lookup(const StringVector &s, ssize_t limit=-1) const;
    HfstOneLevelPaths *	lookup(const std::string &s, ssize_t limit=-1) const;
    HfstOneLevelPaths *	lookup(const HfstTokenizer &tok, const std::string &s, ssize_t limit=-1) const;
    HfstOneLevelPaths * lookup_fd(const std::string & s, ssize_t limit = -1) const;
    HfstTransducer & minimize(void);
    HfstTransducer & n_best(unsigned int n);
    HfstTransducer & operator=(const HfstTransducer & another);
    %rename(assign) operator=(const HfstTransducer &another); // to make the function name legal python
    HfstTransducer & optionalize(void);
    HfstTransducer & output_project(void);
    HfstTransducer & priority_union (const HfstTransducer &another);
    HfstTransducer & push_weights(PushType type);
    HfstTransducer & remove_epsilons(void);
    void remove_from_alphabet(const std::string &symbol);
    HfstTransducer & repeat_n(unsigned int n);
    HfstTransducer & repeat_n_minus(unsigned int n);
    HfstTransducer & repeat_n_plus(unsigned int n);
    HfstTransducer & repeat_n_to_k(unsigned int n, unsigned int k);
    HfstTransducer & repeat_plus(void);
    HfstTransducer & repeat_star(void);
    HfstTransducer & reverse (void);
    HfstTransducer & set_final_weights(float weight);
    void set_name(const std::string &name);
    HfstTransducer & substitute(bool(*func)(const StringPair &sp, StringPairSet &sps));
    HfstTransducer & substitute(const StringPair &old_symbol_pair, const StringPairSet &new_symbol_pair_set);
    HfstTransducer & substitute(const StringPair &symbol_pair, HfstTransducer &transducer);
    HfstTransducer & substitute(const std::string &old_symbol, const std::string &new_symbol, bool input_side=true, bool output_side=true);
    HfstTransducer & substitute(const StringPair &old_symbol_pair, const StringPair &new_symbol_pair);
    HfstTransducer & subtract(const HfstTransducer &another);
    HfstTransducer & transform_weights(float(*func)(float));
    void write_in_att_format(const std::string &filename, bool write_weights=true) const;
    void write_in_att_format(FILE *ofile, bool write_weights=true) const;
    virtual ~HfstTransducer(void);
    static HfstTransducer * read_lexc(const std::string &filename, ImplementationType type);
    static HfstTransducer universal_pair(ImplementationType type);
    
};

class HfstOutputStream{
    HfstOutputStream(const std::string & filename, ImplementationType type,
		     bool hfst_format=true);
    HfstOutputStream(ImplementationType type, bool hfst_format=true);
    void close(void);
    HfstOutputStream & operator<<(HfstTransducer & transducer);
    %rename(redirect) operator<<(HfstTransducer & transducer);
    ~HfstOutputStream(void);
};

class FdOperation{
public:
    FdOperation(const hfst::FdOperation&);
    static bool is_diacritic(const std::string& diacritic_str);
};

}

%pythoncode %{
def lookup_clean(transducer, string):
    '''
    fd-lookup string from transducer, purge flags, return
    list of (string, float)
    '''
    return detokenize_paths(purge_flags(transducer.lookup_fd(input)))
%}
