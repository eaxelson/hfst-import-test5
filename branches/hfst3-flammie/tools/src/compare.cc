//! @file hfst-compare.cc
//!
//! @brief Transducer comparison tool
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

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;
//

#include "conventions/commandline.h"
#include "conventions/options.h"
#include "conventions/globals-common.h"
#include "conventions/globals-binary.h"

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
             "Compare two transducers\n"
        "\n", program_name );
        print_common_program_options();
        fprintf(message_out, "\n");
        print_common_parameter_instructions();
        fprintf(message_out, "Exit status is 0 if inputs are the same, "
                "1 if different, 2 if trouble.\n");
        fprintf(message_out, "\n");
        fprintf(message_out,
            "\n"
            "Examples:\n"
            "  $ %s cat.hfst dog.hfst\n"
            "  cat.hfst[1] != dog.hfst[1]\n"        
            "  $ %s cat.hfst cat.hfst\n"
            "  cat.hfst[1] == cat.hfst[1]\n"        
            "\n",
                program_name, program_name );
        print_report_bugs();
        print_more_info();
}

void
parse_options(int argc, char** argv)
  {
    while (true)
      {
        static const struct option long_options[] =
        {
          HFST_GETOPT_COMMON_LONG,
          HFST_GETOPT_BINARY_LONG,
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_BINARY_SHORT,
                             long_options, &option_index);
        if (-1 == c)
          {
            break;
          }
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        else 
          {
            parse_getopt_error_value(c);
          }
      }
  }

int
make_comparisons()
{
    bool bothInputs = firststream->is_good() && secondstream->is_good();
    size_t transducer_n = 0;
    size_t mismatches = 0;
    while (bothInputs) {
        transducer_n++;
        HfstTransducer first(*firststream);
        HfstTransducer second(*secondstream);
        const char* firstname = hfst_get_name(first, firstfilename);
        const char* secondname = hfst_get_name(second, secondfilename);
        hfst_begin_processing(firstname, secondname, transducer_n,
                              "Comparing");
        try
          {
            if (first.compare(second))
              {
                if (transducer_n == 1)
                  {
                    if (!silent)
                      {
                        fprintf(outfile, "%s == %s\n", firstname, secondname);
                      }
                  }
                else
                  {
                    if (!silent)
                      {
                        fprintf(outfile, "%s[%zu] == %s[%zu]\n",
                                firstname, transducer_n,
                                secondname, transducer_n);
                      }
                  }
              }
            else
              {
                if (transducer_n == 1)
                  {
                    if (!silent)
                      {
                        fprintf(outfile, "%s != %s\n", firstname, secondname);
                      }
                  }
                else
                  {
                    if (!silent)
                      {
                        fprintf(outfile, "%s[%zu] != %s[%zu]\n",
                                firstname, transducer_n, 
                                secondname, transducer_n);
                      }
                  }
                mismatches++;
              }
          }
        catch (TransducerTypeMismatchException ttme)
          {
            // cannot recover yet, but beautify error messages
            hfst_error(2, 0, "Cannot compare `%s' and `%s' [%zu]\n"
                  "the formats %s and %s are not compatible for comparison\n",
                  firstname, secondname, transducer_n,
                  hfst_strformat(firststream->get_type()),
                  hfst_strformat(secondstream->get_type()));
          }
        bothInputs = firststream->is_good() && secondstream->is_good();
    }
    if (firststream->is_good())
    {
      while (firststream->is_good())
        {
          transducer_n++;
          HfstTransducer dummy(*firststream);
          const char* firstname = hfst_get_name(dummy, firstfilename);
          hfst_verbose("Cannot compare %s %zu to non-existent transducer",
                       firstname, transducer_n);
          if (!silent)
            {
              fprintf(outfile, "%s[%zu] != ?\n", firstname, transducer_n);
            }
          mismatches++;
        }
    }
    else if (secondstream->is_good())
    {
      while (secondstream->is_good())
        {
          transducer_n++;
          HfstTransducer dummy(*secondstream);
          const char* secondname = hfst_get_name(dummy, secondfilename);
          verbose_printf("Cannot compare %s %zu to non-existent transducer",
                         secondname, transducer_n);
          if (!silent)
            {
              fprintf(outfile, "? != %s[%zu]\n", secondname, transducer_n);
            }
          mismatches++;
        }
    }
    if (mismatches == 0)
      {
        hfst_verbose("All %zu transducers matched", transducer_n);
        return EXIT_SUCCESS;
      }
    else
      {
        hfst_verbose("%zu/%zu were not equal", mismatches, transducer_n);
        return EXIT_FAILURE;
      }
}


int main(int argc, char **argv)
  {
    hfst_init_commandline(argv[0], "0.1", "HfstCompare",
                          AUTOM_IN_FILE_OUT, READ_TWO);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    int rv = make_comparisons();
    hfst_uninit_commandline();
    return rv;
  }

