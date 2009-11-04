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
static bool space_separated = false;
static char* begin_format; // print before set of lookups
static char* lookup_format; // print for each lookup
static char* no_lookups_format; // print for zero results
static char* end_format; // print after set of lookups
static bool format_given = false;
FlagDiacriticTable flag_diacritic_table;
HFST::KeySet flag_diacritic_set;
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
			"  -f, --format=FORMAT              Use FORMAT printing results sets\n");
	fprintf(message_out,
		   "\n"
		   "If OUTFILE or INFILE is missing or -, "
		   "standard streams will be used.\n"
		   "FORMAT is one of {xerox,cg,apertium,custom:``...''}, "
		   "xerox being default\n"
		   "\n"
		   "More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstLookUp>\n"
		   "\n"
		   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}

void
print_version(const char* program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(message_out, "%s 0.2 (%s)\n"
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
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "df:hi:I:o:sSqvVrR:DW:t:",
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
			break;
		case 'S':
			space_separated = true;
			break;
		case 'f':
			if (strcmp(optarg, "xerox") == 0)
			{
				begin_format = strdup("\n");
				lookup_format = strdup("%i\t%l\n");
				no_lookups_format = strdup("%i\t%i\t+?\n");
				end_format = strdup("\n");
			}
			else if (strcmp(optarg, "cg") == 0)
			{
				begin_format = strdup("\"<%i>\"\n");
				lookup_format = strdup("\t\"%b\"%a\n");
				no_lookups_format = strdup("\t\"%i\"\t ?\n");
				end_format = strdup("\n");
			}
			else if (strcmp(optarg, "apertium") == 0)
			{
				begin_format = strdup("^%i");
				lookup_format = strdup("/%l");
				no_lookups_format = strdup("/*%i");
				end_format = strdup("$\n");
			}
			else if (strncmp(optarg, "custom:", 7) == 0)
			{
				// cannot fail, already checked for existence of :
				char* format_start = strchr(optarg, ':') + 1;
				char* format_end = strchr(format_start, ',');
				if (format_end == NULL)
				{
					goto error_format;
				}
				// no strndup in MacOS X :-(
				begin_format = static_cast<char*>(calloc(sizeof(char), 
							format_end - format_start + 1));
				begin_format = static_cast<char*>(memcpy(begin_format, 
							format_start, format_end - format_start));
				format_start = format_end + 1;
				format_end = strchr(format_start, ',');
				if (format_end == NULL)
				{
					goto error_format;
				}
				lookup_format = static_cast<char*>(calloc(sizeof(char),
							format_end - format_start + 1));
				lookup_format = static_cast<char*>(memcpy(lookup_format,
							format_start, format_end - format_start));
				format_start = format_end + 1;
				format_end = strchr(format_start, ',');
				if (format_end == NULL)
				{
					goto error_format;
				}
				no_lookups_format = static_cast<char*>(calloc(sizeof(char),
							format_end - format_start + 1));
				no_lookups_format = static_cast<char*>(memcpy(no_lookups_format,
							format_start, format_end - format_start));
				format_start = format_end + 1;
				for (format_end = format_start; 
						*format_end != '\0';
						format_end++)
					;
				end_format = static_cast<char*>(calloc(sizeof(char),
							format_end - format_start + 1));
				end_format = static_cast<char*>(memcpy(end_format,
							format_start, format_end - format_start));
			}
			format_given = true;
			break;
error_format:
			fprintf(message_out, "invalid argument for format"
					"custom: must be of form BEGIN,LOOKUP,NO_RES,END\n");
			return EXIT_FAILURE;
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

	if (!format_given)
	{
		begin_format = strdup("\n");
		lookup_format = strdup("%i\t%l\n");
		no_lookups_format = strdup("%i\t%i\t+?\n");
		end_format = strdup("\n");
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
lookup_printf(const char* format, const char* inputform, const char* lookupform)
{
	char* lookup_really;
	if (lookupform == NULL)
	{
		lookup_really = strdup("");
	}
	else
	{
		lookup_really = strdup(lookupform);
	}
	size_t space = 2 * strlen(format) +
		2 * strlen(inputform) +
		2 * strlen(lookup_really) + 10;
	char* result = static_cast<char*>(calloc(sizeof(char), space + 1));
	size_t space_left = space;
	const char* src = format;
	char* dst = result;
	char* i; // %i
	char* l; // %l
	char* b; // %b
	char* a; // %a
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

bool
lookup_print_all(const char* s, KeyTable* kt,
		vector<TransducerHandle> cascade)
{
	KeyVector* lookup_orig = NULL;
	lookup_printf(begin_format, s, NULL);
	if (space_separated)
	{
		lookup_orig = stringSeparatedToKeyVector(s, kt, string(" "),
				true);
	}
	else
	{
		lookup_orig = stringUtf8ToKeyVector(s, kt, true);
	}
	// may be NULL
	if (lookup_orig == NULL)
	{
		VERBOSE_PRINT("No tokenisations for %s\n", s);
		return false;
	}
	else
	{
		lookup_orig->erase(remove_if(lookup_orig->begin(), lookup_orig->end(),
					_is_epsilon), lookup_orig->end());
		KeyVectorVector* final_results = new KeyVectorVector;
		final_results->push_back(lookup_orig);
		size_t cascade_number = 0;
		for (vector<TransducerHandle>::const_iterator t = cascade.begin();
				t != cascade.end();
				++t)
		{
			cascade_number++;
			KeyVectorVector* current_results = new KeyVectorVector;
			KeyVectorVector* past_results = final_results;
			for (KeyVectorVector::const_iterator kv = past_results->begin();
					kv != past_results->end();
					++kv)
			{
				string* kvstring = keyVectorToString(*kv, kt);
				VERBOSE_PRINT("Looking up %s from transducer %zu\n",
						kvstring->c_str(), cascade_number);
				if (is_infinitely_ambiguous(*t, *kv))
				{
					VERBOSE_PRINT("Got infinite results\n");
					return false;
				}
				else
				{
				  KeyVectorVector* lookups = lookup_all(*t, *kv,&flag_diacritic_set);
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
						hmmlkv = flag_diacritic_table.filter_diacritics(hmmlkv);
						if (hmmlkv == NULL)
						  {continue;}
						hmmlkv->erase(remove_if(hmmlkv->begin(), hmmlkv->end(),
											_is_epsilon), hmmlkv->end());
						string* lkvstring = keyVectorToString(hmmlkv, kt);
						VERBOSE_PRINT("Got %s\n", lkvstring->c_str());
						current_results->push_back(hmmlkv);
						delete lkvstring;
					}
				}
			}
			final_results = current_results;
		} // for each transducer in cascade
		// print loop
		if (final_results->size() == 0)
		{
			lookup_printf(no_lookups_format, s, NULL);
		}
		for (KeyVectorVector::iterator lkv = final_results->begin();
				lkv != final_results->end();
				++lkv)
		{
			KeyVector* hmmlkv = *lkv;
			string* lkvstr = keyVectorToString(hmmlkv, kt);
			const char* lookup_full = lkvstr->c_str();
			lookup_printf(lookup_format, s, lookup_full);
			delete lkvstr;
		}
		lookup_printf(end_format, s, NULL);
	} // if proper lookup originally
	return false;
}
}

namespace HWFST
{
bool
_is_epsilon(Key k)
{
	return k == 0;
}

bool
lookup_print_all(const char* s, KeyTable* kt,
		vector<TransducerHandle> cascade)
{
	KeyVector* lookup_orig = NULL;
	lookup_printf(begin_format, s, NULL);
	if (space_separated)
	{
		lookup_orig = stringSeparatedToKeyVector(s, kt, string(" "),
				true);
	}
	else
	{
		lookup_orig = stringUtf8ToKeyVector(s, kt, true);
	}
	// may be NULL
	if (lookup_orig == NULL)
	{
		VERBOSE_PRINT("No tokenisations for %s\n", s);
		return false;
	}
	else
	{
		lookup_orig->erase(remove_if(lookup_orig->begin(), lookup_orig->end(),
					_is_epsilon), lookup_orig->end());
		KeyVectorVector* final_results = new KeyVectorVector;
		final_results->push_back(lookup_orig);
		size_t cascade_number = 0;
		for (vector<TransducerHandle>::const_iterator t = cascade.begin();
				t != cascade.end();
				++t)
		{
			cascade_number++;
			KeyVectorVector* current_results = new KeyVectorVector;
			KeyVectorVector* past_results = final_results;
			for (KeyVectorVector::const_iterator kv = past_results->begin();
					kv != past_results->end();
					++kv)
			{
				string* kvstring = keyVectorToString(*kv, kt);
				VERBOSE_PRINT("Looking up %s from transducer %zu\n",
						kvstring->c_str(), cascade_number);
				if (is_infinitely_ambiguous(*t, *kv))
				{
					VERBOSE_PRINT("Got infinite results\n");
					return false;
				}
				else
				{
				  KeyVectorVector* lookups = lookup_all(*t, *kv,&flag_diacritic_set);
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
						hmmlkv = flag_diacritic_table.filter_diacritics(hmmlkv);
						if (hmmlkv == NULL)
						  { continue; }
						hmmlkv->erase(remove_if(hmmlkv->begin(), hmmlkv->end(),
											_is_epsilon), hmmlkv->end());
						string* lkvstring = keyVectorToString(hmmlkv, kt);
						VERBOSE_PRINT("Got %s\n", lkvstring->c_str());
						current_results->push_back(hmmlkv);
						delete lkvstring;
					}
				}
			}
			final_results = current_results;
		} // for each transducer in cascade
		// print loop
		if (final_results->size() == 0)
		{
			lookup_printf(no_lookups_format, s, NULL);
		}
		for (KeyVectorVector::iterator lkv = final_results->begin();
				lkv != final_results->end();
				++lkv)
		{
			KeyVector* hmmlkv = *lkv;
			string* lkvstr = keyVectorToString(hmmlkv, kt);
			const char* lookup_full = lkvstr->c_str();
			lookup_printf(lookup_format, s, lookup_full);
			delete lkvstr;
		}
		lookup_printf(end_format, s, NULL);
	} // if proper lookup originally
	return false;
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
			VERBOSE_PRINT("\n");
#			define MAX_LINE_LENGTH 254
			char* line =
				static_cast<char*>(malloc(sizeof(char)*MAX_LINE_LENGTH+1));
			while ((line = fgets(line, MAX_LINE_LENGTH, lookup_file)))
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
				HFST::lookup_print_all(line, key_table, cascade);
			} // while lines in input
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
			VERBOSE_PRINT("\n");
#			define MAX_LINE_LENGTH 254
			char* line =
				static_cast<char*>(malloc(sizeof(char)*MAX_LINE_LENGTH+1));
			while ((line = fgets(line, MAX_LINE_LENGTH, lookup_file)))
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
				HWFST::lookup_print_all(line, key_table, cascade);
			} // while lines in input
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

