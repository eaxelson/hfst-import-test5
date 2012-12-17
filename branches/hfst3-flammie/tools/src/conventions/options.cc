//! @file options.cc
//! @brief  implementations of HFST standardised command-line option interfaces.

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

#include <HfstDataTypes.h>

using hfst::HfstInputStream;

#include "conventions/commandline.h"
#include "conventions/options.h"

////////////////////////
// NB: when messing with this file please pay attention to GNU guidelines for
// commandline interfaces.
////////////////////////

void print_common_program_options() 
  {
    fprintf(message_out, "Common options:\n"
            "  -h, --help             Print help message\n"  
            "  -V, --version          Print version info\n"  
            "  -v, --verbose          Print verbosely while processing\n"
           "  -q, --quiet            Only print fatal hfst_errors and requested "
            "output\n"  
            "  -s, --silent           Alias of --quiet\n"
            "  --profile=PFILE        Write profiling data to PFILE\n"
            "  -d, --debug            Print all debugging data "
            "(for bug reports)\n");
    switch (hfst_iomode)
      {
      case AUTOM_IN_AUTOM_OUT:
        switch (inputs)
          {
          case READ_ONE:
            fprintf(message_out, "Input/Output options:\n"
                    "  -i, --input=INFILE     Read input automata "
                    "from INFILE\n"
                    "  -o, --output=OUTFILE   Write output automata "
                    "to OUTFILE\n");
            break;
          case READ_TWO:
            fprintf(message_out, "Input/Output options:\n"
                  "  -1, --input1=INFILE1   Read first input automata "
                  "from INFILE1\n"
                  "  -2, --input2=INFILE2   Read second input automata "
                  "from INFILE2\n"
                  "  -o, --output=OUTFILE   Write output automata "
                  "to OUTFILE\n");
            break;
          default:
            fprintf(message_out, "Input/Output options:\n"
                    "  -o, --output=OUTFILE   Write output automata "
                    "to OUTFILE\n");
              break;
          }
        break;
      case FILE_IN_AUTOM_OUT:
        switch (inputs)
          {
          case READ_ONE:
            fprintf(message_out, "Input/Output options:\n"
                    "  -i, --input=INFILE     Read data from INFILE\n"
                    "  -o, --output=OUTFILE   Write automata to OUTFILE\n"
                    "  -f, --format=FMT       Use FMT library to write "
                    "automata\n");
            break;
          default:
            fprintf(message_out, "Input/Output options:\n"
                    "  -o, --output=OUTFILE   Write automata to OUTFILE\n"
                    "  -f, --format=FMT       Use FMT library to write "
                    "automata\n");
            break;
          }
        break;
      case AUTOM_IN_FILE_OUT:
        switch (inputs)
          {
          case READ_ONE:
            fprintf(message_out, "Input/Output options:\n"
            "  -i, --input=INFILE     Read input automata from INFILE\n"
            "  -o, --output=OUTFILE   Write output data to OUTFILE\n");
            break;
          case READ_TWO:
            fprintf(message_out, "Input/Output options:\n"
                  "  -1, --input1=INFILE1   Read first input automata "
                  "from INFILE1\n"
                  "  -2, --input2=INFILE2   Read second input automata "
                  "from INFILE2\n"
                  "  -o, --output=OUTFILE   Write output data "
                  "to OUTFILE\n");
            break;
          default:
            fprintf(message_out, "Input/Output options:\n"
                    "  -o, --output=OUTFILE   Write output data "
                    "to OUTFILE\n");
              break;
          }
        break;
      case FILE_IN_FILE_OUT:
        switch (inputs)
          {
          case READ_ONE:
            fprintf(message_out, "Input/Output options:\n"
            "  -i, --input=INFILE     Read input data from INFILE\n"
            "  -o, --output=OUTFILE   Write output data to OUTFILE\n");
            break;
          case READ_TWO:
            fprintf(message_out, "Input/Output options:\n"
                  "  -1, --input1=INFILE1   Read first input data "
                  "from INFILE1\n"
                  "  -2, --input2=INFILE2   Read second input data "
                  "from INFILE2\n"
                  "  -o, --output=OUTFILE   Write output data "
                  "to OUTFILE\n");
            break;
          default:
            fprintf(message_out, "Input/Output options:\n"
                    "  -o, --output=OUTFILE   Write output data "
                    "to OUTFILE\n");
            break;
          }
        break;
      case NO_AUTOMAGIC_IO:
      default:
        break;
      } // switch iomode
  }

void
print_common_parameter_instructions() 
  {
    switch (hfst_iomode) 
      {
      case AUTOM_IN_AUTOM_OUT:
        switch (inputs)
          {
          case READ_ONE:
            fprintf(message_out, 
                    "If OUTFILE or INFILE is missing or -, "
                    "standard streams will be used.\n"
                    "Format of result depends on format of INFILE\n");
            break;
          case READ_TWO:
            fprintf(message_out, 
                "If OUTFILE, or either INFILE1 or INFILE2 is missing or -,\n"
                "standard streams will be used.\n"
                "INFILE1, INFILE2, or both, must be specified.\n"
                "Format of result depends on format of INFILE1\n"
                "and INFILE2; both should have the same format.\n");
            break;
          default:
            fprintf(message_out,
                    "If OUTFILE or INFILES is missing or -, "
                    "standard streams will be used.\n"
                    "Format of result depends on format of INFILEs; they "
                    "should have the same format.\n");
            break;
          }
        break;
      case FILE_IN_AUTOM_OUT:
        switch (inputs)
          {
          case READ_ONE:
            fprintf(message_out, 
              "If OUTFILE or INFILE is missing or -, "
              "standard streams will be used.\n"
              "If FMT is not given, "
#   if HAVE_OPENFST
              "OpenFst's tropical format"
#   elif HAVE_FOMA
              "foma format"
#   elif HAVE_SFST
              "SFST format"
#   else
              "HFST internal format"
#   endif
              " will be used.\n"
              "The avalaible values for FMT are {"
#   if HAVE_OPENFST
              "openfst-tropical, openfst-log, "
#   endif
#   if HAVE_SFST
              "sfst, "
#   endif
#   if HAVE_FOMA
              "foma, "
#   endif
              "optimized-lookup-weighted, and "
              "optimized-lookup-unweighted }.\n");
            break;
          default:
            fprintf(message_out,
              "If OUTFILE is missing or -, "
              "standard streams will be used.\n"
              "If FMT is not given, "
#   if HAVE_OPENFST
              "OpenFst's tropical format"
#   elif HAVE_FOMA
              "foma format"
#   elif HAVE_SFST
              "SFST format"
#   else
              "HFST internal format"
#   endif
              " will be used.\n"
              "The avalaible values for FMT are {"
#   if HAVE_OPENFST
              "openfst-tropical, openfst-log, "
#   endif
#   if HAVE_SFST
              "sfst, "
#   endif
#   if HAVE_FOMA
              "foma, "
#   endif
              "optimized-lookup-weighted, and "
              "optimized-lookup-unweighted }.\n");
            break;
          }
        break;
      case AUTOM_IN_FILE_OUT:
      case FILE_IN_FILE_OUT:
      default:
        switch (inputs)
          {
          case READ_ONE:
            fprintf(message_out, 
                    "If OUTFILE or INFILE is missing or -, "
                    "standard streams will be used.\n");
            break;
          case READ_TWO:
            fprintf(message_out, 
                "If OUTFILE, or either INFILE1 or INFILE2 is missing or -,\n"
                "standard streams will be used.\n"
                "INFILE1, INFILE2, or both, must be specified.\n");
            break;
          default:
            fprintf(message_out,
                    "If OUTFILE or INFILES is missing or -, "
                    "standard streams will be used.\n");
            break;
          }
        break;
      } // switch iotype
  }

void
check_common_options(int argc, char** argv)
  {
    if (!output_named)
      {
        outfilename = hfst_strdup("<stdout>");
        outfile = stdout;
        message_out = stderr;
      }
    else if (outfile != stdout)
      {
        message_out = stdout;
      }
    if (auto_colors)
      {
        if (message_out == stdout)
          {
            if (isatty(1) && isatty(2))
              {
                hfst_verbose("enabling color output because of --colour=auto "
                             "and stdout to active terminal");
                print_colors = true;
              }
            else
              {
                print_colors = false;
              }
          }
        else
          {
            if (isatty(2))
              {
                hfst_verbose("enabling color output because of --colour=auto "
                             "and stderr to active terminal");
                print_colors = true;
              }
            else
              {
                print_colors = false;
              }
          }
      }
    if (inputs == READ_ONE)
      {
        if (!input_named)
          {
            if ((argc - optind) == 1)
              {
                inputfilename = hfst_strdup(argv[optind]);
                if (strcmp(inputfilename, "-") == 0)
                  {
                    free(inputfilename);
                    inputfilename = hfst_strdup("<stdin>");
                    inputfile = stdin;
                    is_input_stdin = true;
                  }
              }
            else if ((argc - optind) > 1)
              {
                hfst_error(EXIT_FAILURE, 0,
                      "no more than one input file may be given\n"
                      "(too many free parameters on command line: %s %s)",
                      argv[optind], argv[optind + 1]);
              }
            else
              {
                inputfile = stdin;
                inputfilename = hfst_strdup("<stdin>");
              }
          }
        else 
          {
            if ((argc - optind) > 0)
              {
                hfst_error(EXIT_FAILURE, 0, 
                      "cannot have both -i and other filenames\n"
                      "(too many free parameters in commandline: %s)",
                      argv[optind]);
              }
          }
      }
    else if (inputs == READ_TWO)
      {
        if (first_named && second_named)
          {
            if ((argc - optind) > 0)
              {
                hfst_error(EXIT_FAILURE, 0,
                  "cannot have all -1, -2 and other filenames\n"
                  "(too many free parameters in commandline: %s)",
                  argv[optind]);
              }
          }
        else if (!first_named && !second_named)
          {
            // neither input given in options:
            if ((argc - optind) == 2)
              {
                firstfilename = hfst_strdup(argv[optind]);
                is_input_stdin = false;
                if (strcmp(firstfilename, "-") == 0)
                  {
                    free(firstfilename);
                    firstfilename = hfst_strdup("<stdin>");
                    firstfile = stdin;
                    is_input_stdin = true;
                  }
                secondfilename = hfst_strdup(argv[optind + 1]);
                if (strcmp(secondfilename, "-") == 0)
                  {
                    free(secondfilename);
                    secondfilename = hfst_strdup("<stdin>");
                    secondfile = stdin;
                    is_input_stdin = true;
                  }
              }
            else if ((argc - optind) == 1)
              {
                secondfilename = hfst_strdup(argv[optind]);
                if (strcmp(secondfilename, "-") == 0)
                  {
                    free(secondfilename);
                    secondfilename = hfst_strdup("<stdin>");
                    secondfile = stdin;
                    is_input_stdin = true;
                  }
                firstfilename = hfst_strdup("<stdin>");
                firstfile = stdin;
                is_input_stdin = true;
              }
            else if ((argc - optind) > 2)
              {
                hfst_error(EXIT_FAILURE, 0,
                      "no more than two filenames may be given\n"
                      "(too many free parameters on commandline: %s %s %s)",
                      argv[optind], argv[optind + 1], argv[optind + 2]);
              }
            else
              {
                hfst_error(EXIT_FAILURE, 0,
                         "at least one input must be from -1, -2 or file "
                         "parameter\n"
                         "(not enough free parameters on command line)");
              }
          }
        else if (!first_named)
          {
            if ((argc - optind) == 1)
              {
                firstfilename = hfst_strdup(argv[optind]);
                is_input_stdin = false;
                if (strcmp(firstfilename, "-") == 0)
                  {
                    free(firstfilename);
                    firstfilename = hfst_strdup("<stdin>");
                    firstfile = stdin;
                    is_input_stdin = true;
                  }
              }
            else if ((argc - optind) == 0)
              {
                firstfilename = hfst_strdup("<stdin>");
                firstfile = stdin;
                is_input_stdin = true;
              }
            else 
              {
                hfst_error(EXIT_FAILURE, 0,
                      "cannot give more than -2 and one more filename\n"
                      "(too many free parameters on command line: %s %s)",
                      argv[optind], argv[optind + 1]);
              }
          }
        else if (!second_named)
          {
            if ((argc - optind) == 1)
              {
                secondfilename = hfst_strdup(argv[optind]);
                is_input_stdin = false;
                if (strcmp(secondfilename, "-")  == 0)
                  {
                    free(secondfilename);
                    secondfilename = hfst_strdup("<stdin>");
                    is_input_stdin = true;
                    secondfile = stdin;
                  }
              }
            else if ((argc - optind) == 0)
              {
                secondfilename = hfst_strdup("<stdin>");
                secondfile = stdin;
                is_input_stdin = true;
              }
            else
              {
                hfst_error(EXIT_FAILURE, 0,
                      "cannot give more than -1 and one more filename\n"
                      "(too many free parameters on command line: %s %s)",
                      argv[optind], argv[optind + 1]);
              }
          }
        else
          {
            hfst_error(EXIT_FAILURE, 0,
                    "at least one input from -1, -2 or free parameter must "
                    "be given\n"
                    "(not enough parameters on commandline)");
          }
      }
    else if (inputs == READ_MANY)
      {
        if ((argc - optind) > 0)
          {
            inputs_named = argc - optind;
            inputfilenames = static_cast<char**>(malloc(sizeof(char*) *
                                                        inputs_named));
            inputfiles = static_cast<FILE**>(malloc(sizeof(FILE*) *
                                                    inputs_named));
            instreams = static_cast<HfstInputStream**>(malloc(
                      (sizeof(HfstInputStream*) * inputs_named)));
            unsigned int i = 0;
            while (optind < argc)
              {

                inputfilenames[i] = hfst_strdup(argv[optind]);
                if (strcmp(inputfilenames[i], "-"))
                  {
                    inputfiles[i] = stdin;
                    is_input_stdin = true;
                  }
                optind++;
                i++;
              }
          }
        else
          {
            inputs_named = 1;
            inputfilenames = static_cast<char**>(malloc(sizeof(char*)));
            inputfiles = static_cast<FILE**>(malloc(sizeof(FILE*)));
            instreams = static_cast<HfstInputStream**>
                (malloc(sizeof(HfstInputStream*)));
            inputfilenames[0] = hfst_strdup("<stdin>");
            inputfiles[0] = stdin;
            is_input_stdin = true;
          }
      }
    if (hfst_iomode == FILE_IN_AUTOM_OUT)
      {
        if (format == hfst::UNSPECIFIED_TYPE)
          {
#   if HAVE_OPENFST
            format = hfst::TROPICAL_OPENFST_TYPE;
#   elif HAVE_FOMA
            format = hfst::FOMA_TYPE;
#   elif HAVE_SFST
            format = hfst::SFST_TYPE;
#   else
            format = hfst::OLW_TYPE;
#   endif
            hfst_verbose("Format was not given, defaulting to %s",
                         hfst_strformat(format));
          }
        else
          {
            hfst_verbose("Using %s as output format",
                         hfst_strformat(format));
          }
      }
  }

bool
parse_common_getopt_value(char c)
  {
    switch(c)
      {
      case 'd':
        debug = true;
        return true;
        break;
      case 'h':
        print_usage();
        exit(EXIT_SUCCESS);
        return true;
        break;
      case 'V':
        print_version();
        exit(EXIT_SUCCESS);
        return true;
        break;
      case 'v':
        verbose = true;
        silent = false;
        return true;
        break;
      case 'q':
      case 's':
        verbose = false;
        silent = true;
        return true;
        break;
      case 'o':
        outfilename = hfst_strdup(optarg);
        if (strcmp(outfilename, "-") == 0) 
          {
            message_out = stderr;
            is_output_stdout = true;
          }
        else
          {
            is_output_stdout = false;
          }
        output_named = true;
        return true;
        break;
      case PROFILE_OPT:
        profile_file_name = hfst_strdup(optarg);
        profile_file = hfst_fopen(profile_file_name, "a");
        profile_start = clock();
        return true;
        break;
      case COLOUR_OPT:
        if (strcmp(optarg, "always") == 0)
          {
            auto_colors = false;
            print_colors = true;
          }
        else if(strcmp(optarg, "never") == 0)
          {
            auto_colors = false;
            print_colors = false;
          }
        else if (strcmp(optarg, "auto") == 0)
          {
            auto_colors = true;
            print_colors = true;
          }
        else
          {
            hfst_error(EXIT_FAILURE, 0, "--colour must be one of "
                       "`always', `never' or `auto'");
          }
        return true;
        break;
      }
    if ((inputs == READ_ONE) && (c == 'i'))
      {
        inputfilename = hfst_strdup(optarg);
        if (strcmp(inputfilename, "-") == 0) 
          {
            is_input_stdin = true;
          }
        input_named = true;
        return true;
      }
    else if ((inputs == READ_TWO) && (c == '1'))
      {
        firstfilename = hfst_strdup(optarg);
        if (strcmp(firstfilename, "-") == 0)
          {
            is_input_stdin = true;
          }
        first_named = true;
        return true;
      }
    else if ((inputs == READ_TWO) && (c == '2'))
      {
        secondfilename = hfst_strdup(optarg);
        if (strcmp(secondfilename, "-") == 0)
          {
            is_input_stdin = true;
          }
        second_named = true;
        return true;
      }
    else if ((hfst_iomode == FILE_IN_AUTOM_OUT) && (c == 'f'))
      {
        format = hfst_parse_format_name(optarg);
        return true;
      }
    return false;
  }



void parse_options_getenv()
  {
    char* hfstopts = getenv("HFST_OPTIONS");
    if (NULL == hfstopts)
      {
        return;
      }
    hfst_verbose("parsing $HFST_OPTIONS for additional parameters: %s",
                  hfstopts);
    // getopt does not work for all platforms...
    char* new_arg = strtok(hfstopts, " ");
    while (new_arg != NULL)
      {
        char c = -1;
        if ((strcmp(new_arg, "-h") == 0) || (strcmp(new_arg, "--help") == 0))
          {
            c = 'h';
          }
        else if ((strcmp(new_arg, "-v") == 0) || 
                 (strcmp(new_arg, "--verbose") == 0))
          {
            c = 'v';
          }
        else if ((strcmp(new_arg, "-s") == 0) || 
                 (strcmp(new_arg, "-q") == 0) ||
                 (strcmp(new_arg, "--silent") == 0) ||
                 (strcmp(new_arg, "--quiet") == 0))
          {
            c = 'q';
          }
        else if ((strcmp(new_arg, "-V") == 0) || 
                 (strcmp(new_arg, "--version") == 0))
          {
            c = 'V';
          }
        else if ((strncmp(new_arg, "--color=", strlen("--color=")) == 0) || 
                 (strncmp(new_arg, "--colour=", strlen("--colour=")) == 0))
          {
            c = COLOUR_OPT;
            optarg = hfst_strdup(strstr(new_arg, "=") + 1);
          }
        else
          {
            c = '?';
            hfst_warning("Unrecognised option %s in HFST_OPTIONS",
                         new_arg);
          }
        // XXX: could relieve this a bit...
        if (!parse_common_getopt_value(c))
          {
            hfst_warning("Unrecognised option `%c' in environment variable "
                         "HFST_OPTIONS; allowed options are:\n"
                         "-i, -o, -1, -2, -f, -v, -q, -s, "
                         "and their long versions", c);
            break;
          }
        new_arg = strtok(NULL, " ");
      }
  }

void
parse_getopt_error_value(char c)
  {
    switch (c)
      {
      case '?':
        print_short_help();
        if (optopt == 'c')
          {
            hfst_error(EXIT_FAILURE, 0, "Option -%c requires an argument.\n",
                  optopt);
          }
        else if (isprint (optopt))
          {
            hfst_error(EXIT_FAILURE, 0, "Unknown option `-%c'.\n", optopt);
          }
        else
          {
            hfst_error(EXIT_FAILURE, 0, "Unknown option");
          }
        break;
      case ':':
        print_short_help();
        hfst_error(EXIT_FAILURE, 0, "Option -%c requires an argument", optopt);
        break;
      default:
        print_short_help();
        hfst_error(EXIT_FAILURE, 0 , "invalid option -%c", c);
        break;
      }
  }
