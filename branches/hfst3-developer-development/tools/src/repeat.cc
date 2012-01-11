//! @file hfst-repeat.cc
//!
//! @brief Transducer repetition tool
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

#include "conventions/commandline.h"
#include "conventions/options.h"
#include "conventions/metadata.h"

#include "conventions/globals-common.h"
#include "conventions/globals-unary.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;


// add tools-specific variables here
unsigned long at_least = 0;
unsigned long at_most = UINT_MAX;
bool from_infinity = false;
bool to_infinity = true;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Repeat transducer\n"
        "\n", program_name);

    print_common_program_options();
    print_common_unary_program_options();
    fprintf(message_out, "Repetition options:\n"
            "  -f, --from=FNUM   repeat at least FNUM times\n"
            "  -t, --to=TNUM     repeat at most TNUM times\n");
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions();
    fprintf(message_out, 
            "FNUM and TNUM must be positive integers or infinities "
            "as parsed by strtod(3)\n"
            "if FNUM is omitted it defaults to 0, if TNUM is omitted it defaults to Inf\n"
            "FNUM must be less than TNUM\n");
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
          HFST_GETOPT_UNARY_LONG,
          {"from", required_argument, 0, 'f'},
          {"to", required_argument, 0, 't'},
          // add tool-specific options here 
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "f:t:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "conventions/getopt-cases-common.h"
#include "conventions/getopt-cases-unary.h"
        case 'f':
          at_least = hfst_strtonumber(optarg, &from_infinity);
          break;
        case 't':
          at_most = hfst_strtonumber(optarg, &to_infinity);
          break;
#include "conventions/getopt-cases-error.h"
        }
    }

#include "conventions/check-params-common.h"
#include "conventions/check-params-unary.h"
    if (at_least > at_most)
      {
        error(EXIT_FAILURE, 0, "Cannot repeat from %lu to %lu times\n",
              at_least, at_most);
      }
    if (from_infinity && !to_infinity)
      {
        error(EXIT_FAILURE, 0, "Cannot repeat from infinity to %lu times\n",
              at_most);
      }
    return EXIT_CONTINUE;
}

int
process_stream(HfstInputStream& instream, HfstOutputStream& outstream)
{
  //instream.open();
  //outstream.open();
    
    size_t transducer_n=0;
    while(instream.is_good())
      {
        transducer_n++;
        HfstTransducer trans(instream);
        char* inputname = hfst_get_name(trans, inputfilename);
        if (transducer_n==1)
          {
            if (!from_infinity && !to_infinity)
              {
                verbose_printf("Repeating [%lu..%lu] %s...\n", at_least,
                               at_most, inputname);
              }
          else if (from_infinity && to_infinity)
            {
              verbose_printf("Repeating star %s...\n", inputname);
            }
          else if (!from_infinity && to_infinity)
            {
              verbose_printf("Repeating [%lu..*] %s...\n", at_least,
                             inputname);
            }
          else if (from_infinity && to_infinity)
            {
              error(EXIT_FAILURE, 0, "Repeating *..%lu?", at_most);
            }
        }
        else
        {
          if (!from_infinity && !to_infinity)
            {
              verbose_printf("Repeating [%lu..%lu] %s... %zu\n", 
                             at_least, at_most, inputname, transducer_n);
            }
          else if (from_infinity && to_infinity)
            {
              verbose_printf("Repeating star %s... %zu\n", inputname,
                             transducer_n);
            }
          else if (!from_infinity && to_infinity)
            {
              verbose_printf("Repeating [%lu..*] %s... %zu\n", at_least,
                             inputname, transducer_n);
            }
          else if (from_infinity && to_infinity)
            {
              error(EXIT_FAILURE, 0, "Repeating *..%lu?", at_most);
            }
        }
        
        if (!from_infinity && !to_infinity)
          {
            trans.repeat_n_to_k(at_least, at_most);
            char* composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                             (strlen("repeat-%lu-to-%lu"))
                                             + 1 + 32 + 32));
            if (sprintf(composed_name, "repeat-%lu-to-%lu",
                        at_least, at_most) > 0)
              {
                hfst_set_name(trans, trans, composed_name);
              }
            composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                             (strlen("_%lu^%lu"))
                                             + 1 + 32 + 32));
            if (sprintf(composed_name, "_%lu^%lu",
                        at_least, at_most) > 0)
              {
                hfst_set_formula(trans, trans, composed_name);
              }
            free(composed_name);
          }
        else if (from_infinity && to_infinity)
          {
            trans.repeat_star();
            hfst_set_name(trans, trans, "repeat-star");
            hfst_set_formula(trans, trans, "⋆");
          }
        else if (!from_infinity && to_infinity)
          {
            trans.repeat_n_plus(at_least);
            char* composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                             (strlen("repeat-%lu-plus")) 
                                             + 1 + 32 + 32));
            if (sprintf(composed_name, "repeat-%lu-plus",
                        at_least) > 0)
              {
                hfst_set_name(trans, trans, composed_name);
              }
            composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                             (strlen("_%lu^∞")) 
                                             + 1 + 32 + 32));
            if (sprintf(composed_name, "_%lu^∞",
                        at_least) > 0)
              {
                hfst_set_formula(trans, trans, composed_name);
              }
            free(composed_name);
          }
        else if (from_infinity && !to_infinity)
          {
             error(EXIT_FAILURE, 0, "Repeating *..%lu?", at_most);
          }
        outstream << trans;
    }
    instream.close();
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstRepeat");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    if (inputfile != stdin)
    {
        fclose(inputfile);
    }
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from %s, writing to %s\n", 
        inputfilename, outfilename);
          if (!from_infinity && !to_infinity)
            {
              verbose_printf("Repeating from %lu to %lu times\n",
                             at_least, at_most);
            }
          else if (from_infinity && to_infinity)
            {
              verbose_printf("Repeating star infinitely\n");
            }
          else if (!from_infinity && to_infinity)
            {
              verbose_printf("Repeating from %lu to infinite times\n",
                             at_least);
            }
          else if (from_infinity && !to_infinity)
            {
              error(EXIT_FAILURE, 0, "Repeating at least infinite but"
                    "no more than %lu times?", at_most);
            }

    // here starts the buffer handling part
    HfstInputStream* instream = NULL;
    try {
      instream = (inputfile != stdin) ?
        new HfstInputStream(inputfilename) : new HfstInputStream();
    } catch(const HfstException e)  {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              inputfilename);
        return EXIT_FAILURE;
    }
    HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, instream->get_type()) :
        new HfstOutputStream(instream->get_type());
    
    retval = process_stream(*instream, *outstream);
    delete instream;
    delete outstream;
    free(inputfilename);
    free(outfilename);
    return retval;
}

