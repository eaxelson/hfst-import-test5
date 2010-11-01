//! @file hfst-compose-intersect.cc
//!
//! @brief Transducer inters3cting composition tool
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

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::HfstGrammar;
using hfst::ImplementationType;
using hfst::exceptions::NotTransducerStreamException;

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "inc/globals-common.h"
static char* lexiconfilename = 0;
static FILE* lexiconfile = 0;
static bool lexiconNamed = false;
static char** rulefilenames = 0;
static FILE** rulefiles = 0;
static unsigned int rulecount = 0;
static bool is_input_stdin = true;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] RULEFILE...\n"
             "Compose lexicon with intersection of rules\n"
        "\n", program_name );
        print_common_program_options(message_out);
        fprintf(message_out, "Input/Output options:\n"
                "  -l, --lexicon=LEXFILE   read lexicon from LEXFILE\n"
                "  -o, --output=OUTFILE    write result to OUTFILE\n");
        fprintf(message_out, "\n");
        fprintf(message_out, 
                "If LEXFILE is omitted or - it is read from stdin.\n"
                "If OUTFILE is omitted, result is written to stdout.\n"
                "At least one RULEFILE must be given.\n");
        fprintf(message_out, "\n");
        fprintf(message_out,
            "\n"
            "Examples:\n"
            "  %s -o fi_FI.hfst -l fi_FI.lex.hfst fi_FI.twol.hfst  "
            "compose single ruleset with lexicon\n"
            "\n",
            program_name );
        print_report_bugs();
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
          {"lexicon", required_argument, 0, 'l'},
          {"output", required_argument, 0, 'o'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             "l:o:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        switch (c)
        {
#include "inc/getopt-cases-common.h"
        case 'l':
          lexiconfilename = hfst_strdup(optarg);
          lexiconfile = hfst_fopen(lexiconfilename, "r");
          if (lexiconfile == stdin) {
            free(lexiconfilename);
            lexiconfilename = hfst_strdup("<stdin>");
            is_input_stdin = true;
          }
          lexiconNamed = true;
          break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
    if (lexiconNamed)
      {
        if (argc - optind > 0)
          {
            // rest rules
            rulefilenames = static_cast<char**>(malloc(sizeof(char*)*(argc-optind+1)));
            rulefiles = static_cast<FILE**>(malloc(sizeof(FILE*)*(argc-optind+1)));
            while (optind < argc)
              {
                rulefilenames[rulecount] = hfst_strdup(argv[optind]);
                rulefiles[rulecount] = hfst_fopen(argv[optind], "r");
                rulecount++;
                optind++;
              }
          }
        else
          {
            rulefilenames = static_cast<char**>(malloc(sizeof(char*)));
            rulefiles = static_cast<FILE**>(malloc(sizeof(FILE*)));
            rulefilenames[0] = hfst_strdup("<stdin>");
            rulefiles[0] = stdin;
          }
      }
    else
      {
        lexiconfilename = hfst_strdup("<stdin>");
        lexiconfile = stdin;
        if (argc - optind > 0)
          {
            // rest rules
            rulefilenames = static_cast<char**>(malloc(sizeof(char*)*(argc-optind+1)));
            rulefiles = static_cast<FILE**>(malloc(sizeof(FILE*)*(argc-optind+1)));
            while (optind < argc)
              {
                rulefilenames[rulecount] = hfst_strdup(argv[optind]);
                rulefiles[rulecount] = hfst_fopen(argv[optind], "r");
                rulecount++;
                optind++;
              }
          }
        else
          {
            error(EXIT_FAILURE, 0, "Must give either rules or lexicon from "
                  "named file");
            return EXIT_FAILURE;
          }
      }
    return EXIT_CONTINUE;
}

int
compose_intersect_streams(HfstInputStream& lexiconstream, HfstGrammar& rules,
                    HfstOutputStream& outstream)
{
  //lexiconstream.open();
  //  outstream.open();
    // should be is_good? 
    size_t transducer_n = 0;
    while (lexiconstream.is_good()) {
        transducer_n++;
        if (transducer_n == 1)
        {
            verbose_printf("Composing %s and rule intersection...\n",
                           lexiconfilename);
        }
        else
        {
            verbose_printf("Composing %s and rule intersection... %zu\n",
                           lexiconfilename, transducer_n);
        }
        HfstTransducer lexicon(lexiconstream);
        lexicon.compose_intersect(rules);
        outstream << lexicon;
    }
    
    lexiconstream.close();
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
    if (lexiconfile != stdin)
    {
        fclose(lexiconfile);
    }
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from %s and ", 
        lexiconfilename);
    for (unsigned int i = 0; i < rulecount; i++)
      {
        verbose_printf("%s, ", rulefilenames[i]);
      }
    verbose_printf("writing to %s\n", outfilename);
    // here starts the buffer handling part
    HfstInputStream* lexiconstream = NULL;
    HfstInputStream* rulestream = NULL;
    try {
        lexiconstream = (lexiconfile != stdin) ?
            new HfstInputStream(lexiconfilename) : new HfstInputStream();
    } catch(NotTransducerStreamException)   {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              lexiconfilename);
    }
    hfst::HfstTransducerVector rulevector;
    for (unsigned int i = 0; i < rulecount; i++)
      {
        FILE* rulefile = rulefiles[i];
        size_t rule_n = 0;
        try {
          rulestream = (rulefile != stdin) ?
            new HfstInputStream(rulefilenames[i]) : new HfstInputStream();
          //rulestream->open();
          while (rulestream->is_good())
            {
              rule_n++;
              if (rule_n < 2)
                {
                  verbose_printf("Reading rules from %s...\n", rulefilenames[i]);
                }
              else
                {
                  verbose_printf("Reading rules from %s... %zu\n",
                                 rulefilenames[i], rule_n);
                }
              HfstTransducer newRule(*rulestream);
              rulevector.push_back(newRule);
            }
          }
        catch(NotTransducerStreamException) 
          {
            error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
                  rulefilenames[i]);
          }
      }
    HfstGrammar rules(rulevector);
    HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, lexiconstream->get_type()) :
        new HfstOutputStream(lexiconstream->get_type());

    retval = compose_intersect_streams(*lexiconstream, rules, *outstream);
    delete lexiconstream;
    delete outstream;
    free(lexiconfilename);
    free(outfilename);
    return retval;
}

