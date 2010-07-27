//! @file hfst-flag-diacritics.cc
//!
//! @brief Transducer flag diacritic tool
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

#if HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif

#include "hfst.h"
#include "flag-diacritics/FlagDiacritics.h"
#include "string/string.h"
#include "regexp/regexp.h"

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-unary.h"
// add tools-specific variables here
static bool print_flags = false;
static char* eliminate_flag = 0;

FlagDiacriticTable flag_diacritic_table;
HFST::KeySet flag_diacritic_set;
set<string> flag_strings;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		   "handle Xerox flag diacritics in transducer\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		   "  -d, --debug            Print debugging messages and results\n"
		);
#               endif
	print_common_unary_program_options(message_out);
	fprintf(message_out, 
			"  -p, --print-flags                Print flag diacritics\n"
			"  -e, --eliminate-flag=FLAG        Eliminate flag FLAG by composing relevant rules\n");
	fprintf(message_out,
		   "\n"
		   "If OUTFILE or INFILE is missing or -, "
		   "standard streams will be used.\n"
		   "\n"
		   "More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstFlagDiacritics>\n"
		   "\n"
		   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}

void
print_version(const char* program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(message_out, "%s 0.1 (%s)\n"
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
			{"print-flags", required_argument, 0, 'p'},
			{"eliminate-flag", required_argument, 0, 'e'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "de:f:hi:I:o:psSqvVrR:DW:t:",
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
		case 'p':
			print_flags = true;
			break;
		case 'e':
			eliminate_flag = hfst_strdup(optarg);
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
            message_out = stderr;
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

namespace HFST
{
void define_flag_diacritics(KeyTable * key_table)
{
  
  for (Key k = 0; k < key_table->get_unused_key(); ++k)
    {
      flag_diacritic_table.define_diacritic
	(k,get_symbol_name(get_key_symbol(k,key_table)));
      if (flag_diacritic_table.is_diacritic(k))
	{
      flag_diacritic_set.insert(k);
      flag_strings.insert(get_symbol_name(get_key_symbol(k, key_table)));
    }
    }
}
}

namespace HWFST
{
void define_flag_diacritics(KeyTable * key_table)
{
  
  for (Key k = 0; k < key_table->get_unused_key(); ++k)
    {
      flag_diacritic_table.define_diacritic
	(k,get_symbol_name(get_key_symbol(k,key_table)));
      if (flag_diacritic_table.is_diacritic(k))
	{
      flag_diacritic_set.insert(k); 
      flag_strings.insert(get_symbol_name(get_key_symbol(k, key_table)));
    }
    }
}
}

static
void
parse_flags_yet_again(const string& name,
                      bool* c_name, bool* r_name, bool* d_name,
                      set<string>& p_values, set<string>& n_values,
                      set<string>& r_values, set<string>& d_values,
                      set<string>& u_values)
{
    string findName = string(".") + name;
    for (set<string>::iterator s = flag_strings.begin();
         s != flag_strings.end();
         ++s)
      {
        if (s->find(findName) == 2)
          {
            size_t secondFullStop = s->find(".", s->find(".") + 1);
            if (secondFullStop != string::npos)
              {
                string value = s->substr(secondFullStop + 1,
                                         s->length() - secondFullStop - 2);
                switch (s->at(1))
                  {
                  case 'R':
                    r_values.insert("\"" + *s + "\"" );
                    verbose_printf("Require %s == %s spotted\n", 
                                  name.c_str(), value.c_str());
                    break;
                  case 'D':
                    d_values.insert("\"" + *s + "\"");
                    verbose_printf("Disallow %s == %s spotted\n", 
                                  name.c_str(), value.c_str());
                    break;
                  case 'U':
                    u_values.insert("\"" + *s + "\"");
                    verbose_printf("Unify %s == %s spotted\n", 
                                  name.c_str(), value.c_str());
                    break;
                  case 'P':
                    p_values.insert("\"" + *s + "\"");
                    verbose_printf("PSet %s == %s spotted\n", 
                                  name.c_str(), value.c_str());
                    break;
                  case 'N':
                    n_values.insert("\"" + *s + "\"");
                    verbose_printf("NSet %s == %s spotted\n", 
                                  name.c_str(), value.c_str());
                    break;
                  default:
                    fprintf(stderr, "Couldn't parse flag %s\n", s->c_str());
                    break;
                  }
              }
            else
              {
                switch (s->at(1))
                  {
                  case 'C':
                    *c_name = true;
                    verbose_printf("Clear %s spotted\n", name.c_str());
                    break;
                  case 'R':
                    *r_name = true;
                    verbose_printf("Require %s spotted\n", name.c_str());
                    break;
                  case 'D':
                    *d_name = true;
                    verbose_printf("Disallow %s spotted\n", name.c_str());
                    break;
                  default:
                    fprintf(stderr, "Couldn't parse flag %s\n", s->c_str());
                    break;
                  }
              }
          }
      }
}



int
process_stream(std::istream& inputstream, std::ostream& outstream)
{
	verbose_printf("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
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
				HFST::define_flag_diacritics(key_table);
				if (nth_stream < 2)
				{
					if (print_flags || verbose)
					{
						fprintf(message_out, "Printing flags... ");
						for (HFST::KeySet::const_iterator k = flag_diacritic_set.begin();
								k != flag_diacritic_set.end();
								++k)
						{
							string* ks = HFST::keyToString(*k, key_table);
							fprintf(message_out, "%s ", ks->c_str());
							delete ks;
						}
						fprintf(message_out, "\n");
					}
					if (eliminate_flag)
					{
						verbose_printf("Eliminating %s...\n", eliminate_flag);
					}
				}
				else
				{
					if (print_flags || verbose)
					{
						fprintf(message_out, "Printing flags %zu...\n", nth_stream);
						for (HFST::KeySet::const_iterator k = flag_diacritic_set.begin();
								k != flag_diacritic_set.end();
								++k)
						{
							string* ks = HFST::keyToString(*k, key_table);
							fprintf(message_out, "%s ", ks->c_str());
							delete ks;
						}
						fprintf(message_out, "\n");
					}
					if (eliminate_flag)
					{
						verbose_printf("Eliminating %s, %zu...\n",
								eliminate_flag, nth_stream);
					}
				}
                // for each, R, D, and U, build
				// R.X preceded by P.X.?, N.X.? or U.X.? 
				//        w/o intervening C.X
				//   [P.X.? | N.X.? | U.X.?] \C.X* R.X
				// R.X.Y preceded P.X.Y, U.X.Y or N.X.!Y 
				//       w/o intervening C.X, P.X.!Y, U.X.!Y or N.X.Y
				//   [P.X.Y | U.X.Y | N.X.!Y] \[C.X | P.X.!Y | U.X.Y]* R.X.Y
				// U.X.Y preceded by 0, C.X, P.X.Y, U.X.Y or N.X.!Y
				//      w/o intervening P.X.!Y, U.X.!Y or N.X.Y
				//   [C.X | P.X.Y | U.X.Y | N.X.!Y | #] \[P.X.!Y | U.X.!Y | N.X.Y]* U.X.Y
				// D.X preceded by 0 or C.X
				//      w/o intervening P.X.?, U.X.? or N.X.?
				//   [C.X | #] \[P.X.? | U.X.? | N.X.?]* D.X
				// D.X.Y preceded by 0, C.X, P.X.!Y, U.X.!Y or N.X.Y
				//      w/o intervening P.X.Y, U.X.Y or N.X.!Y
				//   [C.X | P.X.!Y | U.X.!Y | N.X.Y | #] \[P.X.Y | U.X.Y | N.X.!Y] D.X.Y
				
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
                HWFST::KeySet* ks = HWFST::get_key_set(key_table);
                HWFST::KeyPairSet* pi = new HWFST::KeyPairSet;
                for (HWFST::KeySet::const_iterator k1 = ks->begin();
                     k1 != ks->end();
                     ++k1)
                  {
                    if (*k1 == 0)
                      {
                        continue;
                      }
                    pi->insert(HWFST::define_keypair(*k1, *k1));
                  }
				HWFST::define_flag_diacritics(key_table);
				if (nth_stream < 2)
				{
					if (print_flags || verbose)
					{
						verbose_printf("Printing flags... ");
						for (HWFST::KeySet::const_iterator k = flag_diacritic_set.begin();
								k != flag_diacritic_set.end();
								++k)
						{
							string* ks = HWFST::keyToString(*k, key_table);
							fprintf(message_out, "%s ", ks->c_str());
							delete ks;
						}
						fprintf(message_out, "\n");
					}
					if (eliminate_flag)
					{
						verbose_printf("Eliminating %s...\n", eliminate_flag);
					}
				}
				else
				{
					if (print_flags || verbose)
					{
						verbose_printf("Printing flags %zu...\n", nth_stream);
						for (HWFST::KeySet::const_iterator k = flag_diacritic_set.begin();
								k != flag_diacritic_set.end();
								++k)
						{
							string* ks = HWFST::keyToString(*k, key_table);
							verbose_printf("%s ", ks->c_str());
							delete ks;
						}
					}
					if (eliminate_flag)
					{
						verbose_printf("Eliminating %s, %zu...\n",
								eliminate_flag, nth_stream);
					}
				}
                HWFST::TransducerHandle filtered = HWFST::copy(input);
                if (eliminate_flag)
                  {
                    bool c_x = false;
                    string c_x_s("\"@C.");
                    c_x_s += eliminate_flag;
                    c_x_s += "@\"";
                    bool r_x = false;
                    string r_x_s("\"@R.");
                    r_x_s += eliminate_flag;
                    r_x_s += "@\"";
                    bool d_x = false;
                    string d_x_s("\"@D.");
                    d_x_s += eliminate_flag;
                    d_x_s += "@\"";
                    set<string> p_x_y;
                    set<string> n_x_y;
                    set<string> u_x_y;
                    set<string> r_x_y;
                    set<string> d_x_y;
                    parse_flags_yet_again(eliminate_flag,
                          &c_x, &r_x, &d_x,
                          p_x_y, n_x_y, r_x_y, d_x_y, u_x_y);
                    // for each, R, D, and U, build
                    // R.X preceded by P.X.?, N.X.? or U.X.? 
                    //        w/o intervening C.X
                    //   [P.X.? | N.X.? | U.X.?] \C.X* R.X
                    if (r_x)
                      {
                        string r_x_filter("~[ ");
                        if (c_x)
                          {
                            r_x_filter += c_x_s;
                          }
                        r_x_filter += " ";
                        bool first = true;
                        if ((p_x_y.size() + n_x_y.size() + u_x_y.size()) > 0)
                          {
                            r_x_filter += "~$[ ";
                          }
                        for (set<string>::const_iterator y = p_x_y.begin();
                             y != p_x_y.end();
                             ++y)
                          {
                            if (first)
                              {
                                first = false;
                              }
                            else
                              {
                                r_x_filter += " | ";
                              }
                            r_x_filter += *y;
                          }
                        for (set<string>::const_iterator y = n_x_y.begin();
                             y != n_x_y.end();
                             ++y)
                          {
                            if (first)
                              {
                                first = false;
                              }
                            else
                              {
                                r_x_filter += " | ";
                              }
                            r_x_filter += *y;
                          }
                        for (set<string>::const_iterator y = u_x_y.begin();
                             y != u_x_y.end();
                             ++y)
                          {
                            if (first)
                              {
                                first = false;
                              }
                            else
                              {
                                r_x_filter += " | ";
                              }
                            r_x_filter += *y;
                          }
                        if (!first)
                          {
                            r_x_filter += " ] ";
                          }
                        r_x_filter += r_x_s;
                        r_x_filter += "]";
                        verbose_printf("Eliminating %s by composition of %s leftwards...\n", 
                                      r_x_s.c_str(), r_x_filter.c_str());
                        HWFST::TransducerHandle r_x_transducer = HWFST::minimize(HWFST::extract_input_language(HWFST::compile_xre(r_x_filter.c_str(),
                                                                                  pi, pi,
                                                                                  key_table)));
                        HWFST::print_transducer(r_x_transducer, key_table, std::cerr);
                        filtered = HWFST::compose(HWFST::copy(r_x_transducer), filtered);
                        verbose_printf("rightwards...\n");
                        filtered = HWFST::compose(filtered, r_x_transducer);
                    } // if r.x

                    // R.X.Y preceded P.X.Y, U.X.Y or N.X.!Y 
                    //       w/o intervening C.X, P.X.!Y, U.X.!Y or N.X.Y
                    //   [P.X.Y | U.X.Y | N.X.!Y] \[C.X | P.X.!Y | U.X.Y]* R.X.Y
                    // U.X.Y preceded by 0, C.X, P.X.Y, U.X.Y or N.X.!Y
                    //      w/o intervening P.X.!Y, U.X.!Y or N.X.Y
                    //   [C.X | P.X.Y | U.X.Y | N.X.!Y | #] \[P.X.!Y | U.X.!Y | N.X.Y]* U.X.Y
                    // D.X preceded by 0 or C.X
                    //      w/o intervening P.X.?, U.X.? or N.X.?
                    //   [C.X | #] \[P.X.? | U.X.? | N.X.?]* D.X
                    // D.X.Y preceded by 0, C.X, P.X.!Y, U.X.!Y or N.X.Y
                    //      w/o intervening P.X.Y, U.X.Y or N.X.!Y
                    //   [C.X | P.X.!Y | U.X.!Y | N.X.Y | #] \[P.X.Y | U.X.Y | N.X.!Y] D.X.Y
                    // and rewrite as epsilons
                    for (HWFST::KeySet::const_iterator k = flag_diacritic_set.begin();
                         k != flag_diacritic_set.end();
                         ++k)
                      {
                        filtered = HWFST::substitute_key(filtered, *k, 0);
                      }
                }
                verbose_printf("Saving to %s\n", outfilename);
                HWFST::write_transducer(filtered, key_table, outstream);
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

