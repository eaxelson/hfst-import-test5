//! @file hfst-pmatch2fst.cc
//!
//! @brief regular expression compiling command line tool
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

#include <vector>
#include <map>
#include <string>

using std::string;
using std::vector;
using std::pair;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>
#include <errno.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "parsers/PmatchCompiler.h"
#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "hfst-tool-metadata.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::HfstTransducer;
using hfst::pmatch::PmatchCompiler;

static char *epsilonname=NULL;
static bool disjunct_expressions=false;
static bool line_separated = false;

//static unsigned int sum_of_weights=0;
static bool sum_weights=false;
static bool normalize_weights=false;
static bool logarithmic_weights=false;

static hfst::ImplementationType output_format = hfst::UNSPECIFIED_TYPE;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
    "Compile regular expressions into transducer(s)\n (Experimental version)"
        "\n", program_name); 
        print_common_program_options(message_out);
        print_common_unary_program_options(message_out); 
        fprintf(message_out, "String and format options:\n"
"  -f, --format=FMT          Write result in FMT format\n"
"  -j, --disjunct            Disjunct all regexps instead of transforming\n"
"                            each regexp into a separate transducer\n"
"      --sum (todo)          Sum weights of duplicate strings instead of \n"
"                            taking minimum\n"
"      --norm (todo)         Divide each weight by sum of all weights\n"
"      --log (todo)          Take negative logarithm of each weight\n"
"  -l, --line                Input is line separated\n"
"  -S, --semicolon           Input is semicolon separated (default)\n"
"  -e, --epsilon=EPS         Map EPS as zero.\n");
        fprintf(message_out, "\n");

        fprintf(message_out, 
            "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
            "FMT must be one of the following: "
            "{foma, sfst, openfst-tropical, openfst-log}.\n"
            "If EPS is not defined, the default representation of 0 is used\n"
	    "Weights are currently not implemented.\n"
	    "\n"
            );

        fprintf(message_out, "Examples:\n"
"  echo \" c:d a:o t:g \" | %s \n"
"  echo \" cat ; dog \" | %s -S   create transducers \"cat\" and \"dog\"\n"
            "\n", program_name, program_name);
        print_report_bugs();
        fprintf(message_out, "\n");
        print_more_info();
        fprintf(message_out, "\n");
}

int
parse_options(int argc, char** argv)
{
    extend_options_getenv(&argc, &argv);
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
        {
        HFST_GETOPT_COMMON_LONG,
        HFST_GETOPT_UNARY_LONG,
          {"disjunct", no_argument, 0, 'j'},
          {"epsilon", required_argument, 0, 'e'},
          {"sum", no_argument, 0, '1'},
          {"norm", no_argument, 0, '2'},
          {"log", no_argument, 0, '3'},
          {"line", no_argument, 0, 'l'},
          {"semicolon", no_argument, 0, 'S'},
          {"format", required_argument, 0, 'f'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "je:123lSf:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
        case 'e':
            epsilonname = hfst_strdup(optarg);
            break;
        case '1':
            sum_weights = true;
            break;
        case '2':
            normalize_weights = true;
            break;
        case '3':
            logarithmic_weights = true;
            break;
        case 'j':
            disjunct_expressions = true;
            break;
        case 'S':
            line_separated = false;
            break;
        case 'l':
            line_separated = true;
            break;
        case 'f':
            output_format = hfst_parse_format_name(optarg);
            break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    if (output_format == hfst::UNSPECIFIED_TYPE)
      {
        verbose_printf("Output format not specified, "
             "defaulting to openfst tropical\n");
        output_format = hfst::TROPICAL_OPENFST_TYPE;
      }
    return EXIT_CONTINUE;
}

bool is_comment(char * line)
{
    do {
        if (*line == '!') {
            return true;
        }
        ++line;
    } while ((*line == ' ' || *line == '\t') && *line != '0');
    return false;
}

int
process_stream(HfstOutputStream& outstream)
{
  size_t transducer_n = 0;
  char* line = 0;
  size_t len = 0;
  unsigned int line_count = 0;
  PmatchCompiler comp(output_format);
  HfstTransducer disjunction(output_format);
  std::map<std::string, HfstTransducer *> named_transducers;
  //outstream.open();
  int delim = '\n';
  if (line_separated)
    {
      delim = '\n';
    }
  else
    {
      delim = ';';
    }
  char* first_line = 0;
  while (hfst_getdelim(&line, &len, delim, inputfile) != -1)
    {
      if (first_line == 0)
        {
          first_line = strdup(line);
        }
      if (is_comment(line)) {
          continue;
      }
      char* exp = line;
      while ((*exp == '\n') || (*exp == '\r') || (*exp == ' ')) {
          exp++;
      }
      if (*exp == '\0')
        {
          verbose_printf("Skipping whitespace expression #%u", line_count);
          continue;
        }
      line_count++;
      transducer_n++;
      HfstTransducer* compiled;
      verbose_printf("Compiling expression %u\n", line_count);
      compiled = comp.compile(line);
      if (disjunct_expressions)
        {
          disjunction.disjunct(*compiled);
        }
      else
        {
          if (delim == '\n')
            {
              // hfst_set_formula(*compiled,
              //                  string(line).substr(0 ,strlen(line) - 1),
              //                  "X");
            }
          else
            {
//              hfst_set_formula(*compiled, line, "X");
            }
          if (compiled->get_name() != "") {
              named_transducers[compiled->get_name()] = compiled;
          } else {
              delete compiled;
          }
        }
    }

  // When done compiling everything, look for TOP and output it first
        if (named_transducers.count("TOP") == 1) {
          outstream << (*named_transducers["TOP"]).convert(hfst::HFST_OL_TYPE);
          delete named_transducers["TOP"];
          named_transducers.erase("TOP");
      }
        for (std::map<std::string, HfstTransducer *>::iterator it = named_transducers.begin();
           it != named_transducers.end(); ++it) {
          outstream << (*(it->second)).convert(hfst::HFST_OL_TYPE);
          delete it->second;
    }
  if (disjunct_expressions)
    {
          if (delim == '\n')
            {
              // hfst_set_formula(disjunction,
              //                  string(line).substr(0 ,strlen(line) - 1) + "...",
              //                  "X");
            }
          else
            {
//              hfst_set_formula(disjunction, string(line) + "...", "X");
            }
      outstream << disjunction;
    }
  free(line);
  free(first_line);
  return EXIT_SUCCESS;
}

extern int pmatchdebug;

int main( int argc, char **argv ) 
{

//	pmatchdebug = 1;

  hfst_set_program_name(argv[0], "0.1", "Pmatch2Fst");
  int retval = parse_options(argc, argv);
  if (retval != EXIT_CONTINUE)
    {
      return retval;
    }
  // close buffers, we use streams
  if (outfile != stdout)
    {
      fclose(outfile);
    }
  verbose_printf("Reading from %s, writing to %s\n", 
                 inputfilename, outfilename);
  // here starts the buffer handling part
  HfstOutputStream* outstream = (outfile != stdout) ?
      new HfstOutputStream(outfilename, hfst::HFST_OL_TYPE) :
      new HfstOutputStream(hfst::HFST_OL_TYPE);
  process_stream(*outstream);
  free(inputfilename);
  free(outfilename);
  return EXIT_SUCCESS;
}

