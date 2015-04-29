//! @file hfst-proc2.cc
//!
//! @brief A demo of a replacement for hfst-proc using pmatch
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
#include <set>

using std::string;
using std::vector;
using std::pair;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>
#include <errno.h>

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "hfst-tool-metadata.h"
#include "implementations/optimized-lookup/pmatch.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

bool blankline_separated = true;
bool print_all = false;
bool print_weights = false;
std::string tokenizer_filename;
enum OutputFormat {
    tokenize,
    xerox,
    cg,
    finnpos
};
OutputFormat output_format = tokenize;

using hfst_ol::Location;
using hfst_ol::LocationVector;
using hfst_ol::LocationVectorVector;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [--segment | --xerox | --cg] [OPTIONS...] RULESET\n"
            "perform matching/lookup on text streams\n"
            "\n", program_name);
    print_common_program_options(message_out);
    fprintf(message_out,
            "  -n  --newline          Newline as input separator (default is blank line)\n"
            "  -a  --print-all        Print nonmatching text\n"
            "  -w  --print-weight     Print weights\n"
            "  --segment              Segmenting / tokenization mode (default)\n"
            "  --xerox                Xerox output\n"
            "  --cg                   cg output\n"
            " --finnpos               FinnPos output\n");
    fprintf(message_out, 
            "Use standard streams for input and output (for now).\n"
            "\n"
        );
    
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
    fprintf(message_out, "\n");
}

void print_no_output(std::string const & input, std::ostream & outstream)
{
    if (output_format == tokenize) {
        outstream << input;
    } else if (output_format == xerox) {
        outstream << input << "\t" << input << "+?";
    } else if (output_format == cg) {
        outstream << "\"<>\"" << std::endl << input << "\t\"" << input << "\" ?";
    }
//    std::cerr << "from print_no_output\n";
    outstream << "\n\n";
}

void print_nonmatching_sequence(std::string const & str, std::ostream & outstream)
{
    if (output_format == tokenize) {
        outstream << str;
    } else if (output_format == xerox) {
        outstream << str << "\t" << str << "+?";
    } else if (output_format == cg) {
        outstream << "\"<>\"" << std::endl << str << "\t\"" << str << "\" ?";
    } else if (output_format == finnpos) {
        outstream << str << "\t_\t_\t_\t_";
    }
//    std::cerr << "from print_nonmatching_sequence\n";
    outstream << "\n";
}

void print_location_vector(LocationVector const & locations, std::ostream & outstream)
{
    if (output_format == tokenize && locations.size() != 0) {
        outstream << locations.at(0).input;
        if (print_weights) {
            outstream << "\t" << locations.at(0).weight;
        }
        outstream << std::endl;
    } else if (output_format == cg && locations.size() != 0) {
        // Print the cg cohort header
        outstream << "\"<" << locations.at(0).input << ">\"" << std::endl;
        for (LocationVector::const_iterator loc_it = locations.begin();
             loc_it != locations.end(); ++loc_it) {
            // For the most common case, eg. analysis strings that begin with the original input,
            // we try to do what cg tools expect and surround the original input with double quotes.
            // Otherwise we omit the double quotes and assume the rule writer knows what he's doing.
            if (loc_it->output.find(loc_it->input) == 0) {
                // The nice case obtains
                outstream << "\t\"" << loc_it->input << "\"" <<
                    loc_it->output.substr(loc_it->input.size(), std::string::npos);
            } else {
                outstream << "\t" << loc_it->output;
            }
            if (print_weights) {
                outstream << "\t" << loc_it->weight;
            }
            outstream << std::endl;
        }
    } else if (output_format == xerox) {
        for (LocationVector::const_iterator loc_it = locations.begin();
             loc_it != locations.end(); ++loc_it) {
            outstream << loc_it->input << "\t" << loc_it->output;
            if (print_weights) {
                outstream << "\t" << loc_it->weight;
            }
            outstream << std::endl;
        }
    } else if (output_format == finnpos) {
        std::set<std::string> tags;
        std::set<std::string> lemmas;
            for (LocationVector::const_iterator loc_it = locations.begin();
                 loc_it != locations.end(); ++loc_it) {
                // Assume the last space is where the tags begin
                size_t tags_start_at = loc_it->output.find_last_of(" ");
                if (tags_start_at != std::string::npos) {
                    std::string lemma = loc_it->output.substr(0, tags_start_at);
                    if (lemma.find_first_of(" ") == std::string::npos) {
                        // can't have spaces in lemmas
                        lemmas.insert(lemma);
                    }
                    std::string tag = loc_it->output.substr(tags_start_at + 1);
                    if (tag.find_first_of(" ") == std::string::npos) {
                        // or tags
                        tags.insert(tag);
                    }
                }
            }
        outstream << locations.at(0).input << "\t_\t";
        // the input and a blank for features
        if (lemmas.empty()) {
            outstream << "_";
        } else {
            std::string accumulator;
            for (std::set<std::string>::const_iterator it = lemmas.begin();
                 it != lemmas.end(); ++it) {
                accumulator.append(*it);
                accumulator.append(" ");
            }
            outstream << accumulator.substr(0, accumulator.size() - 1);
        }
        outstream << "\t";
        if (tags.empty()) {
            outstream << "_";
        } else {
            std::string accumulator;
            for (std::set<std::string>::const_iterator it = tags.begin();
                 it != tags.end(); ++it) {
                accumulator.append(*it);
                accumulator.append(" ");
            }
            outstream << accumulator.substr(0, accumulator.size() - 1);
        }
        outstream << "\t_";
    }
//    std::cerr << "from print_location_vector\n";
    outstream << std::endl;
}

void match_and_print(hfst_ol::PmatchContainer & container,
                     std::ostream & outstream,
                     std::string & input_text)
{
    if (input_text.size() > 0 && input_text.at(input_text.size() - 1) == '\n') {
        // Remove final newline
        input_text.erase(input_text.size() -1, 1);
    }
    LocationVectorVector locations = container.locate(input_text);
    if (locations.size() == 0 && print_all) {
        print_no_output(input_text, outstream);
    }
    for(LocationVectorVector::const_iterator it = locations.begin();
        it != locations.end(); ++it) {
        if ((it->size() == 1 && it->at(0).output.compare("@_NONMATCHING_@") == 0)) {
            if (print_all) {
                print_nonmatching_sequence(it->at(0).input, outstream);
            }
            continue;
            // All nonmatching cases have been handled
        }
        print_location_vector(*it, outstream);
    }
}
        

int process_input(hfst_ol::PmatchContainer & container,
                  std::ostream & outstream)
{
    std::string input_text;
    char * line = NULL;
    size_t len = 0;
    while (hfst_getline(&line, &len, inputfile) > 0) {
        if (!blankline_separated) {
            // newline separated
            input_text = line;
            match_and_print(container, outstream, input_text);
        } else if (line[0] == '\n') {
            match_and_print(container, outstream, input_text);
            input_text.clear();
        } else {
            input_text.append(line);
        }
        free(line);
        line = NULL;
    }
    
    if (blankline_separated && !input_text.empty()) {
        match_and_print(container, outstream, input_text);
    }
    return EXIT_SUCCESS;
}


int parse_options(int argc, char** argv)
{
    extend_options_getenv(&argc, &argv);
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
            {
                HFST_GETOPT_COMMON_LONG,
                {"newline", no_argument, 0, 'n'},
                {"print-all", no_argument, 0, 'a'},
                {"print-weights", no_argument, 0, 'w'},
                {"segment", no_argument, 0, 't'},
                {"xerox", no_argument, 0, 'x'},
                {"cg", no_argument, 0, 'c'},
                {"finnpos", no_argument, 0, 'f'},
                {0,0,0,0}
            };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT "nawtxcf",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }


        switch (c)
        {
#include "inc/getopt-cases-common.h"
        case 'n':
            blankline_separated = false;
            break;
        case 'a':
            print_all = true;
            break;
        case 'w':
            print_weights = true;
            break;
        case 't':
            output_format = tokenize;
            break;
        case 'x':
            output_format = xerox;
            break;
        case 'c':
            output_format = cg;
            break;
        case 'f':
            output_format = finnpos;
            break;
#include "inc/getopt-cases-error.h"
        }

        
        
    }

//            if (!inputNamed)
//        {
//            inputfile = stdin;
//            inputfilename = hfst_strdup("<stdin>");
//        }
        
        // no more options, we should now be at the input filename
        if ( (optind + 1) < argc)
        {
            std::cerr << "More than one input file given\n";
            return EXIT_FAILURE;
        }
        else if ( (optind + 1) == argc)
        {
            tokenizer_filename = argv[(optind)];
            return EXIT_CONTINUE;
        }
        else
        {
            std::cerr << "No input file given\n";
            return EXIT_FAILURE;
        }


#include "inc/check-params-common.h"


    
    return EXIT_FAILURE;
}

int main(int argc, char ** argv)
{
    hfst_set_program_name(argv[0], "0.1", "HfstProc2");
    hfst_setlocale();
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE) {
        return retval;
    }
    std::ifstream instream(tokenizer_filename.c_str(),
                           std::ifstream::binary);
    if (!instream.good()) {
        std::cerr << "Could not open file " << tokenizer_filename << std::endl;
        return EXIT_FAILURE;
    }
    hfst_ol::PmatchContainer container(instream);
    container.set_verbose(verbose);
//     if (outfile != stdout) {
//         std::filebuf fb;
// fb.open(outfilename, std::ios::out);
// std::ostream outstream(&fb);
// return process_input(container, outstream);
// fb.close();
//     } else {
    return process_input(container, std::cout);
}
