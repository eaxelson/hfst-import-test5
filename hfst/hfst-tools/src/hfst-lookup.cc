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
#include <hfst2/hfst.h>
#include "FlagDiacritics.h"

#if NESTED_BUILD
#include <hfst2/string/string.h>
#endif
#if !NESTED_BUILD
#include <hfst2/string.h>
#endif

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
  APERTIUM_INPUT,
  CUSTOM_INPUT
};

enum lookup_output_format
{
  XEROX_OUTPUT,
  CG_OUTPUT,
  APERTIUM_OUTPUT,
  CUSTOM_OUTPUT
};

static lookup_input_format input_format = UTF8_TOKEN_INPUT;
static lookup_output_format output_format = XEROX_OUTPUT;

// in enum order
static char* begin_format = 0; // print before set of lookups
static char* lookup_format = 0; // print for each lookup
static char* no_lookups_format = 0; // print for zero results
static char* unknown_format = 0; // print when input unrecognized
static char* end_format = 0; // print after set of lookups
static bool print_statistics = false;

// predefined formats
static const char* XEROX_BEGIN_FORMAT = "";
static const char* XEROX_LOOKUP_FORMAT = "%i\t%l%n";
static const char* XEROX_END_FORMAT = "%n";
static const char* XEROX_NO_LOOKUPS_FORMAT = "%i\t%i\t+?%n%n";
static const char* XEROX_UNKNOWN_FORMAT = "%i\t%i\t+?%n%n";
static const char* CG_BEGIN_FORMAT = "\"<%i>\"%n";
static const char* CG_LOOKUP_FORMAT = "\t\"%b\"%a%n";
static const char* CG_END_FORMAT = "%n";
static const char* CG_NO_LOOKUPS_FORMAT = "\"<%i>\"%n\t\"%i\"\t ?%n%n";
static const char* CG_UNKNOWN_FORMAT = "\"<%i>\"%n\t\"%i\"\t ?%n%n";
static const char* APERTIUM_BEGIN_FORMAT = "^%i";
static const char* APERTIUM_LOOKUP_FORMAT = "/%l";
static const char* APERTIUM_END_FORMAT = "$%m%n";
static const char* APERTIUM_NO_LOOKUPS_FORMAT = "^%i/*%i$%m%n";
static const char* APERTIUM_UNKNOWN_FORMAT = "%m%i ";

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
		   "FORMAT is one of {xerox,cg,apertium,custom:``...''}, "
		   "xerox being default\n"
           "IFORMAT is one of {utf8,spaces,apertium,custom}, "
           "default being utf8 unless FORMAT is apertium\n"
		   "\n"
		   "More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstLookUp>\n"
		   "\n"
		   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}

void
print_version(const char* program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(message_out, "%s 0.3 (%s)\n"
		   "Copyright (C) 2009 University of Helsinki,\n"
		   "License GPLv3: GNU GPL version 3 "
		   "<http://gnu.org/licenses/gpl.html>\n"
		   "This is free software: you are free to change and redistribute it.\n"
		   "There is NO WARRANTY, to the extent permitted by law.\n",
		program_name, PACKAGE_STRING);
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
			{"format", required_argument, 0, 'f'},
            {"input-format", required_argument, 0, 'F'},
			{"statistics", no_argument, 0, 'r'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "df:F:hi:I:o:sSqvVrR:DW:t:",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		switch (c)
		{
#include "hfst-common-cases.h"
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
			else if (strncmp(optarg, "custom:", 7) == 0)
			{
              output_format = CUSTOM_OUTPUT;
              // parse custom output parts:
				// cannot fail, already checked for existence of :
				char* format_start = strchr(optarg, ':') + 1;
				char* format_end = strchr(format_start, ',');
				if (format_end == NULL)
				{
					goto error_format;
				}
                begin_format = hfst_strndup(format_start,
                                            format_end - format_start + 1);
				format_start = format_end + 1;
				format_end = strchr(format_start, ',');
				if (format_end == NULL)
				{
					goto error_format;
				}
				lookup_format = hfst_strndup(format_start,
                                             format_end - format_start + 1);
                format_start = format_end + 1;
				format_end = strchr(format_start, ',');
				if (format_end == NULL)
				{
					goto error_format;
				}
				no_lookups_format = hfst_strndup(format_start,
                                                 format_end - format_start + 1);
				format_start = format_end + 1;
				format_end = strchr(format_start, ',');
				if (format_end == NULL)
				{
					goto error_format;
				}
				unknown_format = hfst_strndup(format_start,
                                              format_end - format_start + 1);
				format_start = format_end + 1;
				for (format_end = format_start; 
						*format_end != '\0';
						format_end++)
                  {}
				end_format = hfst_strndup(format_start,
                                          format_end - format_start + 1);
			}
			break;
error_format:
			fprintf(message_out, "invalid argument for format"
					"custom: must be of form BEGIN,LOOKUP,NO_RES,UNK,END\n");
			return EXIT_FAILURE;
			break;
        case 'F':
            if (strcmp(optarg, "utf8"))
              {
                input_format = UTF8_TOKEN_INPUT;
              }
            else if (strcmp(optarg, "spaces"))
              {
                input_format = SPACE_SEPARATED_TOKEN_INPUT;
              }
            else if (strcmp(optarg, "apertium"))
              {
                input_format = APERTIUM_INPUT;
              }
            else if (strcmp(optarg, "custom"))
              {
                input_format = CUSTOM_INPUT;
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
      begin_format = hfst_strdup(XEROX_BEGIN_FORMAT);
      lookup_format = hfst_strdup(XEROX_LOOKUP_FORMAT);
      no_lookups_format = hfst_strdup(XEROX_NO_LOOKUPS_FORMAT);
      unknown_format = hfst_strdup(XEROX_UNKNOWN_FORMAT);
      end_format = hfst_strdup(XEROX_END_FORMAT);
      break;
    case CG_OUTPUT:
      begin_format = hfst_strdup(CG_BEGIN_FORMAT);
      lookup_format = hfst_strdup(CG_LOOKUP_FORMAT);
      no_lookups_format = hfst_strdup(CG_NO_LOOKUPS_FORMAT);
      unknown_format = hfst_strdup(CG_UNKNOWN_FORMAT);
      end_format = hfst_strdup(CG_END_FORMAT);
      break;
    case APERTIUM_OUTPUT:
      begin_format = hfst_strdup(APERTIUM_BEGIN_FORMAT);
      lookup_format = hfst_strdup(APERTIUM_LOOKUP_FORMAT);
      no_lookups_format = hfst_strdup(APERTIUM_NO_LOOKUPS_FORMAT);
      unknown_format = hfst_strdup(APERTIUM_UNKNOWN_FORMAT);
      end_format = hfst_strdup(APERTIUM_END_FORMAT);
      break;
    case CUSTOM_OUTPUT:
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
lookup_printf(const char* format, const char* inputform,
              const char* lookupform, const char* markup)
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
    // print loop
    if (outside_sigma)
      {
        lookup_printf(unknown_format, s, NULL, markup);
        no_analyses++;
      }
    else if (kvs->size() == 0)
      {
        lookup_printf(no_lookups_format, s, NULL, markup);
        no_analyses++;
      }
    else
      {
        analysed++;
      }

    lookup_printf(begin_format, s, NULL, markup);
    for (KeyVectorSet::iterator lkv = kvs->begin();
            lkv != kvs->end();
            ++lkv)
    {
        KeyVector* hmmlkv = *lkv;
        string* lkvstr = keyVectorToString(hmmlkv, kt);
        const char* lookup_full = lkvstr->c_str();
        lookup_printf(lookup_format, s, lookup_full, markup);
        delete lkvstr;
        analyses++;
    }
    if (inf)
      {
        lookup_printf(lookup_format, s, "[...cyclic...]", markup);
      }
    lookup_printf(end_format, s, NULL, markup);
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
    // print loop
    if (outside_sigma)
      {
        lookup_printf(unknown_format, s, NULL, markup);
        no_analyses++;
      }
    else if (kvs->size() == 0)
      {
        lookup_printf(no_lookups_format, s, NULL, markup);
        no_analyses++;
      }
    else
      {
        analysed++;

        lookup_printf(begin_format, s, NULL, markup);
        for (KeyVectorSet::iterator lkv = kvs->begin();
                lkv != kvs->end();
                ++lkv)
        {
            KeyVector* hmmlkv = *lkv;
            string* lkvstr = keyVectorToString(hmmlkv, kt);
            const char* lookup_full = lkvstr->c_str();
            lookup_printf(lookup_format, s, lookup_full, markup);
            delete lkvstr;
            analyses++;
        }
        if (inf)
          {
            lookup_printf(lookup_format, s, "[...cyclic...]", markup);
          }
        lookup_printf(end_format, s, NULL, markup);

      }
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
            char* line = NULL;
            size_t llen = 0;
            while (hfst_getline(&line, &llen, lookup_file) != -1)
            {
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
                VERBOSE_PRINT("Looking up %s...\n", line);
                char* markup = 0;
                bool unknown = false;
                bool infinite = false;
                KeyVector* kv = HFST::line_to_keyvector(&line, key_table,
                                                         &markup, &unknown);
                KeyVectorSet* kvs;
                if (!unknown)
                  {
                    if (cascade.size() == 1)
                      {
                        kvs  = HFST::lookup_unique(kv, cascade[0],
                                                    key_table, &infinite);
                       }
                    else
                      {
                        kvs = HFST::lookup_cascade_unique(kv, cascade,
                                                           key_table,
                                                           &infinite);
                      }
                    delete kv;
                  }
                else
                  {
                    kvs = new KeyVectorSet;
                  }
                HFST::print_lookups(kvs, key_table, line, markup, unknown, infinite);
                for (KeyVectorSet::const_iterator rkv = kvs->begin();
                    rkv != kvs->end();
                    ++rkv)
                 {
                   KeyVector* hmmkv = *rkv;
                   delete hmmkv;
                 }
               delete kvs;
            } // while lines in input
            if (line)
              {
                free(line);
              }
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
            char* line = NULL;
            size_t llen = 0;
            while (hfst_getline(&line, &llen, lookup_file) != -1)
            {
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
                VERBOSE_PRINT("Looking up %s...\n", line);
                char* markup = 0;
                bool unknown = false;
                bool infinite = false;
                KeyVector* kv = HWFST::line_to_keyvector(&line, key_table,
                                                         &markup, &unknown);
                KeyVectorSet* kvs;
                if (!unknown)
                  {
                    if (cascade.size() == 1)
                      {
                        kvs  = HWFST::lookup_unique(kv, cascade[0],
                                                    key_table, &infinite);
                       }
                    else
                      {
                        kvs = HWFST::lookup_cascade_unique(kv, cascade,
                                                           key_table,
                                                           &infinite);
                      }
                    delete kv;
                  }
                else
                  {
                    kvs = new KeyVectorSet;
                  }
                HWFST::print_lookups(kvs, key_table, line, markup, unknown, infinite);
                for (KeyVectorSet::const_iterator rkv = kvs->begin();
                    rkv != kvs->end();
                    ++rkv)
                 {
                   KeyVector* hmmkv = *rkv;
                   delete hmmkv;
                 }
               delete kvs;
            } // while lines in input
            if (line)
              {
                free(line);
              }
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
	VERBOSE_PRINT("Outputting in format:\n"
			"  BEGIN:`%s',\n"
			"  LOOKUP:`%s',\n"
			"  NO_LOOKUPS:`%s',\n"
			"  END:`%s'\n", begin_format, 
			lookup_format, no_lookups_format, end_format);
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
			retval = process_stream(inputstream, outstream);
		}
		else
		{
			retval = process_stream(inputstream, std::cout);
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
			retval = process_stream(std::cin, outstream);
		}
		else
		{
			retval = process_stream(std::cin, std::cout);
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

