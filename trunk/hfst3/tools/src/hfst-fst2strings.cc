//! @file hfst-fst2strings.cc
//!
//! @brief Transducer path printing command line tool
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

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::exceptions::NotTransducerStreamException;
using hfst::WeightedPaths;
using hfst::WeightedPath;

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

// the maximum number of strings printed for each transducer
static int max_strings = 0;
static int cycles = -1;
static int nbest_strings=-1;
static bool display_weights=false;
static bool eval_fd=false;
static bool filter_fd=false;
static unsigned int max_input_length = 0;
static unsigned int max_output_length = 0;
static std::string input_prefix;
static std::string output_prefix;
static std::string input_exclude;
static std::string output_exclude;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Display the strings recognized by a transducer\n"
        "\n", program_name);
    print_common_program_options(message_out);
#               if DEBUG
    fprintf(message_out,
        "%-35s%s", "  -d, --debug", "Print debugging messages and results\n"
        );
#               endif
    fprintf(message_out, "Input/Output options:\n");
    fprintf(message_out, "%-30s%s", "  -o, --output=OUTFILE",        "Write results to OUTFILE\n");
    fprintf(message_out, "%-30s%s", "  -i, --input=INFILE",          "Read input from INFILE\n");

    fprintf(message_out, "Tool-specific options:\n");
    fprintf(message_out, "%-30s%s", "  -n, --max-strings=INT",       "The maximum number of strings printed\n");
    fprintf(message_out, "%-30s%s", "  -N, --nbest=INT",             "Prune the transducer to a max number of best strings\n");
    fprintf(message_out, "%-30s%s", "  -c, --cycles=INT",            "How many times to follow cycles. Negative=infinite (default)\n");
    fprintf(message_out, "%-30s%s", "  -w, --print-weights",         "Display the weight for each string\n");
    fprintf(message_out, "%-30s%s", "  -e, --eval-flags",            "Only print strings with pass flag diacritic checks\n");
    fprintf(message_out, "%-30s%s", "  -f, --filter-flags",          "Don't print flag diacritic symbols (only with -e)\n");
    fprintf(message_out, " Ignore options:\n");
    fprintf(message_out, "%-30s%s", "  -l, --max-in-length=INT",     "Ignore paths with an input string longer than length\n");
    fprintf(message_out, "%-30s%s", "  -L, --max-out-length=INT",    "Ignore paths with an output string longer than length\n");
    fprintf(message_out, "%-30s%s", "  -p, --in-prefix=PREFIX",      "Ignore paths with an input string not beginning with PREFIX\n");
    fprintf(message_out, "%-30s%s", "  -P, --out-prefix=PREFIX",     "Ignore paths with an output string not beginning with PREFIX\n");
    fprintf(message_out, "%-30s%s", "  -x, --in-exclude=STR",        "Ignore paths with an input string containing STR\n");
    fprintf(message_out, "%-30s%s", "  -X, --out-exclude=STR",       "Ignore paths with an output string containing STR\n");
    
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out,
        "\n"
        "Examples:\n"
        "  %s  lexical.hfst  generates all forms of lexical.hfst\n"
        "\n", program_name);
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
        HFST_GETOPT_COMMON_LONG
          ,
        HFST_GETOPT_UNARY_LONG
          ,
          {"nbest", required_argument, 0, 'N'},
            {"max-strings", required_argument, 0, 'n'},
            {"cycles", required_argument, 0, 'c'},
            {"print-weights", no_argument, 0, 'w'},
            {"eval-flags", no_argument, 0, 'e'},
            {"filter-flags", no_argument, 0, 'f'},
            {"max-in-length", required_argument, 0, 'l'},
            {"max-out-length", required_argument, 0, 'L'},
            {"in-prefix", required_argument, 0, 'p'},
            {"out-prefix", required_argument, 0, 'P'},
            {"in-exclude", required_argument, 0, 'x'},
            {"out-exclude", required_argument, 0, 'X'},
            {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, "R:dhi:N:n:c:o:qsvVwefl:L:p:P:x:X:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        //char *level = NULL;
        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
        case 'n':
            max_strings = atoi(hfst_strdup(optarg));
            break;
        case 'N':
            nbest_strings = atoi(hfst_strdup(optarg));
            break;
        case 'c':
          cycles = atoi(hfst_strdup(optarg));
          break;
        case 'w':
            display_weights = true;
            break;
        case 'e':
          eval_fd = true;
          break;
        case 'f':
          if(!eval_fd)
          {
            fprintf(message_out, "Option -f must be used in conjunction with -e\n");
            print_short_help();
            return EXIT_FAILURE;
          }
          filter_fd = true;
          break;
        case 'l':
          max_input_length = atoi(hfst_strdup(optarg));
          break;
        case 'L':
          max_output_length = atoi(hfst_strdup(optarg));
          break;
        case 'p':
          input_prefix = optarg;
          break;
        case 'P':
          output_prefix = optarg;
          break;
        case 'x':
          input_exclude = optarg;
          break;
        case 'X':
          output_exclude = optarg;
          break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

//Print results as they come
class Callback : public hfst::ExtractStringsCb
{
 public:
  int count;
  int max_num;
  
  Callback(int max): count(0), max_num(max) {}
  RetVal operator()(WeightedPath<float>& wp, bool final)
  {
    if(max_input_length > 0 &&
       wp.istring.length() > max_input_length)
      return RetVal(true, false); // continue searching, break off this path
    if(max_output_length > 0 &&
       wp.ostring.length() > max_output_length)
      return RetVal(true, false); // continue searching, break off this path
    
    if(input_prefix.length() > 0)
    {
      if(wp.istring.length() < input_prefix.length())
        return RetVal(true, true);
      if(wp.istring.compare(0, input_prefix.length(), input_prefix) != 0)
        return RetVal(true, false); // continue searching, break off this path
    }
    if(output_prefix.length() > 0)
    {
      if(wp.ostring.length() < output_prefix.length())
        return RetVal(true, true);
      if(wp.ostring.compare(0, output_prefix.length(), output_prefix) != 0)
        return RetVal(true, false); // continue searching, break off this path
    }
    
    if(input_exclude.length() > 0 && wp.istring.find(input_exclude) != std::string::npos)
      return RetVal(true, false); // continue searching, break off this path
    if(output_exclude.length() > 0 && wp.ostring.find(output_exclude) != std::string::npos)
      return RetVal(true, false); // continue searching, break off this path
    
    // the path passed the checks. Print it if it is final
    if(final)
    {
      std::cout << wp.istring;
      if(wp.ostring != wp.istring)
        std::cout << " : " << wp.ostring;
      if(display_weights)
        std::cout << "\t" << wp.weight;
      std::cout << std::endl;
      
      count++;
    }
    return RetVal((max_num < 1) || (count < max_num), true); // continue until we've printed max_num strings
  }
};

int
process_stream(HfstInputStream& instream, std::ostream& outstream)
{
  //instream.open();
  
  bool first_transducer=true;
  while(instream.is_good())
  {
    if (!first_transducer)
      outstream << "\n";
    first_transducer=false;
    
    HfstTransducer t(instream);
    
    if(input_prefix != "")
      verbose_printf("input_prefix: '%s'\n", input_prefix.c_str());
    
    if(nbest_strings > 0)
    {
      verbose_printf("Pruning transducer to %i best path(s)...\n", nbest_strings);
      t.n_best(nbest_strings);
    }
    else
    {
      if(max_strings <= 0 && max_input_length <= 0 && max_output_length <= 0 && cycles < 0 && t.is_cyclic())
      {
        fprintf(stderr, "Transducer is cyclic. Use one or more of these options: -n, -N, -l, -L, -c\n");
        return EXIT_FAILURE;
      }
    }
    
    if(max_strings > 0)
      verbose_printf("Finding at most %i path(s)...\n", max_strings);
    else
      verbose_printf("Finding strings...\n");
    
    Callback cb(max_strings);
    if(eval_fd)
      t.extract_strings_fd(cb, cycles, filter_fd);
    else
      t.extract_strings(cb, cycles);
    
    verbose_printf("Printed %i string(s)", cb.count);
  }
    
  instream.close();
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
  hfst_set_program_name(argv[0], "0.1", "HfstFst2Strings");
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
    } catch(NotTransducerStreamException)   {
        fprintf(stderr, "%s is not a valid transducer file\n", inputfilename);
        return EXIT_FAILURE;
    }
    
    if (outfile == stdout)
    {
      std::ofstream outstream(outfilename);
      retval = process_stream(*instream, outstream);
    }
    else
      retval = process_stream(*instream, std::cout);
    
    delete instream;
    free(inputfilename);
    free(outfilename);
    return retval;
}

