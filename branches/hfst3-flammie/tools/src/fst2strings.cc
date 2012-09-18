//! @file hfst-fst2strings.cc
//!
//! @brief Transducer path printing command line tool
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

#include <hfst.hpp>

#include "conventions/commandline.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::FdOperation;

using hfst::WeightedPaths;
using hfst::WeightedPath;
using hfst::StringPairVector;
using hfst::HfstTwoLevelPath;
using hfst::HfstTwoLevelPaths;

using hfst::HFST_OL_TYPE;
using hfst::HFST_OLW_TYPE;


// the maximum number of strings printed for each transducer
static int max_strings = 0;
static int cycles = -1;
static int nbest_strings=-1;
static int max_random_strings=-1;
static bool display_weights=false;
static bool eval_fd=false;
static bool filter_fd=true;
static bool quote_special=false;
static bool print_spaces=false;
static unsigned int max_input_length = 0;
static unsigned int max_output_length = 0;
static std::string input_prefix;
static std::string output_prefix;
static std::string input_exclude;
static std::string output_exclude;

static bool print_in_pairstring_format=false;
static char* epsilon_format=0;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Display the strings recognized by a transducer\n"
        "\n", program_name);
    print_common_program_options();
    fprintf(message_out, "Traversal options:\n"
            "  -n, --max-strings=NSTR     print at most NSTR strings\n"
            "  -N, --nbest=NBEST          print at most NBEST best strings\n"
            "  -r, --random=NRAND         print at most NRAND random strings\n"
            "  -c, --cycles=NCYC          follow cycles at most NCYC times\n");
    fprintf(message_out, "Output options:\n"
            "  -F, --path-format=PFMT     print paths using PFMT format\n"
            "  -w, --print-weights        display the weight for each string\n"
            "  -e, --epsilon-format=EPS   print epsilon as EPS\n"
            "  -X, --xfst=VARIABLE        toggle xfst compatibility option "
            "VARIABLE\n");
    fprintf(message_out, "Pruning options; print if:\n"
            "  -l, --max-in-length=MIL    first level shorter or "
            "equal to MIL bytes\n"
            "  -L, --max-out-length=MOL   second level shorter or "
            "equal to MOL bytes\n"
            "  -p, --in-prefix=OPREFIX    first level begins with IPREFIX\n"
            "  -P, --out-prefix=OPREFIX   second level begins with OPREFIX\n"
            "  -u, --in-exclude=IXSTR     first level does not "
            "contain IXSTR\n"
            "  -U, --out-exclude=OXST     second level does not "
            "contain OXSTR\n");

    fprintf(message_out, "\n");

    print_common_parameter_instructions();
    fprintf(message_out, "PFMT is one of `xerox'; if omitted, it defaults to "
            "`xerox'\n."
            "If all NSTR, NBEST and NCYC are omitted, "
            "all possible paths are printed:\n"
            "NSTR, NBEST and NCYC default to infinity.\n"
            "NBEST overrides NSTR and NCYC\n"
            "NRAND overrides NBEST, NSTR and NCYC\n"
            "If EPS is not given, default is empty string.\n"
            "Numeric options are parsed with strtod(3).\n"
            "Xfst variables supported are { obey-flags, print-flags,\n"
            "print-pairs, print-space, quote-special }.\n"
            "Xfst variables only have effect if PFMT is xerox\n");
    fprintf(message_out,
        "\n"
        "Examples:\n"
        "  %s lexical.hfst  generates all forms of lexical.hfst\n"
        "\n", program_name);

    fprintf(message_out, 
        "Known bugs:\n"
        "  Does not work correctly for hfst optimized lookup format.\n"
        "  Prints duplicate strings for foma format.\n"
        "\n"
        );

    print_report_bugs();
    print_more_info();
}

void
parse_options(int argc, char** argv)
{
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
          {
            HFST_GETOPT_COMMON_LONG,
            HFST_GETOPT_UNARY_LONG,
            {"cycles", required_argument, 0, 'c'},
            {"epsilon-format", required_argument, 0, 'e'},
            {"in-exclude", required_argument, 0, 'u'},
            {"in-prefix", required_argument, 0, 'p'},
            {"max-in-length", required_argument, 0, 'l'},
            {"max-out-length", required_argument, 0, 'L'},
            {"max-strings", required_argument, 0, 'n'},
            {"nbest", required_argument, 0, 'N'},
            {"random", required_argument, 0, 'r'},
            {"out-exclude", required_argument, 0, 'U'},
            {"out-prefix", required_argument, 0, 'P'},
            {"print-pairstrings", no_argument, 0, 'a'},
            {"print-spaces", no_argument, 0, 'S'},
            {"print-weights", no_argument, 0, 'w'},
            {"xfst", required_argument, 0, 'X'},
            {"path-format", required_argument, 0, 'F'},
            {0,0,0,0}
          };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT
                             "Sawc:e:u:F:p:l:L:n:r:N:U:P:X:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        switch (c)
          {
          case 'n':
            max_strings = hfst_strtoul(optarg, 10);
            break;
          case 'N':
            nbest_strings = hfst_strtoul(optarg, 10);
            break;
          case 'r':
            max_random_strings = hfst_strtoul(optarg, 10);
            break;
          case 'c':
            cycles = hfst_strtoul(optarg, 10);
            break;
          case 'w':
            display_weights = true;
            break;
          case 'X':
            if (strcmp(optarg, "obey-flags") == 0)
              {
                eval_fd = true;
              }
            else if (strcmp(optarg, "print-flags") == 0)
              {
                filter_fd = false;
              }
            else if (strcmp(optarg, "quote-special") == 0)
              {
                quote_special = true;
              }
            else if (strcmp(optarg, "print-pairs") == 0)
              {
                print_in_pairstring_format = true;
              }
            else if (strcmp(optarg, "print-space") == 0)
              {
                print_spaces = true;
              }
            else
              {
                hfst_error(0, EXIT_FAILURE, "Unrecognised xfst option. "
                     "available options are obey-flags, print-flags\n");
              }
            break;
          case 'l':
            max_input_length = hfst_strtoul(optarg, 10);
            break;
          case 'L':
            max_output_length = hfst_strtoul(optarg, 10);
            break;
          case 'p':
            input_prefix = optarg;
            break;
          case 'P':
            output_prefix = optarg;
            break;
          case 'u':
            input_exclude = optarg;
            break;
          case 'U':
            output_exclude = optarg;
            break;
          case 'e':
            epsilon_format = hfst_strdup(optarg);
            break;
          case 'a':
            print_in_pairstring_format = true;
            break;
          case 'S':
            print_spaces = true;
            break;
          case 'F':
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
    }
}

/* Replace all strings \a str1 in \a symbol with \a str2. */
static std::string replace_all(std::string symbol, 
                   const std::string &str1,
                   const std::string &str2)
{
  size_t pos = symbol.find(str1);
  while (pos != std::string::npos) // while there are str1:s to replace
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
  // print epsilon as defined by the user or use the default
  if (hfst::is_epsilon(s))
    return std::string(strdup(epsilon_format));

  if (not quote_special)
    return std::string(s);

  // escape spaces and colons as they have a special meaning
  return 
    replace_all 
    (replace_all
     ( replace_all
       (std::string(s), " ", "@_SPACE_@"),
       ":", "@_COLON_@"),
     "\t", "@_TAB_@");
}

//Print results as they come
class Callback : public hfst::ExtractStringsCb
{
public:
  int count;
  int max_num;
  std::ostream* out_;
  Callback(int max, std::ostream* out): count(0), max_num(max), out_(out) {}
  RetVal operator()(HfstTwoLevelPath &path, bool final)
  {
    std::string istring;
    std::string ostring;
    for (StringPairVector::const_iterator it = path.second.begin();
         it != path.second.end(); it++) {
      istring.append(it->first);
      ostring.append(it->second);
    }


    if ((max_input_length > 0) &&
        (istring.length() > max_input_length))
      {
        // continue searching, break off this path
        return RetVal(true, false);
      }
    if ((max_output_length > 0) &&
        (ostring.length() > max_output_length))
      {
      return RetVal(true, false);
      // continue searching, break off this path
      }
    if (input_prefix.length() > 0)
      {
        if (istring.length() < input_prefix.length())
          {
            return RetVal(true, true);
          }
      if (istring.compare(0, input_prefix.length(), input_prefix) != 0)
        {
          return RetVal(true, false);
          // continue searching, break off this path
        }
      }
    if(output_prefix.length() > 0)
      {
        if (ostring.length() < output_prefix.length())
          {
            return RetVal(true, true);
          }
        if (ostring.compare(0, output_prefix.length(), output_prefix) != 0)
          {
            return RetVal(true, false);
            // continue searching, break off this path
          }
      }
    if (input_exclude.length() > 0 && 
        (istring.find(input_exclude) != std::string::npos))
      {
        return RetVal(true, false);
        // continue searching, break off this path
      }
    if (output_exclude.length() > 0 && 
        (ostring.find(output_exclude) != std::string::npos))
      {
        return RetVal(true, false);
        // continue searching, break off this path
      }
    // the path passed the checks. Print it if it is final
    if (final)
      {
        if (print_in_pairstring_format) 
          {
        bool first_pair=true;
            for (StringPairVector::const_iterator it = path.second.begin();
                 it != path.second.end(); it++) 
              {
        if ((not filter_fd) || 
            (not FdOperation::is_diacritic(it->first))) {
          if (print_spaces && not first_pair) 
            {
              *out_ << " ";
            }
        
          *out_ << get_print_format(it->first);
          first_pair=false;
        }

        if (it->first.compare(it->second) != 0)
          {
            if ((not filter_fd) || 
            (not FdOperation::is_diacritic(it->second)))
              *out_ << ":"
                << get_print_format(it->second);
          }
              }
            if (display_weights) 
              {
                *out_ << "\t" << path.first;
              }
            *out_ << "\n";
          }
        else 
          {
        bool is_automaton=true;

        bool first_symbol=true;
            for (StringPairVector::const_iterator it = path.second.begin();
                 it != path.second.end(); ++it)
              {
        if ((not filter_fd) || 
            (not FdOperation::is_diacritic(it->first)))
          {
            if (print_spaces && not first_symbol)
              {
            *out_ << " ";
              }
            if (it->first.compare(it->second) != 0)
              is_automaton=false;
            
            *out_ << get_print_format(it->first);
          }
        first_symbol=false;
          }
        if (print_spaces)
          {
        *out_ << " ";
          }

        if (not is_automaton) {
          *out_ << ":";
          for (StringPairVector::const_iterator it = path.second.begin();
           it != path.second.end(); ++it)
        {
          if ((not filter_fd) || 
              (not FdOperation::is_diacritic(it->second)))
            {
              if (print_spaces)
            {
              *out_ << " ";
            }
              *out_ << get_print_format(it->second);
            }
        }
        }

            if (display_weights)
              {
                *out_ << "\t" << path.first;
              }
            *out_ << std::endl;
           }
        count++;
      }
    // continue until we've printed max_num strings
    return RetVal((max_num < 1) || (count < max_num), true); 
  }
};

void
print_paths()
  {
    size_t n_transducer = 0;
    while(instream->is_good())
      {
        n_transducer++;
        if (n_transducer > 1)
          {
            // FIXME: it is up to formatter to emit this...?
            fprintf(outfile, "-- \n");
          }
        HfstTransducer t(*instream);
        /* Pairstring format is not supported on optimized lookup format. */
        if (print_in_pairstring_format && 
        (instream->get_type() == HFST_OL_TYPE || 
         instream->get_type() == HFST_OLW_TYPE) ) {
          fprintf(stderr, 
              "Error: option --print-in-pairstring-format not supported on "
              "       optimized lookup transducers, exiting program\n" );
          exit(1);
        }
        if(input_prefix != "")
          verbose_printf("input_prefix: '%s'\n", input_prefix.c_str());
        if(nbest_strings > 0)
        {
          verbose_printf("Pruning transducer to %i best path(s)...\n", 
                 nbest_strings);
          t.n_best(nbest_strings);
        }
        else
        {
          if(max_random_strings <= 0 && max_strings <= 0 && max_input_length <= 0 
         && max_output_length <= 0 &&
         cycles < 0 && t.is_cyclic())
          {
            hfst_error(EXIT_FAILURE, 0,
                  "Transducer is cyclic. Use one or more of these options: "
                  "-n, -N, -r, -l, -L, -c");
          }
        }
        if(max_strings > 0)
          verbose_printf("Finding at most %i path(s)...\n", max_strings);
        else if(max_random_strings > 0)
          verbose_printf("Finding at most %i random path(s)...\n", 
                 max_random_strings);
        else
          verbose_printf("Finding strings...\n");
        /* not random strings */
        if (max_random_strings <= 0)
          {
        Callback cb(max_strings, &cout);
        if(eval_fd)
          t.extract_paths_fd(cb, cycles, filter_fd);
        else
          t.extract_paths(cb, cycles);    
        verbose_printf("Printed %i string(s)\n", cb.count);
          }
        /* random strings */
        else
          {
            HfstTwoLevelPaths results;
            try {
              if (eval_fd) {
                t.extract_random_paths_fd(results, max_random_strings, filter_fd);
              }
              else {
                t.extract_random_paths(results, max_random_strings);
              }
            }
            catch (const HfstException e) {
              fprintf(stderr, "option --random not implemented\n");
            }
            Callback cb(max_random_strings, &cout);
            for (HfstTwoLevelPaths::const_iterator it = results.begin();
                 it != results.end(); it++)
              {
                HfstTwoLevelPath path = *it;
                cb(path, true /*final*/);
              }
            verbose_printf("Printed %i random string(s)\n", cb.count);
          }
    }
  }

void
check_options(int, char**)
  {
    if (NULL == epsilon_format)
      {
        epsilon_format = hfst_strdup("");
        hfst_verbose("Epsilon format defaults to `' (empty string);"
                     "to see epsilons, use -e");
      }
    if ((max_strings > 0) && (max_random_strings > 0))
      {
        hfst_warning("option --max-strings ignored when --random used");
        max_strings = -1;
      }
  }

int main(int argc, char **argv)
  {
    hfst_init_commandline(argv[0], "0.1", "HfstFst2Strings",
                          AUTOM_IN_FILE_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    print_paths();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
}
