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

// todo: see if readline is supported 
#include <readline/readline.h>
#include <readline/history.h>

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
static bool use_readline = true;

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
          "sfst, optimized-lookup-weighted, optimized-lookup-unweighted }\n");
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

bool is_whitespace(char c)
{
  if (c == '\r' || c == '\n' || c == ' ' || c == '\t') {
    return true; }
  return false;
}

bool contains_keyword(const char* keyword, const std::string &str)
{
  unsigned found = str.find(keyword); 
  if (found != std::string::npos)
    {
      for (unsigned i=0; i<found; i++)
        {
          if (! is_whitespace(str[i])) 
            return false;
        }
      return true;
    }
  else 
    {
      return false;
    }
}

bool contains_regex(char* l)
{
  bool retval=false;
  std::string line(l);
  return (contains_keyword("define", line) || contains_keyword("regex", line))
}

bool ends_regex(char* l)
{
  
}

char* get_lines_until_end_of_regex(char *line, size_t size, FILE *file)
{
}

// 0 == read line
// 1 == end of file
// 2 == error in parsing
int xfst_get_line(std::string &str, FILE* file)
{
  str=std::string("");
  char line [LINE_SIZE];
  if (NULL == fgets(line, LINE_SIZE, file))
    {
      return 1;
    }
  else
    {
      str = std::string(line);
      if (contains_regex(linestr))
        {
          while(true)
            {
              if (NULL == fgets(line, LINE_SIZE, file)) 
                {
                  error(EXIT_FAILURE, 0, "parsing of regex failed: no end of expression (;) found before end of file\n");
                  return 2;
                }
              else 
                {
                  std::string next_line(line);
                  str.append(next_line);
                  if (ends_regex(next_line))
                    {
                      return 0;
                    }
                }
            }
        }
    }
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
      if (0 != comp.parse_line(*cmd))
        {
          error(EXIT_FAILURE, 0, "parameter of option --execute could not be parsed\n");
          return EXIT_FAILURE;
        }
    }
  // If needed, execute script in startup file
  if (startupfilename != NULL)
    {
      FILE *startupfile = fopen(startupfilename, "r");
      if (startupfile == NULL)
        {
          error(EXIT_FAILURE, 0, "could not open startupfile\n");
          return EXIT_FAILURE;
        }
      std::string line;
      int retval;
      while(0 == xfst_get_line(line, startupfile))
        {
          if (0 != comp.parse_line(line.c_str()))
            {
              fclose(startupfile);
              error(EXIT_FAILURE, 0, "error in startupfile\n");
              return EXIT_FAILURE;
            }
        }
      fclose(startupfile);
    }
  
  if (pipemode) 
    {
      std::string line;
      while (xfst_get_line(line, stdout))
        {
          if (0 != comp.parse_line(line.c_str()))
            {
              error(EXIT_FAILURE, 0, "error in input\n");
              return EXIT_FAILURE;
            }
        }
    }
  else if (scriptfilename != NULL)
    {
      if (execute_commands == NULL) // parse_line has not been used
        {
          if (0 != comp.parse(scriptfilename))
            {
              error(EXIT_FAILURE, 0, "error in scriptfile\n");
              return EXIT_FAILURE;
            }
        }
      else // we have to use parse_line consistently
        {
          FILE *scriptfile = fopen(scriptfilename, "r");
          if (scriptfile == NULL)
            {
              error(EXIT_FAILURE, 0, "could not open scriptfile\n");
              return EXIT_FAILURE;
            }
          std::string line;
          while (xfst_get_line(line, scriptfile))
            { 
              if (0 != comp.parse_line(line.c_str()))
                {
                  fclose(scriptfile);
                  error(EXIT_FAILURE, 0, "error in scriptfile\n");
                  return EXIT_FAILURE;
                } 
            }
          fclose(scriptfile);
        }
    }
  else if (! use_readline)
    {
      // support for backspace
      char line[256];
      while (cin.getline(line, 256))
        {
          if (0 != comp.parse_line(line))
            {
              return EXIT_FAILURE;
            }
        }
    }
  else
    {
      // support for backspace and Up/Down keys, needs readline library

      comp.setPromptVerbosity(false); // prompts handled manually
      char *buf = NULL;               // result from readline
      rl_bind_key('\t',rl_abort);     // disable auto-complet

      char* promptline = (!silent) ? comp.get_prompt() : strdup("");
      while((buf = readline(promptline)) != NULL)
        {
          if (buf[0] != 0) {
            add_history(buf); }
          
          comp.parse_line(buf);
          
          free(promptline);
          promptline = (!silent) ? comp.get_prompt() : strdup("");
        }
      free(buf);
      free(promptline);
    }

  return EXIT_SUCCESS;
}

