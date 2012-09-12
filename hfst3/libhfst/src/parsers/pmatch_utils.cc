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
    char* rv = static_cast<char*>(malloc(sizeof(char)*strlen(quoted)));
    char* p = quoted;
    char* r = rv;
    while (*p != '\0')
      {
        if (*p != '\\')
          {
            *r = *p;
            r++;
            p++;
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

  HfstTransducer latin1_alpha_acceptor(ImplementationType type)
  {
      HfstTokenizer tok;
      HfstTransducer retval(type);
      const std::string asciialpha =
          "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
      for (std::string::const_iterator it = asciialpha.begin();
           it != asciialpha.end(); ++it) {
          retval.disjunct(HfstTransducer(std::string(1, *it), type));
      }
      retval.disjunct(HfstTransducer("à", tok, type));
      retval.disjunct(HfstTransducer("á", tok, type));
      retval.disjunct(HfstTransducer("â", tok, type));
      retval.disjunct(HfstTransducer("ã", tok, type));
      retval.disjunct(HfstTransducer("ä", tok, type));
      retval.disjunct(HfstTransducer("å", tok, type));
      retval.disjunct(HfstTransducer("æ", tok, type));
      retval.disjunct(HfstTransducer("ç", tok, type));
      retval.disjunct(HfstTransducer("è", tok, type));
      retval.disjunct(HfstTransducer("é", tok, type));
      retval.disjunct(HfstTransducer("ê", tok, type));
      retval.disjunct(HfstTransducer("ë", tok, type));
      retval.disjunct(HfstTransducer("ì", tok, type));
      retval.disjunct(HfstTransducer("í", tok, type));
      retval.disjunct(HfstTransducer("î", tok, type));
      retval.disjunct(HfstTransducer("ï", tok, type));
      retval.disjunct(HfstTransducer("ð", tok, type));
      retval.disjunct(HfstTransducer("ñ", tok, type));
      retval.disjunct(HfstTransducer("ò", tok, type));
      retval.disjunct(HfstTransducer("ó", tok, type));
      retval.disjunct(HfstTransducer("ô", tok, type));
      retval.disjunct(HfstTransducer("õ", tok, type));
      retval.disjunct(HfstTransducer("ö", tok, type));
      retval.disjunct(HfstTransducer("ø", tok, type));
      retval.disjunct(HfstTransducer("ù", tok, type));
      retval.disjunct(HfstTransducer("ú", tok, type));
      retval.disjunct(HfstTransducer("û", tok, type));
      retval.disjunct(HfstTransducer("ü", tok, type));
      retval.disjunct(HfstTransducer("ý", tok, type));
      retval.disjunct(HfstTransducer("þ", tok, type));
      retval.disjunct(HfstTransducer("ÿ", tok, type));
      retval.disjunct(HfstTransducer("À", tok, type));
      retval.disjunct(HfstTransducer("Á", tok, type));
      retval.disjunct(HfstTransducer("Â", tok, type));
      retval.disjunct(HfstTransducer("Ã", tok, type));
      retval.disjunct(HfstTransducer("Ä", tok, type));
      retval.disjunct(HfstTransducer("Å", tok, type));
      retval.disjunct(HfstTransducer("Æ", tok, type));
      retval.disjunct(HfstTransducer("Ç", tok, type));
      retval.disjunct(HfstTransducer("È", tok, type));
      retval.disjunct(HfstTransducer("É", tok, type));
      retval.disjunct(HfstTransducer("Ê", tok, type));
      retval.disjunct(HfstTransducer("Ë", tok, type));
      retval.disjunct(HfstTransducer("Ì", tok, type));
      retval.disjunct(HfstTransducer("Í", tok, type));
      retval.disjunct(HfstTransducer("Î", tok, type));
      retval.disjunct(HfstTransducer("Ï", tok, type));
      retval.disjunct(HfstTransducer("Ð", tok, type));
      retval.disjunct(HfstTransducer("Ñ", tok, type));
      retval.disjunct(HfstTransducer("Ò", tok, type));
      retval.disjunct(HfstTransducer("Ó", tok, type));
      retval.disjunct(HfstTransducer("Ô", tok, type));
      retval.disjunct(HfstTransducer("Õ", tok, type));
      retval.disjunct(HfstTransducer("Ö", tok, type));
      retval.disjunct(HfstTransducer("Ø", tok, type));
      retval.disjunct(HfstTransducer("Ù", tok, type));
      retval.disjunct(HfstTransducer("Ú", tok, type));
      retval.disjunct(HfstTransducer("Û", tok, type));
      retval.disjunct(HfstTransducer("Ü", tok, type));
      retval.disjunct(HfstTransducer("Ý", tok, type));
      retval.disjunct(HfstTransducer("Þ", tok, type));
      retval.disjunct(HfstTransducer("ß", tok, type));
// Combining accents: grave, acute, circumflex, tilde, overline,
      // diaresis, cedilla
      retval.disjunct(HfstTransducer("\u0300", tok, type));
      retval.disjunct(HfstTransducer("\u0301", tok, type));
      retval.disjunct(HfstTransducer("\u0302", tok, type));
      retval.disjunct(HfstTransducer("\u0303", tok, type));
      retval.disjunct(HfstTransducer("\u0305", tok, type));
      retval.disjunct(HfstTransducer("\u0308", tok, type));
      retval.disjunct(HfstTransducer("\u0327", tok, type));
      // Small solidus and large combining solidus
      retval.disjunct(HfstTransducer("\u0337", tok, type));
      retval.disjunct(HfstTransducer("\u0338", tok, type));
      return retval;
  }
  
  HfstTransducer latin1_numeral_acceptor(ImplementationType type)
  {
      HfstTransducer retval(type);
      const std::string num =
          "0123456789";
      for (std::string::const_iterator it = num.begin(); it != num.end();
           ++it) {
          retval.disjunct(HfstTransducer(std::string(1, *it), type));
      }
      return retval;
  }

  HfstTransducer latin1_punct_acceptor(ImplementationType type)
  {
      HfstTokenizer tok;
      HfstTransducer retval(type);
      const std::string punct =
          "!\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~";
      for (std::string::const_iterator it = punct.begin(); it != punct.end();
           ++it) {
          retval.disjunct(HfstTransducer(std::string(1, *it), type));
      }
      retval.disjunct(HfstTransducer("`", tok, type));
      retval.disjunct(HfstTransducer("´", tok, type));
      retval.disjunct(HfstTransducer("¡", tok, type));
      retval.disjunct(HfstTransducer("«", tok, type));
      retval.disjunct(HfstTransducer("»", tok, type));
      retval.disjunct(HfstTransducer("¿", tok, type));
      return retval;
  }

HfstTransducer latin1_whitespace_acceptor(ImplementationType type)
{
    HfstTokenizer tok;
    HfstTransducer retval(type);
    retval.disjunct(HfstTransducer(" ", tok, type));
      retval.disjunct(HfstTransducer("\n", tok, type));
      retval.disjunct(HfstTransducer("\t", tok, type));
      // Non-breaking space, CR
      retval.disjunct(HfstTransducer("\u00A0", tok, type));
      retval.disjunct(HfstTransducer("\r", tok, type));
      // punctuation space, thin space, line separator, par separator
      retval.disjunct(HfstTransducer("\u2008", tok, type));
      retval.disjunct(HfstTransducer("\u2009", tok, type));
      retval.disjunct(HfstTransducer("\u2028", tok, type));
      retval.disjunct(HfstTransducer("\u2029", tok, type));
      return retval;
}


} }
