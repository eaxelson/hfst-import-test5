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
#include <limits>
#include <math.h>

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "HfstLookupFlagDiacritics.h"
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "implementations/HfstTransitionGraph.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"
#include "HfstStrings2FstTokenizer.h"

using hfst::HfstTransducer;
using hfst::HFST_OL_TYPE;
using hfst::HFST_OLW_TYPE;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::HfstOneLevelPath;
using hfst::HfstOneLevelPaths;
using hfst::HfstTwoLevelPath;
using hfst::HfstTwoLevelPaths;

using hfst::StringPair;
using hfst::StringPairVector;
using hfst::StringVector;

using std::string;
using std::vector;

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

// XFST variables for apply
static bool show_flags = false;
static bool obey_flags = true;
static bool print_pairs = false;
static bool print_space = false;
static bool quote_special = false;

static char* epsilon_format = "";
static char* space_format = "";

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
static const char* XEROX_LOOKUPF = "%i\t%l\t%w%n";
static const char* XEROX_END_SETF = "%n";
// notaword notaword+?
static const char* XEROX_EMPTY_BEGIN_SETF = "";
static const char* XEROX_EMPTY_LOOKUPF = "%i\t%i+?\t%w%n";
static const char* XEROX_EMPTY_END_SETF = "%n";
// ¶    ¶+? 
static const char* XEROX_UNKNOWN_BEGIN_SETF = "";
static const char* XEROX_UNKNOWN_LOOKUPF = "%i\t%i+?\t%w%n";
static const char* XEROX_UNKNOWN_END_SETF = "%n";
// 0    0 NUM SG
// 0    [...cyclic...]
static const char* XEROX_INFINITE_BEGIN_SETF = "";
static const char* XEROX_INFINITE_LOOKUPF = "%i\t%l\t%w%n";
static const char* XEROX_INFINITE_END_SETF = "%i\t[...cyclic...]%n%n";
// CG:
// "<word>"
//      "word"  N SG
//      "word"  V PRES
static const char* CG_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_LOOKUPF = "\t\"%b\"%a\t%w%n";
static const char* CG_END_SETF = "%n";
// "<notaword>"
//      "notaword" ?
static const char* CG_EMPTY_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_EMPTY_LOOKUPF = "\t\"%i\" ?\tInf%n";
static const char* CG_EMPTY_END_SETF = "%n";
// "<¶>"
//      "¶" ?
static const char* CG_UNKNOWN_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_UNKNOWN_LOOKUPF = "\t\"%i\"\t ?\tInf%n";
static const char* CG_UNKNOWN_END_SETF = "%n";
// "<0>"
//      "0" NUM SG
//      "0" [...cyclic...]
static const char* CG_INFINITE_BEGIN_SETF = "\"<%i>\"%n";
static const char* CG_INFINITE_LOOKUPF = "\t\"%b\"%a\t%w%n";
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
    fprintf(message_out, 
        "Input/Output options:\n"
        "  -i, --input=INFILE     Read input transducer from INFILE\n"
        "  -o, --output=OUTFILE   Write output to OUTFILE\n");

    fprintf(message_out, "Lookup options:\n"
            "  -I, --input-strings=SFILE        Read lookup strings from SFILE\n"
            "  -O, --output-format=OFORMAT      Use OFORMAT printing results sets\n"
            "  -e, --epsilon-format=EPS         Print epsilon as EPS\n"
            "  -F, --input-format=IFORMAT       Use IFORMAT parsing input\n"
            "  -x, --statistics                 Print statistics\n"
            "  -X, --xfst=VARIABLE              Toggle xfst VARIABLE\n"
            "  -c, --cycles=INT                 How many times to follow input epsilon cycles\n");
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out, 
            "OFORMAT is one of {xerox,cg,apertium}, xerox being default\n"
            "IFORMAT is one of {text,spaced,apertium}, default being text,\n"
            "unless OFORMAT is apertium\n"
            "VARIABLEs relevant to lookup are {print-pairs,print-space,\n"
            "quote-special,show-flags,obey-flags}\n"
            "Input epsilon cycles are followed by default INT=5 times.\n"
            "Epsilon is printed by default as empty string.\n");
    fprintf(message_out, "\n");

    fprintf(message_out, 
            "Todo:\n"
            "  For optimized lookup format, only strings that pass "
            "flag diacritic checks\n"
            "  are printed and flag diacritic symbols are not printed.\n"
            "  For other formats, all flag paths are allowed.\n"
            "  Support VARIABLE 'print-space' for optimized lookup format\n");
    fprintf(message_out,
            "\n"
            "Known bugs:\n"
            "  'quote-special' quotes spaces that come from 'print-space'\n");

    fprintf(message_out, "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
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
            {"cycles", required_argument, 0, 'c'},
            {"xfst", required_argument, 0, 'X'},
            {"epsilon-format", required_argument, 0, 'e'},
            {"epsilon-format2", required_argument, 0, 'E'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "I:O:F:xc:X:e:E:",
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
            if (strcmp(optarg, "text") == 0)
              {
                input_format = UTF8_TOKEN_INPUT;
              }
            else if (strcmp(optarg, "spaced") == 0)
              {
                input_format = SPACE_SEPARATED_TOKEN_INPUT;
              }
            else if (strcmp(optarg, "apertium") == 0)
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
        case 'e':
        case 'E':
            epsilon_format = hfst_strdup(optarg);
            break;
        case 'x':
            print_statistics = true;
            break;
        case 'X':
            if (strcmp(optarg, "print-pairs") == 0)
              {
                print_pairs = true;
                /* error(EXIT_FAILURE, 0, "Unimplemented pair printing"); */
              }
            else if (strcmp(optarg, "print-space") == 0)
              {
                print_space = true;
                space_format = strdup(" ");
              }
            else if (strcmp(optarg, "show-flags") == 0)
              {
                show_flags = true;
              }
            else if (strcmp(optarg, "quote-special") == 0)
              {
                quote_special = true;
              }
	    else if (strcmp(optarg, "obey-flags") == 0)
	      {
		obey_flags = false;
	      }
            else 
              {
                error(EXIT_FAILURE, 0, "Xfst variable %s unrecognised",
                      optarg);
              }
        case 'c':
            infinite_cutoff = (size_t)atoi(hfst_strdup(optarg));
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


static std::string get_print_format(const std::string &s) ;

// local flag handling
bool
is_flag_diacritic(const std::string& label)
  {
    const char* p = label.c_str();
    if (*p == '\0') 
      {
        return false;
      }
    if (*p != '@')
      {
        return false;
      }
    p++;
    if (*p == '\0') 
      {
        return false;
      }
    if ((*p != 'P') && (*p != 'D') && (*p != 'C') && (*p != 'R') &&
        (*p != 'U') && (*p != 'N'))
      {
        return false;
      }
    p++;
    if (*p == '\0') 
      {
        return false;
      }
    if (*p != '.')
      {
        return false;
      }
    while (*p != '\0')
      {
        p++;
      }
    if (*(p-1) != '@')
      {
       return false;
      }
    return true;
  } 

bool
is_valid_flag_diacritic_path(StringVector arcs)
  {
    //if (not print_pairs)
    //  fprintf(stderr, "Allowing all flag paths!\n");
    FlagDiacriticTable FdT;
    bool res = FdT.is_valid_string(arcs);
    return res;
  }



int
lookup_printf(const char* format, const HfstOneLevelPath* input,
              const HfstOneLevelPath* result, const char* markup,
              FILE * ofile)
{
    size_t space = 0;
    size_t input_len = 0;
    size_t lookup_len = 0;
    if (result != NULL)
      {
        bool first = true;
        for (vector<string>::const_iterator s = result->second.begin();
             s != result->second.end();
             ++s)
          {
            if (!first && print_space)
              {
                lookup_len += strlen(space_format);
              }
            if (s->compare("@_EPSILON_SYMBOL_@") == 0)
              {
                lookup_len += strlen(epsilon_format);
              }
            else if (is_flag_diacritic(*s))
              {
                if (show_flags)
                  {
                    lookup_len += s->size();
                  }
              }
            else
              {
                lookup_len += s->size();
              }
          }
      }
    if (input != NULL)
      {
        bool first = true;
        for (vector<string>::const_iterator s = input->second.begin();
             s != input->second.end();
             ++s)
          {
            if (!first && print_space)
              {
                input_len += strlen(space_format);
              }
            if (s->compare("@_EPSILON_SYMBOL_@") == 0)
              {
                input_len += strlen(epsilon_format);
              }
            else if (is_flag_diacritic(*s))
              {
                if (show_flags)
                  {
                    input_len += s->size();
                  }
              }
            else
              {
                input_len += s->size();
              }
            first = false;
          }
      }
    for (const char *f = format; *f != '\0'; f++)
      {
        if (*f == '%')
          {
            f++;
            if (*f == 'i')
              {
                space += input_len;
              }
            else if (*f == 'l')
              {
                space += lookup_len;
              }
            else if (*f == 'w')
              {
                space += strlen("0.012345678901234567890");
              }
            else if (*f == '\0')
              {
                break;
              }
            else
              {
                space++;
              }
          }
        space++;
      }
    char* lookupform = strdup("");
    if (result != NULL)
      {
        lookupform = static_cast<char*>(malloc(sizeof(char)*lookup_len+1));
        char* p = lookupform;
        bool first = true;
        for (vector<string>::const_iterator s = result->second.begin();
             s != result->second.end();
             ++s)
          {
            if (!first && print_space)
              {
                p = strcpy(p, space_format);
                p += strlen(space_format);
              }
            if (s->compare("@_EPSILON_SYMBOL_@") == 0)
              {
                p = strcpy(p, epsilon_format);
                p += strlen(epsilon_format);
              }
            else if (is_flag_diacritic(*s))
              {
                if (show_flags)
                  {
                    p = strcpy(p, s->c_str());
                    p += s->size();
                  }
              }
            else
              {
                p = strcpy(p, s->c_str());
                p += s->size();
              }
            first = false;
          }
        *p = '\0';
      }
    else
      {
        lookupform = strdup("");
      }
    char* inputform = strdup("");
    if (input != NULL)
      {
        inputform = static_cast<char*>(malloc(sizeof(char)*input_len+1));
        char* p = inputform;
        bool first = true;
        for (vector<string>::const_iterator s = input->second.begin();
             s != input->second.end();
             ++s)
          {
            if (!first && print_space)
              {
                p = strcpy(p, space_format);
                p += strlen(space_format);
              }
            if (s->compare("@_EPSILON_SYMBOL_@") == 0)
              {
                p = strcpy(p, epsilon_format);
                p += strlen(epsilon_format);
              }
            else if (is_flag_diacritic(*s))
              {
                if (show_flags)
                  {
                    p = strcpy(p, s->c_str());
                    p += s->size();
                  }
              }
            else
              {
                p = strcpy(p, s->c_str());
                p += s->size();
              }
            first = false;
          }
        *p = '\0';
      }
    else
      {
        inputform = strdup("");
      }
    if (markup != NULL)
      {
        space += 2 * strlen(markup);
      }
    char* res = static_cast<char*>(calloc(sizeof(char), space*32 + 1));
    size_t space_left = space;
    const char* src = format;
    char* dst = res;
    char* i; // %i
    char* l; // %l
    char* b; // %b
    char* a; // %a
    char* m; // %m
    float w = 0.0f;
    if (result != NULL)
      {
        w = result->first; // %w
      }
    else
      {
        w = INFINITY;
      }
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
    int rv;
    if (not quote_special)
      rv = fprintf(ofile, "%s", res);
    else
      rv = fprintf(ofile, "%s", get_print_format(res).c_str());
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

/* Add a '\' in front of ':', ' ' and '\'. */
static std::string escape_special_characters(char *s)
{
  std::string retval;
  for (unsigned int i=0; s[i] != '\0'; i++)
    {
      if (s[i] == ':' || s[i] == '\\' || s[i] == ' ')
        retval.append(1, '\\');
      retval.append(1, s[i]);
    }
  return retval;
}

HfstOneLevelPath*
line_to_lookup_path(char** s, HfstStrings2FstTokenizer& tok,
                    char** markup, bool* outside_sigma, bool optimized_lookup)
{
    HfstOneLevelPath* rv = new HfstOneLevelPath;
    rv->first = 0;
    *outside_sigma = false;
    inputs++;
    switch (input_format)
      {
      case SPACE_SEPARATED_TOKEN_INPUT:
        {
          /*
          vector<string> path;
          char* token = strtok(*s, " ");
          while (token)
            {
              path.push_back(token);
              token = strtok(NULL, " ");
            }
          rv->second = path;
          break;
          */
          
          std::string S = escape_special_characters(*s);

          StringPairVector spv 
            = tok.tokenize_string_pair(S, true);
          
          for (StringPairVector::const_iterator it = spv.begin();
               it != spv.end(); it++)
            {
              rv->second.push_back(it->first);
            }
          break;
        }
      case UTF8_TOKEN_INPUT:
        {
          if (optimized_lookup)
            {
              vector<string>* path = string_to_utf8(*s);
              rv->second = *path;
              delete path;
            }
          else
            {
              std::string S = escape_special_characters(*s);

              StringPairVector spv 
                = tok.tokenize_string_pair(S, false);
              
              for (StringPairVector::const_iterator it = spv.begin();
                   it != spv.end(); it++)
                {
                  rv->second.push_back(it->first);
                }
            }
          break;
        }
      case APERTIUM_INPUT:
          {
            char* real_s 
              = static_cast<char*>(calloc(sizeof(char),strlen(*s)+1));

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
            rv->second = *path;
            break;
          }
      default:
        fprintf(stderr, "Unknown input format");
        break;
      } // switch input format
    return rv;
}

template<class T> HfstOneLevelPaths*
lookup_simple(const HfstOneLevelPath& s, T& t, bool* infinity)
{
  HfstOneLevelPaths* results = new HfstOneLevelPaths;

  (void)s;
  (void)t;
  (void)infinity;
  if (results->size() == 0)
    {
       // no results as empty result
      verbose_printf("Got no results\n");
    }
  return results;
}

bool is_lookup_infinitely_ambiguous
(HfstBasicTransducer &t, const HfstOneLevelPath& s, 
 unsigned int& index, HfstState state, 
 std::set<HfstState> &epsilon_path_states) 
{
  // Whether the end of the lookup path s has been reached
  bool only_epsilons=false;
  if ((unsigned int)s.second.size() == index)
    only_epsilons=true;

  // Go through all transitions in this state
  HfstBasicTransducer::HfstTransitions transitions = t[state];
  for (HfstBasicTransducer::HfstTransitions::const_iterator it 
         = transitions.begin();
       it != transitions.end(); it++)
    {
      // CASE 1: Input epsilons do not consume a symbol in the lookup path s,
      //         so they can be added freely.
      if (it->get_input_symbol().compare("@_EPSILON_SYMBOL_@") == 0)
    {
      epsilon_path_states.insert(state);
      if (epsilon_path_states.find(it->get_target_state()) 
          != epsilon_path_states.end())
        return true;
      if (is_lookup_infinitely_ambiguous
          (t, s, index, it->get_target_state(), epsilon_path_states))
        return true;
      epsilon_path_states.erase(state);
    }

      /* CASE 2: Other input symbols consume a symbol in the lookup path s,
         so they can be added only if the end of the lookup path s has not 
         been reached. */
      else if (not only_epsilons)
    {
      if (it->get_input_symbol().compare(s.second.at(index)) == 0)
        {
          index++; // consume an input symbol in the lookup path s
          std::set<HfstState> empty_set;
          if (is_lookup_infinitely_ambiguous
              (t, s, index, it->get_target_state(), empty_set)) {
            return true; }
          index--; // add the input symbol back to the lookup path s.
        }
    }
    }  
  return false;
}

bool is_lookup_infinitely_ambiguous(HfstBasicTransducer &t, 
                                    const HfstOneLevelPath& s)
{
  std::set<HfstState> epsilon_path_states;
  //epsilon_path_states.insert(t.get_initial_state());
  epsilon_path_states.insert(0);
  unsigned int index=0;
  HfstState initial_state=0;

  return is_lookup_infinitely_ambiguous(t, s, index, initial_state, 
                    epsilon_path_states);
}
// HERE
HfstOneLevelPaths*
lookup_simple(const HfstOneLevelPath& s, HfstTransducer& t, bool* infinity)
{
  HfstOneLevelPaths* results = new HfstOneLevelPaths;
  if (t.is_lookup_infinitely_ambiguous(s.second))
    {
      if (!silent && infinite_cutoff > 0) {
    warning(0, 0, "Got infinite results, number of cycles limited to %zu",
        infinite_cutoff);
      }
      t.lookup_fd(*results, s.second, infinite_cutoff);
      *infinity = true;
    }
  else
    {
      t.lookup_fd(*results, s.second);
    }

  if (results->size() == 0)
    {
       // no results as empty result
      verbose_printf("Got no results\n");
    }
  return results;
}


/* A class for handling input epsilon cycles in function lookup_fd. */
class EpsilonHandler
{
protected:
  typedef std::vector<HfstState> HfstStateVector;
  // the path of consecutive input epsilon transitions
  HfstStateVector epsilon_path; 
  size_t max_cycles; // maximum number of consecutive epsilon cycles allowed
  size_t cycles;     // number of cycles detected so far
public:
  EpsilonHandler(size_t cutoff);
  void push_back(HfstState s);
  void pop_back();
  bool can_continue(HfstState s);
};

/* Create an epsilon handler which allows a maximum of \a cutoff
   consecutive input epsilon cycles. The cycles do not need to be
   the same. */
EpsilonHandler::EpsilonHandler(size_t cutoff): 
  epsilon_path(), max_cycles(cutoff), cycles(0) {};

/* Called before calling lookup_fd recursively. 
   Appends state \a s to the epsilon path if not found at the end already. */
void EpsilonHandler::push_back(HfstState s)
{
  if (not epsilon_path.empty()) {
    if  (epsilon_path.back() != s ) {
      epsilon_path.push_back(s);
    }
  }
  else {
    epsilon_path.push_back(s);
  }
};

/* Removes a state from the end of the epsilon path, 
   unless the path it is empty. */
void EpsilonHandler::pop_back()
{
  if (not epsilon_path.empty())
    epsilon_path.pop_back();
};

/* This function is called at the beginning of lookup_fd. 
   It tells whether we can proceed in the current state (i.e. state \a s)
   and updates the cycle counter and epsilon path of the EpsilonHandler. */
bool EpsilonHandler::can_continue(HfstState s)
{
  for(HfstStateVector::iterator it = epsilon_path.begin();
      it != epsilon_path.end(); it++)
    {
      if (*it == s) // a cycle detected
        {
          it++;
          // erase the cycle from the epsilon path 
          // and check whether the number of cycles is exceeded
          epsilon_path.erase(it, epsilon_path.end());
          cycles++;
          if (cycles > max_cycles) {
            return false;
          }
          return true;
        }
    }
  return true;  // no cycle detected
};

static void push_back_to_two_level_path
(HfstTwoLevelPath &path,
 const StringPair &sp,
 const float &weight)
{
  path.second.push_back(sp);
  path.first = path.first + weight;
}

static void pop_back_from_two_level_path
(HfstTwoLevelPath &path,
 const float &weight)
{
  path.second.pop_back();
  path.first = path.first - weight;
}

static void add_to_results
(HfstTwoLevelPaths &results,
 HfstTwoLevelPath &path_so_far,
 const float &final_weight)
{
  path_so_far.first = path_so_far.first + final_weight;
  results.insert(path_so_far);
  path_so_far.first = path_so_far.first - final_weight;
}

static bool is_possible_transition
(const HfstBasicTransition &transition,
 const StringVector &lookup_path,
 const unsigned int &lookup_index,
 bool &input_symbol_consumed)
{
  std::string isymbol = transition.get_input_symbol();

  // If we are not at the end of lookup_path,
  if (not (lookup_index == (unsigned int)lookup_path.size()))
    {
      // we can go further if the current symbol in lookup_path
      // matches to the input symbol of the transition.
      if ( isymbol.compare(lookup_path.at(lookup_index)) == 0 )
        {
          input_symbol_consumed=true;
          return true;
        }
    }
  // Whether there are more symbols in lookup_path or not,
  // we can always go further if the input symbol of the transition
  // is an epsilon or a flag diacritic.
  if ( isymbol.compare("@_EPSILON_SYMBOL_@") == 0 || 
       is_flag_diacritic(isymbol) )
    {
      input_symbol_consumed=false;
      return true;
    }

  // No matches.
  return false;
}

static void lookup_fd
(HfstBasicTransducer &t,
 const StringVector &lookup_path,
 HfstTwoLevelPaths &results,
 HfstState state,
 unsigned int lookup_index, // an iterator instead?
 HfstTwoLevelPath &path_so_far,
 EpsilonHandler Eh)
{
  // Check whether the number of input epsilon cycles is exceeded
  if (not Eh.can_continue(state)) {
    return;
  }

  // If we are at the end of lookup_path,
  if (lookup_index == lookup_path.size())
    {
      // and if the current state is final, 
      if (t.is_final_state(state)) 
        {
          // path_so_far is a valid result.
          add_to_results
            (results, path_so_far, t.get_final_weight(state) );
        }
    }

  // Whether there are more symbols in lookup_path or not,
  // go through all transitions in the current state.
  HfstBasicTransducer::HfstTransitions transitions = t[state];
  for (HfstBasicTransducer::HfstTransitions::const_iterator it 
         = transitions.begin();
       it != transitions.end(); it++)
    {
      bool input_symbol_consumed=false;
      if ( is_possible_transition
           (*it, lookup_path, lookup_index, input_symbol_consumed) )
        {
          // update path_so_far and lookup_index
          push_back_to_two_level_path
            (path_so_far, 
             StringPair(it->get_input_symbol(), it->get_output_symbol()),
             it->get_weight());
          EpsilonHandler * Ehp = NULL;
          if (input_symbol_consumed) {
            lookup_index++;
            Ehp = new EpsilonHandler(infinite_cutoff);
          }
          else {
            Eh.push_back(state);
            Ehp = &Eh;
	  }

          // call lookup for the target state of the transition
          lookup_fd(t, lookup_path, results, it->get_target_state(),
                    lookup_index, path_so_far, *Ehp);

          // return to the original values of path_so_far and lookup_index
          if (input_symbol_consumed) {
            lookup_index--;
            delete Ehp;
          }
          else {
            // Eh.pop_back();  not needed because the destructor of Eh is 
            //                 automatically called next
          }

          pop_back_from_two_level_path(path_so_far, it->get_weight());
        }
    }
  
}

static void lookup_fd
(HfstBasicTransducer &t,
 const StringVector &lookup_path,
 HfstTwoLevelPaths &results)
{
  HfstState state = 0;
  unsigned int lookup_index = 0;
  HfstTwoLevelPath path_so_far;
  EpsilonHandler Eh(infinite_cutoff);
  lookup_fd(t, lookup_path, results, state, lookup_index, path_so_far, Eh);
}



/* Replace all strings \a str1 in \a symbol with \a str2. */
static std::string replace_all(std::string symbol, 
                               const std::string &str1,
                               const std::string &str2)
{
  size_t pos = symbol.find(str1);
  while (pos != string::npos) // while there are str1:s to replace
    {
      symbol.erase(pos, str1.size()); // erase str1
      symbol.insert(pos, str2);       // insert str2 instead
      pos = symbol.find               // find next str1
        (str1, pos+str2.size());      
    }
  return symbol;
}


static std::string get_print_format(const std::string &s) 
{
  if (s.compare("@_EPSILON_SYMBOL_@") == 0)
    return std::string(strdup(epsilon_format));

  if (quote_special) 
    {
      return 
        replace_all
        ( replace_all
          ( replace_all
            (s, "\\", "\\\\"),
            ":", "\\:"),
          " ", "\\ ");
    }

  return std::string(s);
}

static void print_lookup_string(const StringVector &s) {
  for (StringVector::const_iterator it = s.begin(); 
       it != s.end(); it++) {
    fprintf(stderr, "%s", get_print_format(*it).c_str());
  }
}

void lookup_fd(HfstBasicTransducer &t, HfstOneLevelPaths& results, 
               const HfstOneLevelPath& s, ssize_t limit = -1)
{
  (void)limit;

  /* If we want a StringPairVector representation */
  HfstTwoLevelPaths results_spv;
  StringPairVector path_spv;

  lookup_fd(t, s.second, results_spv);

  if (print_pairs) {
    
    /* No results, print just the lookup string. */
    if (results_spv.size() == 0) {
      print_lookup_string(s.second);
      fprintf(outfile, "\n");
    }
    else {
      /* For all result strings, */
      for (HfstTwoLevelPaths::const_iterator it
             = results_spv.begin(); it != results_spv.end(); it++) {
        
        /* print the lookup string */
        print_lookup_string(s.second);
        fprintf(outfile, "\t");
        
        /* and the path that yielded the result string */
        bool first_pair=true;
        for (StringPairVector::const_iterator IT = it->second.begin();
             IT != it->second.end(); IT++) {
          if (print_space && not first_pair) {
            fprintf(outfile, " ");
          }
          first_pair=false;
          fprintf(outfile, "%s:%s", 
                  get_print_format(IT->first).c_str(), 
                  get_print_format(IT->second).c_str());
        }
        /* and the weight of that path. */
        fprintf(outfile, "\t%f\n", it->first);
      }
    }
    fprintf(outfile, "\n");
  }

  // Convert HfstTwoLevelPaths into HfstOneLevelPaths
  for (HfstTwoLevelPaths::const_iterator it = results_spv.begin();
       it != results_spv.end(); it++)
    {
      StringVector sv;
      for (StringPairVector::const_iterator spv_it = it->second.begin();
           spv_it != it->second.end(); spv_it++)
        {
          sv.push_back(spv_it->second);
        }
      HfstOneLevelPath path(it->first, sv);
      results.insert(path);
    }
 
  HfstOneLevelPaths filtered;
  for (HfstOneLevelPaths::iterator res = results.begin();
       res != results.end();
       ++res)
    {
      if (is_valid_flag_diacritic_path(res->second) || !obey_flags)
        {
          StringVector unflagged;
          for (StringVector::const_iterator arc = res->second.begin();
               arc != res->second.end();
               arc++)
            {
              if (show_flags || !is_flag_diacritic(*arc))
                {
                  unflagged.push_back(*arc);
                }
            }
          filtered.insert(HfstOneLevelPath(res->first, unflagged));
        }
    }
  results = filtered;
}

HfstOneLevelPaths*
lookup_simple(const HfstOneLevelPath& s, HfstBasicTransducer& t, bool* infinity)
{
  HfstOneLevelPaths* results = new HfstOneLevelPaths;

  if (is_lookup_infinitely_ambiguous(t,s))
    {
      if (!silent && infinite_cutoff > 0) {
    warning(0, 0, "Got infinite results, number of cycles limited to %zu",
        infinite_cutoff);
      }
      lookup_fd(t, *results, s, infinite_cutoff);
      *infinity = true;
    }
  else
    {
      lookup_fd(t, *results, s);
    }

  if (results->size() == 0)
    {
       // no results as empty result
      verbose_printf("Got no results\n");
    }

  return results;
}



template<class T> HfstOneLevelPaths*
lookup_cascading(const HfstOneLevelPath& s, vector<T> cascade,
                 bool* infinity)
{
  HfstOneLevelPaths* kvs = new HfstOneLevelPaths;
  kvs->insert(s);
  for (unsigned int i = 0; i < cascade.size(); i++)
    {
      // cascade here
      HfstOneLevelPaths* newkvs = new HfstOneLevelPaths;
      for (HfstOneLevelPaths::const_iterator ckv = kvs->begin();
           ckv != kvs->end();
           ++ckv)
        {
          HfstOneLevelPaths* xyzkvs = lookup_simple<T>(*ckv, cascade[i],
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
          for (HfstOneLevelPaths::const_iterator xyzkv = xyzkvs->begin();
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
print_lookups(const HfstOneLevelPaths& kvs,
              const HfstOneLevelPath& kv, char* markup,
              bool outside_sigma, bool inf, FILE * ofile)
{
    if (outside_sigma)
      {
        lookup_printf(unknown_begin_setf, &kv, NULL, markup, ofile);
        lookup_printf(unknown_lookupf, &kv, NULL, markup, ofile);
        lookup_printf(unknown_end_setf, &kv, NULL, markup, ofile);
        no_analyses++;
      }
    else if (kvs.size() == 0)
      {
        lookup_printf(empty_begin_setf, &kv, NULL, markup, ofile);
        lookup_printf(empty_lookupf, &kv, NULL, markup, ofile);
        lookup_printf(empty_end_setf, &kv, NULL, markup, ofile);
        no_analyses++;
      }
    else if (inf)
      {
        analysed++;
        lookup_printf(infinite_begin_setf, &kv, NULL, markup, ofile);
        for (HfstOneLevelPaths::const_iterator lkv = kvs.begin();
                lkv != kvs.end();
                ++lkv)
          {
            HfstOneLevelPath lup = *lkv;
            lookup_printf(infinite_lookupf, &kv, &lup, markup, ofile);
            analyses++;
          }
        lookup_printf(infinite_end_setf, &kv, NULL, markup, ofile);
      }
    else
      {
        analysed++;

        lookup_printf(begin_setf, &kv, NULL, markup, ofile);
        for (HfstOneLevelPaths::const_iterator lkv = kvs.begin();
                lkv != kvs.end();
                ++lkv)
          {
            HfstOneLevelPath lup = *lkv;
            lookup_printf(lookupf, &kv, &lup, markup, ofile);
            analyses++;
        }
        lookup_printf(end_setf, &kv, NULL, markup, ofile);
      }
}

template<class T> HfstOneLevelPaths*
perform_lookups(HfstOneLevelPath& origin, std::vector<T>& cascade, 
                bool unknown, bool* infinite)
{
  HfstOneLevelPaths* kvs;
    if (!unknown)
      {
        if (cascade.size() == 1)
          {
            kvs = lookup_simple(origin, cascade[0], infinite);
          }
        else
         {
       kvs = lookup_cascading<T>(origin, cascade, infinite);
         }
      }
    else
      {
        kvs = new HfstOneLevelPaths;
      }
    return kvs;
}

int
process_stream(HfstInputStream& inputstream, FILE* outstream)
{
    std::vector<HfstTransducer> cascade;
    std::vector<HfstBasicTransducer> cascade_mut;
    bool internal_transducers=false;

    size_t transducer_n=0;
    StringVector mc_symbols;
    while (inputstream.is_good())
      {
        transducer_n++;
        HfstTransducer trans(inputstream);
        char* inputname = strdup(trans.get_name().c_str());
        if (strlen(inputname) <= 0)
          {
            inputname = strdup(inputfilename);
          }
        if (transducer_n==1)
          {
            verbose_printf("Reading %s...\n", inputname); 
          }
        else
          {
            verbose_printf("Reading %s...%zu\n", inputname,
                           transducer_n); 
          }

        // add multicharacter symbols to mc_symbols
        hfst::ImplementationType type = trans.get_type();
        if (type == hfst::SFST_TYPE || 
            type == hfst::TROPICAL_OPENFST_TYPE ||
            type == hfst::LOG_OPENFST_TYPE ||
            type == hfst::FOMA_TYPE)
          {
            HfstBasicTransducer basic(trans);
            for (HfstBasicTransducer::const_iterator it = basic.begin();
                 it != basic.end(); it++)
              {
                for (HfstBasicTransducer::HfstTransitions::const_iterator 
		       tr_it = it->second.begin();
                     tr_it != it->second.end(); tr_it++)
                  {
                    std::string mcs = tr_it->get_input_symbol();
                    if (mcs.size() > 1) {
                      mc_symbols.push_back(mcs);
              if (verbose)
                      fprintf(stderr, "adding mc symbol: %s\n", mcs.c_str());
                    }
                  }
              }
          }

        cascade.push_back(trans);
      }

    inputstream.close();

    // if transducer type is other than optimized_lookup,
    // convert to HfstBasicTransducer

    if (print_pairs && 
        (inputstream.get_type() == HFST_OL_TYPE || 
         inputstream.get_type() == HFST_OLW_TYPE) ) {
      fprintf(outstream, 
              "Error: pair printing not supported on "
              "       optimized lookup transducers, exiting program\n" );
      exit(1);
    }

    char* line = 0;
    size_t llen = 0;

    HfstStrings2FstTokenizer input_tokenizer(mc_symbols, 
					     std::string(epsilon_format));

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

        bool optimized_lookup=
          (cascade[0].get_type() == HFST_OL_TYPE ||
           cascade[0].get_type() == HFST_OLW_TYPE);

        HfstOneLevelPath* kv = line_to_lookup_path(&line, input_tokenizer,
                                                   &markup,
                                                   &unknown, optimized_lookup);
        HfstOneLevelPaths* kvs;
        try 
          {
            kvs = perform_lookups<HfstTransducer>(*kv, cascade, unknown,
                                                  &infinite);
            /* This does not throw an exception only if the type of the
               transducer is HFST_OL or HFST_OLW. */
          }
        // FunctionNotImplementedException
        catch (const HfstException e)
          {
            if (!internal_transducers)
              {
                warning(0, 0, 
                        "Lookup not supported on this automaton "
                        "format: converting to HFST basic transducer "
                        "and trying\n");
                for (unsigned int i=0; i<cascade.size(); i++) 
                  {
                    HfstBasicTransducer mut(cascade[i]);
                    cascade_mut.push_back(mut);
                  }
                internal_transducers = true;
              }
            kvs = perform_lookups<HfstBasicTransducer>(*kv, cascade_mut,
                                                         unknown,
                                                         &infinite);
          }
        if (not print_pairs) { 
          // printing was already done in function lookup_fd
          print_lookups(*kvs, *kv, markup, unknown, infinite, outstream);
        }
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
            "  infinite:`%s'`%s'`%s\n"
            "  epsilon: `%s', space: `%s', flags: %d\n",
            begin_setf, lookupf, end_setf,
            empty_begin_setf, empty_lookupf, empty_end_setf,
            unknown_begin_setf, unknown_lookupf, unknown_end_setf,
            infinite_begin_setf, infinite_lookupf, infinite_end_setf,
            epsilon_format, space_format, show_flags);
    // here starts the buffer handling part
    HfstInputStream* instream = NULL;
    try 
      {
        instream = (inputfile != stdin) ?
          new HfstInputStream(inputfilename) :
          new HfstInputStream();
      } 
    catch(const HfstException e)
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

