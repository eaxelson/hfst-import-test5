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

#include "TaggerBuilder.h"

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Compile training data file into an hfst pos tagger.\n"
        "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    fprintf(message_out, "\n");
    );
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
}

void read_input_file(const char * filename)
{
  std::ofstream out(filename);

  std::string line;

  while (std::getline(std::cin,line))
    { out << line; }

  out.close();
}

void process_input_data(const char * input_file_name,
		        const char * output_file_name)
{
  // Construct system call, which will read the training data from the
  // correct file and process it using compute_data_statistics.py and
  // write the result into the file output_file_name ".temp_stats".
  const char * system_call = NULL;
  
  if (std::string(input_file_name) == "<stdin>")
    {
      system_call = "cat " outputfilename 
	".temp_input_data | compute_data_statistics.py > " 
	output_file_name ".temp_stats";
    }
  else
    {
      system_call = "cat " input_file_name " | compute_data_statistics.py > " 
	output_file_name ".temp_stats";
    }

  verbose_printf("Running system call:\n" 
		 system_call);

  int data_statistics_exit_code = 
    system(system_call);

  // If the input originally came from STDIN, remove the temporary
  // input file which was created.
  if (std::string(input_file_name) == "<stdin>")
    { system("rm -f " outputfilename ".temp_input_data"); }
  
  // If there was a problem during compute_data_statistics.py, halt.
  if (data_statistics_exit_code != EXIT_CONTINUE)
    { return data_statistics_exit_code; }

  // Construct the tagger builder, which will compile the final tagger.
  TaggerBuilder tagger_builder(output_file_name ".temp_stats");

  // Remove the temporary statistics file outputfilename ".temp_stats".
  system("rm -f " outputfilename ".temp_stats");
    
  // Write the result in STDOUT or a file.
  if (std::string(output_file_name) == "<stdout>")
    { tagger_builder.store(); }
  else
    { tagger_builder.store(output_file_name); }
  
 
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
      { read_input_file(outputfilename ".temp_input_data"); }

    retval = process_input_data(inputfilename,outputfilename);

    free(inputfilename);
    free(outfilename);
    return retval;
}
