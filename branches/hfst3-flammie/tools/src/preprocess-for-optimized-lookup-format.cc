//! @file hfst-remove-epsilons.cc
//!
//! @brief Transducer epsilon removal tool
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

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;


// add tools-specific variables here

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Remove epsilons from a transducer\n"
        "\n", program_name);

    print_common_program_options();
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
}


void
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
make_foobars()
{
    size_t transducer_n=0;
    while (instream->is_good())
    {
        transducer_n++;
        HfstTransducer trans(*instream);
        const char* inputname = hfst_get_name(trans, inputfilename);
        hfst_begin_processing(inputname, transducer_n, "Fubaring");
        trans.remove_epsilons();
        hfst_processing("Rebuilding and fixing");
        HfstBasicTransducer original(trans);
        HfstBasicTransducer replication;
        HfstState state_count = 1;
        std::map<HfstState,HfstState> rebuilt;
        rebuilt[0] = 0;
        HfstState source_state=0;
        for (HfstBasicTransducer::const_iterator state = original.begin();
             state != original.end();
             ++state)
          {
            if (rebuilt.find(source_state) == rebuilt.end())
              {
                replication.add_state(state_count);
                if (original.is_final_state(source_state))
                  {
                    replication.set_final_weight(state_count,
                                                 original.get_final_weight(source_state));
                  }
                rebuilt[source_state] = state_count;
                state_count++;
              }
            for (HfstBasicTransducer::HfstTransitions::const_iterator arc =
                 state->begin();
                 arc != state->end();
                 ++arc)
              {
                if (rebuilt.find(arc->get_target_state()) == rebuilt.end())
                  {
                    replication.add_state(state_count);
                    if (original.is_final_state(arc->get_target_state()))
                      {
                        replication.set_final_weight(state_count,
                                                     original.get_final_weight(arc->get_target_state()));
                      }
                    rebuilt[arc->get_target_state()] = state_count;
                    state_count++;
                  }
                HfstBasicTransition nu(rebuilt[arc->get_target_state()],
                                       arc->get_input_symbol(),
                                       arc->get_output_symbol(),
                                       arc->get_weight());
                replication.add_transition(rebuilt[source_state], nu);
              }
            source_state++;
          }
        trans = HfstTransducer(replication, trans.get_type());
        hfst_set_name(trans, trans, "fu");
        hfst_set_formula(trans, trans, "FU");
        *outstream << (trans.remove_epsilons());
      }
  }


int main(int argc, char **argv) {
    hfst_init_commandline(argv[0], "0.1", 
                          "HfstPreprocessForOptimizedLookupFormat",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_foobars();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
}

