//! @file hfst-ataq.cc
//!
//! @brief Apertium's tokenise as u analyse quickly tool
//!
//! @author The new guy


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

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#define EXIT_CONTINUE 42
static const char* program_name = "hfst_ataq";
static FILE* input_file = stdin;

static
void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(stdout, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Apertium tokenise and analyse quickly now\n"
        "\n", program_name);
    fprintf(stdout, "Common options:\n"
          "  -h, --help      print this help message\n"
          "  -V, --version   print version information\n"
          "\n");
}

static
void
print_version()
{
    fprintf(stdout, "%s 0.0.0\n"
                    "Copyright (C) 2014\n"
                    "License GPLv3: GNU GPL version 3 <http://gnu.org/licenses/gpl.html>\n"
                    "This is free software: you are free to change and redistribute it.\n"
                    "There is NO WARRANTY, to the extent permitted by law.\n",
           program_name);
}

int
parse_options(int argc, char** argv)
  {
    // use of this function requires options are settable on global scope
    while (true)
      {
        static const struct option long_options[] =
          {
            {"help", no_argument, 0, 'h'},
            {"version", no_argument, 0, 'V'},
            {0,0,0,0}
          };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, "hV",
                             long_options, &option_index);
        if (-1 == c)
          {
            break;
          }

        switch (c)
          {
        case 'h':
            print_usage();
            exit(0);
            break;
        case 'V':
            print_version();
            exit(0);
            break;
        default:
            fprintf(stderr, "Something wrong with getopt\n");
            exit(1);
            break;
          }
      }

    return EXIT_CONTINUE;
  }

int main( int argc, char **argv ) 
  {
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
      {
        return retval;
      }
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, input_file)) != -1)
      {
        fprintf(stdout, "%s", line);
      }
    free(line);
    return EXIT_SUCCESS;
  }

