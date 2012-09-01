//! @file reweight.cc
//!
//! @brief Transducer reweighting tool
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
#include <cmath>
#include <cfloat>
#include <getopt.h>

#include <hfst.hpp>

#include "conventions/commandline.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;


// id function to point to for id transformation
static
float
id(float w)
  {
    return w;
  }
float addition = 0;
float multiplier = 1;
char* funcname = 0;
float (*func)(float) = id;
float upper_bound = FLT_MAX;
float lower_bound = 0;
char* input_symbol = 0;
char* output_symbol = 0;
char* symbol = 0;
bool ends_only = false;
bool no_ends = false;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Reweight transducer weights simply\n"
        "\n", program_name);

    print_common_program_options();
    fprintf(message_out, "Reweighting options:\n"
            "  -a, --addition=AVAL        add AVAL to matching weights\n"
            "  -b, --multiplier=BVAL      multiply matching weights by BVAL\n"
            "  -F, --function=FNAME       operate matching weights by FNAME\n"
            "  -l, --lower-bound=LVAL     match weights greater than LVAL\n"
            "  -u, --upper-bound=UVAL     match weights less than UVAL\n"
            "  -I, --input-symbol=ISYM    match arcs with input symbol ISYM\n"
            "  -O, --output-symbol=OSYM   match arcs with output symbol OSYM\n"
            "  -S, --symbol=SYM           match arcs havins symbol SYM\n"
            "  -e, --end-states-only      match end states only, no arcs\n"
            "\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "If AVAL, BVAL or FNAME are omitted, they default to neutral "
            "elements of addition, multiplication or identity function.\n"
            "If LVAL or UVAL are omitted, they default to minimum and maximum "
            "values of the weight structure.\n"
            "If ISYM, OSYM or SYM are omitted, they default to value that "
            "matches all arcs.\n"
            "Float values are parsed with strtod(3) and integers strtoul(3)\n"
            "The functions allowed for FNAME are <cmath> float functions with "
            "parameter count of 1 and a matching return value:\n"
            "abs, acos, asin, ... sqrt, tan, tanh\n"
            "The precedence of operands follows the formula "
            "BVAL * FNAME(w) + AVAL\n"
            "The formula is applied iff\n"
            "((LVAL <= w) && (w <= UVAL)),\n"
            "where w is weight of arc, and \n"
            "(ISYM == i) && (OSYM == o) && ((SYM == i) || (SYM == o)) ^^ \n"
            "(end state && -e).\n"
            "\n");
    fprintf(message_out, "\n");
    print_report_bugs();
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
            {"addition", required_argument, 0, 'a'},
            {"multiplier", required_argument, 0, 'b'},
            {"function", required_argument, 0, 'F'},
            {"lower-bound", required_argument, 0, 'l'},
            {"upper-bound", required_argument, 0, 'u'},
            {"input-symbol", required_argument, 0, 'I'},
            {"output-symbol", required_argument, 0, 'O'},
            {"symbol", required_argument, 0, 'S'},
            {"end-state-only", required_argument, 0, 'e'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "a:b:F:l:u:I:O:S:e",
                             long_options, &option_index);
        if (-1 == c)
          {
            break;
          }
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        switch (c)
          {
          case 'a':
            addition = hfst_strtoweight(optarg);
            break;
          case 'b':
            multiplier = hfst_strtoweight(optarg);
            break;
          case 'F':
            funcname = hfst_strdup(optarg);
            if (strcmp(optarg, "cos") == 0)
              {
                func = &cosf;
              }
            else if (strcmp(optarg, "sin") == 0)
              {
                func = &sinf;
              }
            else if (strcmp(optarg, "tan") == 0)
              {
                func = &tanf;
              }
            else if (strcmp(optarg, "acos") == 0)
              {
                func = &acosf;
              }
            else if (strcmp(optarg, "asin") == 0)
              {
                func = &asinf;
              }
            else if (strcmp(optarg, "atan") == 0)
              {
                func = &atanf;
              }
            else if (strcmp(optarg, "cosh") == 0)
              {
                func = &coshf;
              }
            else if (strcmp(optarg, "sinh") == 0)
              {
                func = &sinhf;
              }
            else if (strcmp(optarg, "tanh") == 0)
              {
                func = &tanhf;
              }
            else if (strcmp(optarg, "exp") == 0)
              {
                func = &expf;
              }
            else if (strcmp(optarg, "log") == 0)
              {
                func = &logf;
              }
            else if (strcmp(optarg, "log10") == 0)
              {
                func = &log10f;
              }
            else if (strcmp(optarg, "sqrt") == 0)
              {
                func = &sqrtf;
              }
            else if (strcmp(optarg, "floor") == 0)
              {
                func = &floorf;
              }
            else if (strcmp(optarg, "ceil") == 0)
              {
                func = &ceilf;
              }
            else
              {
                hfst_error(EXIT_FAILURE, 0, "Cannot parse %s as function name",
                      optarg);
              }
            break;
          case 'l':
            lower_bound = hfst_strtoweight(optarg);
            break;
          case 'u':
            upper_bound = hfst_strtoweight(optarg);
            break;
          case 'I':
            input_symbol = hfst_strdup(optarg);
            break;
          case 'O':
            output_symbol = hfst_strdup(optarg);
            break;
          case 'S':
            symbol = hfst_strdup(optarg);
            break;
          case 'e':
            ends_only = true;
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
check_options(int, char**)
  {
    if (funcname == 0)
      {
        funcname = hfst_strdup("id");
      }
    if (upper_bound < lower_bound)
      {
        hfst_warning( "Lower bound %f exceeds upper bound %f so reweight will"
                " never apply", lower_bound, upper_bound);
      }
}

static
float
reweight(float w, const char* i, const char* o)
  {
    if ((w < lower_bound) || (w > upper_bound))
      {
        // not within weight bounds, don't apply
        return w;
      }
    if ((i != 0) && (o != 0))
      {
        if (ends_only)
          {
            return w;
          }
        else if ((symbol != 0) && ((strcmp(i, symbol) != 0) && 
                              (strcmp(o, symbol) != 0) ) )
          {
            // symbol doesn't match, don't apply
            return w;
          }
        else if ((input_symbol != 0) && (strcmp(i, input_symbol) != 0))
          {
            // input doesn't match, don't apply
            return w;
          }
        else if ((output_symbol != 0) && (strcmp(o, output_symbol) != 0))
          {
            // output doesn't match, don't apply
            return w;
          }
      }
    else if (no_ends)
      {
        return w;
      }
    return multiplier * (*func)(w) + addition;
  }

void
make_weightings()
  {
    size_t transducer_n=0;
    while (instream->is_good())
    {
        transducer_n++;
        HfstTransducer trans(*instream);
        const char* inputname = hfst_get_name(trans, inputfilename);
        hfst_begin_processing(inputname, transducer_n, "Reweighting");
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
                                                 reweight(original.get_final_weight(source_state), 0, 0));
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
                                                     reweight(original.get_final_weight(arc->get_target_state()), 0, 0));
                      }
                    rebuilt[arc->get_target_state()] = state_count;
                    state_count++;
                  }
                HfstBasicTransition nu(rebuilt[arc->get_target_state()],
                                       arc->get_input_symbol(),
                                       arc->get_output_symbol(),
                                       reweight(arc->get_weight(), 
                                                arc->get_input_symbol().c_str(),
                                                arc->get_output_symbol().c_str()));
                replication.add_transition(rebuilt[source_state], nu);
              }
            source_state++;
          }
        trans = HfstTransducer(replication, trans.get_type());
        trans.set_name(inputname);
        hfst_set_name(trans, trans, "reweight");
        hfst_set_formula(trans, trans, "W");
        *outstream << trans.remove_epsilons();
      }
  }


int main( int argc, char **argv ) {
    hfst_init_commandline(argv[0], "0.1", "HfstReweight",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    verbose_printf("Modifying weights %f < w < %f as %f * %s(w) + %f\n",
                   lower_bound, upper_bound, multiplier, funcname, addition);
    if (symbol)
      {
        verbose_printf("only if arc has symbol %s\n", symbol);
      }
    if (input_symbol)
      {
        verbose_printf("only if first symbol is %s\n", input_symbol);
      }
    if (output_symbol)
      {
        verbose_printf("only if second symbol is %s\n", output_symbol);
      }
    if (ends_only)
      {
        verbose_printf("only on final weights");
      }
    make_weightings();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

