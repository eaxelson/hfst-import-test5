//! @file hfst-omor-evaluate.cc
//!
//! @brief Evaluate omor style transducer sets
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

#include "hfst-common-unary-variables.h"
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
// for ranked test
unsigned long* ranked_at = 0;
unsigned long ranked_lowest_counted = 0;
unsigned long ranked_lower = 0;
unsigned long ranked_none = 0;
// for substring test
static char* substring = 0;
unsigned long substrings_needed = 0;
unsigned long substrings_correct = 0;
unsigned long substrings_incorrect = 0;
unsigned long substrings_missed = 0;
enum TEST_TYPE { 
	TEST_EXACTLY,
	TEST_NONE,
	TEST_AT_LEAST,
	TEST_SUBSTRING,
	TEST_RANKED };

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
		   "TTYPE is one of {exactly,at_least,none,ranked,substring=S}, "
		   " exactly being default\n"
		   "\n"
		   "More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstOmorEvaluate>\n"
		   "\n"
		   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}

void
print_version(const char* program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(message_out, "%s 0.1 (%s)\n"
		   "Copyright (C) 2010 University of Helsinki,\n"
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
#include "hfst-common-cases.h"
#include "hfst-common-unary-cases.h"
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
			else if (strncmp(optarg, "ranked", 6) == 0)
			{
				test_type = TEST_RANKED;
				ranked_lowest_counted = hfst_strtonumber(optarg + 7, NULL);
				ranked_at = static_cast<unsigned long*>(calloc(sizeof(unsigned long), ranked_lowest_counted + 2));
			}
			else if (strncmp(optarg, "substring", 9) == 0)
			{
				test_type = TEST_SUBSTRING;
				substring = strdup(optarg + 10);
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
	if (test_type == TEST_RANKED)
	{
      fprintf(message_out, "Total\t");
		for (unsigned int i = 1; i <= ranked_lowest_counted; ++i)
		{
			fprintf(message_out, "Rank %u\t", i);
		}
		fprintf(message_out, "Lower ranks\tNo rank\n");
      // counts
      fprintf(message_out, "%lu\t", test_lines);
		for (unsigned int i = 1; i <= ranked_lowest_counted; ++i)
		{
			fprintf(message_out, "%lu\t", ranked_at[i]);
		}
		fprintf(message_out, "%lu\t%lu\n", ranked_lower, ranked_none);
      // percentages
      fprintf(message_out, "%f\t", static_cast<float>(test_lines)/static_cast<float>(test_lines));
		for (unsigned int i = 1; i <= ranked_lowest_counted; ++i)
		{
			fprintf(message_out, "%f\t", 
					static_cast<float>(ranked_at[i])/static_cast<float>(test_lines));
		}
		fprintf(message_out, "%f\t%f\n", 
				static_cast<float>(ranked_lower)/static_cast<float>(test_lines),
				static_cast<float>(ranked_none)/static_cast<float>(test_lines));
	}
	else if (test_type == TEST_SUBSTRING)
	{
		fprintf(message_out, "Positions needed\tCorrect\tMissed\tFalse\n"
				"%lu\t%lu\t%lu\t%lu\n"
				"%f\t%f\t%f\t%f\n",
				substrings_needed, substrings_correct,
				substrings_missed, substrings_incorrect,
				static_cast<float>(substrings_needed)/static_cast<float>(substrings_needed),
				static_cast<float>(substrings_correct)/static_cast<float>(substrings_needed),
				static_cast<float>(substrings_missed)/static_cast<float>(substrings_needed),
				static_cast<float>(substrings_incorrect)/static_cast<float>(substrings_needed));
	}
	if (test_type != TEST_RANKED)
	{
		fprintf(message_out, "Lines analysed\tPositive\tNegative (of which:)\tNone\tNInfinite\n"
				"%lu\t%lu\t%lu\t%lu\t%lu\n"
				"%f\t%f\t%f\t%f\t%f\n",
				test_lines, positive_results, negative_results,
				no_results, infinite_results,
				static_cast<float>(test_lines)/static_cast<float>(test_lines),
				static_cast<float>(positive_results)/static_cast<float>(test_lines),
				static_cast<float>(negative_results)/static_cast<float>(test_lines),
				static_cast<float>(no_results)/static_cast<float>(test_lines),
				static_cast<float>(infinite_results)/static_cast<float>(test_lines));
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

void
get_subsets(const set<KeyVector>& r, const set<KeyVector> e,
		set<KeyVector>& rAndE, set<KeyVector>& rMinusE,
		set<KeyVector>& eMinusR)
{
	set_intersection(r.begin(), r.end(),
			e.begin(), e.end(),
			inserter(rAndE, rAndE.end()));
	set_difference(e.begin(), e.end(),
			r.begin(), r.end(),
			inserter(eMinusR, eMinusR.end()));
	set_difference(r.begin(), r.end(),
			e.begin(), e.end(),
			inserter(rMinusE, rMinusE.end()));
}

bool
test_exactly(const string& testcase, 
		set<KeyVector>& r, set<KeyVector>& e,
		bool infinite_r, KeyTable* key_table)
{
	set<KeyVector> rAndE, rMinusE, eMinusR;
	get_subsets(r, e, rAndE, rMinusE, eMinusR);
	// substring test add to exact test, cf. later
	if (infinite_r)
	{
		fprintf(outfile, " * INFINITE %s\n",
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
		for (set<KeyVector>::const_iterator kv = rMinusE.begin();
				kv != rMinusE.end(); 
				++kv)
		{
			KeyVector hmmkv = *kv;
			string* kvs = keyVectorToString(&hmmkv, key_table);
			fprintf(message_out, " * EXTRA %s : %s\n", 
					testcase.c_str(), kvs->c_str());
			delete kvs;
		}
		for (set<KeyVector>::const_iterator kv = eMinusR.begin();
				kv != eMinusR.end();
				++kv)
		{
			KeyVector hmmkv = *kv;
			string* kvs = keyVectorToString(&hmmkv, key_table);
			fprintf(message_out, " * MISSING %s : %s\n",
					testcase.c_str(), kvs->c_str());
			delete kvs;
		}
		return false;
	}
}

bool
test_none(const string& testcase, 
		set<KeyVector>& r, set<KeyVector>& e,
		bool infinite_r, KeyTable* key_table)
{
	set<KeyVector> rAndE, rMinusE, eMinusR;
	get_subsets(r, e, rAndE, rMinusE, eMinusR);
	if (infinite_r)
	{
		fprintf(message_out, " * INFINITE %s\n", testcase.c_str());
		return false;
	}
	else if (rAndE.size() == 0)
	{
		return true;
	}
	else
	{
		// print r u e
		for (set<KeyVector>::const_iterator kv = rAndE.begin();
				kv != rAndE.end();
				++kv)
		{
			KeyVector hmmkv = *kv;
			string* kvs = keyVectorToString(&hmmkv, key_table);
			fprintf(message_out, " * UNEXPECTED %s : %s\n", 
					testcase.c_str(), kvs->c_str());
			delete kvs;
		}
		return false;
	}
}

bool
test_at_least(const string& testcase, 
		set<KeyVector>& r, set<KeyVector>& e,
		bool infinite_r, KeyTable* key_table)
{
	set<KeyVector> rAndE, rMinusE, eMinusR;
	get_subsets(r, e, rAndE, rMinusE, eMinusR);
	if (infinite_r)
	{
		fprintf(message_out, " * INFINITE %s\n",
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
		for (set<KeyVector>::const_iterator kv = eMinusR.begin();
				kv != eMinusR.end();
				++kv)
		{
			KeyVector hmmkv = *kv;
			string* kvs = keyVectorToString(&hmmkv, key_table);
			fprintf(message_out, " * MISSING %s : %s\n", 
					testcase.c_str(), kvs->c_str());
			delete kvs;
		}
		return false;
	}
}

bool
test_substring(const string& testcase,
		set<KeyVector>& r, set<KeyVector>& e,
		bool infinite_r, KeyTable* key_table)
{
	bool rv = test_at_least(testcase, r, e, infinite_r, key_table);
	if (e.size() > 1)
	{
		fprintf(message_out, "Warning: expected more than 1 will fail at least once\n");
	}
	Key sought = stringToKey(substring, key_table);
    KeyVector joined;
    if (r.size() == 1)
      {
        for (KeyVector::const_iterator rk = r.begin()->begin();
             rk != r.begin()->end();
             ++rk)
          {
            joined.push_back(*rk);
          }
      }
    else if (r.size() > 1)
      {
        // combine multiple suggestions
        vector<unsigned short> hyphenity(r.begin()->size(), 0);
        for (set<KeyVector>::const_iterator rkv = r.begin();
             rkv != r.end();
             ++rkv)
          {
            unsigned int i = 0;
            for (KeyVector::const_iterator rk = rkv->begin();
                 rk != rkv->end();
                 ++rk)
              {
                if (*rk == sought)
                  {
                    hyphenity[i]++;
                  }
                else
                  {
                    i++;
                  }
              }
          }
        unsigned int j = 0;
        KeyVector rk = *r.begin();
        for (vector<unsigned short>::const_iterator h = hyphenity.begin();
             h != hyphenity.end();
             ++h)
          {
            if (j >= rk.size())
              {
                break;
              }
            if (rk[j] == sought)
              {
                j++;
              }
            if (*h >= r.size())
              {
                joined.push_back(sought);
              }
            joined.push_back(rk[j]);
            j++;
          }
      }
    else if (r.size() == 0)
      {
        // use source as result
      }
	for (set<KeyVector>::iterator ekv = e.begin();
			ekv != e.end();
			++ekv)
	{
		// find points before which add substring, e.g.
		// hyphenation:hy-phen-a-tion
		// 01234567890 00 1000 1 1000 =: {2, 6, 7}
		// (will crap out on --)
		vector<bool> expectedSpots = vector<bool>(ekv->size(), false);
		unsigned int i = 0;
		unsigned int substring_count = 0;
		KeyVector hmmekv = *ekv;
		string* ekvs = keyVectorToString(&hmmekv, key_table);
		for (KeyVector::iterator ek = hmmekv.begin();
				ek != hmmekv.end();
				++ek)
		{
			if (*ek == sought)
			{
				expectedSpots[i] = true;
				substring_count++;
			}
			else
			{
				i++;
			}
		}
		if (r.size() == 0)
		{
			fprintf(message_out, " * SUB_MISS_ALL %s : %s\n", 
					testcase.c_str(), ekvs->c_str());
			substrings_missed += substring_count;
			substrings_needed += substring_count;
		}
        else
          {
            substrings_needed += substring_count;
            i = 0;
            for (KeyVector::iterator rk = joined.begin();
                    rk != joined.end();
                    ++rk)
            {
                if (*rk == sought)
                {
                    if (expectedSpots[i])
                    {
                        substrings_correct++;
                        // no two subsequent hyphens
                        ++rk;
                        ++i;
                        if (rk == joined.end())
                        {
                            break;
                        }
                    }
                    else
                    {
                        substrings_incorrect++;
                        string* rkvs = keyVectorToString(&joined, key_table);
                        fprintf(message_out, " * SUB_EXTRA for %s : %s (%s[%u])\n",
                                testcase.c_str(), ekvs->c_str(), rkvs->c_str(),
                                i);
                    }
                }
                else
                {
                    if (expectedSpots[i])
                    {
                        substrings_missed++;
                        string* rkvs = keyVectorToString(&joined, key_table);
                        fprintf(message_out, " * SUB_MISS for %s : %s (%s[%u])\n",
                                testcase.c_str(), ekvs->c_str(), rkvs->c_str(),
                                i);
                    }
                    i++;
                }
            } // for each key in result
         }
	} // for each expected key vector
	return rv;
}

bool
test_ranked(const string& testcase, vector<TransducerHandle>* ranked,
		set<KeyVector>& r, set<KeyVector>& e,
		bool infinite_r, KeyTable* key_table)
{
	bool rv = false;
	if (e.size() > 1)
	{
		fprintf(message_out, "Warning: more than one expected result to be"
				"ranked?\n");
	}
	for (set<KeyVector>::iterator ekv = e.begin();
			ekv != e.end();
			++ekv)
	{
		unsigned int rank = 0;
		KeyVector hmmekv = *ekv;
		string* ekvs = keyVectorToString(&hmmekv, key_table);
		bool found_result = false;
		set<KeyVector> uniqStrings;
		for (vector<TransducerHandle>::iterator t = ranked->begin();
				t != ranked->end();
				++t)
		{
			KeyVector* flagtest = new KeyVector;
			State current_state = get_initial_state(*t);
			while (!is_final_state(current_state, *t))
			{
				TransitionIterator ti = begin_ti(*t, current_state);
				Transition tr = get_ti_transition(ti);
				current_state = get_transition_to(tr);
				KeyPair* kp = get_transition_keypair(tr);
				flagtest->push_back(get_input_key(kp));
			}
			KeyVector* filtered = flag_diacritic_table.filter_diacritics(flagtest);
			if (filtered == NULL)
			{
				continue;
			}
			filtered->erase(remove_if(filtered->begin(), filtered->end(),
								_is_epsilon), filtered->end());
			if (uniqStrings.find(*filtered) != uniqStrings.end())
			{
				continue;
			}
			else
			{
				uniqStrings.insert(*filtered);
				rank++;
			}
			KeyVectorVector* lookups = lookup_all(*t, &hmmekv,
					&flag_diacritic_set);
			for (KeyVectorVector::iterator lkv = lookups->begin();
					lkv != lookups->end();
					++lkv)
			{
				KeyVector* hmmlkv = *lkv;
				if (rank <= ranked_lowest_counted)
				{
					ranked_at[rank]++;
					fprintf(message_out, " * RANK %u  %s : %s\n", rank,
							testcase.c_str(), ekvs->c_str());
				}
				else
				{
					fprintf(message_out, " * LOW RANK %u  %s : %s\n", rank,
							testcase.c_str(), ekvs->c_str());
					ranked_lower++;
				}
				found_result = true;
				break;
			} // for each lookup
			if (found_result)
			{
				break;
			}
		} // for each path
		if (!found_result)
		{
			fprintf(message_out, " * NO RANK %s : %s\n",
				testcase.c_str(), ekvs->c_str());
			ranked_none++;
		}
		delete ekvs;
		rv = found_result;
	} // for each expected
	return rv;
}

KeyVectorVector*
lookup_all(const char* s, KeyTable* kt,
		vector<TransducerHandle> cascade, bool* infinite)
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
		VERBOSE_PRINT("No tokenisations for %s\n", s);
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
				VERBOSE_PRINT("Looking up %s from transducer %zu\n",
						kvstring->c_str(), cascade_number);
				delete kvstring;
				if (is_infinitely_ambiguous(*t,true, *kv))
				{
					VERBOSE_PRINT("Got infinite results\n");
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
						VERBOSE_PRINT("Got no results\n");
						lookups = new KeyVectorVector;
					}
					for (KeyVectorVector::iterator lkv = lookups->begin();
							lkv != lookups->end();
							++lkv)
					{
						KeyVector* hmmlkv = *lkv;
						string* lkvstring = keyVectorToString(hmmlkv, kt);
						VERBOSE_PRINT("Got %s\n", lkvstring->c_str());
						delete lkvstring;
						KeyVector* filtlkv = flag_diacritic_table.filter_diacritics(hmmlkv);
						if (filtlkv == NULL)
						{
							VERBOSE_PRINT("Filtered out\n");
							continue;
						}
						filtlkv->erase(remove_if(filtlkv->begin(), filtlkv->end(),
											_is_epsilon), filtlkv->end());
						string* filtlkvs = keyVectorToString(filtlkv, kt);
						VERBOSE_PRINT("Filtered %s\n", filtlkvs->c_str());
						delete filtlkvs;
						current_results->push_back(filtlkv);
					}
                    delete lookups;
				}
			}
			final_results = current_results;
			for (KeyVectorVector::iterator kv = past_results->begin();
					kv != past_results->end();
					++kv)
			{
				KeyVector* hmmkv = *kv;
				delete hmmkv;
			}
			delete past_results;
		} // for each transducer in cascade
		return final_results;
	} // if proper lookup originally
	return NULL;
}


vector<TransducerHandle>*
lookup_all_weighted(const char* s, KeyTable* kt,
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
		VERBOSE_PRINT("No tokenisations for %s\n", s);
		*infinite = false;
		return NULL;
	}
	else
	{
		TransducerHandle currentPaths = create_epsilon_transducer();
		for (KeyVector::iterator k = lookup_orig->begin();
				k != lookup_orig->end();
				++k)
		{
			currentPaths = minimize(concatenate(currentPaths, define_transducer(*k)));
		}
		size_t cascade_number = 0;
		for (vector<TransducerHandle>::iterator t = cascade.begin();
				t != cascade.end();
				++t)
		{
			cascade_number++;
			if (cascade_number < 2)
			{
				VERBOSE_PRINT("Composing %s...\n", s);
			}
			else
			{
				VERBOSE_PRINT("Composing %s depth %zu...\r", s, cascade_number);
			}
			vector<TransducerHandle> rulesVec(1, currentPaths);
			TransducerHandle newPaths = intersecting_composition(
					invert(copy(*t)), &rulesVec, kt);
			if (newPaths != NULL)
			{
				currentPaths = minimize(newPaths);
			}
			else
			{
				currentPaths = create_empty_transducer();
			}
		} // for each transducer in cascade
		VERBOSE_PRINT("\n");
		vector<TransducerHandle> unordered_paths = find_all_paths(
				minimize(extract_input_language(currentPaths)));
		vector<float> path_weights;
		vector<TransducerHandle>* ordered_paths = new vector<TransducerHandle>;
		for (vector<TransducerHandle>::iterator t = unordered_paths.begin();
				t != unordered_paths.end();
				++t)
		{
			float this_weight = get_weight(*t);
			unsigned int i = 0;
			for (i = 0; i < path_weights.size(); i++)
			{
				if (path_weights[i] > this_weight)
				{
					break;
				}
			}
			ordered_paths->insert(ordered_paths->begin() + i, *t);
			path_weights.insert(path_weights.begin() + i, this_weight);
		}
		return ordered_paths;
	} // if proper lookup originally
	return NULL;
}

} // namespace

int
process_stream(std::istream& inputstream, std::ostream& outstream)
{
	VERBOSE_PRINT("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
	bool any_failures = false;
	if (format_type == SFST_FORMAT)
	{
		VERBOSE_PRINT("Skipping unweighted format!\n");
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
			HWFST::define_flag_diacritics(key_table);
			VERBOSE_PRINT("\n");
			size_t len;
            char* line = 0;
			while ((hfst_getline(&line, &len, relation_file)) != -1)
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
				bool infinity = false;
				HFST::KeyVectorVector* results = 0;
				vector<HWFST::TransducerHandle>* rankedResults = 0;
				if (test_type == TEST_RANKED)
				{
					VERBOSE_PRINT("Ranking %s (may take a while)...\n", line);
					rankedResults = HWFST::lookup_all_weighted(line,
							key_table, cascade, &infinity);
					results = new KeyVectorVector;
				}
				else
				{
                    VERBOSE_PRINT("Looking up %s...\n", line);
					results = HWFST::lookup_all(line,
						key_table, cascade,
						&infinity);
					rankedResults = new vector<HWFST::TransducerHandle>;
				}
				set<HWFST::KeyVector> expectedLines;
                size_t expLen;
                ssize_t readBytes;
				char* expectedLine = 0; 
				readBytes = hfst_getline(&expectedLine, &expLen, relation_file);
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
					VERBOSE_PRINT("Expecting %s\n", expectedLine);
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
				readBytes =
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
				bool this_failed;
				switch (test_type)
				{
				case TEST_EXACTLY:
					this_failed = !(HWFST::test_exactly(line,
							resultLines, expectedLines,
							infinity, key_table));
					break;
				case TEST_NONE:
					this_failed = !(HWFST::test_none(line,
								resultLines, expectedLines,
								infinity, key_table));
					break;
				case TEST_AT_LEAST:
					this_failed = !(HWFST::test_at_least(line,
								resultLines, expectedLines,
								infinity, key_table));
					break;
				case TEST_SUBSTRING:
					this_failed = !(HWFST::test_substring(line,
								resultLines, expectedLines,
								infinity, key_table));
					break;
				case TEST_RANKED:
					this_failed = !(HWFST::test_ranked(line, rankedResults,
								resultLines, expectedLines,
								infinity, key_table));
					break;
				default:
					this_failed = true;
					fprintf(message_out, "Unknown test type %d!\n", test_type);
					break;
				}
				for (vector<TransducerHandle>::iterator t = rankedResults->begin();
						t != rankedResults->end();
						++t)
				{
					HWFST::delete_transducer(*t);
				}
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
			VERBOSE_PRINT("FAIL\n");
			return EXIT_FAILURE;
		}
		else
		{
			VERBOSE_PRINT("OK\n");
			return EXIT_SUCCESS;
		}
	}
	else
	{
		fprintf(message_out, "ERROR: Transducer has wrong type.\n");
		return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
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
	switch (test_type)
	{
	case TEST_EXACTLY:
		VERBOSE_PRINT("Matching if result strings are same as expected\n");
		break;
	case TEST_AT_LEAST:
		VERBOSE_PRINT("Matching if result strings are subset of expected\n");
		break;
	case TEST_NONE:
		VERBOSE_PRINT("Matching if none of the result strings are expected\n");
		break;
	case TEST_SUBSTRING:
		VERBOSE_PRINT("Matching if %s appears at same positions\n", substring);
		break;
	case TEST_RANKED:
		VERBOSE_PRINT("Matching if top %lu result strings are in same order as expected\n", ranked_lowest_counted);
		break;
	default:
		fprintf(message_out, "Broken test setting\n");
		return EXIT_FAILURE;
		break;
	}
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

