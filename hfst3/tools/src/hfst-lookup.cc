//! @file hfst-lookup.cc
//!
//! @brief Transducer lookdown command line tool
//!
//! @author HFST Team


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <getopt.h>



#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "HfstTransducer.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::HfstLookupPath;
using hfst::HfstLookupPaths;
using hfst::exceptions::NotTransducerStreamException;

// add tools-specific variables here
static char* lookup_file_name;
static FILE* lookup_file;
static size_t linen = 0;
static bool lookup_given = false;
static size_t infinite_cutoff = 5;

enum lookup_input_format
{
  UTF8_TOKEN_INPUT,
  SPACE_SEPARATED_TOKEN_INPUT,
  APERTIUM_INPUT
};

enum lookup_output_format
{
  XEROX_OUTPUT,
  CG_OUTPUT,
  APERTIUM_OUTPUT
};

static lookup_input_format input_format = UTF8_TOKEN_INPUT;
static lookup_output_format output_format = XEROX_OUTPUT;

// the formats for lookup cases go like so:
//  BEGIN LOOKUP LOOKUP LOOKUP... END

// for standard case of more than 0 and less than infinite results:
static char* begin_setf = 0; // print before set of lookups
static char* lookupf = 0; // print before each lookup
static char* end_setf = 0; // print for each lookup
// when there are 0 results:
static char* empty_begin_setf = 0; // print before empty set of results
static char* empty_lookupf = 0; // print in place of empty lookup result
static char* empty_end_setf = 0; // print in end of empty set of results
// when there are 0 results and token is unrecognizable by analyser:
static char* unknown_begin_setf = 0; // print before unknown set of results
static char* unknown_lookupf = 0; // print in place of unknown result
static char* unknown_end_setf = 0; // print in end of set of unknown results
// when there are infinite results:
static char* infinite_begin_setf = 0; // print before infinite set of results
static char* infinite_lookupf = 0; // print in place of infinite results
static char* infinite_end_setf = 0; // print in end of infinite results

static bool print_statistics = false;

// predefined formats
// Xerox:
// word     word N SG
// word     word V PRES
static const char* XEROX_BEGIN_SETF = "";
static const char* XEROX_LOOKUPF = "%i\t%l%n";
static const char* XEROX_END_SETF = "%n";
// notaword notaword+?
static const char* XEROX_EMPTY_BEGIN_SETF = "";
static const char* XEROX_EMPTY_LOOKUPF = "%i\t%i\t+?%n";
static const char* XEROX_EMPTY_END_SETF = "%n";
// ¶    ¶+?
static const char* XEROX_UNKNOWN_BEGIN_SETF = "";
static const char* XEROX_UNKNOWN_LOOKUPF = "%i\t%i\t+?%n";
static const char* XEROX_UNKNOWN_END_SETF = "%n";
// 0    0 NUM SG
// 0    [...cyclic...]
static const char* XEROX_INFINITE_BEGIN_SETF = "";
static const char* XEROX_INFINITE_LOOKUPF = "%i\t%l%n";
static const char* XEROX_INFINITE_END_SETF = "%i\t[...cyclic...]%n%n";
// CG:
// "<word>"
//      "word"  N SG
//      "word"  V PRES
static const char* CG_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_LOOKUPF = "\t\"%b\"%a%n";
static const char* CG_END_SETF = "%n";
// "<notaword>"
//      "notaword" ?
static const char* CG_EMPTY_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_EMPTY_LOOKUPF = "\t\"%i\" ?%n";
static const char* CG_EMPTY_END_SETF = "%n";
// "<¶>"
//      "¶" ?
static const char* CG_UNKNOWN_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_UNKNOWN_LOOKUPF = "\t\"%i\"\t ?%n";
static const char* CG_UNKNOWN_END_SETF = "%n";
// "<0>"
//      "0" NUM SG
//      "0" [...cyclic...]
static const char* CG_INFINITE_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_INFINITE_LOOKUPF = "\t\"%b\"%a%n";
static const char* CG_INFINITE_END_SETF = "\t\"%i\"...cyclic...%n%n";
// apertium:
// ^word/word N SG/word V PRES$[apertium superblank markup]
static const char* APERTIUM_BEGIN_SETF = "^%i";
static const char* APERTIUM_LOOKUPF = "/%l";
static const char* APERTIUM_END_SETF = "$%m%n";
// ^notaword/*notaword$[apertium superblank markup]
static const char* APERTIUM_EMPTY_BEGIN_SETF = "^%i";
static const char* APERTIUM_EMPTY_LOOKUPF = "/*%i";
static const char* APERTIUM_EMPTY_END_SETF = "$%m%n";
// ¶[apertium superblank markup]
static const char* APERTIUM_UNKNOWN_BEGIN_SETF = " ";
static const char* APERTIUM_UNKNOWN_LOOKUPF = "%i%m";
static const char* APERTIUM_UNKNOWN_END_SETF = " ";
// ^0/0 NUM SG/...$
static const char* APERTIUM_INFINITE_BEGIN_SETF = "^%i";
static const char* APERTIUM_INFINITE_LOOKUPF = "/%l";
static const char* APERTIUM_INFINITE_END_SETF = "/...$%n";

// statistic counting
static unsigned long inputs = 0;
static unsigned long no_analyses = 0;
static unsigned long analysed = 0;
static unsigned long analyses = 0;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "perform transducer lookup (apply)\n"
        "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    fprintf(message_out, "Lookup options:\n"
            "  -I, --input-strings=SFILE        Read lookup strings from SFILE\n"
            "  -O, --output-format=OFORMAT      Use OFORMAT printing results sets\n"
            "  -F, --input-format=IFORMAT       Use IFORMAT parsing input\n"
            "  -x, --statistics                 Print statistics\n");
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out, "OFORMAT is one of {xerox,cg,apertium}, "
           "xerox being default\n"
           "IFORMAT is one of {text,spaced,apertium}, "
           "default being text, unless OFORMAT is apertium\n");
    fprintf(message_out, "\n");
    // bug report address
    print_report_bugs();
    // external docs
    print_more_info();
}

int
parse_options(int argc, char** argv)
{
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
        {
        HFST_GETOPT_COMMON_LONG,
        HFST_GETOPT_UNARY_LONG,
          // add tool-specific options here
            {"input-strings", required_argument, 0, 'I'},
            {"output-format", required_argument, 0, 'O'},
            {"input-format", required_argument, 0, 'F'},
            {"statistics", no_argument, 0, 'x'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "I:O:F:x",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
          // add tool-specific cases here
        case 'I':
            lookup_file_name = hfst_strdup(optarg);
            lookup_file = hfst_fopen(lookup_file_name, "r");
            lookup_given = true;
            break;
        case 'O':
            if (strcmp(optarg, "xerox") == 0)
            {
              output_format = XEROX_OUTPUT;
            }
            else if (strcmp(optarg, "cg") == 0)
            {
              output_format = CG_OUTPUT;
            }
            else if (strcmp(optarg, "apertium") == 0)
            {
              output_format = APERTIUM_OUTPUT;
              input_format = APERTIUM_INPUT;
            }
            else
              {
                error(EXIT_FAILURE, 0,
                      "Unknown output format %s; valid values are: "
                      "xerox, cg, apertium\n", optarg);
                return EXIT_FAILURE;
              }
            break;
        case 'F':
            if (strcmp(optarg, "text"))
              {
                input_format = UTF8_TOKEN_INPUT;
              }
            else if (strcmp(optarg, "spaced"))
              {
                input_format = SPACE_SEPARATED_TOKEN_INPUT;
              }
            else if (strcmp(optarg, "apertium"))
              {
                input_format = APERTIUM_INPUT;
              }
            else
              {
                error(EXIT_FAILURE, 0,
                      "Unknown input format %s; valid values are:"
                       "utf8, spaced, apertium\n", optarg);
                return EXIT_FAILURE;
              }
            break;
        case 'x':
            print_statistics = true;
            break;

#include "inc/getopt-cases-error.h"
        }
    }

    switch (output_format)
      {
      case XEROX_OUTPUT:
        begin_setf = hfst_strdup(XEROX_BEGIN_SETF);
        lookupf = hfst_strdup(XEROX_LOOKUPF);
        end_setf = hfst_strdup(XEROX_END_SETF);
        empty_begin_setf = hfst_strdup(XEROX_EMPTY_BEGIN_SETF);
        empty_lookupf = hfst_strdup(XEROX_EMPTY_LOOKUPF);
        empty_end_setf = hfst_strdup(XEROX_EMPTY_END_SETF);
        unknown_begin_setf = hfst_strdup(XEROX_UNKNOWN_BEGIN_SETF);
        unknown_lookupf = hfst_strdup(XEROX_UNKNOWN_LOOKUPF);
        unknown_end_setf = hfst_strdup(XEROX_UNKNOWN_END_SETF);
        infinite_begin_setf = hfst_strdup(XEROX_INFINITE_BEGIN_SETF);
        infinite_lookupf = hfst_strdup(XEROX_INFINITE_LOOKUPF);
        infinite_end_setf = hfst_strdup(XEROX_INFINITE_END_SETF);
      break;
    case CG_OUTPUT:
        begin_setf = hfst_strdup(CG_BEGIN_SETF);
        lookupf = hfst_strdup(CG_LOOKUPF);
        end_setf = hfst_strdup(CG_END_SETF);
        empty_begin_setf = hfst_strdup(CG_EMPTY_BEGIN_SETF);
        empty_lookupf = hfst_strdup(CG_EMPTY_LOOKUPF);
        empty_end_setf = hfst_strdup(CG_EMPTY_END_SETF);
        unknown_begin_setf = hfst_strdup(CG_UNKNOWN_BEGIN_SETF);
        unknown_lookupf = hfst_strdup(CG_UNKNOWN_LOOKUPF);
        unknown_end_setf = hfst_strdup(CG_UNKNOWN_END_SETF);
        infinite_begin_setf = hfst_strdup(CG_INFINITE_BEGIN_SETF);
        infinite_lookupf = hfst_strdup(CG_INFINITE_LOOKUPF);
        infinite_end_setf = hfst_strdup(CG_INFINITE_END_SETF);
        break;
    case APERTIUM_OUTPUT:
        begin_setf = hfst_strdup(APERTIUM_BEGIN_SETF);
        lookupf = hfst_strdup(APERTIUM_LOOKUPF);
        end_setf = hfst_strdup(APERTIUM_END_SETF);
        empty_begin_setf = hfst_strdup(APERTIUM_EMPTY_BEGIN_SETF);
        empty_lookupf = hfst_strdup(APERTIUM_EMPTY_LOOKUPF);
        empty_end_setf = hfst_strdup(APERTIUM_EMPTY_END_SETF);
        unknown_begin_setf = hfst_strdup(APERTIUM_UNKNOWN_BEGIN_SETF);
        unknown_lookupf = hfst_strdup(APERTIUM_UNKNOWN_LOOKUPF);
        unknown_end_setf = hfst_strdup(APERTIUM_UNKNOWN_END_SETF);
        infinite_begin_setf = hfst_strdup(APERTIUM_INFINITE_BEGIN_SETF);
        infinite_lookupf = hfst_strdup(APERTIUM_INFINITE_LOOKUPF);
        infinite_end_setf = hfst_strdup(APERTIUM_INFINITE_END_SETF);
        break;
    default:
      fprintf(stderr, "Unknown output format\n");
      return EXIT_FAILURE;
      break;
    }

    if (!lookup_given)
      {
        lookup_file = stdin;
        lookup_file_name = strdup("<stdin>");
      }
#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

int
lookup_printf(const char* format, const char* inputform,
              const HfstLookupPath* result, const char* markup)
{
    size_t space = 2 * strlen(format) +
        2 * strlen(inputform) + 10;
    char* lookupform = strdup("");
    if (result != NULL)
      {
        size_t lookup_len = 0;
        char* p = lookupform;
        for (vector<string>::const_iterator s = result->first.begin();
             s != result->first.end();
             ++s)
          {
            lookup_len += s->size();
            p = strdup(s->c_str());
            p += s->size();
          }
        *p = '\0';
        lookup_len += 1;
        space += 2 * strlen(lookupform);
      }
    else
      {
        lookupform = strdup("");
      }
    if (markup != NULL)
      {
        space += 2 * strlen(markup);
      }
    char* res = static_cast<char*>(calloc(sizeof(char), space + 1));
    size_t space_left = space;
    const char* src = format;
    char* dst = res;
    char* i; // %i
    char* l; // %l
    char* b; // %b
    char* a; // %a
    char* m; // %m
    float w = result->second; // %w
    i = strdup(inputform);
    if (lookupform != NULL)
    {
        l = strdup(lookupform);
        const char* lookup_end;
        for (lookup_end = lookupform; *lookup_end != '\0'; lookup_end++)
            ;
        const char* anal_start = strchr(lookupform, '+');
        if (anal_start == NULL)
        {
            anal_start = strchr(lookupform, ' ');
        }
        if (anal_start == NULL)
        {
            anal_start = strchr(lookupform, '<');
        }
        if (anal_start == NULL)
        {
            anal_start = strchr(lookupform, '[');
        }
        if (anal_start == NULL)
        {
            // give up trying
            anal_start = lookupform;
        }
        b = static_cast<char*>(calloc(sizeof(char), 
                    anal_start - lookupform + 1));
        b = static_cast<char*>(memcpy(b, lookupform, anal_start - lookupform));
        a = static_cast<char*>(calloc(sizeof(char),
                    lookup_end - anal_start + 1));
        a = static_cast<char*>(memcpy(a, anal_start, lookup_end - anal_start));
    }
    else
    {
        l = strdup("");
        b = strdup("");
        a = strdup("");
    }
    if (markup != NULL)
      {
        m = strdup(markup);
      }
    else
      {
        m = strdup("");
      }
    bool percent = false;
    while ((*src != '\0') && (space_left > 0))
    {
        if (percent)
        {
            if (*src == 'b')
            {
                int skip = snprintf(dst, space_left, "%s", b);
                dst += skip;
                space_left -= skip;
                src++;
            }
            else if (*src == 'l')
            {
                int skip = snprintf(dst, space_left, "%s", l);
                dst += skip;
                space_left -= skip;
                src++;
            }
            else if (*src == 'i')
            {
                int skip = snprintf(dst, space_left, "%s", i);
                dst += skip;
                space_left -= skip;
                src++;
            }
            else if (*src == 'a')
            {
                int skip = snprintf(dst, space_left, "%s", a);
                dst += skip;
                space_left -= skip;
                src++;
            }
            else if (*src == 'm')
              {
                int skip = snprintf(dst, space_left, "%s", m);
                dst += skip;
                space_left -= skip;
                src++;
              }
            else if (*src == 'n')
            {
                *dst = '\n';
                dst++;
                space_left--;
                src++;
            }
            else if (*src == 'w')
              {
                int skip = snprintf(dst, space_left, "%f", w);
                dst += skip;
                space_left -= skip;
                src++;
              }
            else
            {
                // unknown format, retain % as well
                *dst = '%';
                dst++;
                space_left--;
                *dst = *src;
                dst++;
                space_left--;
                src++;
            }
            percent = false;
        }
        else if (*src == '%')
        {
            percent = true;
            src++;
        }
        else
        {
            *dst = *src;
            dst++;
            space_left--;
            src++;
        }
    }
    *dst = '\0';
    free(a);
    free(l);
    free(b);
    free(i);
    free(m);
    int rv = fprintf(message_out, "%s", res);
    free(res);
    return rv;
}


vector<string>*
string_to_utf8(char* p)
{
  vector<string>* path = new vector<string>;
    while ((p != 0) && (*p != '\0'))
      {
        unsigned char c = static_cast<unsigned char>(*p);
        unsigned short u8len = 1;
        if (c <= 127)
          {
            u8len = 1;
          }
        else if ( (c & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16) )
          {
            u8len = 4;
          }
        else if ( (c & (128 + 64 + 32 )) == (128 + 64 + 32) )
          {
            u8len = 3;
          }
        else if ( (c & (128 + 64 )) == (128 + 64))
          {
            u8len = 2;
          }
        else
          {
            error_at_line(EXIT_FAILURE, 0, inputfilename, linen,
                          "%s not valid UTF-8\n", p);
          }
        char* nextu8 = hfst_strndup(p, u8len);
        path->push_back(nextu8);
        p += u8len;
      }
    return path;
}

HfstLookupPath*
line_to_lookup_path(char** s, const hfst::HfstTokenizer& /* tok */,
                    char** markup, bool* outside_sigma)
{
    HfstLookupPath* rv = new HfstLookupPath;
    rv->second = 0;
    *outside_sigma = false;
    inputs++;
    switch (input_format)
      {
      case SPACE_SEPARATED_TOKEN_INPUT:
        {
          vector<string> path;
          char* token = strtok(*s, " ");
          while (token)
            {
              path.push_back(token);
            }
          rv->first = path;
          break;
        }
      case UTF8_TOKEN_INPUT:
        {
          vector<string>* path = string_to_utf8(*s);
          rv->first = *path;
          delete path;
          break;
        }
      case APERTIUM_INPUT:
          {
            char* real_s = static_cast<char*>(calloc(sizeof(char),strlen(*s)+1));
            *markup = static_cast<char*>(calloc(sizeof(char), strlen(*s)+1));
            char* m = *markup;
            char* sp = real_s;
            bool inbr = false;
            for (const char* p = *s; *p != '\0'; p++)
              {
                if (inbr)
                  {
                    if (*p == ']')
                      {
                        *m = *p;
                        m++;
                        inbr = false;
                      }
                    else if ((*p == '\\') && (*(p+1) == ']'))
                      {
                        p++;
                        *m = *p;
                        m++;
                      }
                    else
                      {
                        *m = *p;
                        m++;
                      }
                  }
                else if (!inbr)
                  {
                    if (*p == '[')
                      {
                        *m = *p;
                        m++;
                        inbr = true;
                      }
                    else if (*p == ']')
                      {
                        *m = *p;
                        m++;
                        continue;
                      }
                    else if (*p == '\\')
                      {
                        p++;
                        *real_s = *p;
                        real_s++;
                      }
                    else
                      {
                        *real_s = *p;
                        real_s++;
                      }
                  }
              } // for each character in input
            vector<string>* path = string_to_utf8(sp);
            free(*s);
            *s = sp;
            rv->first = *path;
            break;
          }
      default:
        fprintf(stderr, "Unknown input format");
        break;
      } // switch input format
    return rv;
}

HfstLookupPaths*
lookup_simple(const HfstLookupPath& s, HfstTransducer& t, bool* infinity)
{
  HfstLookupPaths* results = new HfstLookupPaths;
  if (t.is_lookup_infinitely_ambiguous(s))
    {
      warning(0, 0, "Got infinite results, using %zu first",
              infinite_cutoff);
      t.lookup_fd(*results, s, infinite_cutoff);
      *infinity = true;
    }
  else
    {
      t.lookup_fd(*results, s);
    }
  if (results->size() == 0)
    {
       // no results as empty result
      verbose_printf("Got no results\n");
    }
  return results;
}

HfstLookupPaths*
lookup_cascading(const HfstLookupPath& s, vector<HfstTransducer> cascade,
                 bool* infinity)
{
  HfstLookupPaths* kvs = new HfstLookupPaths;
  kvs->insert(s);
  for (unsigned int i = 0; i < cascade.size(); i++)
    {
      // cascade here
      HfstLookupPaths* newkvs = new HfstLookupPaths;
      for (HfstLookupPaths::const_iterator ckv = kvs->begin();
           ckv != kvs->end();
           ++ckv)
        {
          HfstLookupPaths* xyzkvs = lookup_simple(*ckv, cascade[i],
                                                  infinity);
          if (infinity)
            {
              verbose_printf("Inf results @ level %u, using %zu\n",
                             i, xyzkvs->size());
            }
          else
            {
              verbose_printf("%zu results @ level %u\n", xyzkvs->size(), i);
            }
          for (HfstLookupPaths::const_iterator xyzkv = xyzkvs->begin();
               xyzkv != xyzkvs->end();
               ++xyzkv)
            {
              newkvs->insert(*xyzkv);
            }
          delete xyzkvs;
        }
      delete kvs;
      kvs = newkvs;
   }
  return kvs;
}

void
print_lookups(const HfstLookupPaths& kvs,
              const char* s, char* markup,
              bool outside_sigma, bool inf)
{
    if (outside_sigma)
      {
        lookup_printf(unknown_begin_setf, s, NULL, markup);
        lookup_printf(unknown_lookupf, s, NULL, markup);
        lookup_printf(unknown_end_setf, s, NULL, markup);
        no_analyses++;
      }
    else if (kvs.size() == 0)
      {
        lookup_printf(empty_begin_setf, s, NULL, markup);
        lookup_printf(empty_lookupf, s, NULL, markup);
        lookup_printf(empty_end_setf, s, NULL, markup);
        no_analyses++;
      }
    else if (inf)
      {
        analysed++;
        lookup_printf(infinite_begin_setf, s, NULL, markup);
        for (HfstLookupPaths::const_iterator lkv = kvs.begin();
                lkv != kvs.end();
                ++lkv)
          {
            HfstLookupPath lup = *lkv;
            lookup_printf(infinite_lookupf, s, &lup, markup);
            analyses++;
          }
        lookup_printf(infinite_end_setf, s, NULL, markup);
      }
    else
      {
        analysed++;

        lookup_printf(begin_setf, s, NULL, markup);
        for (HfstLookupPaths::const_iterator lkv = kvs.begin();
                lkv != kvs.end();
                ++lkv)
          {
            HfstLookupPath lup = *lkv;
            lookup_printf(lookupf, s, &lup, markup);
            analyses++;
        }
        lookup_printf(end_setf, s, NULL, markup);
      }
}

HfstLookupPaths*
perform_lookups(HfstLookupPath& origin, vector<HfstTransducer>& cascade, bool unknown, bool* infinite)
{
  HfstLookupPaths* kvs;
    if (!unknown)
      {
        if (cascade.size() == 1)
          {
            kvs = lookup_simple(origin, cascade[0], infinite);
          }
        else
         {
            kvs = lookup_cascading(origin, cascade, infinite);
         }
      }
    else
      {
        kvs = new HfstLookupPaths;
      }
    return kvs;
}

int
process_stream(HfstInputStream& inputstream, FILE* outstream)
{
    std::vector<HfstTransducer> cascade;
    size_t transducer_n=0;
    HfstTokenizer tok;
    while (inputstream.is_good())
      {
        transducer_n++;
        if (transducer_n==1)
          {
            verbose_printf("Reading %s...\n", inputfilename); 
          }
        else
          {
            verbose_printf("Reading %s...%zu\n", inputfilename,
                           transducer_n); 
          }
        HfstTransducer trans(inputstream);
        cascade.push_back(trans);
      }
    inputstream.close();
    char* line = 0;
    size_t llen = 0;
    while (hfst_getline(&line, &llen, lookup_file) != -1)
      {
        linen++;
        char *p = line;
        while (*p != '\0')
          {
            if (*p == '\n')
              {
                *p = '\0';
                break;
              }
            p++;
          }
        verbose_printf("Looking up %s...\n", line);
        char* markup = 0;
        bool unknown = false;
        bool infinite = false;
        HfstLookupPath* kv = line_to_lookup_path(&line, tok, &markup,
                                                 &unknown);
        HfstLookupPaths* kvs;
        try 
          {
            kvs = perform_lookups(*kv, cascade, unknown, &infinite);
          }
        catch (hfst::exceptions::FunctionNotImplementedException)
          {
            // TODO: implement compose and strings here
            error(EXIT_FAILURE, 0, "Lookup not supported on this automata "
                  "format\n"
                  "Try compose and fst2strings instead\n");
            return EXIT_FAILURE;
          }
        print_lookups(*kvs, line, markup, unknown, infinite);
        delete kv;
        delete kvs;
      } // while lines in input
    free(line);
    if (print_statistics)
      {
        fprintf(outstream, "Strings\tFound\tMissing\tResults\n"
                "%lu\t%lu\t%lu\t%lu\n", 
                inputs, analysed, no_analyses, analyses);
        fprintf(outstream, "Coverage\tAmbiguity\n"
                "%f\t%f\n",
                (float)analysed/(float)inputs,
                (float)analyses/(float)inputs);
      }
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.6", "HfstLookup");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    if (inputfile != stdin)
    {
        fclose(inputfile);
    }
    verbose_printf("Reading from %s, writing to %s\n", 
        inputfilename, outfilename);
    verbose_printf("Output formats:\n"
            "  regular:`%s'`%s...'`%s',\n"
            "  unanalysed:`%s'`%s'`%s',\n"
            "  untokenised:`%s'`%s'`%s',\n"
            "  infinite:`%s'`%s'`%s\n",
            begin_setf, lookupf, end_setf,
            empty_begin_setf, empty_lookupf, empty_end_setf,
            unknown_begin_setf, unknown_lookupf, unknown_end_setf,
            infinite_begin_setf, infinite_lookupf, infinite_end_setf);
    // here starts the buffer handling part
    HfstInputStream* instream = NULL;
    try 
      {
        instream = (inputfile != stdin) ?
          new HfstInputStream(inputfilename) :
          new HfstInputStream();
      } 
    catch (NotTransducerStreamException)
      {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              inputfilename);
        return EXIT_FAILURE;
      }
    process_stream(*instream, outfile);
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}

