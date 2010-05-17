//! @file hfst-proc.cc
//!
//! @brief Transducer lookdown command line tool
//!
//! @author HFST Team, Brian Croom


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

#include "hfst2/hfst.h"
#include "hfst2/flag-diacritics/FlagDiacritics.h"

#include "hfst2/string/string.h"

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "hfst-common-unary-variables.h"
// add tools-specific variables here
static char* lookup_file_name;
static FILE* lookup_file;
static bool lookup_given = false;
static KeySet* sigma;
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

enum GenerationMode
{
  gm_clean,      // clear all
  gm_unknown,    // display unknown words, clear transfer and generation tags
  gm_all,        // display all
  gm_tagged      // tagged generation
};


static lookup_input_format input_format = APERTIUM_INPUT; //UTF8_TOKEN_INPUT;
static lookup_output_format output_format = APERTIUM_OUTPUT; //XEROX_OUTPUT;

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
static const char* APERTIUM_END_SETF = "$%m";
// ^notaword/*notaword$[apertium superblank markup]
static const char* APERTIUM_EMPTY_BEGIN_SETF = "^%i";
static const char* APERTIUM_EMPTY_LOOKUPF = "/*%i";
static const char* APERTIUM_EMPTY_END_SETF = "$%m";
// ¶[apertium superblank markup]
static const char* APERTIUM_UNKNOWN_BEGIN_SETF = " ";
static const char* APERTIUM_UNKNOWN_LOOKUPF = "%i%m";
static const char* APERTIUM_UNKNOWN_END_SETF = " ";
// ^0/0 NUM SG/...$
static const char* APERTIUM_INFINITE_BEGIN_SETF = "^%i";
static const char* APERTIUM_INFINITE_LOOKUPF = "/%l";
static const char* APERTIUM_INFINITE_END_SETF = "/...$";

// statistic counting
static unsigned long inputs = 0;
static unsigned long no_analyses = 0;
static unsigned long analysed = 0;
static unsigned long analyses = 0;

FlagDiacriticTable flag_diacritic_table;
HFST::KeySet flag_diacritic_set;

struct KeyVectorCmp
{
  bool operator() (const KeyVector * kv1, const KeyVector * kv2) const
  {
    if (kv1 == kv2)
      {
        return false;
      }
    else
      {
        return (*kv1 < *kv2);
      }
  }
};

typedef std::set<KeyVector*,KeyVectorCmp> KeyVectorSet;

int stream_mode = 0;
GenerationMode genmode = gm_all;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		   "compare results of lookup against reference set\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		   "  -d, --debug            Print debugging messages and results\n"
		);
#               endif
	print_common_unary_program_options(message_out);
	fprintf(message_out, 
			"  -I, --input-strings=SFILE        Read lookup strings from SFILE\n"
			"  -S, --spaces                     Use space separated tokens in strings\n"
            "                                   (deprecated, use -F)\n"
			"  -f, --format=FORMAT              Use FORMAT printing results sets\n"
            "  -F, --input-format=IFORMAT       Use IFORMAT parsing input\n"
			"  -r, --statistics                 Print statistics\n");
	fprintf(message_out,
		   "\n"
		   "If OUTFILE or INFILE is missing or -, "
		   "standard streams will be used.\n"
		   "FORMAT is one of {xerox,cg,apertium}, "
		   "apertium being default\n"
           "IFORMAT is one of {utf8,spaces,apertium}, "
           "default being apertium\n"
		   "\n"
		   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}

void
print_version(const char* program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(message_out, "%s 0.5 (%s)\n"
		   "Copyright (C) 2009 University of Helsinki,\n"
		   "License GPLv3: GNU GPL version 3 "
		   "<http://gnu.org/licenses/gpl.html>\n"
		   "This is free software: you are free to change and redistribute it.\n"
		   "There is NO WARRANTY, to the extent permitted by law.\n",
		program_name, PACKAGE_STRING);
}

string
removeTags(string const &str)
{
  int off = str.find('<');
  
  return str.substr(0, off);
}

int
parse_options(int argc, char** argv)
{
	// use of this function requires options are settable on global scope
	while (true)
	{
		static const struct option long_options[] =
		{
#include "hfst-common-options.h"
		  ,
#include "hfst-common-unary-options.h"
		  ,
		  // add tool-specific options here
			{"input-strings", required_argument, 0, 'I'},
			{"spaces", no_argument, 0, 'S'},
			{"generation", no_argument, 0, 'g'},
			{"debugged-gen", no_argument, 0, 'd'},
			{"format", required_argument, 0, 'f'},
  	            {"input-format", required_argument, 0, 'F'},
			{"statistics", no_argument, 0, 'r'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "gdf:F:hi:I:o:sSqvVrR:DW:t:",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		switch (c)
		{
//#include "hfst-common-cases.h"
#include "hfst-common-unary-cases.h"
		  // add tool-specific cases here
		case 'I':
			lookup_file_name = hfst_strdup(optarg);
			lookup_file = hfst_fopen(lookup_file_name, "r");
			lookup_given = true;
            break;
		case 'S':
			input_format = SPACE_SEPARATED_TOKEN_INPUT;
			break;
		case 'd': 
			stream_mode = 1;
			genmode = gm_tagged;
                        break;
		case 'g': 
			stream_mode = 1;
                        break;
		case 'f':
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
                fprintf(stderr, "Unknown output format %s; valid values are: "
                        "xerox, cg, apertium\n", optarg);
                print_short_help(argv[0]);
                return EXIT_FAILURE;
              }
			break;
        case 'F':
            if (strcmp(optarg, "utf8"))
              {
                input_format = UTF8_TOKEN_INPUT;
              }
            else if (strcmp(optarg, "spaces"))
              {
                input_format = SPACE_SEPARATED_TOKEN_INPUT;
				fprintf(stderr, "Warning: option -S is being deprecated in "
						"favor of -F spaces\n");
              }
            else if (strcmp(optarg, "apertium"))
              {
                input_format = APERTIUM_INPUT;
              }
            else
              {
                fprintf(stderr, "Unknown input format %s; valid values are:"
                        "utf8, spaces, apertium\n", optarg);
                print_short_help(argv[0]);
                return EXIT_FAILURE;
              }
            break;
		case 'r':
			print_statistics = true;
			break;
		case '?':
			fprintf(message_out, "invalid option --%s\n",
					long_options[option_index].name);
			print_short_help(argv[0]);
			return EXIT_FAILURE;
			break;
		default:
			fprintf(message_out, "invalid option -%c\n", c);
			print_short_help(argv[0]);
			return EXIT_FAILURE;
			break;
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
	if (is_output_stdout)
	{
			outfilename = hfst_strdup("<stdout>");
			outfile = stdout;
	}
	message_out = outfile;
	if (!lookup_given)
	{
		lookup_file = stdin;
		lookup_file_name = hfst_strdup("<stdin>");
	}
	// rest of arguments are files...
	if (is_input_stdin && ((argc - optind) == 1))
	{
		inputfilename = hfst_strdup(argv[optind]);
		inputfile = hfst_fopen(inputfilename, "r");
		is_input_stdin = false;
	}
	else if (inputfile) {

	}
	else if ((argc - optind) == 0)
	{
		inputfilename = hfst_strdup("<stdin>");
		inputfile = stdin;
		is_input_stdin = true;
	}
	else if ((argc - optind) > 1)
	{
		fprintf(message_out, "Exactly one input transducer file must be given\n");
		print_short_help(argv[0]);
		return EXIT_FAILURE;
	}
	else
	{
		fprintf(message_out, "???\n");
		return 73;
	}
	return EXIT_CONTINUE;
}

void define_flag_diacritics(HFST::KeyTable * key_table)
{
  
  for (HFST::Key k = 0; k < key_table->get_unused_key(); ++k)
    {
      flag_diacritic_table.define_diacritic
	(k,HFST::get_symbol_name(HFST::get_key_symbol(k,key_table)));
      if (flag_diacritic_table.is_diacritic(k))
	{ flag_diacritic_set.insert(k); }
    }
}
int
lookup_printf(const char* format, const char* inputform, const char* lookupform, const char* markup)
{
	size_t space = 2 * strlen(format) +
		2 * strlen(inputform) + 10;
	if (lookupform != NULL)
	{
		space += 2 * strlen(lookupform);
	}
    if (markup != NULL)
      {
        space += 2 * strlen(markup);
      }
	char* result = static_cast<char*>(calloc(sizeof(char), space + 1));
	size_t space_left = space;
	const char* src = format;
	char* dst = result;
	char* i; // %i
	char* l; // %l
	char* b; // %b
	char* a; // %a
    char* m; // %m
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
	int rv = fprintf(message_out, "%s", result);
	free(result);
	return rv;
}

/**
 get_u8_char - Read the next UTF-8 encoded character from a stream and
               return it as a newly-allocated character array (len 1-4)
 */
char* 
get_u8_char(FILE* stream) {
	unsigned short u8len = 0;
	unsigned char c1 = static_cast<unsigned char>(getc(stream));
	
	if (c1 <= 127) {
		u8len = 1;
	}
	else if ( (c1 & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16) ) {
		u8len = 4;
	}
	else if ( (c1 & (128 + 64 + 32 )) == (128 + 64 + 32) ) {
		u8len = 3;
	}
	else if ( (c1 & (128 + 64 )) == (128 + 64)) {
		u8len = 2;
	}
	else
	{
	  fprintf(stderr, "%c not valid start of UTF-8\n", c1);
	  assert(false);
	  return 0;
	}
	
	char* nextu8 = static_cast<char*>(malloc(sizeof(char)*u8len+1));
	nextu8[0] = static_cast<char>(c1);
	nextu8[u8len] = '\0';
	
	char* nextptr = nextu8+1;
	while(u8len > 1) {
		*nextptr = getc(stream);
		nextptr++;
		u8len--;
	}

	return nextu8;
}

void
print_keyset(KeySet* ks, KeyTable* kt) {
	puts("Printing KeySet:");
	for (KeySet::const_iterator k = ks->begin();
             k != ks->end();
             ++k)
    {
    	string* str = HFST::keyToString(*k, kt);
    	printf("%d / %s\n", *k, str->c_str());
    	delete str;
    }
}

namespace HFST
{

bool
_is_epsilon(Key k)
{
	return k == 0;
}

KeyVector*
line_to_keyvector(char** s, KeyTable* kt, char** markup, bool* outside_sigma)
{
	KeyVector* rv = NULL;
    *outside_sigma = false;
	inputs++;
    switch (input_format)
      {
      case SPACE_SEPARATED_TOKEN_INPUT:
		rv = stringSeparatedToKeyVector(*s, kt, string(" "), true);
        break;
      case UTF8_TOKEN_INPUT:
		rv = stringUtf8ToKeyVector(*s, kt, true);
        break;
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
        rv = stringUtf8ToKeyVector(sp, kt, true);
        free(*s);
        *s = sp;
        break;
          }
      default:
        fprintf(stderr, "Unknown input format");
        break;
      } // switch input format
	// may be NULL
	if (rv == NULL)
	{
		VERBOSE_PRINT("No tokenisations for %s\n", *s);
        *outside_sigma = true;
	}
    else
      {
        for (KeyVector::const_iterator k = rv->begin();
             k != rv->end();
             ++k)
          {
            if (sigma->find(*k) == sigma->end())
              {
                *outside_sigma = true;
                break;
              }
          }
      }
    return rv;
}

KeyVectorSet*
lookup_unique(KeyVector* kv, TransducerHandle t,
              KeyTable* kt, bool* infinity)
{
  KeyVectorVector* lookups;
  KeyVectorSet* results = new KeyVectorSet;
  if (is_infinitely_ambiguous(t, true, kv))
    {
      fprintf(message_out, "WARNING: Got infinite results\n");
      lookups = new KeyVectorVector;
      lookups->push_back(lookup_first(t, kv, &flag_diacritic_set));
      *infinity = true;
    }
  else
    {
      lookups = lookup_all(t, kv, &flag_diacritic_set);
    }
  if (lookups == NULL)
    {
       // no results as empty result
      VERBOSE_PRINT("Got no results\n");
      lookups = new KeyVectorVector;
    }
  for (KeyVectorVector::iterator lkv = lookups->begin();
       lkv != lookups->end();
       ++lkv)
    {
      KeyVector* hmmlkv = *lkv;
      KeyVector* filtlkv = flag_diacritic_table.filter_diacritics(hmmlkv);
      if (filtlkv == NULL)
        {
          VERBOSE_PRINT("Filtered by flag diacritics\n");
          continue;
        }
      filtlkv->erase(remove_if(filtlkv->begin(), filtlkv->end(),
                               _is_epsilon), filtlkv->end());
      if (results->find(filtlkv) == results->end())
        {
          results->insert(filtlkv);
        }
      else
        {
          delete filtlkv;
        }
    }
  delete lookups;
  return results;
}

KeyVectorSet*
lookup_cascade_unique(KeyVector* kv, vector<TransducerHandle> cascade,
                      KeyTable* kt, bool* infinity)
{
  KeyVectorSet* kvs = new KeyVectorSet;
  kvs->insert(new KeyVector(*kv));
  for (unsigned int i = 0; i < cascade.size(); i++)
    {
      // cascade here
      KeyVectorSet* newkvs = new KeyVectorSet;
      for (KeyVectorSet::const_iterator ckv = kvs->begin();
           ckv != kvs->end();
           ++ckv)
        {
          KeyVectorSet* xyzkvs = lookup_unique(*ckv,
                                                      cascade[i],
                                                      kt,
                                                      infinity);
          VERBOSE_PRINT("%zu results @ level %u\n", xyzkvs->size(), i);
          for (KeyVectorSet::const_iterator xyzkv = xyzkvs->begin();
               xyzkv != xyzkvs->end();
               ++xyzkv)
            {
              if (newkvs->find(*xyzkv) == newkvs->end())
                {
                  newkvs->insert(*xyzkv);
                }
              else
                {
                  KeyVector* hmmkv = *xyzkv;
                  delete hmmkv;
                }

            }
          delete xyzkvs;
        }
      for (KeyVectorSet::const_iterator nkv = kvs->begin();
           nkv != kvs->end();
           ++nkv)
        {
          KeyVector* hmmkv = *nkv;
          delete hmmkv;
        }
      delete kvs;
      kvs = newkvs;
   }
  return kvs;
}

void
print_lookups(KeyVectorSet* kvs, KeyTable* kt, const char* s, char* markup,
              bool outside_sigma, bool inf)
{
    if (outside_sigma)
      {
        lookup_printf(unknown_begin_setf, s, NULL, markup);
        lookup_printf(unknown_lookupf, s, NULL, markup);
        lookup_printf(unknown_end_setf, s, NULL, markup);
        no_analyses++;
      }
    else if (kvs->size() == 0)
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
        for (KeyVectorSet::iterator lkv = kvs->begin();
                lkv != kvs->end();
                ++lkv)
        {
            KeyVector* hmmlkv = *lkv;
            string* lkvstr = keyVectorToString(hmmlkv, kt);
            const char* lookup_full = lkvstr->c_str();
            lookup_printf(infinite_lookupf, s, lookup_full, markup);
            delete lkvstr;
            analyses++;
        }
        lookup_printf(infinite_end_setf, s, NULL, markup);
      }
    else
      {
        analysed++;

        lookup_printf(begin_setf, s, NULL, markup);
        for (KeyVectorSet::iterator lkv = kvs->begin();
                lkv != kvs->end();
                ++lkv)
        {
            KeyVector* hmmlkv = *lkv;
            string* lkvstr = keyVectorToString(hmmlkv, kt);
            const char* lookup_full = lkvstr->c_str();
            lookup_printf(lookupf, s, lookup_full, markup);
            delete lkvstr;
            analyses++;
        }
        lookup_printf(end_setf, s, NULL, markup);
      }
}

}

namespace HWFST
{
bool
_is_epsilon(Key k)
{
	return k == 0;
}

KeyVector*
line_to_keyvector(char** s, KeyTable* kt, char** markup, bool* outside_sigma)
{
	KeyVector* rv = NULL;
    *outside_sigma = false;
	inputs++;
    switch (input_format)
      {
      case SPACE_SEPARATED_TOKEN_INPUT:
		rv = stringSeparatedToKeyVector(*s, kt, string(" "), true);
        break;
      case UTF8_TOKEN_INPUT:
		rv = stringUtf8ToKeyVector(*s, kt, true);
        break;
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
        rv = stringUtf8ToKeyVector(sp, kt, true);
        free(*s);
        *s = sp;
        break;
          }
      default:
        fprintf(stderr, "Unknown input format");
        break;
      } // switch input format
	// may be NULL
	if (rv == NULL)
	{
		VERBOSE_PRINT("No tokenisations for %s\n", *s);
        *outside_sigma = true;
	}
    else
      {
        for (KeyVector::const_iterator k = rv->begin();
             k != rv->end();
             ++k)
          {
            if (sigma->find(*k) == sigma->end())
              {
                *outside_sigma = true;
                break;
              }
          }
      }
    return rv;
}

KeyVectorSet*
lookup_unique(KeyVector* kv, TransducerHandle t,
              KeyTable* kt, bool* infinity)
{
  KeyVectorVector* lookups;
  KeyVectorSet* results = new KeyVectorSet;
  if (is_infinitely_ambiguous(t, true, kv))
    {
      fprintf(message_out, "WARNING: Got infinite results\n");
      lookups = new KeyVectorVector;
      lookups->push_back(lookup_first(t, kv, &flag_diacritic_set));
      *infinity = true;
    }
  else
    {
      lookups = lookup_all(t, kv, &flag_diacritic_set);
    }
  if (lookups == NULL)
    {
       // no results as empty result
      VERBOSE_PRINT("Got no results\n");
      lookups = new KeyVectorVector;
    }
  for (KeyVectorVector::iterator lkv = lookups->begin();
       lkv != lookups->end();
       ++lkv)
    {
      KeyVector* hmmlkv = *lkv;
      KeyVector* filtlkv = flag_diacritic_table.filter_diacritics(hmmlkv);
      if (filtlkv == NULL)
        {
          VERBOSE_PRINT("Filtered by flag diacritics\n");
          continue;
        }
      filtlkv->erase(remove_if(filtlkv->begin(), filtlkv->end(),
                               _is_epsilon), filtlkv->end());
      if (results->find(filtlkv) == results->end())
        {
          results->insert(filtlkv);
        }
      else
        {
          delete filtlkv;
        }
    }
  delete lookups;
  return results;
}

KeyVectorSet*
lookup_cascade_unique(KeyVector* kv, vector<TransducerHandle> cascade,
                      KeyTable* kt, bool* infinity)
{
  KeyVectorSet* kvs = new KeyVectorSet;
  kvs->insert(new KeyVector(*kv));
  for (unsigned int i = 0; i < cascade.size(); i++)
    {
      // cascade here
      KeyVectorSet* newkvs = new KeyVectorSet;
      for (KeyVectorSet::const_iterator ckv = kvs->begin();
           ckv != kvs->end();
           ++ckv)
        {
          KeyVectorSet* xyzkvs = HWFST::lookup_unique(*ckv,
                                                      cascade[i],
                                                      kt,
                                                      infinity);
          VERBOSE_PRINT("%zu results @ level %u\n", xyzkvs->size(), i);
          for (KeyVectorSet::const_iterator xyzkv = xyzkvs->begin();
               xyzkv != xyzkvs->end();
               ++xyzkv)
            {
              if (newkvs->find(*xyzkv) == newkvs->end())
                {
                  newkvs->insert(*xyzkv);
                }
              else
                {
                  KeyVector* hmmkv = *xyzkv;
                  delete hmmkv;
                }

            }
          delete xyzkvs;
        }
      for (KeyVectorSet::const_iterator nkv = kvs->begin();
           nkv != kvs->end();
           ++nkv)
        {
          KeyVector* hmmkv = *nkv;
          delete hmmkv;
        }
      delete kvs;
      kvs = newkvs;
   }
  return kvs;
}

void
print_lookups(KeyVectorSet* kvs, KeyTable* kt, const char* s, char* markup,
              bool outside_sigma, bool inf)
{
    if (outside_sigma)
      {
        lookup_printf(unknown_begin_setf, s, NULL, markup);
        lookup_printf(unknown_lookupf, s, NULL, markup);
        lookup_printf(unknown_end_setf, s, NULL, markup);
        no_analyses++;
      }
    else if (kvs->size() == 0)
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
        for (KeyVectorSet::iterator lkv = kvs->begin();
                lkv != kvs->end();
                ++lkv)
        {
            KeyVector* hmmlkv = *lkv;
            string* lkvstr = keyVectorToString(hmmlkv, kt);
            const char* lookup_full = lkvstr->c_str();
            lookup_printf(infinite_lookupf, s, lookup_full, markup);
            delete lkvstr;
            analyses++;
        }
        lookup_printf(infinite_end_setf, s, NULL, markup);
      }
    else
      {
        analysed++;

        lookup_printf(begin_setf, s, NULL, markup);
        for (KeyVectorSet::iterator lkv = kvs->begin();
                lkv != kvs->end();
                ++lkv)
        {
            KeyVector* hmmlkv = *lkv;
            string* lkvstr = keyVectorToString(hmmlkv, kt);
            const char* lookup_full = lkvstr->c_str();
            lookup_printf(lookupf, s, lookup_full, markup);
            delete lkvstr;
            analyses++;
        }
        lookup_printf(end_setf, s, NULL, markup);
      }
}

}

/**
 tokenize_on - determine whether the character (UTF-8) passed is considered
				loosely non-alphabetic, and thus should be considered a
				break for tokenization purposes.
  */
bool
tokenize_on(const char* u8char)
{
	static const int num_chars = 10;
	static const char* t[num_chars] = {" ", ",", ".", "[", ":", ";", "\"", "'", "`", "!"};
	
	for(int i=0;i<num_chars;i++)
		if(strcmp(u8char, t[i]) == 0)
			return true;
	
	return false;
}

/**
 pass_through_superblank - once a '[' has been found in the stream call this
 							method to read the superblank and send it to the
 							output file
 */
void
pass_through_superblank()
{
	fputc('[', message_out); // output the '[' already read from the stream
	
	while(true) // until the end of the input or an unescaped ']'
	{
		int c = getc(lookup_file);
		
		if (!c || c == EOF) break;
		
		if (c == '\\')
		{
			int next = getc(lookup_file);
			if (next == ']') putc(next, message_out);
			else
			{
				putc(c, message_out);
				putc(next, message_out);
			}
			continue;
		}
		
		putc(c, message_out);
		
		if (c == ']') break;
	}
}

int
process_stream_generation(std::istream& inputstream, std::ostream& outstream)
{
        //std::cerr << "Generation" << std::endl;
	VERBOSE_PRINT("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
	if (format_type == SFST_FORMAT)
	{
		VERBOSE_PRINT("Using unweighted format\n");
		try 
		{
			sigma = new KeySet;
			HFST::KeyTable *key_table;
			if (read_symbols_from_filename != NULL) 
			{
				ifstream is(read_symbols_from_filename);
				key_table = HFST::read_symbol_table(is);
				is.close();
			}
			else
				key_table = HFST::create_key_table();
			bool transducer_has_symbol_table=false;
			HFST::TransducerHandle input = NULL;
			size_t nth_stream = 0;
			std::vector<HFST::TransducerHandle> cascade;
			while (true) {
				int inputformat = HFST::read_format(inputstream);
				nth_stream++;
				if (inputformat == EOF_FORMAT)
				{
					break;
				}
				else if (inputformat == SFST_FORMAT)
				{
					transducer_has_symbol_table = HFST::has_symbol_table(inputstream);
					input = HFST::read_transducer(inputstream, key_table);
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				if (nth_stream < 2)
				{
					VERBOSE_PRINT("Reading cascade...\r");
				}
				else
				{
					VERBOSE_PRINT("Reading cascade... %zu\r", nth_stream);
				}
				// add your code here
				cascade.push_back(input);
			}
			define_flag_diacritics(key_table);
            sigma = HFST::get_key_set(key_table);
            VERBOSE_PRINT("\n");

            // go through character by character, converting each character
            // into a Key. Add each Key to a KeyVector until a Key is found
            // which is not in the alphabet. Then tokenize.

	HFST::KeyVector key_vec;
	string token;
	bool inWord = false;
			
	while (true) // while there is input
	{
		char* utf8_char = get_u8_char(lookup_file);
            	
            	// if we have the escape character, skip it and use the next
		if (strcmp(utf8_char, "\\") == 0)
		{
            		free(utf8_char);
            		utf8_char = get_u8_char(lookup_file);
            	}

		if(utf8_char[0] == '^') 
		{
			inWord = true;
			token.clear();
			key_vec.clear();
			continue;
		}

            	if (!utf8_char[0] || utf8_char[0] == EOF)
            	{
            		VERBOSE_PRINT("Breaking..\n");
            		break;
            	}

		HFST::KeyVector* kv;
		if(utf8_char[0] == '<' && inWord)  
		{
			string buf;
            		token.append(utf8_char);
            		buf.append(utf8_char);
			while(utf8_char[0] != '>')
			{
				utf8_char = get_u8_char(lookup_file);
				if(utf8_char[0] != '>') 
				{
            				token.append(utf8_char);
				}
				buf.append(utf8_char);
			}
            		kv = HFST::stringUtf8ToKeyVector(buf.c_str(), key_table, true);
		} else {
        	    	kv = HFST::stringUtf8ToKeyVector(utf8_char, key_table, true);
		}
		//std::cerr << inWord << ": " << utf8_char << ": tok: " << token << " kv: " << key_vec.size() << std::endl;

            	if (kv->size() > 1 && utf8_char[0] != '>')  // ^@ja<Conj>$
            	{
            		fprintf(stderr, "UTF-8 character %s converted to multiple keys\n", utf8_char);
            		assert(false);
            		return EXIT_FAILURE;
            	}

		// if the character is out of the alphabet or is whitespace/punctuation
            	if (strcmp(utf8_char, "$") == 0 && inWord)
            	{
			inWord = false;

			if (!key_vec.empty()) 
			{
				// tokenize
		       		VERBOSE_PRINT("Looking up '%s'...\n", token.c_str());

	        		KeyVectorSet* kvs;
	        		bool infinite = false;

	        		if (cascade.size() == 1)
				{
		                    kvs  = HFST::lookup_unique(&key_vec, cascade[0],
		                                                key_table, &infinite);
				}
		                else
				{
		                    kvs = HFST::lookup_cascade_unique(&key_vec, cascade,
		                                                       key_table, 
		                                                       &infinite);
				}

		                //std::cerr << "+ tok: " << token << "; " << kvs->empty() << "; " << key_vec.size() << std::endl;
				if(kvs->empty()) 
				{
					if(genmode != gm_tagged) 
					{
						token = removeTags(token);
					}
					// unknown word in generation
					if(token.c_str()[0] == '@' || token.c_str()[0] == '*') 
					{
						std::cout << token;
					}
					else
					{
						std::cout << "#" << token;
					}
				}
				bool first = true;
			        for (KeyVectorSet::iterator lkv = kvs->begin(); lkv != kvs->end(); ++lkv)
			        {
					KeyVector* hmmlkv = *lkv;
					string* lkvstr = HFST::keyVectorToString(hmmlkv, key_table);
                                        //std::cerr << "lkvstr: " << lkvstr->c_str() << std::endl;
					const char* lookup_full = lkvstr->c_str();
					if(first)
					{
						std::cout << lookup_full;
					}
					else	
					{
						std::cout << "/" << lookup_full;
					}
					delete lkvstr;
					first = false;
				}

		            	for (KeyVectorSet::const_iterator rkv = kvs->begin(); rkv != kvs->end(); ++rkv)
		             	{
		          		KeyVector* hmmkv = *rkv;
		               		delete hmmkv;
		             	}
		           	delete kvs;
		           	key_vec.clear();
		       	        token.clear();
				continue;
			} 
		           	
			// pass through superblanks untouched
			if (strcmp(utf8_char, "[") == 0)
			{
				inWord = false;
				pass_through_superblank();
			}
		        else
		        {
			        // output the non-alphabetic character
				inWord = false;
            			fputs(utf8_char, message_out);
		        }
            	}
		else if(!inWord) 
		{
			fputs(utf8_char, message_out);
			continue;
		}
            	else
            	{
            		key_vec.push_back(kv->front());
            		token.append(utf8_char);
            	}
            	
            	delete kv;
            	free(utf8_char);

		}  // while not at end of input
            
		// Cleanup
		for (vector<TransducerHandle>::iterator t = cascade.begin();
                 t != cascade.end();
                 ++t)
              {
                HFST::delete_transducer(*t);
              }
			if (write_symbols_to_filename != NULL) {
			  ofstream os(write_symbols_to_filename);
			  HFST::write_symbol_table(key_table, os);
			  os.close();
			}
		}
		catch (const char *p)
		{
			printf("HFST library error: %s\n", p);
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
    else if (format_type == OPENFST_FORMAT) 
    {
        VERBOSE_PRINT("Using weighted format\n");
        try
        {
            HWFST::KeyTable *key_table;
            if (read_symbols_from_filename != NULL) 
            {
                ifstream is(read_symbols_from_filename);
                key_table = HWFST::read_symbol_table(is);
                is.close();
            }
            else
                key_table = HWFST::create_key_table();
            bool transducer_has_symbol_table=false;
            HWFST::TransducerHandle input = NULL;
            size_t nth_stream = 0;
            std::vector<HWFST::TransducerHandle> cascade;
            while (true) {
                int inputformat = HWFST::read_format(inputstream);
                nth_stream++;
                if (inputformat == EOF_FORMAT)
                {
                    break;
                }
                else if (inputformat == OPENFST_FORMAT)
                {
                    transducer_has_symbol_table = HWFST::has_symbol_table(inputstream);
                    input = HWFST::read_transducer(inputstream, key_table);
                }
                else
                {
                    fprintf(message_out, "stream format mismatch\n");
                    return EXIT_FAILURE;
                }
                if (nth_stream < 2)
                {
                    VERBOSE_PRINT("Reading cascade...\r");
                }
                else
                {
                    VERBOSE_PRINT("Reading cascade... %zu\r", nth_stream);
                }
                // add your code here
                cascade.push_back(input);
            }
            define_flag_diacritics(key_table);
            sigma = HWFST::get_key_set(key_table);
            VERBOSE_PRINT("\n");
            
            // go through character by character, converting each character
            // into a Key. Add each Key to a KeyVector until a Key is found
            // which is not in the alphabet. Then tokenize.

			HWFST::KeyVector key_vec;
			string token;
			
            while (true) // while there is input
            {
            	char* utf8_char = get_u8_char(lookup_file);
            	
            	// if we have the escape character, skip it and use the next
            	if (strcmp(utf8_char, "\\") == 0)
            	{
            		free(utf8_char);
            		utf8_char = get_u8_char(lookup_file);
            	}

            	if (!utf8_char[0] || utf8_char[0] == EOF)
            	{
            		VERBOSE_PRINT("Breaking..\n");
            		break;
            	}

            	HWFST::KeyVector* kv = HWFST::stringUtf8ToKeyVector(utf8_char, key_table, true);
            	if (kv->size() > 1) 
            	{
            		fprintf(stderr, "UTF-8 character %s converted to multiple keys\n", utf8_char);
            		assert(false);
            		return EXIT_FAILURE;
            	}

				// if the character is out of the alphabet or is whitespace/punctuation
            	if (sigma->find(kv->front()) == sigma->end() || tokenize_on(utf8_char))
            	{
            		if (!key_vec.empty()) {
		        		// tokenize
		        		VERBOSE_PRINT("Looking up '%s'...\n", token.c_str());

		        		KeyVectorSet* kvs;
		        		bool infinite = false;

		        		if (cascade.size() == 1)
		                  {
		                    kvs  = HWFST::lookup_unique(&key_vec, cascade[0],
		                                                key_table, &infinite);
		                   }
		                else
		                  {
		                    kvs = HWFST::lookup_cascade_unique(&key_vec, cascade,
		                                                       key_table, \
		                                                       &infinite);
		                  }

		                HWFST::print_lookups(kvs, key_table, token.c_str(), NULL, false, infinite);
		            	for (KeyVectorSet::const_iterator rkv = kvs->begin();
		                	rkv != kvs->end();
		                	++rkv)
		             	{
		          			KeyVector* hmmkv = *rkv;
		               		delete hmmkv;
		             	}
		           		delete kvs;
		           		key_vec.clear();
		                token.clear();
		           	}
		           	
				    // pass through superblanks untouched
		        	if (strcmp(utf8_char, "[") == 0)
		        	{
		        		pass_through_superblank();
		        	}
		        	else
		        	{
		        		// output the non-alphabetic character
            			fputs(utf8_char, message_out);
		        	}
            	}
            	else
            	{
            		key_vec.push_back(kv->front());
            		token.append(utf8_char);
            	}
            	
            	delete kv;
            	free(utf8_char);
            } // while not at end of input
            
			// Cleanup

            if (print_statistics)
            {
                fprintf(message_out, "Strings\tFound\tMissing\tResults\n"
                        "%lu\t%lu\t%lu\t%lu\n", 
                        inputs, analysed, no_analyses, analyses);
                fprintf(message_out, "Coverage\tAmbiguity\n"
                        "%f\t%f\n",
                        (float)analysed/(float)inputs,
                        (float)analyses/(float)inputs);
            }
            for (vector<TransducerHandle>::iterator t = cascade.begin();
                 t != cascade.end();
                 ++t)
              {
                HWFST::delete_transducer(*t);
              }
            if (write_symbols_to_filename != NULL) {
              ofstream os(write_symbols_to_filename);
              HWFST::write_symbol_table(key_table, os);
              os.close();
            }
        }
        catch (const char *p)
        {
            printf("HFST library error: %s\n", p);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(message_out, "ERROR: Transducer has wrong type.\n");
        return EXIT_FAILURE;
    }
}



int
process_stream(std::istream& inputstream, std::ostream& outstream)
{
	VERBOSE_PRINT("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
	if (format_type == SFST_FORMAT)
	{
		VERBOSE_PRINT("Using unweighted format\n");
		try 
		{
            sigma = new KeySet;
			HFST::KeyTable *key_table;
			if (read_symbols_from_filename != NULL) 
			{
				ifstream is(read_symbols_from_filename);
				key_table = HFST::read_symbol_table(is);
				is.close();
			}
			else
				key_table = HFST::create_key_table();
			bool transducer_has_symbol_table=false;
			HFST::TransducerHandle input = NULL;
			size_t nth_stream = 0;
			std::vector<HFST::TransducerHandle> cascade;
			while (true) {
				int inputformat = HFST::read_format(inputstream);
				nth_stream++;
				if (inputformat == EOF_FORMAT)
				{
					break;
				}
				else if (inputformat == SFST_FORMAT)
				{
					transducer_has_symbol_table = HFST::has_symbol_table(inputstream);
					input = HFST::read_transducer(inputstream, key_table);
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				if (nth_stream < 2)
				{
					VERBOSE_PRINT("Reading cascade...\r");
				}
				else
				{
					VERBOSE_PRINT("Reading cascade... %zu\r", nth_stream);
				}
				// add your code here
				cascade.push_back(input);
			}
			define_flag_diacritics(key_table);
            sigma = HFST::get_key_set(key_table);
            VERBOSE_PRINT("\n");

            // go through character by character, converting each character
            // into a Key. Add each Key to a KeyVector until a Key is found
            // which is not in the alphabet. Then tokenize.

			HFST::KeyVector key_vec;
			string token;
			
            while (true) // while there is input
            {
            	char* utf8_char = get_u8_char(lookup_file);
            	
            	// if we have the escape character, skip it and use the next
            	if (strcmp(utf8_char, "\\") == 0)
            	{
            		free(utf8_char);
            		utf8_char = get_u8_char(lookup_file);
            	}

            	if (!utf8_char[0] || utf8_char[0] == EOF)
            	{
            		VERBOSE_PRINT("Breaking..\n");
            		break;
            	}

            	HFST::KeyVector* kv = HFST::stringUtf8ToKeyVector(utf8_char, key_table, true);
            	if (kv->size() > 1) 
            	{
            		fprintf(stderr, "UTF-8 character %s converted to multiple keys\n", utf8_char);
            		assert(false);
            		return EXIT_FAILURE;
            	}

				// if the character is out of the alphabet or is whitespace/punctuation
            	if (sigma->find(kv->front()) == sigma->end() || tokenize_on(utf8_char))
            	{
            		if (!key_vec.empty()) {
		        		// tokenize
		        		VERBOSE_PRINT("Looking up '%s'...\n", token.c_str());

		        		KeyVectorSet* kvs;
		        		bool infinite = false;

		        		if (cascade.size() == 1)
		                  {
		                    kvs  = HFST::lookup_unique(&key_vec, cascade[0],
		                                                key_table, &infinite);
		                   }
		                else
		                  {
		                    kvs = HFST::lookup_cascade_unique(&key_vec, cascade,
		                                                       key_table, \
		                                                       &infinite);
		                  }

		                HFST::print_lookups(kvs, key_table, token.c_str(), NULL, false, infinite);
		            	for (KeyVectorSet::const_iterator rkv = kvs->begin();
		                	rkv != kvs->end();
		                	++rkv)
		             	{
		          			KeyVector* hmmkv = *rkv;
		               		delete hmmkv;
		             	}
		           		delete kvs;
		           		key_vec.clear();
		                token.clear();
		           	}
		           	
				    // pass through superblanks untouched
		        	if (strcmp(utf8_char, "[") == 0)
		        	{
		        		pass_through_superblank();
		        	}
		        	else
		        	{
		        		// output the non-alphabetic character
            			fputs(utf8_char, message_out);
		        	}
            	}
            	else
            	{
            		key_vec.push_back(kv->front());
            		token.append(utf8_char);
            	}
            	
            	delete kv;
            	free(utf8_char);
            } // while not at end of input
            
			// Cleanup

            if (print_statistics)
            {
                fprintf(message_out, "Strings\tFound\tMissing\tResults\n"
                        "%lu\t%lu\t%lu\t%lu\n", 
                        inputs, analysed, no_analyses, analyses);
                fprintf(message_out, "Coverage\tAmbiguity\n"
                        "%f\t%f\n",
                        (float)analysed/(float)inputs,
                        (float)analyses/(float)inputs);
            }
            for (vector<TransducerHandle>::iterator t = cascade.begin();
                 t != cascade.end();
                 ++t)
              {
                HFST::delete_transducer(*t);
              }
			if (write_symbols_to_filename != NULL) {
			  ofstream os(write_symbols_to_filename);
			  HFST::write_symbol_table(key_table, os);
			  os.close();
			}
		}
		catch (const char *p)
		{
			printf("HFST library error: %s\n", p);
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
    else if (format_type == OPENFST_FORMAT) 
    {
        VERBOSE_PRINT("Using weighted format\n");
        try
        {
            HWFST::KeyTable *key_table;
            if (read_symbols_from_filename != NULL) 
            {
                ifstream is(read_symbols_from_filename);
                key_table = HWFST::read_symbol_table(is);
                is.close();
            }
            else
                key_table = HWFST::create_key_table();
            bool transducer_has_symbol_table=false;
            HWFST::TransducerHandle input = NULL;
            size_t nth_stream = 0;
            std::vector<HWFST::TransducerHandle> cascade;
            while (true) {
                int inputformat = HWFST::read_format(inputstream);
                nth_stream++;
                if (inputformat == EOF_FORMAT)
                {
                    break;
                }
                else if (inputformat == OPENFST_FORMAT)
                {
                    transducer_has_symbol_table = HWFST::has_symbol_table(inputstream);
                    input = HWFST::read_transducer(inputstream, key_table);
                }
                else
                {
                    fprintf(message_out, "stream format mismatch\n");
                    return EXIT_FAILURE;
                }
                if (nth_stream < 2)
                {
                    VERBOSE_PRINT("Reading cascade...\r");
                }
                else
                {
                    VERBOSE_PRINT("Reading cascade... %zu\r", nth_stream);
                }
                // add your code here
                cascade.push_back(input);
            }
            define_flag_diacritics(key_table);
            sigma = HWFST::get_key_set(key_table);
            VERBOSE_PRINT("\n");
            
            // go through character by character, converting each character
            // into a Key. Add each Key to a KeyVector until a Key is found
            // which is not in the alphabet. Then tokenize.

			HWFST::KeyVector key_vec;
			string token;
			
            while (true) // while there is input
            {
            	char* utf8_char = get_u8_char(lookup_file);
            	
            	// if we have the escape character, skip it and use the next
            	if (strcmp(utf8_char, "\\") == 0)
            	{
            		free(utf8_char);
            		utf8_char = get_u8_char(lookup_file);
            	}

            	if (!utf8_char[0] || utf8_char[0] == EOF)
            	{
            		VERBOSE_PRINT("Breaking..\n");
            		break;
            	}

            	HWFST::KeyVector* kv = HWFST::stringUtf8ToKeyVector(utf8_char, key_table, true);
            	if (kv->size() > 1) 
            	{
            		fprintf(stderr, "UTF-8 character %s converted to multiple keys\n", utf8_char);
            		assert(false);
            		return EXIT_FAILURE;
            	}

				// if the character is out of the alphabet or is whitespace/punctuation
            	if (sigma->find(kv->front()) == sigma->end() || tokenize_on(utf8_char))
            	{
            		if (!key_vec.empty()) {
		        		// tokenize
		        		VERBOSE_PRINT("Looking up '%s'...\n", token.c_str());

		        		KeyVectorSet* kvs;
		        		bool infinite = false;

		        		if (cascade.size() == 1)
		                  {
		                    kvs  = HWFST::lookup_unique(&key_vec, cascade[0],
		                                                key_table, &infinite);
		                   }
		                else
		                  {
		                    kvs = HWFST::lookup_cascade_unique(&key_vec, cascade,
		                                                       key_table, \
		                                                       &infinite);
		                  }

		                HWFST::print_lookups(kvs, key_table, token.c_str(), NULL, false, infinite);
		            	for (KeyVectorSet::const_iterator rkv = kvs->begin();
		                	rkv != kvs->end();
		                	++rkv)
		             	{
		          			KeyVector* hmmkv = *rkv;
		               		delete hmmkv;
		             	}
		           		delete kvs;
		           		key_vec.clear();
		                token.clear();
		           	}
		           	
				    // pass through superblanks untouched
		        	if (strcmp(utf8_char, "[") == 0)
		        	{
		        		pass_through_superblank();
		        	}
		        	else
		        	{
		        		// output the non-alphabetic character
            			fputs(utf8_char, message_out);
		        	}
            	}
            	else
            	{
            		key_vec.push_back(kv->front());
            		token.append(utf8_char);
            	}
            	
            	delete kv;
            	free(utf8_char);
            } // while not at end of input
            
			// Cleanup

            if (print_statistics)
            {
                fprintf(message_out, "Strings\tFound\tMissing\tResults\n"
                        "%lu\t%lu\t%lu\t%lu\n", 
                        inputs, analysed, no_analyses, analyses);
                fprintf(message_out, "Coverage\tAmbiguity\n"
                        "%f\t%f\n",
                        (float)analysed/(float)inputs,
                        (float)analyses/(float)inputs);
            }
            for (vector<TransducerHandle>::iterator t = cascade.begin();
                 t != cascade.end();
                 ++t)
              {
                HWFST::delete_transducer(*t);
              }
            if (write_symbols_to_filename != NULL) {
              ofstream os(write_symbols_to_filename);
              HWFST::write_symbol_table(key_table, os);
              os.close();
            }
        }
        catch (const char *p)
        {
            printf("HFST library error: %s\n", p);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(message_out, "ERROR: Transducer has wrong type.\n");
        return EXIT_FAILURE;
    }
}


int main( int argc, char **argv ) {
	message_out = stdout;
	verbose = false;
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
	VERBOSE_PRINT("Reading from %s, writing to %s\n", 
		inputfilename, outfilename);
	VERBOSE_PRINT("Output formats:\n"
			"  regular:`%s'`%s...'`%s',\n"
			"  unanalysed:`%s'`%s'`%s',\n"
			"  untokenised:`%s'`%s'`%s',\n"
			"  infinite:`%s'`%s'`%s\n",
            begin_setf, lookupf, end_setf,
            empty_begin_setf, empty_lookupf, empty_end_setf,
            unknown_begin_setf, unknown_lookupf, unknown_end_setf,
            infinite_begin_setf, infinite_lookupf, infinite_end_setf);
	// here starts the buffer handling part
	if (!is_input_stdin)
	{
		std::filebuf fbinput;
		fbinput.open(inputfilename, std::ios::in);
		std::istream inputstream(&fbinput);
		if (!is_output_stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);

                        if(stream_mode == 1)
                        {
			  retval = process_stream_generation(inputstream, outstream);
                        }
                        else
                        { 
			  retval = process_stream(inputstream, outstream);
                        }
		}
		else
		{
                        if(stream_mode == 1)
                        {
			  retval = process_stream_generation(inputstream, std::cout);
                        }
                        else
                        {
			  retval = process_stream(inputstream, std::cout);
                        }
		}
		return retval;
	}
	else if (is_input_stdin)
	{
		if (!is_output_stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
                        if(stream_mode == 1) 
                        {
			  retval = process_stream_generation(std::cin, outstream);
                        } 
                        else
                        {
			  retval = process_stream(std::cin, outstream);
                        } 
		}
		else
		{
                        if(stream_mode == 1)
                        {
			  retval = process_stream_generation(std::cin, std::cout);
                        }
                        else
                        {
			  retval = process_stream(std::cin, std::cout);
                        }
		}
		return retval;
	}
	if (outfile != stdout)
	{
		fclose(outfile);
	}
	free(inputfilename);
	free(outfilename);
	return EXIT_SUCCESS;
}

