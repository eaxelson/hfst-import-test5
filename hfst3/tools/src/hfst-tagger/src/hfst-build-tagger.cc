//! @file hfst-fst2txt.cc
//!
//! @brief Transducer array printing command line tool
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

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

#include "build_model_src/TaggerBuilder.h"

void
print_usage()
{ /* nothing */ }

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
                             HFST_GETOPT_UNARY_SHORT "wDnf:",
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
        break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

void read_input_file(std::string filename)
{
  StringVector file_contents;
  std::string line;

  while (std::getline(std::cin,line))
    { file_contents.push_back(line); }

  std::ofstream out(filename.c_str());

  for (StringVector::const_iterator it = file_contents.begin();
       it != file_contents.end();
       ++it)
    { out << *it << std::endl; }

  out.close();
}

int process_input_data(std::string output_file_prefix)
{
  // Read training statistics from STDIN.
  try
    {
      TaggerBuilder tagger_builder;
      
      // Write the result in STDOUT or files depending on command-line args.
      if (std::string(output_file_prefix) == "<stdout>")
	{ tagger_builder.store(); }
      else
	{ tagger_builder.store(output_file_prefix); }
    }
  catch (...)
    { return EXIT_FAILURE; }
 
  return EXIT_CONTINUE;
}

int main( int argc, char **argv ) 
{
    hfst_set_program_name(argv[0], "0.1", "HfstBuildTagger");
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
    
    verbose_printf("Reading training data from %s, writing tagger to\n"
		   "%s.{lex,seq}", 
		   inputfilename, outfilename);

    if (std::string(inputfilename) == "<stdin>")
      {
	std::string temp_data_file_name = std::string(argv[0]) + 
	  ".temp_input_data";
	read_input_file(temp_data_file_name); 
      }

    retval = process_input_data(outfilename);

    free(inputfilename);
    free(outfilename);
    return retval;
}
