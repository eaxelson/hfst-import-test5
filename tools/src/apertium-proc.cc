//! @file apertium-proc.cc
//! @brief frontend for apertium-compatible corpus tool.

//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <getopt.h>
#include <fstream>
#include <cstdlib>

#include "utils/ProcTransducer.h"
#include "utils/ProcFormatters.h"
#include "utils/ProcApplicators.h"

#include "conventions/commandline.h"
#include "conventions/options.h"
#include "conventions/globals-common.h"

unsigned int maxAnalyses = std::numeric_limits<int>::max();
int output_type = 0;
int cmd = 0;
int capitalization = 0;
bool rawMode = false;
bool processCompounds = false;
bool printDebuggingInformationFlag = false;
bool preserveDiacriticRepresentationsFlag = false;
bool null_flush = false;
bool filter_compound_analyses = true;
bool displayWeightsFlag = false;
bool displayUniqueFlag = false;
bool inputIsStdin = false;
bool outputIsStdout = false;

std::istream* input = &std::cin;
std::ostream* output = &std::cout;
std::ifstream* in = 0;
void
print_usage(void)
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] "
            "TRANSFILE [INFILE [OUTFILE]] \n"
           "perform a transducer lookup on an apertium stream, "
           "tokenizing on the fly\n"
        "\n", program_name);

    print_common_program_options();
    fprintf(message_out, "Processing mode options:\n"
    "  -a, --analysis          Morphological analysis (default)\n"
    "  -g, --generation        Morphological generation\n"
    "  -n, --non-marked-gen    Morph. generation without unknown word marks\n"
    "  -t  --tokenize          Tokenize input into symbols (for debugging)\n");
    fprintf(message_out, "Output formatting options:\n"
    "  -p  --apertium          Apertium output format for analysis (default)\n"
    "  -C  --cg                Constraint Grammar output format for analysis\n"
    "  -x, --xerox             Xerox output format for analysis\n"
    "  -T, --tsv               Tab separated values format for analysis\n"
    "  -e, --do-compounds      Treat '+' and '#' as compound boundaries\n"
    "  -k, --keep-compounds    Retain compound analyses even when analyses "
    "with fewer\n"
    "                          compound-boundaries are available\n"
    "  -W, --show-weights      Print final analysis weights (if any)\n"
    "  -N N, --analyses=N      Output no more than N analyses\n"
    "                          (if the transducer is weighted,"
    "the N best analyses)\n");
    fprintf(message_out, "Input mangling options:\n"
    "  -c, --case-sensitive    Perform lookup using the literal case of the "
    "input\n"
    "                          characters\n"
    "  -w  --dictionary-case   Output results using dictionary case "
    "instead of\n"
    "                          surface case\n" 
    "  -z  --null-flush        Flush output on the null character\n"
    "  -X, --raw               Do not perform any mangling to:\n"
    "                          case, ``superblanks'' or anything else!!!\n");
    fprintf(message_out, "\n");
    fprintf(message_out, 
            "TRANSFILE must be a file containing exactly one optimized-lookup"
            "transducer.\n"
            "INFILE must be file containing apertium stream format encoded "
            "corpus data. If INFILE is omitted, standard input will be used.\n"
            "If OUTFILE is omitted, standard output will be used.\n");
    fprintf(message_out, "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
}

int
parse_options(int argc, char** argv)
  {
    extend_options_getenv(&argc, &argv);
    while (true)
      {
        static struct option long_options[] =
        {
          HFST_GETOPT_COMMON_LONG,
          // the hfst-proc-specific options
          {"analysis",       no_argument,       0, 'a'},
          {"generation",     no_argument,       0, 'g'},
          {"non-marked-gen", no_argument,       0, 'n'},
          {"tokenize",       no_argument,       0, 't'},
          {"apertium",       no_argument,       0, 'p'},
          {"xerox",          no_argument,       0, 'x'},
          {"cg",             no_argument,       0, 'C'},
          {"tsv",            no_argument,       0, 'T'},
          {"keep-compounds", no_argument,       0, 'k'},
          {"do-compounds",   no_argument,       0, 'e'},
          {"show-weights",   no_argument,       0, 'W'},
          {"analyses",       required_argument, 0, 'N'},
          {"case-sensitive", no_argument,       0, 'c'},
          {"dictionary-case",no_argument,       0, 'w'},
          {"null-flush",     no_argument,       0, 'z'},
          {"raw",            no_argument,       0, 'X'},
          {0,                0,                 0,  0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                            "agndtpxCkeWN:cwzXT", long_options, &option_index);

        if (c == -1) // no more options to look at
          break;

        switch (c)
        {
#       include "conventions/getopt-cases-common.h"
        case 'a':
        case 'g':
        case 'n':
        case 't':
          if (cmd == 0)
            {
              cmd = c;
            }
          else
            {
              hfst_error(EXIT_FAILURE, 0, 
                    "-%c and -%c conflict; only one processing mode is allowed",
                    cmd, c);
              return EXIT_FAILURE;
            }
          break;
        case 'p':
        case 'C':
        case 'x':
        case 'T':
          if (output_type == 0)
            {
              output_type = c;
            }
          else
            {
              hfst_error(EXIT_FAILURE, 0,
                    "-%c and -%c conflict; only one output mode is allowed",
                    output_type, c);
              return EXIT_FAILURE;
            }
          break;
        case 'k':
          filter_compound_analyses = false;
          break;
        case 'W':
          displayWeightsFlag = true;
          break;
        case 'N':
          maxAnalyses = atoi(optarg);
          if (maxAnalyses < 1)
            {
              std::cerr << "Invalid or no argument for analyses count\n";
              return EXIT_FAILURE;
            }
          break;
        case 'c':
        case 'e': 
          processCompounds = true;
        case 'w':
        case 'X':
          if (capitalization==0)
            {
              capitalization=c;
            }
          else
            {
              hfst_error(EXIT_FAILURE, 0,
                    "-%c and -%c conflict; only one capitalisation mode is "
                    "allowed", capitalization, c);
              return EXIT_FAILURE;
            }
          break;
        case 'z':
          null_flush = true;
          break;
#       include "conventions/getopt-cases-error.h"
        }
      }
    int fst_arg = optind, in_arg = optind+1, out_arg = optind+2;

    if (optind == (argc-2))
      {
        outputIsStdout = true;
        inputIsStdin = false;
      }
    else if (optind == (argc-1))
      {
        outputIsStdout = true;
        inputIsStdin = true;
      }
    else if (optind < (argc-3))
      {
        hfst_error(EXIT_FAILURE, 0, "No more than three file names can be given");
        return EXIT_FAILURE;
      }
    else
      {
        hfst_error(EXIT_FAILURE, 0, "At least transducer file name must be given");
        return EXIT_FAILURE;
      }
    in = new std::ifstream(argv[(fst_arg)], std::ios::in | std::ios::binary);
    if (!*in)
      {
        hfst_error(EXIT_FAILURE, 0, "Could not open transducer file %s", 
              argv[(optind)]);
        return EXIT_FAILURE;
      }

    if (!inputIsStdin)
      {
        input = new std::ifstream(argv[in_arg], 
                                  std::ios::in | std::ios::binary);
        if (!(*input))
          {
            hfst_error(EXIT_FAILURE, 0, "Could not open input file %s",
                  argv[in_arg]);
            return EXIT_FAILURE;
          }
        inputIsStdin = false;
      }
    else
      {
        inputIsStdin = true;
      }

    if (!outputIsStdout)
      {
        output = new std::ofstream(argv[out_arg],
                                   std::ios::out | std::ios::binary);
        if (!(*output))
          {
            hfst_error(EXIT_FAILURE, 0, "Could not open output file %s",
                  argv[out_arg]);
            return EXIT_FAILURE;
          }
        outputIsStdout = false;
      }
    else
      {
        outputIsStdout = true;
      }
    return EXIT_CONTINUE;
  }


int main(int argc, char **argv)
  {
    hfst_set_program_name(argv[0], "0.1", "HfstApertiumProc");
    hfst_setlocale();
    int rv = parse_options(argc, argv);
    if (rv != EXIT_CONTINUE)
      {
        return rv;
      }
    CapitalizationMode capitalization_mode;
    switch (capitalization)
    {
      case 'c':
        capitalization_mode = CaseSensitive;
        break;
      case 'w':
        capitalization_mode = DictionaryCase;
        break;
      case 'X':
        capitalization_mode = CaseSensitiveDictionaryCase;
        rawMode = true;
        break;
      default:
        if (capitalization == 0 && output_type == 'C')
          capitalization_mode = DictionaryCase;
        else
          capitalization_mode = IgnoreCase;
      }
    try
      {
        verbose_printf("Loading transducer from %s\n");
        ProcTransducer t(*in);
        in->close();

        TokenIOStream token_stream(*input, *output, 
                                   t.get_alphabet(), null_flush, rawMode);
        Applicator* applicator = NULL;
        OutputFormatter* output_formatter = NULL;
        switch (cmd)
          {
          case 't':
            applicator = new TokenizationApplicator(t, token_stream);
            break;
          case 'g':
            applicator = new GenerationApplicator(t, token_stream,
                                                  gm_unknown,
                                                  capitalization_mode);
            break;
          case 'n':
            applicator = new GenerationApplicator(t, token_stream,
                                                  gm_clean,
                                                  capitalization_mode);
            break;
          case 'd':
            applicator = new GenerationApplicator(t, token_stream,
                                                  gm_all,
                                                  capitalization_mode);
            break;
          case 'a':
          default:
            switch(output_type)
              {
              case 'C':
                output_formatter = (OutputFormatter*)new 
                    CGOutputFormatter(token_stream, filter_compound_analyses);
                break;
              case 'x':
                output_formatter = (OutputFormatter*)new 
                    XeroxOutputFormatter(token_stream,
                                         filter_compound_analyses);
                break;
              case 'T':
                output_formatter = (OutputFormatter*)new
                    TsvOutputFormatter(token_stream, filter_compound_analyses);
                break;
              default:
                output_formatter = (OutputFormatter*)new
                    ApertiumOutputFormatter(token_stream,
                                            filter_compound_analyses);
              }
            applicator = new AnalysisApplicator(t, token_stream,
                                                *output_formatter,
                                                capitalization_mode);
            break;
          }
        applicator->apply();
        delete applicator;
        if (output_formatter != NULL)
          delete output_formatter;
      }
    catch (std::exception& e)
      {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
      }
    if (!inputIsStdin)
      {
        dynamic_cast<std::ifstream*>(input)->close();
        delete input;
      }
    if (!outputIsStdout)
      {
        dynamic_cast<std::ofstream*>(output)->close();
        delete output;
      }
  return EXIT_SUCCESS;
}

