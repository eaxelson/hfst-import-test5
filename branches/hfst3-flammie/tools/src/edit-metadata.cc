//! @file hfst-edit-metadata.cc
//!
//! @brief Transducer metadata tool
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
#include <string>
#include <map>

using std::string;
using std::map;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include <hfst.hpp>

#include "conventions/commandline.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;

using std::map;

// add tools-specific variables here

static map<string, string> properties;
static bool properties_given          = false;
static bool print_all_properties      = true;
static char* print_property           = 0;
static unsigned long truncate_length  = 0;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Name a transducer\n"
        "\n", program_name);
    fprintf(message_out, "Name options:\n"
            "  -a, --add=ANAME=VALUE       add or replace property ANAME"
            "with VALUE\n"
            "  -p, --print[=NAME]          print the current PNAME\n"
            "  -t, --truncate_length=LEN   truncate added properties' lengths to LEN\n");
    print_common_program_options();
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out,
            "If PNAME is omitted, all values are printed\n");
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
          {"add", required_argument, 0, 'a'},
          {"print-name", optional_argument, 0, 'p'},
          {"truncate_length", required_argument, 0, 't'},
          {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "a:p::t:",
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
            {
                char* equals = strstr(optarg, "=");
                if (equals == NULL)
                  {
                    hfst_error(EXIT_FAILURE, 0, 
                               "Equals sign `=' missing from %s",
                                optarg);
                  }
                char* property = hfst_strndup(optarg, equals-optarg);
                char* value = hfst_strdup(equals + 1);
                properties[property] = value;
                properties_given=true;
                print_all_properties=false;
                break;
            }
          case 'p':
            if (optarg != NULL)
              {
                print_property = hfst_strdup(optarg);
                print_all_properties = false;
              }
            else
              {
                print_all_properties = true;
              }
          break;
          case 't':
            truncate_length = hfst_strtoul(optarg, 10);
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
    if (!properties_given)
      {
        hfst_iomode = AUTOM_IN_FILE_OUT;
      }
  }

void
make_edits()
  {
    size_t transducer_n=0;
    while(instream->is_good())
    {
        transducer_n++;
        if ((transducer_n > 1) && (print_all_properties || 
                                   (print_property != NULL))) 
          {
             fprintf(outfile, "--- \n");
          }
        if (transducer_n==1)
          {
            verbose_printf("Metadata %s...\n", inputfilename); 
          }
        else
          {
            verbose_printf("Metadata %s...%zu\n", 
                   inputfilename, transducer_n); 
          }
        HfstTransducer trans(*instream);
        if (!print_all_properties && (print_property == NULL)) 
          {
            for (map<string,string>::const_iterator prop = properties.begin();
                 prop != properties.end();
                 ++prop)
              {
                if (prop->first == "type")
                  {
                    hfst_warning( "Changing `type' metadata will not change "
                            "type of transducer in file;\n"
                            "having wrong type may cause breakage, use with "
                            "caution");
                  }
                else if (prop->first == "version")
                  {
                    hfst_warning( "Changing `version' changes parsing "
                            "semantics for header;\n"
                            "use with caution");
                  }
                else if (prop->first == "character-encoding" && 
                    !((prop->second == "utf-8") || (prop->second == "UTF-8")))
                  {
                    hfst_error(EXIT_FAILURE, 0, 
                               "Cannot set `character-encoding' "
                          "to unsupported value;\n"
                          "consider recoding sources of automaton");
                  }
                if (truncate_length > 0)
                  {
                    char* trunc = hfst_strndup(prop->second.c_str(),
                                               truncate_length);
                    hfst_verbose("Setting %s to %s (chopped at %lu)\n",
                                 prop->first.c_str(), trunc,
                                 truncate_length);
                    trans.set_property(prop->first, trunc);
                    free(trunc);
                  }
                else if (prop->second.size() >= 8192)
                  {
                    char* trunc = hfst_strndup(prop->second.c_str(),
                                               8192);
                    char* printable = hfst_strndup(prop->second.c_str(),
                                                   72);
                    hfst_warning("Setting %s to %s (chopped at %lu, "
                                 "because HFST header format is buggy)",
                                 prop->first.c_str(), printable,
                                 8192);
                    trans.set_property(prop->first, trunc);
                    free(trunc);
                    free(printable);
                  }
                else
                  {
                    hfst_verbose("Setting %s to %s",
                                 prop->first.c_str(), prop->second.c_str());
                    trans.set_property(prop->first, prop->second);
                  }
              }
          *outstream << trans;
        }
        else
          {
            const map<string, string>& props = trans.get_properties();
            if (print_all_properties)
              {
                for (map<string, string>::const_iterator prop = props.begin();
                     prop != props.end();
                     ++prop)
                  {
                    fprintf(outfile, "%s: %s\n", prop->first.c_str(),
                            prop->second.c_str());
                  }
              }
            else
              {
                if (props.find(print_property) != props.end())
                  {
                    fprintf(outfile, "%s\n",
                        props.find(print_property)->second.c_str());
                  }
                else
                  {
                    hfst_warning("Property %s was not found",
                                 print_property);
                    fprintf(outfile, "\n");
                  }
              }
          }
      }
  }


int main(int argc, char **argv)
  {
    hfst_init_commandline(argv[0], "0.1", "HfstEditMetadata",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_edits();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

