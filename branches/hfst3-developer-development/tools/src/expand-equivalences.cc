//! @file extend-equivalences.cc
//!
//! @brief Transducer label modification
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

#include "conventions/commandline.h"
#include "conventions/options.h"
#include "conventions/metadata.h"

#include "conventions/globals-common.h"
#include "conventions/globals-unary.h"

static char* only_from_label = 0;
static char* only_to_label = 0;

static char* acx_file_name = 0;
static FILE* acx_file = 0;
static char* tsv_file_name = 0;
static FILE* tsv_file = 0;

enum fsa_level_t
  {
    FSA_LEVEL_FIRST,
    FSA_LEVEL_SECOND,
    FSA_LEVEL_BOTH
  };
static fsa_level_t level = FSA_LEVEL_FIRST;


void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Extend transducer arcs for equivalence classes\n"
        "\n", program_name);

    print_common_program_options();
    print_common_unary_program_options();
    fprintf(message_out, "Eqv. class extension options:\n"
            "  -f, --from=ISYM     convert single symbol ISYM to allow OSYM\n"
            "  -t, --to=OSYM       convert to OSYM\n"
            "  -a, --acx=ACXFILE   read extensions in acx format "
            "from ACXFILE\n"
            "  -t, --tsv=TSVFILE   read extensions in tsv format "
            "from TSVFILE\n"
            "  -l, --level=LEVEL   perform extensions on LEVEL of fsa\n"
           );
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions();
    fprintf(message_out, "Either ACXFILE, TSVFILE or both ISYM and OSYM "
            "must be specified.\n"
            "LEVEL should be either {upper, first, 1, input, surface}, "
            "{lower, second, 2, output, analysis} or both.\n"
            "If LEVEL is omitted, default is first.\n");
    fprintf(message_out,
           "Examples:\n"
           "  %s -o rox.hfst -a romanian.acx ro.hfst  extend romanian char"
           "equivalences\n"
           "\n", program_name);
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
          // add tool-specific options here
            {"from",  required_argument, 0, 'f'},
            {"to",    required_argument, 0, 't'},
            {"acx",   required_argument, 0, 'a'},
            {"tsv",   required_argument, 0, 'T'},
            {"level", required_argument, 0, 'l'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "f:t:a:T:l:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        switch (c)
        {
#include "conventions/getopt-cases-common.h"
#include "conventions/getopt-cases-unary.h"
          // add tool-specific cases here
            case 'f':
              only_from_label = hfst_strdup(optarg);
              break;
            case 't':
              only_to_label = hfst_strdup(optarg);
              break;
            case 'a':
              acx_file_name = hfst_strdup(optarg);
              break;
            case 'T':
              tsv_file_name = hfst_strdup(optarg);
              break;
            case 'l':
              if ((strcmp("first", optarg) == 0) ||
                  (strcmp("upper", optarg) == 0) ||
                  (strcmp("input", optarg) == 0) ||
                  (strcmp("1", optarg) == 0))
                {
                  level = FSA_LEVEL_FIRST;
                }
              else if ((strcmp("second", optarg) == 0) ||
                       (strcmp("lower", optarg) == 0) ||
                       (strcmp("output", optarg) == 0) ||
                       (strcmp("2", optarg) == 0))
                {
                  level = FSA_LEVEL_SECOND;
                }
              else if ((strcmp("both", optarg) == 0))
                {
                  level = FSA_LEVEL_BOTH;
                }
              else 
                {
                  error(EXIT_FAILURE, 0, "The option for level parameter must"
                        " be one of:\n"
                        "upper, first, input, second, lower, output, both, "
                        "1 or 2.");
                }
              break;
#include "conventions/getopt-cases-error.h"
        }
    }
    // handling erroneous input combinations:
    // one of(-f & -t), -a, -T
    if ((only_from_label != 0) || (only_to_label != 0))
      {
        if ((tsv_file_name != 0) || (acx_file_name != 0))
          {
            error(EXIT_FAILURE, 0,
                  "Only one of -a, -T or -f and -t may be given");
          }
        else if (only_from_label == 0)
          {
            error(EXIT_FAILURE, 0,
                  "option -t requires -f");
          }
        else if (only_to_label == 0)
          {
            error(EXIT_FAILURE, 0,
                  "option -f requires -t");
          }
      }
    else if ((tsv_file_name == 0) && (acx_file_name == 0))
    {
        error(EXIT_FAILURE, 0,
              "Must give extension specification file with either -a or -t.");
        return EXIT_FAILURE;
    }
    else if ((tsv_file_name != 0) && (acx_file_name != 0))
    {
        error(EXIT_FAILURE, 0,
              "Only one of parameters -a, -t, must be used.");
        return EXIT_FAILURE;
    }
#include "conventions/check-params-common.h"
#include "conventions/check-params-unary.h"
    return EXIT_CONTINUE;
}

static
void
add_extension(HfstTransducer* t, const char* from, const char* to)
  {
    verbose_printf("extending %s by %s\n", from, to);
    HfstTransducer remap(from, to, t->get_type());
    t->disjunct(remap);
  }

static
int
process_stream(HfstInputStream& instream, HfstOutputStream& outstream)
  {
    size_t transducer_n = 0;
    while (instream.is_good())
      {
        transducer_n++;
        HfstTransducer trans(instream);
        HfstTransducer* extensions = new HfstTransducer(hfst::internal_identity,
                                  hfst::internal_identity, trans.get_type());
        char* inputname = strdup(trans.get_name().c_str());
        if (strlen(inputname) <= 0)
          {
            inputname = strdup(inputfilename);
          }
        if (transducer_n == 1)
          {
            verbose_printf("performing extensions to %s...\n", inputname);
          }
        else
          {
            verbose_printf("performing extensions to %s... %zu\n", inputname,
                           transducer_n);
          }
        if (only_from_label != 0)
          {
            verbose_printf("using single commandline extension %s with %s\n",
                           only_from_label, only_to_label);
            add_extension(extensions, only_from_label, only_to_label);
          }
        else if (tsv_file)
          {
            char* line = NULL;
            size_t len = 0;
            size_t line_n = 0;
            verbose_printf("reading extensions from %s...\n", tsv_file_name);
            while (hfst_getline(&line, &len, tsv_file) != -1)
              {
                line_n++;
                if (*line == '\n')
                  {
                    continue;
                  }
                const char* tab = strstr(line, "\t");
                if (tab == NULL)
                  {
                    if (*line == '#')
                      {
                        // a comment is a line starting with # without tabs
                        continue;
                      }
                    else
                      {
                        error_at_line(EXIT_FAILURE, 0, tsv_file_name, line_n,
                                      "At least one tab required per line");
                      }
                  }
                char* from_char = hfst_strndup(line, tab - line);
                if (strlen(from_char) == 0)
                  {
                    error_at_line(EXIT_FAILURE, 0, tsv_file_name, line_n,
                                  "First field is empty;\n"
                                  "if you REALLY want to extend epsilons as "
                                  "equivalent, use @0@ or %s",
                                  hfst::internal_epsilon.c_str());
                  }
                char* to_char = 0;
                const char* endstr = tab + 1;
                tab = strstr(endstr, "\t");
                while (tab != NULL)
                  {
                    to_char = hfst_strndup(endstr, tab - endstr);
                    if (strlen(to_char) == 0)
                      {
                        error_at_line(EXIT_FAILURE, 0, tsv_file_name, line_n,
                                      "Extension field seems empty;\n"
                                      "if you REALLY mean something is equivalent"
                                      " to epsilons, use @0@ or %s",
                                  hfst::internal_epsilon.c_str());
                      }
                    add_extension(extensions, from_char, to_char);
                    free(to_char);
                    endstr = tab + 1;
                    tab = strstr(endstr, "\t");
                  }
                tab = endstr;
                while (endstr != '\0')
                  {
                    endstr++;
                  }
                to_char = hfst_strndup(tab, endstr - tab);
                if (strlen(to_char) == 0)
                  {
                    error_at_line(EXIT_FAILURE, 0, tsv_file_name, line_n,
                                  "Extension field seems empty;\n"
                                  "if you REALLY mean something is equivalent"
                                  " to epsilons, use @0@ or %s",
                                  hfst::internal_epsilon.c_str());
                  }
                add_extension(extensions, from_char, to_char);
              } // while getline
          } // if tsv_file
        else if (acx_file)
          {
            error(EXIT_FAILURE, 0, "ACX not implemented");
          } // if acx_file
        switch (level)
          {
          case FSA_LEVEL_BOTH:
            verbose_printf("Applying extensions on second level\n");
            trans = trans.compose(*extensions);
            verbose_printf("Applying extensions on first level\n");
            trans = extensions->invert().compose(trans);
            break;
          case FSA_LEVEL_FIRST:
            verbose_printf("Applying extensions on first level\n");
            trans = extensions->invert().compose(trans);
            break;
          case FSA_LEVEL_SECOND:
            verbose_printf("Applying extensions on second level\n");
            trans = trans.compose(*extensions);
            break;
          }
        outstream << trans;
        delete extensions;
      } // for each automaton
    return EXIT_SUCCESS;
  }


int main( int argc, char **argv ) 
  {
    hfst_set_program_name(argv[0], "0.1", "HfstExpandEquivalences");
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
    process_stream(*instream, *outstream);
    if (profile_file != 0)
      {
        hfst_print_profile_line();
      }
    free(inputfilename);
    free(outfilename);  
    return EXIT_SUCCESS;
}

