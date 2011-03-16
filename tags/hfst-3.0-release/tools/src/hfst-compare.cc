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

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;
//

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "inc/globals-common.h"
#include "inc/globals-binary.h"

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
             "Compare two transducers\n"
        "\n", program_name );
        print_common_program_options(message_out);
        print_common_binary_program_options(message_out);
        fprintf(message_out, "\n");
        print_common_binary_program_parameter_instructions(message_out);
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
        fprintf(message_out, "\n");
        print_more_info();
}

int
parse_options(int argc, char** argv)
{
    // use of this function requires options are settable on global scope
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
        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-binary.h"
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-binary.h"
    return EXIT_CONTINUE;
}

int
compare_streams(HfstInputStream& firststream, HfstInputStream& secondstream)
{
  //firststream.open();
  //secondstream.open();
    // should be is_good? 
    bool bothInputs = firststream.is_good() && secondstream.is_good();
    size_t transducer_n = 0;
    size_t mismatches = 0;
    while (bothInputs) {
        transducer_n++;
        if (transducer_n == 1)
        {
            verbose_printf("Comparing %s and %s...\n", firstfilename, 
                        secondfilename);
        }
        else
        {
            verbose_printf("Comparing %s and %s... %zu\n",
                           firstfilename, secondfilename, transducer_n);
        }
        HfstTransducer first(firststream);
        HfstTransducer second(secondstream);
        if (first.compare(second))
          {
            if (transducer_n == 1)
              {
		if (not silent)
		  fprintf(outfile, "%s == %s\n", firstfilename, secondfilename);
              }
            else
              {
		if (not silent)
		  fprintf(outfile, "%s[%zu] == %s[%zu]\n",
			  firstfilename, transducer_n,
			  secondfilename, transducer_n);
              }
          }
        else
          {
            if (transducer_n == 1)
              {
		if (not silent)
		  fprintf(outfile, "%s != %s\n", firstfilename, secondfilename);
              }
            else
              {
		if (not silent)
		  fprintf(outfile, "%s[%zu] != %s[%zu]\n",
			  firstfilename, transducer_n, 
			  secondfilename, transducer_n);
              }
            mismatches++;
          }
        bothInputs = firststream.is_good() && secondstream.is_good();
    }
    
    if (firststream.is_good())
    {
      while (firststream.is_good())
        {
          transducer_n++;
          HfstTransducer dummy(firststream);
          verbose_printf("Cannot compare %s %zu to non-existent transducer",
                         firstfilename, transducer_n);
	  if (not silent)
	    fprintf(outfile, "%s[%zu] != ?\n", firstfilename, transducer_n);
          mismatches++;
        }
    }
    else if (secondstream.is_good())
    {
      while (secondstream.is_good())
        {
          transducer_n++;
          HfstTransducer dummy(secondstream);
          verbose_printf("Cannot compare %s %zu to non-existent transducer",
                         secondfilename, transducer_n);
	  if (not silent)
	    fprintf(outfile, "? != %s[%zu]\n", secondfilename, transducer_n);
          mismatches++;
        }
    }
    firststream.close();
    secondstream.close();
    fclose(outfile);
    if (mismatches == 0)
      {
        verbose_printf("All %zu transducers matched\n", transducer_n);
        return EXIT_SUCCESS;
      }
    else
      {
        verbose_printf("%zu/%zu were not equal\n", mismatches, transducer_n);
        return EXIT_FAILURE;
      }
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstCompare");
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
    verbose_printf("Reading from %s and %s, writing log to %s\n", 
        firstfilename, secondfilename, outfilename);
    // here starts the buffer handling part
    HfstInputStream* firststream = NULL;
    HfstInputStream* secondstream = NULL;
    try {
        firststream = (firstfile != stdin) ?
            new HfstInputStream(firstfilename) : new HfstInputStream();
    } //catch(const HfstException e)   {
    catch (const HfstException e) {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              firstfilename);
    }
    try {
        secondstream = (secondfile != stdin) ?
            new HfstInputStream(secondfilename) : new HfstInputStream();
    } //catch(const HfstException e)   {
    catch (const HfstException e) {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              secondfilename);
    }

    retval = compare_streams(*firststream, *secondstream);
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    delete firststream;
    delete secondstream;
    free(firstfilename);
    free(secondfilename);
    free(outfilename);
    return retval;
}

