//! @file hfst-pmatch2fst.cc
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>
#include <errno.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "parsers/PmatchCompiler.h"
#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "hfst-tool-metadata.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::HfstTransducer;
using hfst::pmatch::PmatchCompiler;

static char *epsilonname=NULL;
static bool disjunct_expressions=false;
static bool line_separated = false;

//static unsigned int sum_of_weights=0;
static bool sum_weights=false;
static bool normalize_weights=false;
static bool logarithmic_weights=false;

#if HAVE_OPENFST
static hfst::ImplementationType compilation_format = hfst::TROPICAL_OPENFST_TYPE;
#elif HAVE_FOMA
static hfst::ImplementationType compilation_format = hfst::FOMA_TYPE;
#elif HAVE_SFST
static hfst::ImplementationType compilation_format = hfst::SFST_TYPE;
#else
static hfst::ImplementationType compilation_format = hfst::ERROR_TYPE;
#endif

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
            "Compile regular expressions into transducer(s)\n (Experimental version)"
            "\n", program_name); 
    print_common_program_options(message_out);
    print_common_unary_program_options(message_out); 
    fprintf(message_out, "String and format options:\n"
            "  -e, --epsilon=EPS         Map EPS as zero.\n");
    fprintf(message_out, "\n");

    fprintf(message_out, 
            "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
            "If EPS is not defined, the default representation of 0 is used\n"
            "Weights are currently not implemented.\n"
            "\n"
        );

    fprintf(message_out, "Examples:\n"
            "  echo \"Define TOP  UppercaseAlpha Alpha* LC(\\\"professor\\\") EndTag(ProfName);\" | %s \n"
            "  create matcher that tags \"professor Chomsky\" as \"professor <ProfName>Chomsky</ProfName>\"\n"
            "\n", program_name, program_name);
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
    fprintf(message_out, "\n");
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
                {"epsilon", required_argument, 0, 'e'},
                {0,0,0,0}
            };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "e::",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
        case 'e':
            epsilonname = hfst_strdup(optarg);
            break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

int
process_stream(HfstOutputStream& outstream)
{
    PmatchCompiler comp(compilation_format);
    std::string file_contents;
    std::map<std::string, HfstTransducer*> definitions;
    int c;
    while ((c = fgetc(inputfile)) != EOF) {
        file_contents.push_back(c);
    }
    definitions = comp.compile(file_contents);
    
    // When done compiling everything, look for TOP and output it first
    if (definitions.count("TOP") == 1) {
        outstream << (*definitions["TOP"]).convert(hfst::HFST_OL_TYPE);
        delete definitions["TOP"];
        definitions.erase("TOP");
    }
    for (std::map<std::string, HfstTransducer *>::iterator it = definitions.begin();
         it != definitions.end(); ++it) {
        delete it->second;
    }
    return EXIT_SUCCESS;
}

extern int pmatchdebug;

int main( int argc, char **argv ) 
{

//    pmatchdebug = 1;

    hfst_set_program_name(argv[0], "0.1", "Pmatch2Fst");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from %s, writing to %s\n", 
                   inputfilename, outfilename);
    // here starts the buffer handling part
    HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, hfst::HFST_OL_TYPE) :
        new HfstOutputStream(hfst::HFST_OL_TYPE);
    process_stream(*outstream);
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}

