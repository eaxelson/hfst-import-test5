//! @file hfst-xfst2fst.cc
//!
//! @brief Implemetation of command line program for parsing xfst commands

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
#   include <config.h>
#endif

#include "XfstCompiler.h"

#ifdef HAVE_READLINE
  #include <readline/readline.h>
  #include <readline/history.h>
#endif

#include <getopt.h>
#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "hfst-tool-metadata.h"
#include "inc/globals-common.h"

static hfst::ImplementationType output_format = hfst::UNSPECIFIED_TYPE;
static char* scriptfilename = NULL;
static char* startupfilename = NULL;
static std::vector<char*> execute_commands;
static bool pipemode = false;
#ifdef HAVE_READLINE
  static bool use_readline = true;
#else
  static bool use_readline = false;
#endif

void
print_usage()
{
  // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp                                                                                                                                                               
  // Usage line                                                                                                                                                                                                                            
  fprintf(message_out, "Usage: %s [OPTIONS...]\n"
          "XFST parser\n"
          "\n", program_name);
  
  print_common_program_options(message_out);
  fprintf(message_out, "\n");
  fprintf(message_out, "Xfst-specific options:\n");
  fprintf(message_out, 
          "  -e, --execute=CMD        Execute command CMD on startup\n" 
          "  -f, --format=FMT         Write result using FMT as backend format\n"
          "  -F. --scriptfile=FILE    Read commands from FILE, and quit\n"
          "  -l, --startupfile=FILE   Read commands from FILE on startup\n"
          "  -p, --pipe-mode          Pipe mode (non-interactive), reads from standard input\n"
          "  -r, --no-readline        Do not use readline library for input\n"
          "\n"
          "Option --execute can be invoked many times.\n"
          "If FMT is not given, OpenFst's tropical format will be used.\n"
          "The possible values for FMT are { foma, openfst-tropical, openfst-log,\n"
          "sfst, optimized-lookup-weighted, optimized-lookup-unweighted }\n"
          "Readline library, if enabled when configuring, is used for input be default.\n");
  fprintf(message_out, "\n");
  print_report_bugs();
  fprintf(message_out, "\n");
  print_more_info();
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
            // add tool-specific options here
            {"format", required_argument, 0, 'f'},
            {"scriptfile", required_argument, 0, 'F'},
            {"execute", required_argument, 0, 'e'},
            {"startupfile", required_argument, 0, 'l'},
            {"pipe-mode", no_argument, 0, 'p'},
            {"no-readline", no_argument, 0, 'r'},
            {0,0,0,0}
          };
        int option_index = 0;
        // add tool-specific options here
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT "f:F:e:l:pr",
                             long_options, &option_index);
        if (-1 == c)
          {
            break;
          }

        switch (c)
          {
            //copied from "inc/getopt-cases-common.h"
          case 'd':
            debug = true;
            break;
          case 'h':
            print_usage();
            return EXIT_SUCCESS;
            break;
          case 'V':
            print_version();
            return EXIT_SUCCESS;
            break;
          case 'v':
            verbose = true;
            silent = false;
            break;
          case 'q':
          case 's':
            verbose = false;
            silent = true;
            break;
          case 'f':
            output_format = hfst_parse_format_name(optarg);
            break;
          case 'F':
            scriptfilename = hfst_strdup(optarg);
            break;
          case 'e':
            execute_commands.push_back(hfst_strdup(optarg));
            break;
          case 'l':
            startupfilename = hfst_strdup(optarg);
            break;
          case 'p':
            pipemode = true;
            break;
          case 'r':
            use_readline = false;
            break;
#include "inc/getopt-cases-error.h"
          }
    }

  if (output_format == hfst::UNSPECIFIED_TYPE)
    {
      output_format = hfst::TROPICAL_OPENFST_TYPE;
      verbose_printf("Using default output format OpenFst "
                     "with tropical weight class\n");
    }

  return EXIT_CONTINUE;
}

// Based on a function in foma written by Mans Hulden.
// Read the file 'filename' to memory and return a pointer to it.
// Filename "<stdout>" uses stdout for reading.
// Returns NULL if file cannot be opened or read or memory cannot be allocated.
char *file_to_mem(char *filename) {

  FILE    *infile;
  size_t    numbytes;
  char *buffer;
  infile = (strcmp(filename, "<stdout>") == 0)? stdout : fopen(filename, "r");
  if(infile == NULL) 
    {
      error(EXIT_FAILURE, 0, "Error opening file '%s'\n", filename);
      return NULL;
    }
  fseek(infile, 0L, SEEK_END);
  numbytes = ftell(infile);
  fseek(infile, 0L, SEEK_SET);
  // FIX: use malloc instead
  buffer = (char*)xxmalloc((numbytes+1) * sizeof(char));
  if(buffer == NULL) 
    {
      error(EXIT_FAILURE, 0, "Error allocating memory to read file '%s'\n", filename);
      return NULL;
    }
  if (fread(buffer, sizeof(char), numbytes, infile) != numbytes) 
    {
      error(EXIT_FAILURE, 0, "Error reading file '%s' to memory\n", filename);
      return NULL;
    }
  if (strcmp(filename, "<stdout>") != 0)
    {
      fclose(infile);
    }
  *(buffer+numbytes)='\0';
  return(buffer);
}

// Parse file 'filename' using compiler 'comp'.
// Filename "<stdout>" uses stdout for reading.
int parse_file(const char* filename, hfst::xfst::XfstCompiler &comp)
{
  char* line = file_to_mem(startupfilename);
  if (NULL == line)
    {
      return EXIT_FAILURE;
    }
  if (0 != comp.parse_line(line))
    {
      error(EXIT_FAILURE, 0, "error when parsing startupfile\n");
      return EXIT_FAILURE;
    }
  free(line);
  return 0;
}

int main(int argc, char** argv)
{

  hfst_set_program_name(argv[0], "0.1", "HfstXfst2Fst");
  int retval = parse_options(argc, argv);
  if (retval != EXIT_CONTINUE)
    {
      return retval;
    }

  switch (output_format)
    {
    case hfst::SFST_TYPE:
      verbose_printf("Using SFST as output handler\n");
      break;
    case hfst::TROPICAL_OPENFST_TYPE:
      verbose_printf("Using OpenFst's tropical weights as output\n");
      break;
    case hfst::LOG_OPENFST_TYPE:
      verbose_printf("Using OpenFst's log weight output\n");
      break;
    case hfst::FOMA_TYPE:
      verbose_printf("Using foma as output handler\n");
      break;
    case hfst::HFST_OL_TYPE:
      verbose_printf("Using optimized lookup output\n");
      break;
    case hfst::HFST_OLW_TYPE:
      verbose_printf("Using optimized lookup weighted output\n");
      break;
    default:
      error(EXIT_FAILURE, 0, "Unknown format cannot be used as output\n");
      return EXIT_FAILURE;
    }
  
  if (pipemode && (scriptfilename != NULL))
    {
      error(EXIT_FAILURE, 0 , "--pipe-mode and --scriptfile cannot be used simultaneously\n");
      return EXIT_FAILURE;
    }

  if ((startupfilename != NULL) && (scriptfilename != NULL))
    {
      error(EXIT_FAILURE, 0, "--startupfile and --scriptfile cannot be used simultaneously\n");
      return EXIT_FAILURE;
    }


  // Create XfstCompiler
  hfst::xfst::XfstCompiler comp(output_format);
  comp.setVerbosity(!silent);

  // If needed, execute scripts given in command line
  for (std::vector<char*>::const_iterator cmd = execute_commands.begin();
       cmd != execute_commands.end(); cmd++)
    {
      verbose_printf("Executing xfst command '%s' given on command line...\n", *cmd);
      if (0 != comp.parse_line(*cmd))
        {
          error(EXIT_FAILURE, 0, "command '%s' could not be parsed\n", *cmd);
          return EXIT_FAILURE;
        }
    }
  // If needed, execute script in startup file
  if (startupfilename != NULL)
    {
      verbose_printf("Executing startup file '%s'...\n", startupfilename);
      if (parse_file(startupfilename, comp) == EXIT_FAILURE)
        {
          return EXIT_FAILURE;
        }
    }

  if (pipemode) 
    {
      verbose_printf("Reading from standard input...\n");
      if (parse_file("<stdout>", comp) == EXIT_FAILURE)
        {
          return EXIT_FAILURE;
        }
    }
  else if (scriptfilename != NULL)
    {
      verbose_printf("Reading from script file '%s'\n", scriptfilename);
      if (parse_file(scriptfilename, comp) == EXIT_FAILURE)
        {
          return EXIT_FAILURE;
        }
    }
  // Use interactive mode
  else if (! use_readline)
    {
      verbose_printf("Starting interactive mode...\n");
      comp.setPromptVerbosity(!silent);
      if (!silent)
        comp.prompt();
      // support for backspace
      char line[256];
      while (cin.getline(line, 256))
        {
          if (0 != comp.parse_line(line))
            {
              error(EXIT_FAILURE, 0, "line '%s' could not be parsed\n", line);
              return EXIT_FAILURE;
            }
        }
    }
  else
    {
#ifdef HAVE_READLINE
      // support for backspace and Up/Down keys, needs readline library

      verbose_printf("Starting interactive mode...\n");
      comp.setPromptVerbosity(false); // prompts handled manually
      char *buf = NULL;               // result from readline
      rl_bind_key('\t',rl_abort);     // disable auto-complet

      char* promptline = (!silent) ? comp.get_prompt() : strdup("");
      while((buf = readline(promptline)) != NULL)
        {
          if (buf[0] != 0) {
            add_history(buf); }
          
          if (0 != comp.parse_line(buf))
            {
              error(EXIT_FAILURE, 0, "line '%s' could not be parsed\n", buf);
              return EXIT_FAILURE;
            }
          
          free(promptline);
          promptline = (!silent) ? comp.get_prompt() : strdup("");
        }
      free(buf);
      free(promptline);
#else
      fprintf(stderr, "ERROR: missing readline library\n");
      return EXIT_FAILURE;
#endif
    }

  return EXIT_SUCCESS;
}

