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

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;
using hfst::HfstTransducerVector;

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "inc/globals-common.h"
#include "inc/globals-binary.h"

static bool insert_missing_flags=false;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
             "Compose a lexicon with one or more rule transducers.\n"
        "\n", program_name );
        print_common_program_options(message_out);
        print_common_binary_program_options(message_out);
        print_common_binary_program_parameter_instructions(message_out);
        fprintf(message_out, "\n");
        fprintf(message_out,
            "\n"
            "Examples:\n"
            "  %s -o analyzer.hfst hfst-lexc-lexicon.hfst " 
            " hfst-twolc-rules.hfst  compose rules with lexicon\n"
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
compose_streams(HfstInputStream& firststream, HfstInputStream& secondstream,
                HfstOutputStream& outstream)
{
    bool bothInputs = firststream.is_good() && secondstream.is_good();
    if (firststream.get_type() != secondstream.get_type())
      {
        warning(0, 0, "Transducer type mismatch in %s and %s; "
              "using former type as output\n",
              firstfilename, secondfilename);
      }

    verbose_printf("Reading lexicon...");
    HfstTransducer lexicon(firststream);
    const char* lexiconname = lexicon.get_name().c_str();
    if (strlen(lexiconname) < 1) 
      {
        lexiconname = hfst_strdup(firstfilename);
      }
    verbose_printf(" %s read\n", lexiconname);
    HfstTransducerVector rules;
    size_t transducer_n = 1;
    while (secondstream.is_good()) {
      HfstTransducer rule(secondstream);
      const char* rulename = rule.get_name().c_str();
      if (strlen(rulename) > 0)
        {
          verbose_printf("Reading and minimizing rule %s...\n",
                         rulename);
        }
      else
        {
          verbose_printf("Reading and minimizing rule %zu...\n",
                         transducer_n);
        }
      rule.minimize();
      rules.push_back(rule);      
    }
    
    verbose_printf("Computing intersecting composition...\n");
    lexicon.compose_intersect(rules);
    char* composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                             (strlen(lexiconname) +
                                              strlen(secondfilename) +
                                              strlen("hfst-compose-intersect=(%s o &(%s))")) 
                                             + 1));
    if (sprintf(composed_name, "hfst-compose-intersect=(%s o &(%s))", 
                lexiconname, secondfilename) > 0)
      {
        lexicon.set_name(composed_name);
      }


    verbose_printf("Storing result in %s...\n", outfilename);
    outstream << lexicon;

    firststream.close();
    secondstream.close();
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstComposeIntersect");
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
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              firstfilename);
    }
    try {
        secondstream = (secondfile != stdin) ?
            new HfstInputStream(secondfilename) : new HfstInputStream();
    } catch(const HfstException e)   {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              secondfilename);
    }
    HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, firststream->get_type()) :
        new HfstOutputStream(firststream->get_type());

    retval = compose_streams(*firststream, *secondstream, *outstream);
    delete firststream;
    delete secondstream;
    delete outstream;
    free(firstfilename);
    free(secondfilename);
    free(outfilename);
    return retval;
}

