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

#include <hfst.hpp>

using std::map;
using std::string;

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::implementations::HfstTransitionGraph;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstState;
using hfst::StringSet;

#include "conventions/commandline.h"

// add tools-specific variables here

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Calculate the properties of a transducer\n"
        "\n", program_name);

    print_common_program_options();
    // fprintf(message_out, (tool-specific options and short descriptions)
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "\n");
    print_report_bugs();
    print_more_info();
  }

void
parse_options(int argc, char** argv)
  {
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
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        else
          {
            parse_getopt_error_value(c);
          }
      }
  }

void
make_summaries()
  {
    size_t transducer_n = 0;
    while (instream->is_good())
      {
        transducer_n++;
        if (transducer_n < 2)
          {
            verbose_printf("Summarizing...\n");
          }
        else
          {
            verbose_printf("Summarizing... %zu\n", transducer_n);
          }
        HfstTransducer *trans = new HfstTransducer(*instream);
        HfstBasicTransducer *mutt = new HfstBasicTransducer(*trans);
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
        StringSet transducerAlphabet;
        bool transducerKnowsAlphabet = false;
        try
          {
            transducerAlphabet = trans->get_alphabet();
            transducerKnowsAlphabet = true;
          }
        catch (FunctionNotImplementedException)
          {
            transducerKnowsAlphabet = false;
          }
        StringSet foundAlphabet;
        //bool expanded = true;
        bool is_mutable = true;
        bool acceptor = true;
        bool input_deterministic = true;
        bool output_deterministic = true;
        //bool input_label_sorted = false;
        //bool output_label_sorted = false;
        bool weighted = true;
        bool cyclic = false;
        bool cyclic_at_initial_state = false;
        //bool topologically_sorted = false;
        //bool accessible = true;
        //bool coaccessible = true;
        //bool is_string = true;
        //bool minimised = false;
        // assign data from knowledge about source type
        switch (trans->get_type())
          {
          case hfst::SFST_TYPE:
            is_mutable = true;
            weighted = false;
            break;
          case hfst::TROPICAL_OPENFST_TYPE:
            is_mutable = true;
            weighted = true;
            break;
          case hfst::LOG_OPENFST_TYPE:
            is_mutable = true;
            weighted = true;
            break;
          case hfst::FOMA_TYPE:
            is_mutable = true;
            weighted = false;
            break;
          case hfst::HFST_OL_TYPE:
            is_mutable = false;
            weighted = false;
            break;
          case hfst::HFST_OLW_TYPE:
            is_mutable = false;
            weighted = true;
            break;
          default:
            is_mutable = false;
            break;
          }
        // iterate states in random order
        HfstState source_state=0;
        for (HfstBasicTransducer::const_iterator it = mutt->begin();
             it != mutt->end();
             it++)
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
                    input_deterministic = false;
                    output_deterministic = false;
                  }
                else if (hfst::is_epsilon(tr_it->get_input_symbol()))
                  {
                    input_epsilons++;
                    input_deterministic = false;
                  }
                else if (hfst::is_epsilon(tr_it->get_output_symbol()))
                  {
                    output_epsilons++;
                    output_deterministic = false;
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
                if (input_ambiguity[tr_it->get_input_symbol()] > 1)
                  {
                    input_deterministic = false;
                  }
                output_ambiguity[tr_it->get_output_symbol()]++;
                if (output_ambiguity[tr_it->get_output_symbol()] > 1)
                  {
                    output_deterministic = false;
                  }
                if (it == mutt->begin() && (tr_it->get_target_state() == 0))
                  {
                    cyclic = true;
                    cyclic_at_initial_state = true;
                  }
                if (source_state == tr_it->get_target_state())
                  {
                    cyclic = true;
                  }
                  
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
        double expected_arcs_per_symbol = 
            static_cast<double>(average_arcs_per_state)/
            static_cast<float>(foundAlphabet.size());
  
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
                "mutable: %s\n"
                "acceptor: %s\n"
                "input deterministic: %s\n"
                "output deterministic: %s\n"
                "input label sorted: ???\n"
                "output label sorted: ???\n"
                "weighted: %s\n"
                "cyclic: %s\n"
                "cyclic at initial state: %s\n"
                "topologically sorted: ???\n"
                "accessible: ???\n"
                "coaccessible: ???\n"
                "string: ???\n"
                "minimised: ???\n",
                is_mutable? "yes": "no",
                acceptor? "yes": "no",
                input_deterministic? "yes": "no",
                output_deterministic? "yes": "no",
                weighted? "yes": "no",
                cyclic? "yes": "no",
                cyclic_at_initial_state? "yes": "no");
        // all above here are for OpenFst compatibility ^^
        if (verbose)
          {
            // our extensions for nice statistics maybe
            fprintf(outfile,
                    "number of arcs in sparsest state: %zu\n"
                    "number of arcs in densest state: %zu\n"
                    "average arcs per state: %f\n"
                    "average input epsilons per state: %f\n"
                    "most ambiguous input: %s %u\n"
                    "most ambiguous output: %s %u\n"
                    "average input ambiguity: %f\n"
                    "average output ambiguity: %f\n"
                    "expected arcs per symbol: %f\n",
                    sparsest_arcs, densest_arcs,
                    average_arcs_per_state,
                    average_input_epsilons,
                    most_ambiguous_input.first.c_str(), most_ambiguous_input.second,
                    most_ambiguous_output.first.c_str(), most_ambiguous_output.second,
                    average_input_ambiguity, average_output_ambiguity,
                    expected_arcs_per_symbol
                );
            // alphabets
            fprintf(outfile,
                    "sigma set:\n");
            if (transducerKnowsAlphabet)
              {
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
                fprintf(outfile, "sigma set size: %zu\n",
                        transducerAlphabet.size());
              }
            else
              {
                fprintf(outfile, "<Unknown in used transducer format>\n");
              }
            fprintf(outfile, "arc symbols actually seen in transducer:\n");
            bool first = true;
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
            fprintf(outfile, "sigma set size: %zu\n", foundAlphabet.size());
            fprintf(outfile, "sigma symbols missing from transducer:\n");
            if (transducerKnowsAlphabet)
              {
                StringSet transducerMinusSet;
                std::set_difference(transducerAlphabet.begin(), 
                                    transducerAlphabet.end(),
                                    foundAlphabet.begin(), 
                                    foundAlphabet.end(),
                                    std::inserter(transducerMinusSet, 
                                                  transducerMinusSet.end()));

                first = true;
                for (StringSet::const_iterator s = transducerMinusSet.begin();
                     s != transducerMinusSet.end();
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
                fprintf(outfile, "sigma missing from transducer alphabet size:"
                        " %zu\n",
                        transducerMinusSet.size());
              }
            else
              {
                fprintf(outfile, "<Unknown in used transducer format>\n");
              }
          }
      }
    fprintf(outfile, "\n# of automata in archive: %zu\n", transducer_n);
  }


int main(int argc, char **argv) {
    hfst_init_commandline(argv[0], "0.1", "HfstSummarize",
                          AUTOM_IN_FILE_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_summaries();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

