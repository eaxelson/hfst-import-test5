//! @file hfst-regexp2fst.cc
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
using std::map;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>
#include <errno.h>

#include <hfst.hpp>

#include "conventions/commandline.h"

using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::HfstTransducer;
using hfst::xre::XreCompiler;

static char *epsilonname = NULL;
static bool disjunct_expressions = false;
static bool line_separated = true;
static map<string,string> definitions;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Compile simple regular expressions into transducer(s)\n"
        "\n", program_name); 
        print_common_program_options();
        fprintf(message_out, "String and format options:\n"
                "  -f, --format=FMT          Write result in FMT format\n"
                "  -j, --disjunct            Disjunct all regexps instead of "
                    "transforming each regexp into a separate transducer\n"
                "  -l, --line                Input is line separated\n"
                "  -S, --semicolon           Input is semicolon separated\n"
                "  -e, --epsilon=EPS         Map EPS as zero.\n"
                "  -D, --define=VAR=XRE      Map variable VAR to regex XRE\n");
        fprintf(message_out, "\n");
        print_common_parameter_instructions();
        fprintf(message_out, 
            "If EPS is not defined, the default representation of 0 is used\n"
            "If neither -l nor -S is specified, default is line separated\n"
            "XRE must be compilable xerox regular expression\n"
            );

        fprintf(message_out, "Examples:\n"
            "  echo \"?* ;\" | %s     compile universal language\n"
            "\n", program_name);
        fprintf(message_out, "\n");
        print_report_bugs();
        print_more_info();
        fprintf(message_out, "\n");
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
        HFST_GETOPT_CREATIONAL_LONG,
          {"disjunct", no_argument, 0, 'j'},
          {"epsilon", required_argument, 0, 'e'},
          {"sum", no_argument, 0, '1'},
          {"norm", no_argument, 0, '2'},
          {"log", no_argument, 0, '3'},
          {"line", no_argument, 0, 'l'},
          {"semicolon", no_argument, 0, 'S'},
          {"define", required_argument, 0, 'D'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_CREATIONAL_SHORT "je:123lSf:D:",
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
          case 'e':
            epsilonname = hfst_strdup(optarg);
            break;
          case '1':
          case '2':
          case '3':
            hfst_warning("Weight mangling options are deprecated");
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
          case 'D':
          {
            const char* eqsign = strchr(optarg, '=');
            if (0 == eqsign)
              {
                hfst_error(EXIT_FAILURE, 0, "There must be `=' in definition: %s",
                      optarg);
              }
            const char* endstr = eqsign;
            while (*endstr != '\0')
              {
                endstr++;
              }
            char* name = strndup(optarg, eqsign - optarg);
            char* val = strndup(eqsign + 1, endstr - eqsign - 1);
            definitions[name] = val;
            free(name);
            free(val);
            break;
          }
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
make_regexes()
  {
    size_t transducer_n = 0;
    char* line = 0;
    size_t len = 0;
    unsigned int line_count = 0;
    XreCompiler comp(format);
    HfstTransducer disjunction(format);
    for (map<string,string>::const_iterator def = definitions.begin();
         def != definitions.end();
         ++def)
      {
        verbose_printf("Defining variable %s as `%s'\n", def->first.c_str(),
                       def->second.c_str());
        comp.define(def->first, def->second);
      }
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
        transducer_n++;
        line_count++;
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
                hfst_set_name(*compiled, 
                              string(line).substr(0, strlen(line) - 1),
                              "xre");
                hfst_set_formula(*compiled,
                                 string(line).substr(0 ,strlen(line) - 1),
                                 "X");
              }
            else
              {
                hfst_set_name(*compiled, line, "xre");
                hfst_set_formula(*compiled, line, "X");
              }
            *outstream << *compiled;
          }
        delete compiled;
      }
    if (disjunct_expressions)
      {
        if (delim == '\n')
          {
            hfst_set_name(disjunction, 
                          string(line).substr(0, strlen(line) - 1) + "...",
                          "xre");
            hfst_set_formula(disjunction,
                             string(line).substr(0 ,strlen(line) - 1) + "...",
                             "X");
          }
        else
          {
            hfst_set_name(disjunction, string(line) + "...", "xre");
            hfst_set_formula(disjunction, string(line) + "...", "X");
          }
        *outstream << disjunction;
      }
    free(line);
    free(first_line);
  }


int main(int argc, char **argv) 
  {
    hfst_init_commandline(argv[0], "0.2", "Regexp2Fst",
                          FILE_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_regexes();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

