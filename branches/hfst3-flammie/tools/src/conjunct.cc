//! @file hfst-conjunct.cc
//!
//! @brief Transducer conjunction tool
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


#include "conventions/commandline.h"

static bool harmonize_flags=false;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
             "Conjunct (intersect, AND) two transducers\n"
            "\n", program_name);
        print_common_program_options();
        fprintf(message_out,
                "Flag diacritics:\n"
                "  -F, --harmonize-flags  Harmonize flag diacritics\n");
        fprintf(message_out, "\n");
        print_common_parameter_instructions();
        fprintf(message_out, "\n");
        fprintf(message_out,
            "\n"
            "Examples:\n"
            "  %s -o dog.hfst cat_or_dog.hfst dog_or_mouse.hfst\n"
            "\n",
            program_name );
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
          {"harmonize-flags", no_argument, 0, 'F'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_BINARY_SHORT "F",
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
          case 'F':
            harmonize_flags=true;
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
make_conjunctions()
  {
    bool bothInputs = firststream->is_good() && secondstream->is_good();
    if (firststream->get_type() != secondstream->get_type())
      {
        hfst_warning( "Tranducer type mismatch in %s and %s; "
              "using former type as output",
              firstfilename, secondfilename);
      }
    size_t transducer_n = 0;
    while (bothInputs) 
      {
        HfstTransducer first(*firststream);
        HfstTransducer second(*secondstream);
        transducer_n++;
        const char* firstname = hfst_get_name(first, firstfilename);
        const char* secondname = hfst_get_name(second, secondfilename);
        hfst_begin_processing(firstname, secondname, transducer_n,
                               "Intersecting");
        if (first.has_flag_diacritics() || second.has_flag_diacritics()) 
          {
            if (!harmonize_flags)
              {
                if (!silent) 
                  {
                    hfst_warning("At least one of the arguments contains "
                                 "flag diacritics. Use -F to harmonize them.", 
                                 secondname, firstname);
                  }
              }
            else
              {
                first.harmonize_flag_diacritics(second);
              }
        }

        try 
          {
            first.intersect(second);
          }
        catch (TransducerTypeMismatchException ttme)
          {
            hfst_error(EXIT_FAILURE, 0, "Could not conjunct %s and %s [%zu]\n"
                  "formats %s and %s are not compatible for intersection",
                  firstname, secondname, transducer_n,
                  hfst_strformat(firststream->get_type()),
                  hfst_strformat(secondstream->get_type()));
          }
        hfst_set_name(first, first, second, "intersect");
        hfst_set_formula(first, first, second, "∩");
        *outstream << first;
        bothInputs = firststream->is_good() && secondstream->is_good();
      }
    if (firststream->is_good())
      {
        hfst_warning("%s contains more transducers than %s; "
                     "residue skipped\n", firstfilename, secondfilename);
      }
    else if (secondstream->is_good())
      {
        hfst_warning("%s contains fewer transducers than %s; "
                     "residue skipped\n", firstfilename, secondfilename);
      }
  }


int main(int argc, char **argv)
  {
    hfst_init_commandline(argv[0], "0.1", "HfstConjunct",
                          AUTOM_IN_AUTOM_OUT, READ_TWO);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_conjunctions();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

