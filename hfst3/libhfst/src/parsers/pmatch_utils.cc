/**
 * @file pmatch.cc
 *
 * @brief implements pmatch routines.
 */

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "pmatch_utils.h"
#include "HfstTransducer.h"

using std::string;
using std::map;

extern char* pmatchtext;
extern int pmatchparse();
extern int pmatchnerrs;

int
pmatcherror(const char *msg)
{
#ifndef NDEBUG
    fprintf(stderr, "*** pmatch parsing failed: %s\n", msg);
    if (strlen(hfst::pmatch::data) < 60)
    {
        fprintf(stderr, "***    parsing %s [near %s]\n", hfst::pmatch::data,
                pmatchtext);
    }
    else
    {
        fprintf(stderr, "***    parsing %60s [near %s]...\n", 
                hfst::pmatch::data, pmatchtext);
    }
#endif
    return 0;
}

namespace hfst 
{ 
namespace pmatch 
{

char* data;
std::map<std::string,hfst::HfstTransducer*> definitions;
char* startptr;
hfst::HfstTransducer* last_compiled;
hfst::ImplementationType format;
size_t len;

std::map<std::string, hfst::HfstTransducer> named_transducers;
PmatchUtilityTransducers utils;


int*
get_n_to_k(const char* s)
{
    int* rv = static_cast<int*>(malloc(sizeof(int)*2));
    char *endptr;
    char *finalptr;
    if (*(s + 1) == '{')
      {
        rv[0] = strtol(s + 2, &endptr, 10);
        rv[1] = strtol(endptr + 1, &finalptr, 10);
        assert(*finalptr == '}');
      }
    else
      {
        rv[0] = strtol(s + 1, &endptr, 10);
        rv[1] = strtol(endptr + 1, &finalptr, 10);
        assert(*finalptr == '\0');
      }
    return rv;
}



int
getinput(char *buf, int maxlen)
{
    int retval = 0;
    if ( maxlen > len ) {
        maxlen = len;
    }
    memcpy(buf, data, maxlen);
    data += maxlen;
    len -= maxlen;
    retval = maxlen;
    return retval;
}

char*
strip_percents(const char *s)
{
    const char *c = s;
    char *stripped = (char*)calloc(sizeof(char),strlen(s)+1);
    size_t i = 0;
    while (*c != '\0')
    {
        if (*c == '%')
        {
            if (*(c + 1) == '\0')
            {
                break;
            }
            else
            {
                stripped[i] = *(c + 1);
                i++;
                c += 2;
            }
        }
        else
        {
            stripped[i] = *c;
            i++;
            c++;
        }
    }
    stripped[i] = '\0';
    return stripped;
}

char*
add_percents(const char *s)
  {
    char* ns = static_cast<char*>(malloc(sizeof(char)*strlen(s)*2+1));
    char* p = ns;
    while (*s != '\0')
      {
        if ((*s == '@') || (*s == '-') || (*s == ' ') || (*s == '|') ||
            (*s == '!') || (*s == ':') || (*s == ';') || (*s == '0') ||
            (*s == '\\') || (*s == '&') || (*s == '?') || (*s == '$') ||
            (*s == '+') || (*s == '*') || (*s == '/') || (*s == '/') ||
            (*s == '_') || (*s == '(') || (*s == ')') || (*s == '{') ||
            (*s == '}') || (*s == '[') || (*s == ']'))
          {
            *p = '%';
            p++;
          }
        *p = *s;
        p++;
        s++;
      }
    *p = '\0';
    return ns;
  }

char *
get_Ins_transition(const char *s)
{
    char* rv = static_cast<char*>(malloc(sizeof(char)*(strlen(s) + 4 + 1)));
    rv = strcpy(rv, "@I.");
    rv = strcat(rv, s);
    rv = strcat(rv, "@");
    return rv;
}

char *
get_RC_transition(const char *s)
{
    char* rv = static_cast<char*>(malloc(sizeof(char)*(strlen(s) + 5 + 1)));
    rv = strcpy(rv, "@RC.");
    rv = strcat(rv, s);
    rv = strcat(rv, "@");
    return rv;
}

char *
get_LC_transition(const char *s)
{
    char* rv = static_cast<char*>(malloc(sizeof(char)*(strlen(s) + 5 + 1)));
    rv = strcpy(rv, "@LC.");
    rv = strcat(rv, s);
    rv = strcat(rv, "@");
    return rv;
}

HfstTransducer * add_pmatch_delimiters(HfstTransducer * regex)
{
    HfstTransducer * delimited_regex = new HfstTransducer(hfst::internal_epsilon,
                                                          "@_PMATCH_ENTRY_@",
                                                          regex->get_type());
    delimited_regex->concatenate(*regex);
    delimited_regex->concatenate(HfstTransducer(hfst::internal_epsilon,
                                                "@_PMATCH_EXIT_@",
                                                regex->get_type()));
    delete regex;
    delimited_regex->minimize();
    return delimited_regex;
}

void add_end_tag(HfstTransducer * regex, std::string tag)
{
    HfstTransducer end_tag(hfst::internal_epsilon,
                           "@_PMATCH_ENDTAG_" + tag + "_@",
                           regex->get_type());
    regex->concatenate(end_tag);
}

char *
get_quoted(const char *s)
{
    const char *qstart = strchr((char*) s, '"') + 1;
    const char *qend = strrchr((char*) s, '"');
    char* qpart = strdup(qstart);
    *(qpart+ (size_t) (qend - qstart)) = '\0';
    return qpart;
}

char*
parse_quoted(const char *s)
{
    char* quoted = get_quoted(s);
    // Mysteriously, when the quoted string is 24 + n * 16 bytes in length, an
    // extra byte is needed for rv.
    char* rv = static_cast<char*>(malloc(sizeof(char)*(strlen(quoted) + 1)));
    char* p = quoted;
    char* r = rv;
    while (*p != '\0')
      {
        if (*p != '\\')
          {
            *r = *p;
            ++r;
            ++p;
          }
        else if (*p == '\\')
          {
            switch (*(p + 1))
              {
              case '0':
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
                fprintf(stderr, "*** PMATCH unimplemented: "
                        "parse octal escape in %s", p);
                *r = '\0';
                p = p + 5;
                break;
              case 'a':
                *r = '\a';
                r++;
                p = p + 2;
                break;
              case 'b':
                *r = '\b';
                r++;
                p = p + 2;
                break;
              case 'f':
                *r = '\f';
                r++;
                p = p + 2;
                break;
              case 'n':
                *r = '\n';
                r++;
                p = p + 2;
                break;
              case 'r':
                *r = '\r';
                r++;
                p = p + 2;
                break;
              case 't':
                *r = '\t';
                r++;
                p = p + 2;
                break;
              case 'u':
                fprintf(stderr, "Unimplemented: parse unicode escapes in %s", p);
                *r = '\0';
                r++;
                p = p + 6;
                break;
              case 'v':
                *r = '\v';
                r++;
                p = p + 2;
                break;
              case 'x':
                  {
                    char* endp;
                    int i = strtol(p + 2, &endp, 10);
                    if ( 0 < i && i <= 127)
                      {
                        *r = static_cast<char>(i);
                      }
                    else
                      {
                        fprintf(stderr, "*** PMATCH unimplemented: "
                                "parse \\x%d\n", i);
                        *r = '\0';
                      }
                    r++;
                    assert(endp != p);
                    p = endp;
                   break;
                }
              case '\0':
                fprintf(stderr, "End of line after \\ escape\n");
                *r = '\0';
                r++;
                p++;
                break;
              default:
                *r = *(p + 1);
                r++;
                p += 2;
                break;
              }
          }
      }
    *r = '\0';
    free(quoted);
    return rv;
}

double
get_weight(const char *s)
{
    double rv = -3.1415;
    const char* weightstart = s;
    while ((*weightstart != '\0') && 
           ((*weightstart == ' ') || (*weightstart == '\t') ||
            (*weightstart == ';')))
    {
        weightstart++;
    }
    char* endp;
    rv = strtod(weightstart, &endp);
    assert(endp != weightstart);
    return rv;
}

HfstTransducer*
compile(const string& pmatch, map<string,HfstTransducer*>& defs,
        ImplementationType impl)
{
    // lock here?
    data = strdup(pmatch.c_str());
    startptr = data;
    len = strlen(data);
    definitions = defs;
    format = impl;
    pmatchparse();
    free(startptr);
    data = 0;
    len = 0;
    if (pmatchnerrs == 0)
      {
        HfstTransducer* rv = new HfstTransducer(*last_compiled);
        delete last_compiled;
        return rv;
      }
    else
      {
        return new HfstTransducer(impl);
      }
}

HfstTransducer * read_text(char * filename, ImplementationType type)
{
    std::ifstream infile;
    std::string line;
    infile.open(filename);
    HfstTokenizer tok;
    HfstTransducer * retval = new HfstTransducer(type);
    if(!infile.good()) {
        std::cerr << "Pmatch: could not open text file " << filename <<
            " for reading\n";
    } else {
        while(infile.good()) {
            std::getline(infile, line);
            if(!line.empty()) {
                retval->disjunct(HfstTransducer(line, tok, type));
            }
        }
    }
    infile.close();
    return retval;
}

PmatchUtilityTransducers::PmatchUtilityTransducers(void)
{
    latin1_acceptor = make_latin1_acceptor();
    latin1_alpha_acceptor = make_latin1_alpha_acceptor();
    latin1_lowercase_acceptor = make_latin1_lowercase_acceptor();
    latin1_uppercase_acceptor = make_latin1_uppercase_acceptor();
    combining_accent_acceptor = make_combining_accent_acceptor();
    latin1_numeral_acceptor = make_latin1_numeral_acceptor();
    latin1_punct_acceptor = make_latin1_punct_acceptor();
    latin1_whitespace_acceptor = make_latin1_whitespace_acceptor();
    lowerfy = make_lowerfy();
    capify = make_capify();
}

PmatchUtilityTransducers::~PmatchUtilityTransducers(void)
{
    delete latin1_acceptor;
    delete latin1_alpha_acceptor;
    delete latin1_lowercase_acceptor;
    delete latin1_uppercase_acceptor;
    delete combining_accent_acceptor;
    delete latin1_numeral_acceptor;
    delete latin1_punct_acceptor;
    delete latin1_whitespace_acceptor;
    delete lowerfy;
    delete capify;
}

HfstTransducer * PmatchUtilityTransducers::make_latin1_acceptor(ImplementationType type)
  {
      HfstTransducer * retval = make_latin1_alpha_acceptor();
      HfstTransducer * tmp = make_latin1_numeral_acceptor();
      retval->disjunct(*tmp); delete tmp;
      tmp = make_latin1_punct_acceptor(); retval->disjunct(*tmp); delete tmp;
      tmp = make_latin1_whitespace_acceptor(); retval->disjunct(*tmp); delete tmp;
      return retval;
  }

HfstTransducer * PmatchUtilityTransducers::make_latin1_alpha_acceptor(ImplementationType type)
  {
      HfstTransducer * retval = make_latin1_lowercase_acceptor();
      HfstTransducer * tmp = make_latin1_uppercase_acceptor();
      retval->disjunct(*tmp); delete tmp;
      return retval;
  }

HfstTransducer * PmatchUtilityTransducers::make_latin1_lowercase_acceptor(ImplementationType type)
  {
      HfstTransducer * retval = acceptor_from_cstr(latin1_lower, type);
      HfstTransducer * tmp = make_combining_accent_acceptor();
      retval->disjunct(*tmp); delete tmp;
      return retval;
  }

HfstTransducer * PmatchUtilityTransducers::make_latin1_uppercase_acceptor(ImplementationType type)
  {
      HfstTransducer * retval = acceptor_from_cstr(latin1_upper, type);
      HfstTransducer * tmp = make_combining_accent_acceptor();
      retval->disjunct(*tmp); delete tmp;
      return retval;
  }

HfstTransducer * PmatchUtilityTransducers::make_combining_accent_acceptor(ImplementationType type)
  {
      return acceptor_from_cstr(combining_accents, type);
  }

HfstTransducer * PmatchUtilityTransducers::make_latin1_numeral_acceptor(ImplementationType type)
  {
      HfstTransducer * retval = new HfstTransducer(type);
      const std::string num =
          "0123456789";
      for (std::string::const_iterator it = num.begin(); it != num.end();
           ++it) {
          retval->disjunct(HfstTransducer(std::string(1, *it), type));
      }
      return retval;
  }

HfstTransducer * PmatchUtilityTransducers::make_latin1_punct_acceptor(ImplementationType type)
  {
      return acceptor_from_cstr(latin1_punct, type);
  }

HfstTransducer * PmatchUtilityTransducers::make_latin1_whitespace_acceptor(ImplementationType type)
{
    return acceptor_from_cstr(latin1_whitespace, type);
}

HfstTransducer * PmatchUtilityTransducers::make_capify(ImplementationType type)
{
    HfstTransducer * retval = new HfstTransducer(type);
    HfstTokenizer tok;
    for (size_t i = 0; i < array_len(latin1_upper); ++i) {
        retval->disjunct(HfstTransducer(latin1_lower[i], latin1_upper[i],
                                        tok, type));
    }
    return retval;
}

HfstTransducer * PmatchUtilityTransducers::make_lowerfy(ImplementationType type)
{
    HfstTransducer * retval = new HfstTransducer(type);
    HfstTokenizer tok;
    for (size_t i = 0; i < array_len(latin1_upper); ++i) {
        retval->disjunct(HfstTransducer(latin1_upper[i], latin1_lower[i],
                                        tok, type));
    }
    return retval;
}

HfstTransducer * PmatchUtilityTransducers::optcap(HfstTransducer & t)
{
    HfstTokenizer tok;
    HfstTransducer lower_to_upper_or_vice_versa(*capify);
    lower_to_upper_or_vice_versa.disjunct(*lowerfy);
    lower_to_upper_or_vice_versa.concatenate(HfstTransducer::identity_pair(
                                                 t.get_type()).repeat_star());
    HfstTransducer * retval = new HfstTransducer(t);
    retval->compose(lower_to_upper_or_vice_versa);
    retval->output_project();
    retval->disjunct(t);
    return retval;
}

HfstTransducer * PmatchUtilityTransducers::tolower(HfstTransducer & t)
{
    HfstTokenizer tok;
    HfstTransducer lowercase(*lowerfy);
    lowercase.disjunct(*latin1_numeral_acceptor);
    lowercase.disjunct(*latin1_punct_acceptor);
    lowercase.disjunct(*latin1_whitespace_acceptor);
    HfstTransducer * retval = new HfstTransducer(t);
    retval->compose(lowercase.repeat_star());
    retval->output_project();
    return retval;
}

HfstTransducer * PmatchUtilityTransducers::toupper(HfstTransducer & t)
{
    HfstTokenizer tok;
    HfstTransducer uppercase(*capify);
    uppercase.disjunct(*latin1_numeral_acceptor);
    uppercase.disjunct(*latin1_punct_acceptor);
    uppercase.disjunct(*latin1_whitespace_acceptor);
    HfstTransducer * retval = new HfstTransducer(t);
    retval->compose(uppercase.repeat_star());
    retval->output_project();
    return retval;
}


} }
