//! @file hfst-diff-test.cc
//!
//! @brief Diff-test command line tool
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
#include <getopt.h>
#include "hfst2/hfst.h"
#include "hfst2/flag-diacritics/FlagDiacritics.h"
#include "hfst2/string/string.h"


#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-unary.h"
// add tools-specific variables here
static char* relation_file_name;
static FILE* relation_file;
static bool space_separated = false;
FlagDiacriticTable flag_diacritic_table;
HFST::KeySet flag_diacritic_set;

bool print_statistics = false;
unsigned long test_lines = 0;
unsigned long positive_results = 0;
unsigned long negative_results = 0;
unsigned long infinite_results = 0;
unsigned long no_results = 0;

enum TEST_TYPE { TEST_EXACTLY, TEST_NONE, TEST_AT_LEAST };
static TEST_TYPE test_type = TEST_EXACTLY;

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
			"  -r, --relation=FILE              Read test cases from a file\n"
			"  -S, --spaces                     Use space separated tokens in strings\n"
			"  -t, --test=TTYPE                 Use TTYPE test comparing result sets\n"
			"  -x, --statistics                 Print statistics of results\n");
	fprintf(message_out,
		   "\n"
		   "If OUTFILE or INFILE is missing or -, "
		   "standard streams will be used.\n"
		   "TTYPE is one of {exactly,at_least,none}, exactly being default\n"
		   "\n"
		   "More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstDiffTest>\n"
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
		HFST_GETOPT_COMMON_LONG
		  ,
		HFST_GETOPT_UNARY_LONG
		  ,
		  // add tool-specific options here
			{"relation", required_argument, 0, 'r'},
			{"spaces", no_argument, 0, 'S'},
			{"test-type", required_argument, 0, 't'},
			{"statistics", no_argument, 0, 'x'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "dhi:o:sSqvVR:DW:r:t:x",
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
		case 'r':
			relation_file_name = hfst_strdup(optarg);
			relation_file = hfst_fopen(relation_file_name, "r");
			break;
		case 't':
			if (strcmp(optarg, "exactly") == 0)
			{
				test_type = TEST_EXACTLY;
			}
			else if (strcmp(optarg, "at-least") == 0)
			{
				test_type = TEST_AT_LEAST;
			}
			else if (strcmp(optarg, "none") == 0)
			{
				test_type = TEST_NONE;
			}
			else
			{
				fprintf(message_out, "Unknown test type %s\n", optarg);
				print_short_help(argv[0]);
				return EXIT_FAILURE;
			}
			break;
		case 'S':
			space_separated = true;
			break;
		case 'x':
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

	if (is_output_stdout)
	{
			outfilename = hfst_strdup("<stdout>");
			outfile = stdout;
	}
	message_out = outfile;
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

static
void
do_print_statistics()
{
	fprintf(message_out, "Lines analysed\tPositive\tNegative\tInfinite\tNone\n"
			"%lu\t%lu\t%lu\t%lu\t%lu\n"
			"%f\t%f\t%f\t%f\t%f\n",
			test_lines, positive_results, negative_results, infinite_results,
			no_results,
			static_cast<float>(test_lines)/static_cast<float>(test_lines),
			static_cast<float>(positive_results)/static_cast<float>(test_lines),
			static_cast<float>(negative_results)/static_cast<float>(test_lines),
			static_cast<float>(infinite_results)/static_cast<float>(test_lines),
			static_cast<float>(no_results)/static_cast<float>(test_lines));
}

namespace HFST
{

void
define_flag_diacritics(KeyTable * key_table)
{
  
  for (Key k = 0; k < key_table->get_unused_key(); ++k)
    {
      flag_diacritic_table.define_diacritic
	(k, get_symbol_name(get_key_symbol(k, key_table)));
      if (flag_diacritic_table.is_diacritic(k))
	{ flag_diacritic_set.insert(k); }
    }
}

bool
_is_epsilon(Key k)
{
	if ((k == 0) || (flag_diacritic_set.find(k) != flag_diacritic_set.end()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
compare_sets(const string& testcase, 
		set<KeyVector>& r, set<KeyVector>& e, TEST_TYPE t,
		bool infinite_r, KeyTable* key_table)
{
	set<KeyVector> rAndE, rMinusE, eMinusR;
	set_intersection(r.begin(), r.end(),
			e.begin(), e.end(),
			inserter(rAndE, rAndE.end()));
	set_difference(e.begin(), e.end(),
			r.begin(), r.end(),
			inserter(eMinusR, eMinusR.end()));
	set_difference(r.begin(), r.end(),
			e.begin(), e.end(),
			inserter(rMinusE, rMinusE.end()));
	if (t == TEST_EXACTLY)
	{
		if (infinite_r)
		{
			fprintf(outfile, "Some of infinite results for %s could not be "
					"matched against finite strings of test file\n",
					testcase.c_str());
			return false;
		}
		else if (r == e)
		{
			return true;
		}
		else
		{
			// print e \ r and r \ e
			if (rMinusE.size() > 0)
			{
				fprintf(message_out, "Strings for %s in test file, not in "
						"transducer\n", testcase.c_str());
			}
			for (set<KeyVector>::const_iterator kv = rMinusE.begin();
					kv != rMinusE.end(); 
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			if (eMinusR.size() > 0)
			{
				fprintf(message_out, "Strings for %s in transducer, not in "
						"test file\n", testcase.c_str());
			}
			for (set<KeyVector>::const_iterator kv = eMinusR.begin();
					kv != eMinusR.end();
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			return false;
		}
	}
	else if (t == TEST_NONE)
	{
		if (infinite_r)
		{
			fprintf(message_out, "Could not verify that finite set of "
					"test file strings is not contained in "
					"infinite results of %s\n", testcase.c_str());
			return false;
		}
		else if (rAndE.size() == 0)
		{
			return true;
		}
		else
		{
			// print r u e
			fprintf(message_out, "Unwanted strings of test file found "
					"in transducer for %s\n", testcase.c_str());
			for (set<KeyVector>::const_iterator kv = rAndE.begin();
					kv != rAndE.end();
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			return false;
		}
	}
	else if (t == TEST_AT_LEAST)
	{
		if (infinite_r)
		{
			fprintf(message_out, "Some of infinite results for %s could not be "
					"matched against finite strings of test file\n",
					testcase.c_str());
			return false; // maybe
		}
		else if (eMinusR.size() == 0)
		{
			return true;
		}
		else
		{
			// print e \ r
			fprintf(message_out, "Strings for %s in test file were missing "
					"from transducer\n", testcase.c_str());
			for (set<KeyVector>::const_iterator kv = eMinusR.begin();
					kv != eMinusR.end();
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			return false;
		}
	}
	return false;
}

KeyVectorVector*
lookup_all(const char* s, KeyTable* kt,
		vector<TransducerHandle>& cascade, bool* infinite)
{
	*infinite = false;
	KeyVector* lookup_orig = NULL;
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
		verbose_printf("No tokenisations for %s\n", s);
		*infinite = false;
		return NULL;
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
				verbose_printf("Looking up %s from transducer %zu\n",
						kvstring->c_str(), cascade_number);
				if (is_infinitely_ambiguous(*t,true, *kv))
				{
					verbose_printf("Got infinite results\n");
					*infinite = true;
					delete current_results;
                    return NULL;
				}
				else
				{
					KeyVectorVector* lookups = lookup_all(*t, *kv, &flag_diacritic_set);
					if (lookups == NULL)
					{
						// no results as empty result
						verbose_printf("Got no results\n");
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
						verbose_printf("Got %s\n", lkvstring->c_str());
						current_results->push_back(hmmlkv);
						delete lkvstring;

					}
                    delete lookups;
				}
			}
			final_results = current_results;
		} // for each transducer in cascade
		return final_results;
	} // if proper lookup originally
	return NULL;
}
}
namespace HWFST
{

void
define_flag_diacritics(KeyTable * key_table)
{
  
  for (Key k = 0; k < key_table->get_unused_key(); ++k)
    {
      flag_diacritic_table.define_diacritic
	(k, get_symbol_name(get_key_symbol(k, key_table)));
      if (flag_diacritic_table.is_diacritic(k))
	{ flag_diacritic_set.insert(k); }
    }
}

bool
_is_epsilon(Key k)
{
	if ((k == 0) || (flag_diacritic_set.find(k) != flag_diacritic_set.end()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
compare_sets(const string& testcase, 
		set<KeyVector>& r, set<KeyVector>& e, TEST_TYPE t,
		bool infinite_r, KeyTable* key_table)
{
	set<KeyVector> rAndE, rMinusE, eMinusR;
	set_intersection(r.begin(), r.end(),
			e.begin(), e.end(),
			inserter(rAndE, rAndE.end()));
	set_difference(e.begin(), e.end(),
			r.begin(), r.end(),
			inserter(eMinusR, eMinusR.end()));
	set_difference(r.begin(), r.end(),
			e.begin(), e.end(),
			inserter(rMinusE, rMinusE.end()));
	if (t == TEST_EXACTLY)
	{
		if (infinite_r)
		{
			fprintf(message_out, "Some of infinite results for %s could not be "
					"matched against finite strings of test file\n",
					testcase.c_str());
			return false;
		}
		else if (r == e)
		{
			return true;
		}
		else
		{
			// print e \ r and r \ e
			if (rMinusE.size() > 0)
			{
				fprintf(message_out, "Strings for %s in test file missing "
						"from transducer\n", testcase.c_str());
			}
			for (set<KeyVector>::const_iterator kv = rMinusE.begin();
					kv != rMinusE.end(); 
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			if (eMinusR.size() > 0)
			{
				fprintf(message_out, "Strings for %s in transducer missing "
						"from test file\n", testcase.c_str());
			}
			for (set<KeyVector>::const_iterator kv = eMinusR.begin();
					kv != eMinusR.end();
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			return false;
		}
	}
	else if (t == TEST_NONE)
	{
		if (infinite_r)
		{
			fprintf(message_out, "Could not verify that finite set of "
					"test file strings is not contained in "
					"infinite results of %s\n", testcase.c_str());
			return false;
		}
		else if (rAndE.size() == 0)
		{
			return true;
		}
		else
		{
			// print r u e
			fprintf(message_out, "Unwanted strings of test file found "
					"in transducer for %s\n", testcase.c_str());
			for (set<KeyVector>::const_iterator kv = rAndE.begin();
					kv != rAndE.end();
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			return false;
		}
	}
	else if (t == TEST_AT_LEAST)
	{
		if (infinite_r)
		{
			fprintf(message_out, "Some of infinite results for %s could not be "
					"matched against finite strings of test file\n",
					testcase.c_str());
			return false; // maybe
		}
		else if (eMinusR.size() == 0)
		{
			return true;
		}
		else
		{
			// print e \ r
			fprintf(message_out, "Strings for %s in test file were missing "
					"from transducer\n", testcase.c_str());
			for (set<KeyVector>::const_iterator kv = eMinusR.begin();
					kv != eMinusR.end();
					++kv)
			{
				KeyVector hmmkv = *kv;
				string* kvs = keyVectorToString(&hmmkv, key_table);
				fprintf(message_out, " * %s\n", kvs->c_str());
				delete kvs;
			}
			return false;
		}
	}
	return false;
}

KeyVectorVector*
lookup_all(const char* s, KeyTable* kt,
		vector<TransducerHandle>& cascade, bool* infinite)
{
	*infinite = false;
	KeyVector* lookup_orig = NULL;
	if (space_separated)
	{
		lookup_orig = stringSeparatedToKeyVector(s, kt, string(" "), true);
	}
	else
	{
		lookup_orig = stringUtf8ToKeyVector(s, kt, true);
	}
	// may be NULL
	if (lookup_orig == NULL)
	{
		verbose_printf("No tokenisations for %s\n", s);
		*infinite = false;
		return NULL;
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
				verbose_printf("Looking up %s from transducer %zu\n",
						kvstring->c_str(), cascade_number);
				if (is_infinitely_ambiguous(*t,true, *kv))
				{
					verbose_printf("Got infinite results\n");
					*infinite = true;
                    delete current_results;
					return NULL;
				}
				else
				{
					KeyVectorVector* lookups = lookup_all(*t, *kv, &flag_diacritic_set);
					if (lookups == NULL)
					{
						// no results as empty result
						verbose_printf("Got no results\n");
						lookups = new KeyVectorVector;
					}
					for (KeyVectorVector::const_iterator lkv = lookups->begin();
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
						verbose_printf("Got %s\n", lkvstring->c_str());
						current_results->push_back(hmmlkv);
						delete lkvstring;
					}
                    delete lookups;
				}
			}
			final_results = current_results;
		} // for each transducer in cascade
		return final_results;
	} // if proper lookup originally
	return NULL;
}
}

int
process_stream(std::istream& inputstream, std::ostream& outstream)
{
	verbose_printf("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
	bool any_failures = false;
	if (format_type == SFST_FORMAT)
	{
		verbose_printf("Using unweighted format\n");
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
					verbose_printf("Reading cascade...\r");
				}
				else
				{
					verbose_printf("Reading cascade... %zu\r", nth_stream);
				}
				// add your code here
				cascade.push_back(input);
			}
			HFST::define_flag_diacritics(key_table);
			verbose_printf("\n");
            size_t len = 0;
			char* line;
			while (hfst_getline(&line, &len, relation_file)!= -1)
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
				verbose_printf("Looking up %s...\n", line);
				bool infinity = false;
				HFST::KeyVectorVector* results = HFST::lookup_all(line,
						key_table, cascade,
						&infinity);
				set<HFST::KeyVector> expectedLines;
                size_t expLen = 0;
				char* expectedLine;
                ssize_t readBytes = hfst_getline(&expectedLine, &expLen,
                                                  relation_file);
				while ((readBytes != -1) &&
						(strcmp(expectedLine, "\n") != 0))
				{
					char *q = expectedLine;
					while (*q != '\0')
					{
						if (*q == '\n')
						{
							*q = '\0';
							break;
						}
						q++;
					}
					verbose_printf("Expecting %s\n", expectedLine);
					HFST::KeyVector* expected = NULL;
					if (space_separated)
					{
						expected = 
							HFST::stringSeparatedToKeyVector(expectedLine,
									key_table, string(" "), true);
					}
					else
					{
						expected = HFST::stringUtf8ToKeyVector(expectedLine,
								key_table, true);
					}
					// may be NULL
					if (expected == NULL)
					{
						if (test_type != TEST_NONE)
						{
							fprintf(message_out, "String %s cannot be formed from alphabet of transducers\n",
									expectedLine);
						}
					}
					else
					{
						expectedLines.insert(*expected);
					}
				    readBytes = hfst_getline(&expectedLine, &expLen, relation_file);
				}
                free(expectedLine);
				set<HFST::KeyVector> resultLines;
				for (HFST::KeyVectorVector::const_iterator rkv = results->begin();
						rkv != results->end();
						++rkv)
				{
					HFST::KeyVector* rl = *rkv;
					rl->erase(remove_if(rl->begin(), rl->end(), HFST::_is_epsilon), rl->end());
					resultLines.insert(*rl);
				}
				bool this_failed = !(HFST::compare_sets(line,
							resultLines, expectedLines, test_type,
							infinity, key_table));
				any_failures = any_failures || this_failed;
				test_lines++;
				if (this_failed)
				{
					negative_results++;
				}
				else if (!this_failed)
				{
					positive_results++;
				}
				if (infinity)
				{
					infinite_results++;
				}
				if (resultLines.size() == 0)
				{
					no_results++;
				}
			} // while lines in input
            free(line);
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
		if (any_failures)
		{
			fprintf(message_out, "FAIL\n");
			return EXIT_FAILURE;
		}
		else
		{
			fprintf(message_out, "OK\n");
			return EXIT_SUCCESS;
		}
	}
	else if (format_type == OPENFST_FORMAT) 
	{
		verbose_printf("Using weighted format\n");
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
					verbose_printf("Reading cascade...\r");
				}
				else
				{
					verbose_printf("Reading cascade... %zu\r", nth_stream);
				}
				// add your code here
				cascade.push_back(input);
			}
			HWFST::define_flag_diacritics(key_table);
			verbose_printf("\n");
			size_t len;
            char* line;
			while (hfst_getline(&line, &len, relation_file) != -1)
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
				verbose_printf("Looking up %s...\n", line);
				bool infinity = false;
				HWFST::KeyVectorVector* results = HWFST::lookup_all(line,
						key_table, cascade,
						&infinity);
				set<HWFST::KeyVector> expectedLines;
				size_t expLen;
                char* expectedLine;
				ssize_t read_bytes = hfst_getline(&expectedLine, &expLen,
                                                  relation_file);
				while ((read_bytes != -1) &&
						(strcmp(expectedLine, "\n") != 0))
				{
					char *q = expectedLine;
					while (*q != '\0')
					{
						if (*q == '\n')
						{
							*q = '\0';
							break;
						}
						q++;
					}
					verbose_printf("Expecting %s\n", expectedLine);
					HWFST::KeyVector* expected = NULL;
					if (space_separated)
					{
						expected = 
							HWFST::stringSeparatedToKeyVector(expectedLine,
									key_table, string(" "), true);
					}
					else
					{
						expected = HWFST::stringUtf8ToKeyVector(expectedLine,
								key_table, true);
					}
					// may be NULL
					if (expected == NULL)
					{
						if (test_type != TEST_NONE)
						{
							fprintf(message_out, "String %s cannot be formed from alphabet of transducers\n",
									expectedLine);
						}
					}
					else
					{
						expectedLines.insert(*expected);
					}
				read_bytes =
					hfst_getline(&expectedLine, &expLen, relation_file);
				}
                free(expectedLine);
				set<HWFST::KeyVector> resultLines;
				for (HWFST::KeyVectorVector::const_iterator rkv = results->begin();
						rkv != results->end();
						++rkv)
				{
					HWFST::KeyVector* rl = *rkv;
					rl->erase(remove_if(rl->begin(), rl->end(), HWFST::_is_epsilon), rl->end());
					resultLines.insert(*rl);
				}
				bool this_failed = !(HWFST::compare_sets(line,
							resultLines, expectedLines, test_type,
							infinity, key_table));
				any_failures = any_failures || this_failed;
				test_lines++;
				if (this_failed)
				{
					negative_results++;
				}
				else if (!this_failed)
				{
					positive_results++;
				}
				if (infinity)
				{
					infinite_results++;
				}
				if (resultLines.size() == 0)
				{
					no_results++;
				}
			} // while lines in input
            free(line);
			if (write_symbols_to_filename != NULL) {
			  ofstream os(write_symbols_to_filename);
			  HWFST::write_symbol_table(key_table, os);
			  os.close();
			}
		}
		catch (const char *p)
		{
			printf("HWFST library error: %s\n", p);
			return EXIT_FAILURE;
		}
		if (print_statistics)
		{
			do_print_statistics();
		}
		if (any_failures)
		{
			fprintf(message_out, "FAIL\n");
			return EXIT_FAILURE;
		}
		else
		{
			fprintf(message_out, "OK\n");
			return EXIT_SUCCESS;
		}
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
	verbose_printf("Reading from %s, writing to %s\n", 
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

