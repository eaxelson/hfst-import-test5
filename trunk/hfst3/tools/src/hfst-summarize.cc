//! @file hfst-summarize.cc
//!
//! @brief Transducer information command line tool
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

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "implementations/HfstTransitionGraph.h"

using std::map;
using std::string;

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::implementations::HfstTransitionGraph;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstState;
using hfst::StringSet;

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

// add tools-specific variables here

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Calculate the properties of a transducer\n"
        "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    // fprintf(message_out, (tool-specific options and short descriptions)
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
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
          // add tool-specific options here 
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT,
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
          // add tool-specific cases here
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

int
process_stream(HfstInputStream& instream)
{
  size_t transducer_n = 0;
  while (instream.is_good())
    {
      if (transducer_n < 2)
        {
          verbose_printf("Summarizing...\n");
        }
      else
        {
          verbose_printf("Summarizing... %zu\n", transducer_n);
        }
      HfstTransducer *trans;
      try { 
    trans = new HfstTransducer(instream);
      } catch (const HfstException e) {
    fprintf(stderr,"An error happened when reading transducer from stream.\n");
    exit(1);
      }
      HfstBasicTransducer *mutt;
      try {
    mutt = new HfstBasicTransducer(*trans);
      } catch (const HfstException e) {
    fprintf(stderr,"An error happened when converting transducer to internal format.\n");
    exit(1);
      }
      size_t states = 0;
      size_t final_states = 0;
      //size_t paths = 0;
      size_t arcs = 0;
      //size_t sccs = 0;
      size_t io_epsilons = 0;
      size_t input_epsilons = 0;
      size_t output_epsilons = 0;
      // others
      size_t densest_arcs = 0;
      size_t sparsest_arcs = 1<<31;
      size_t uniq_input_arcs = 0;
      size_t uniq_output_arcs = 0;
      std::pair<string,unsigned int> most_ambiguous_input;
      std::pair<string,unsigned int> most_ambiguous_output;
      unsigned int initial_state = 0; // mutt.get_initial_state();
      StringSet transducerAlphabet = trans->get_alphabet();
      StringSet foundAlphabet;
      bool acceptor = true;
      // iterate states in random order
      HfstState source_state=0;
      for (HfstBasicTransducer::const_iterator it = mutt->begin();
       it != mutt->end(); it++)
    {
          HfstState s = source_state;
          ++states;
          if (mutt->is_final_state(s))
            {
              ++final_states;
            }
          size_t arcs_here = 0;
          map<string,unsigned int> input_ambiguity;
          map<string,unsigned int> output_ambiguity;

      for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
           = it->begin();
           tr_it != it->end(); tr_it++)
        {
              arcs++;
              arcs_here++;
              foundAlphabet.insert(tr_it->get_input_symbol());
              foundAlphabet.insert(tr_it->get_output_symbol());
              if (tr_it->get_input_symbol() != tr_it->get_output_symbol())
                {
                  acceptor = false;
                }
              if ( hfst::is_epsilon(tr_it->get_input_symbol()) && 
                   hfst::is_epsilon(tr_it->get_output_symbol()) )
                {
                  io_epsilons++;
                  input_epsilons++;
                  output_epsilons++;
                }
              else if (hfst::is_epsilon(tr_it->get_input_symbol()))
                {
                  input_epsilons++;
                }
              else if (hfst::is_epsilon(tr_it->get_output_symbol()))
                {
                  output_epsilons++;
                }
              if (input_ambiguity.find(tr_it->get_input_symbol()) == input_ambiguity.end())
                {
                  input_ambiguity[tr_it->get_input_symbol()] = 0;
                }
              if (output_ambiguity.find(tr_it->get_output_symbol()) == output_ambiguity.end())
                {
                  output_ambiguity[tr_it->get_output_symbol()] = 0;
                }
              input_ambiguity[tr_it->get_input_symbol()]++;
              output_ambiguity[tr_it->get_output_symbol()]++;
            }
          if (arcs_here > densest_arcs)
            {
              densest_arcs = arcs_here;
            }
          if (arcs_here < sparsest_arcs)
            {
              sparsest_arcs = arcs_here;
            }
          for (map<string, unsigned int>::iterator ambit = input_ambiguity.begin();
               ambit != input_ambiguity.end();
               ++ambit)
            {
              if (ambit->second > most_ambiguous_input.second)
                {
                  most_ambiguous_input.first = ambit->first;
                  most_ambiguous_input.second = ambit->second;
                }
              uniq_input_arcs++;
            }
          for (map<string, unsigned int>::iterator ambit = output_ambiguity.begin();
               ambit != output_ambiguity.end();
               ++ambit)
            {
              if (ambit->second > most_ambiguous_output.second)
                {
                  most_ambiguous_output.first = ambit->first;
                  most_ambiguous_output.second = ambit->second;
                }
              uniq_output_arcs++;
            }
      source_state++;
      }
      delete mutt;
      // traverse
      
      // count physical size
      
      // average calculations
      double average_arcs_per_state = 
    static_cast<double>(arcs)/static_cast<float>(states);
      double average_input_epsilons = 
    static_cast<double>(input_epsilons)/static_cast<double>(states);
      double average_input_ambiguity = 
    static_cast<double>(arcs)/static_cast<double>(uniq_input_arcs);
      double average_output_ambiguity = 
    static_cast<double>(arcs)/static_cast<double>(uniq_output_arcs);
      if (transducer_n > 1)
        {
          fprintf(outfile, "-- \nTransducer #%zu:\n", transducer_n);
        }
      fprintf(outfile, "name: \"%s\"\n", trans->get_name().c_str());
      // next is printed as in OpenFST's fstinfo
      // do not modify for compatibility
      switch (trans->get_type())
        {
        case hfst::SFST_TYPE:
          fprintf(outfile, "fst type: SFST\n"
                  "arc type: SFST\n");
          break;
        case hfst::TROPICAL_OPENFST_TYPE:
          fprintf(outfile, "fst type: OpenFST\n"
                  "arc type: tropical\n");
          break;
        case hfst::LOG_OPENFST_TYPE:
          fprintf(outfile, "fst type: OpenFST\n"
                  "arc type: log\n");
          break;
        case hfst::FOMA_TYPE:
          fprintf(outfile, "fst type: foma\n"
                  "arc type: foma\n");
          break;
        case hfst::HFST_OL_TYPE:
          fprintf(outfile, "fst type: HFST optimized lookup\n"
                  "arc type: unweighted\n");
          break;
        case hfst::HFST_OLW_TYPE:
          fprintf(outfile, "fst type: HFST optimized lookup\n"
                  "arc type: weighted\n");
          break;
        default:
          fprintf(outfile, "fst type: ???\n"
                  "arc type: ???\n");
          break;
        }
      delete trans;
      fprintf(outfile, "input symbol table: yes\n"
              "output symbol table: yes\n"
              "# of states: %zu\n"
              "# of arcs: %zu\n"
              "initial state: %ld\n"
              "# of final states: %zu\n"
              "# of input/output epsilons: %zu\n"
              "# of input epsilons: %zu\n"
              "# of output epsilons: %zu\n"
              "# of ... accessible states: ???\n"
              "# of ... coaccessible states: ???\n"
              "# of ... connected states: ???\n"
              "# of ... strongly conn components: ???\n",
              states, arcs, 
              static_cast<long int>(initial_state),
              final_states, io_epsilons,
              input_epsilons, output_epsilons);
      // other names from properties...
      fprintf(outfile, "expanded: ???\n"
              "mutable: ???\n"
              "acceptor: %s\n"
              "input deterministic: ???\n"
              "output deterministic: ???\n"
              "input label sorted: ???\n"
              "output label sorted: ???\n"
              "weighted: ???\n"
              "cyclic: ???\n"
              "cyclic at initial state: ???\n"
              "topologically sorted: ???\n"
              "accessible: ???\n"
              "coaccessible: ???\n"
              "string: ???\n"
              "minimised: ???\n",
              acceptor? "yes": "no");
      // our extensions for nice statistics maybe
      fprintf(outfile,
              "number of arcs in sparsest state: %zu\n"
              "number of arcs in densest state: %zu\n"
              "average arcs per state: %f\n"
              "average input epsilons per state: %f\n"
              "most ambiguous input: %s %u\n"
              "most ambiguous output: %s %u\n"
              "average input ambiguity: %f\n"
              "average output ambiguity: %f\n",
              sparsest_arcs, densest_arcs,
              average_arcs_per_state,
              average_input_epsilons,
              most_ambiguous_input.first.c_str(), most_ambiguous_input.second,
              most_ambiguous_output.first.c_str(), most_ambiguous_output.second,
              average_input_ambiguity, average_output_ambiguity);
      // alphabets
      fprintf(outfile,
              "sigma set:\n");
      bool first = true;
      for (StringSet::const_iterator s = transducerAlphabet.begin();
           s != transducerAlphabet.end();
           ++s)
        {
          if (!first) 
            {
              fprintf(outfile, ", ");
            }
          fprintf(outfile, "%s", s->c_str());
          first = false;
        }
      fprintf(outfile, "\n");
      fprintf(outfile, "arc symbols actually seen in transducer:\n");
      first = true;
      for (StringSet::const_iterator s = foundAlphabet.begin();
           s != foundAlphabet.end();
           ++s)
        {
          if (!first) 
            {
              fprintf(outfile, ", ");
            }
          fprintf(outfile, "%s", s->c_str());
          first = false;
        }
      fprintf(outfile, "\n");
    }
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstSummarize");
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
    verbose_printf("Reading from %s, writing to %s\n", 
        inputfilename, outfilename);
    // here starts the buffer handling part
    HfstInputStream* instream = NULL;
    try {
        instream = (inputfile != stdin) ?
        new HfstInputStream(inputfilename) : new HfstInputStream();
    } // NotTransducerStreamException
    catch (const HfstException e) {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              inputfilename);
        return EXIT_FAILURE;
    }
    retval = process_stream(*instream);
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}

