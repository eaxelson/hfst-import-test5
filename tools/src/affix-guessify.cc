//! @file hfst-affix-guessify.cc
//!
//! @brief Transducer guesser maker
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
using std::set;

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::implementations::HfstTransitionGraph;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;
using hfst::implementations::HfstState;
using hfst::StringSet;

#include "conventions/commandline.h"

// add tools-specific variables here
enum guess_direction {
    GUESS_PREFIX,
    GUESS_SUFFIX
};

static guess_direction direction = GUESS_SUFFIX;
static float weight = 1.0f;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Create weighted affix guesser from automaton\n"
        "\n", program_name);
    print_common_program_options();
    fprintf(message_out, "Guesser parameters:\n"
            "  -D, --direction=DIR   set direction of guessing\n"
            "  -w, --weight=WEIGHT   set weight difference of affix lengths\n"
            "\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "DIR is either suffix or prefix, "
            "or suffix if omitted.\n"
            "WEIGHT is a weight of each arc not in the known suffix or "
            "prefix being guessed, as parsed with strtod(3), "
            "or 1.0 if omitted.\n");
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
            {"weight", required_argument, 0, 'w'},
            {"direction", required_argument, 0, 'D'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT
                             "w:D:",
                             long_options, &option_index);
        if (-1 == c)
          {
            break;
          }
        else if (parse_common_getopt_value(c))
          {
            continue;
          }
        switch (c)
          {
          case 'w':
            weight = hfst_strtoweight(optarg);
            break;
          case 'D':
            if (strncmp(optarg, "prefix", strlen("prefix")) == 0)
              {
                direction = GUESS_PREFIX;
              }
            else if (strncmp(optarg, "suffix", strlen("suffix")) == 0)
              {
                direction = GUESS_SUFFIX;
              }
            else
              {
                hfst_error(EXIT_FAILURE, 0, "Unable to parse guessing "
                           "direction from %s;\n"
                      "please use one of `prefix' or `suffix'",
                      optarg);
              }
            break;
          default:
            parse_getopt_error_value(c);
            break;
          } //
      } // while true getopt loop
  }


HfstTransducer*
make_suffix_guesser(HfstTransducer* trans)
  {
    HfstTransducer* t = 0;
    set<HfstState> replayed;
    HfstBasicTransducer* mutt = new HfstBasicTransducer(*trans);
    HfstBasicTransducer* repl = new HfstBasicTransducer();
    hfst_processing("Creating guesser prefix");
    HfstState guess_state = repl->add_state(0);
    HfstBasicTransition guess_arc(guess_state,
                                  hfst::internal_identity,
                                  hfst::internal_identity,
                                  weight);
    HfstBasicTransducer::const_iterator i = mutt->begin();
    repl->add_transition(guess_state, guess_arc);
    hfst_done();
    hfst_processing("Rebuilding suffix");
    for (HfstState s = 0; s <= mutt->get_max_state(); s++)
      {
        HfstState d = repl->add_state(s + 1);
        if (mutt->is_final_state(s))
          {
            repl->set_final_weight(d,
                                   mutt->get_final_weight(s));
          }
        HfstBasicTransition guess_arc(d,
                                      hfst::internal_identity,
                                      hfst::internal_identity,
                                      weight);
        repl->add_transition(guess_state, guess_arc);
        for (HfstBasicTransducer::HfstTransitions::const_iterator arc =
             i->begin();
             arc != i->end();
             ++arc)
          {
            HfstBasicTransition newarc(arc->get_target_state() + 1,
                                       arc->get_input_symbol(),
                                       arc->get_output_symbol(),
                                       arc->get_weight());
            repl->add_transition(d, newarc);
          }
        ++i;
      } // for each state
    hfst_done();
    hfst_processing("converting");
    t = new HfstTransducer(*repl, format);
    hfst_done();
    return t;
  }

HfstTransducer*
make_prefix_guesser(HfstTransducer* trans)
  {
    HfstTransducer* t = 0;
    set<HfstState> replayed;
    HfstBasicTransducer::const_iterator i;
    hfst_processing("Creating guesser suffix");
    HfstBasicTransducer* repl = new HfstBasicTransducer(*trans);
    HfstState guess_state = repl->add_state();
    repl->set_final_weight(guess_state, 0);
    HfstBasicTransition guess_arc(guess_state,
                                hfst::internal_identity,
                                hfst::internal_identity,
                                weight);
    repl->add_transition(guess_state, guess_arc);
    hfst_done();
    hfst_processing("Linking prefix");
    HfstState max_state = repl->get_max_state();
    for (HfstState s = 0; s <= max_state; s++)
    {
      HfstBasicTransition newarc(guess_state,
                                 hfst::internal_identity,
                                 hfst::internal_identity,
                                 weight);
      repl->add_transition(s, newarc);
    }
    hfst_processing("converting");
    t = new HfstTransducer(*repl, format);
    hfst_done();
    return t;
  }

void
make_affix_guesser()
{
  size_t transducer_n = 0;
  while (instream->is_good())
    {
      HfstTransducer *trans = new HfstTransducer(*instream);
      HfstTransducer* res = 0;
      const char* inputname = hfst_get_name(*trans, inputfilename);
      hfst_begin_processing(inputname, transducer_n, "Guessifying affix");
      switch (direction)
        {
        case GUESS_SUFFIX:
          res = make_suffix_guesser(trans);
          hfst_set_name(*res, inputname, "longest-suffix");
          break;
        case GUESS_PREFIX:
          res = make_prefix_guesser(trans);
          hfst_set_name(*res, inputname, "longest-prefix");
          break;
        } // which direction
      hfst_writing(outfilename);
      *outstream << *res;
      hfst_done();
    
    } // good instream
  hfst_end_processing();
}


int main(int argc, char **argv) {
    hfst_init_commandline(argv[0], "0.1", "HfstAffixGuessify",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_affix_guesser();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
}

