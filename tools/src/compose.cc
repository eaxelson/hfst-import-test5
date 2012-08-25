//! @file hfst-compose.cc
//!
//! @brief Transducer composition tool
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
#include "conventions/metadata.h"
#include "conventions/options.h"
#include "conventions/globals-common.h"
#include "conventions/globals-binary.h"

static bool harmonize_flags=false;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
             "Compose two transducers\n"
        "\n", program_name );
        print_common_program_options();
        print_common_binary_program_options();
        fprintf(message_out,
                "Flag diacritics:\n"
                "  -F, --harmonize-flags  Harmonize flag diacritics.");
        fprintf(message_out, "\n");
        print_common_binary_program_parameter_instructions();
        fprintf(message_out, "\n");
        fprintf(message_out,
            "\n"
            "Examples:\n"
            "  %s -o cat2dog.hfst cat2mouse.hfst mouse2dog.hfst  "
            "composes two automata\n"
            "\n",
            program_name );
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
        switch (c)
        {
#include "conventions/getopt-cases-common.h"
#include "conventions/getopt-cases-binary.h"
        case 'F':
          harmonize_flags=true;
          break;
#include "conventions/getopt-cases-error.h"
        }
    }

#include "conventions/check-params-common.h"
#include "conventions/check-params-binary.h"
    return EXIT_CONTINUE;
}

int
compose_streams(HfstInputStream& firststream, HfstInputStream& secondstream,
                HfstOutputStream& outstream)
{
    bool bothInputs = firststream.is_good() && secondstream.is_good();
    if (firststream.get_type() != secondstream.get_type())
      {
        warning(0, 0, "Tranducer type mismatch in %s and %s; "
              "using former type as output\n",
              firstfilename, secondfilename);
      }
    size_t transducer_n = 0;
    while (bothInputs) {
        transducer_n++;
        HfstTransducer first(firststream);
        HfstTransducer second(secondstream);
        char* firstname = hfst_get_name(first, firstfilename);
        char* secondname = hfst_get_name(second, secondfilename);
        if (transducer_n == 1)
        {
            verbose_printf("Composing %s and %s...\n", firstname, 
                           secondname);
        }
        else
        {
            verbose_printf("Composing %s and %s... %zu\n",
                           firstname, secondname, transducer_n);
        }

        try {
        if (first.has_flag_diacritics() or second.has_flag_diacritics()) 
          {
            if (not harmonize_flags)
              {
                if (not silent) 
                  {
                    warning(0, 0, "At least one of the arguments contains "
                    "flag diacritics. Use -F to harmonize them.", 
                    secondname, firstname);
          }
              }
            else
              {
                first.harmonize_flag_diacritics(second);
              }
        }

        hfst_set_name(first, first, second, "compose");
        hfst_set_formula(first, first, second, "∘");
        first.compose(second);
        outstream << first;

        }
        catch (HfstTransducerTypeMismatchException)
          {
            hfst_error(EXIT_FAILURE, 0, "Could not compose %s and %s [%zu]\n"
                  "types %s and %s are not compatible for composition",
                  firstname, secondname, transducer_n,
                  hfst_strformat(firststream.get_type()),
                  hfst_strformat(secondstream.get_type()));
          }

        bothInputs = firststream.is_good() && secondstream.is_good();
    }
    
    if (firststream.is_good())
    {
      warning(0, 0, "%s contains more transducers than %s; "
                     "residue skipped", firstfilename, secondfilename);
    }
    else if (secondstream.is_good())
    {
      warning(0, 0, "%s contains fewer transducers than %s; "
                     "residue skipped", firstfilename, secondfilename);
    }
    firststream.close();
    secondstream.close();
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstCompose");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    if (firstfile != stdin)
    {
        fclose(firstfile);
    }
    if (secondfile != stdin)
    {
        fclose(secondfile);
    }
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from %s and %s, writing to %s\n", 
        firstfilename, secondfilename, outfilename);
    // here starts the buffer handling part
    HfstInputStream* firststream = NULL;
    HfstInputStream* secondstream = NULL;
    try {
        firststream = (firstfile != stdin) ?
            new HfstInputStream(firstfilename) : new HfstInputStream();
    } catch(const HfstException e)   {
        hfst_error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              firstfilename);
    }
    try {
        secondstream = (secondfile != stdin) ?
            new HfstInputStream(secondfilename) : new HfstInputStream();
    } catch(const HfstException e)   {
        hfst_error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              secondfilename);
    }
    HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, firststream->get_type()) :
        new HfstOutputStream(firststream->get_type());

    retval = compose_streams(*firststream, *secondstream, *outstream);
    if (profile_file != 0)
      {
        hfst_print_profile_line();
      }
    delete firststream;
    delete secondstream;
    delete outstream;
    free(firstfilename);
    free(secondfilename);
    free(outfilename);
    return retval;
}

