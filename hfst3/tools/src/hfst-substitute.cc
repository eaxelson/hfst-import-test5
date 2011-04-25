//! @file hfst-substitute.cc
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

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "implementations/HfstTransitionGraph.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;
using hfst::StringPair;

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

static char* from_label = 0;
static StringPair* from_pair = 0;
static char* from_file_name = 0;
static FILE* from_file = 0;
static char* to_label = 0;
static StringPair* to_pair = 0;
static char* to_transducer_filename = 0;
static HfstTransducer* to_transducer = 0;

/**
 * @brief parse string pair from arc label.
 *
 * @return new stringpair, or null if not a pair.
 */
static
StringPair*
label_to_stringpair(const char* label)
  {
    const char* colon = strchr(label, ':');
    const char* endstr = strchr(label, '\0');
    while (colon != NULL)
      {
        if (colon == label)
          {
            colon = strchr(colon + 1, ':');
          }
        else if (colon == (endstr - 1))
          {
            colon = 0;
          }
        else if (*(colon - 1) == '\\')
          {
            if (colon > (label + 1))
              {
                if (*(colon - 2) == '\\')
                  {
                    break;
                  }
                else
                  {
                    colon = strchr(colon + 1, ':');
                  }
              }
          }
        else
          {
            break;
          }
      }
    char* first = 0;
    char* second = 0;
    if ((label < colon) && (colon < endstr))
      {
        first = hfst_strndup(label, colon-label);
        second = hfst_strndup(colon + 1, endstr - colon - 1);
      }
    else
      {
        return NULL;
      }
    if (strcmp(first, "@0@") == 0)
      {
        free(first);
        first = hfst_strdup(hfst::internal_epsilon.c_str());
      }
    if (strcmp(second, "@0@") == 0)
      {
        free(second);
        second = hfst_strdup(hfst::internal_epsilon.c_str());
      }

    StringPair* rv = new StringPair(first, second);
    free(first);
    free(second);
    return rv;
  }

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Relabel transducer arcs\n"
        "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    fprintf(message_out, "Relabeling options:\n"
            "  -f, --from-label=FLABEL      replace FLABEL\n"
            "  -t, --to-label=TLABEL        replace with TLABEL\n"
            "  -T, --to-transducer=TFILE    replace with transducer "
            "read from TFILE\n"
            "  -F, --from-file=LABELFILE    read replacements from LABELFILE\n"
           );
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out, "LABEL must be a symbol name in single arc in "
            "transducer,"
            " or colon separated pair defining an arc\n"
            "if TFILE is specified, FLABEL must be a pair\n"
            "LABELFILE is a 2 column tsv file where col 1 is FLABEL and"
            "col 2 TLABEL specifications");
    fprintf(message_out,
           "\n"
           "Examples:\n"
           "  %s -o cg.hfst -F omor2cg.relabel omor.hfst  transform omor tags "
           "cg \n"
           "\n", program_name);
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
        HFST_GETOPT_UNARY_LONG,
          // add tool-specific options here 
            {"from-label", required_argument, 0, 'f'},
            {"from-file", required_argument, 0, 'F'},
            {"to-label", required_argument, 0, 't'},
            {"to-transducer", required_argument, 0, 'T'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "f:F:t:T:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        FILE* f = 0;
        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
          // add tool-specific cases here
        case 'f':
            from_label = hfst_strdup(optarg);
            from_pair = label_to_stringpair(from_label);
            break;
        case 'F':
            from_file_name = hfst_strdup(optarg);
            from_file = hfst_fopen(from_file_name, "r");
            if (from_file == NULL)
            {
                return EXIT_FAILURE;
            }
            break;
        case 't':
            to_label = hfst_strdup(optarg);
            to_pair = label_to_stringpair(to_label);
            break;
        case 'T':
            to_transducer_filename = hfst_strdup(optarg);
            f = hfst_fopen(to_transducer_filename, "r");
            if (f == NULL)
            {
                return EXIT_FAILURE;
            }
            fclose(f);
            break;
#include "inc/getopt-cases-error.h"
        }
    }
    
    if ((from_label == 0) && (from_file_name == 0))
    {
        error(EXIT_FAILURE, 0,
              "Must state name of labels to rewrite with -f or -F");
        return EXIT_FAILURE;
    }
    if ((to_label == 0) && (to_transducer_filename == 0) && 
            (from_file_name == 0))
    {
        error(EXIT_FAILURE, 0,
              "Must give target labels with -t, -T or -F");
        return EXIT_FAILURE;
    }
#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

static
HfstBasicTransducer&
do_substitute(HfstBasicTransducer& trans, size_t transducer_n)
{
  if (from_pair && to_pair)
    {
      if (transducer_n < 2)
        {
          verbose_printf("Substituting pair %s:%s with pair %s:%s...\n",
                         from_pair->first.c_str(),
                         from_pair->second.c_str(),
                         to_pair->first.c_str(),
                         to_pair->second.c_str());
        }
      else
        {
          verbose_printf("Substituting pair %s:%s with pair %s:%s...\n",
                         from_pair->first.c_str(),
                         from_pair->second.c_str(),
                         to_pair->first.c_str(),
                         to_pair->second.c_str());
        }
      trans.substitute(*from_pair, *to_pair);
    }
  else if (from_label && to_label)
    {
      if (transducer_n < 2)
        {
          verbose_printf("Substituting label %s with label %s...\n", from_label,
                         to_label);
        }
      else
        {
          verbose_printf("Substituting label %s with label %s... %zu\n",
                         from_label, to_label, transducer_n);
        }
      trans.substitute(from_label, to_label);
    }
  else if (from_pair && to_transducer)
    {
      char* to_name = strdup(to_transducer->get_name().c_str());
      if (strlen(to_name) <= 0)
        {
          to_name = strdup(to_transducer_filename);
        }
      if (transducer_n < 2)
        {
          verbose_printf("Substituting pair %s:%s with transducer %s...\n", 
                         from_pair->first.c_str(),
                         from_pair->second.c_str(),
                         to_name);
        }
      else
        {
          verbose_printf("Substituting pair %s:%s with transducer %s... %zu\n", 
                         from_pair->first.c_str(), 
                         from_pair->second.c_str(), to_name,
                         transducer_n);
        }
      trans.substitute(*from_pair, *to_transducer);
    }

  else if (from_label && to_transducer)
    {
      char* to_name = strdup(to_transducer->get_name().c_str());
      if (strlen(to_name) <= 0)
        {
          to_name = strdup(to_transducer_filename);
        }
      if (transducer_n < 2)
        {
          verbose_printf("Substituting id. label %s with transducer %s...\n", 
                         from_label, to_name);
        }
      else
        {
          verbose_printf("Substituting id. label %s with transducer %s... %zu\n", 
                         from_label, to_name,
                         transducer_n);
        }
      hfst::StringPair from_arc(from_label, from_label);
      trans.substitute(from_arc, *to_transducer);
    }
  return trans;
}

static
HfstTransducer&
do_substitute(HfstTransducer& trans, size_t transducer_n)
{
  if (from_pair && to_pair)
    {
      if (transducer_n < 2)
        {
          verbose_printf("Substituting pair %s:%s with pair %s:%s...\n",
                         from_pair->first.c_str(),
                         from_pair->second.c_str(),
                         to_pair->first.c_str(),
                         to_pair->second.c_str());
        }
      else
        {
          verbose_printf("Substituting pair %s:%s with pair %s:%s...\n",
                         from_pair->first.c_str(),
                         from_pair->second.c_str(),
                         to_pair->first.c_str(),
                         to_pair->second.c_str());
        }
      trans.substitute(*from_pair, *to_pair);
    }
  else if (from_label && to_label)
    {
      if (transducer_n < 2)
        {
          verbose_printf("Substituting label %s with label %s...\n", from_label,
                         to_label);
        }
      else
        {
          verbose_printf("Substituting label %s with label %s... %zu\n",
                         from_label, to_label, transducer_n);
        }
      trans.substitute(from_label, to_label);
    }
  else if (from_pair && to_transducer)
    {
      char* to_name = strdup(to_transducer->get_name().c_str());
      if (strlen(to_name) <= 0)
        {
          to_name = strdup(to_transducer_filename);
        }
      if (transducer_n < 2)
        {
          verbose_printf("Substituting pair %s:%s with transducer %s...\n", 
                         from_pair->first.c_str(),
                         from_pair->second.c_str(),
                         to_name);
        }
      else
        {
          verbose_printf("Substituting pair %s:%s with transducer %s... %zu\n", 
                         from_pair->first.c_str(), 
                         from_pair->second.c_str(), to_name,
                         transducer_n);
        }
      trans.substitute(*from_pair, *to_transducer);
    }

  else if (from_label && to_transducer)
    {
      char* to_name = strdup(to_transducer->get_name().c_str());
      if (strlen(to_name) <= 0)
        {
          to_name = strdup(to_transducer_filename);
        }
      if (transducer_n < 2)
        {
          verbose_printf("Substituting id. label %s with transducer %s...\n", 
                         from_label, to_name);
        }
      else
        {
          verbose_printf("Substituting id. label %s with transducer %s... %zu\n", 
                         from_label, to_name,
                         transducer_n);
        }
      hfst::StringPair from_arc(from_label, from_label);
      trans.substitute(from_arc, *to_transducer);
    }
  return trans;
}

int
process_stream(HfstInputStream& instream, HfstOutputStream& outstream)
{
  //instream.open();
  //outstream.open();
  size_t transducer_n = 0;
  HfstTransducer* to_transducer = NULL;
  if (to_transducer_filename)
    {
      try {
        HfstInputStream tostream(to_transducer_filename);
        to_transducer = new HfstTransducer(tostream);
      } //catch (NotTransducerStreamException ntse)  
      catch (HfstException ntse)
        {
          error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
                to_transducer_filename);
          return EXIT_FAILURE;
        }
    }
  HfstBasicTransducer* fallback = 0;
  bool warnedAlready = false;
  bool fellback = false;
  while (instream.is_good())
    {
      transducer_n++;
      HfstTransducer trans(instream);
      char* inputname = strdup(trans.get_name().c_str());
      if (strlen(inputname) <= 0)
        {
          inputname = strdup(inputfilename);
        }
      if (transducer_n == 1)
        {
          verbose_printf("performing substitutions in %s...\n", inputname);
        }
      else
        {
          verbose_printf("performing substitutions in %s... %zu\n", inputname,
                         transducer_n);
        }
      if (from_file)
        {
          char* line = NULL;
          size_t len = 0;
          size_t line_n = 0;
          verbose_printf("reading substitutions from %s...\n", from_file_name);
          while (hfst_getline(&line, &len, from_file) != -1)
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
                      continue;
                    }
                  else
                    {
                      error_at_line(EXIT_FAILURE, 0, from_file_name, line_n,
                                    "At least one tab required per line");
                    }
                }
              const char* endstr = tab+1;
              while ((*endstr != '\0') && (*endstr != '\n'))
                {
                  endstr++;
                }
              from_label = hfst_strndup(line, tab-line);
              to_label = hfst_strndup(tab+1, endstr-tab-1);
              from_pair = label_to_stringpair(from_label);
              to_pair = label_to_stringpair(to_label);
              try 
                {
                  do_substitute(trans, transducer_n);
                }
              catch (FunctionNotImplementedException fnse)
                {
                  if (!warnedAlready)
                    {
                      warning(0, 0, "substitution is not supported for this transducer type"
                              " falling back to internal formats and trying...");
                      fallback = new HfstBasicTransducer(trans);
                    }
                  do_substitute(*fallback, transducer_n);
                  fellback = true;
                }
            }
        }
      else
        {
          try
            {
              do_substitute(trans, transducer_n);
            }
          catch (FunctionNotImplementedException fnse)
            {
              if (!warnedAlready)
                {
                  warning(0, 0, "substitution is not supported for this transducer type"
                          " falling back to internal formats and trying...");
                  fallback = new HfstBasicTransducer(trans);
                }
              do_substitute(*fallback, transducer_n);
              fellback = true;
            }
        }
      if (fellback)
        {
          trans = HfstTransducer(*fallback, trans.get_type());
        }
      if (from_file)
        {
            char* composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                         (strlen(inputname) +
                                          strlen(from_file_name) +
                                          strlen("hfst-substitute=(%s, @%s)")) 
                                         + 1));
            if (sprintf(composed_name, "hfst-substitute=(%s, @%s))",
                        inputname, from_file_name) > 0)
              {
                trans.set_name(composed_name);
              }
        }
      else if (from_label && to_label)
        {
            char* composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                         (strlen(inputname) +
                                          strlen(from_label) +
                                          strlen(to_label) +
                                          strlen("hfst-substitute=(%s, %s->%s)")) 
                                         + 1));
            if (sprintf(composed_name, "hfst-substitute=(%s, %s->%s))",
                        inputname, from_label, to_label) > 0)
              {
                trans.set_name(composed_name);
              }

        }
      else if (to_transducer_filename)
        {
            char* composed_name = static_cast<char*>(malloc(sizeof(char) * 
                                         (strlen(inputname) +
                                          strlen(from_label) +
                                          strlen(to_transducer_filename) +
                                          strlen("hfst-substitute=(%s, %s->(%s))")) 
                                         + 1));
            if (sprintf(composed_name, "hfst-substitute=(%s, %s->(%s))",
                        inputname, from_label, to_transducer_filename) > 0)
              {
                trans.set_name(composed_name);
              }


        }
      fallback = new HfstBasicTransducer(trans);
      fallback->prune_alphabet();
      trans = HfstTransducer(*fallback, trans.get_type());
      outstream << trans;
    }
  delete to_transducer;
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) 
{
  hfst_set_program_name(argv[0], "0.1", "HfstSubstitute");
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
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}

