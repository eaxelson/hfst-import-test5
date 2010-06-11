/*
  
  Copyright 2009 University of Helsinki
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  
*/

#include <getopt.h>
#include <fstream>
#include <cstdlib>
#include "hfst-proc.h"
#include "transducer.h"
#include "formatter.h"


OutputType outputType = Apertium;

bool verboseFlag = false;

bool displayWeightsFlag = false;
bool displayUniqueFlag = false;
int maxAnalyses = std::numeric_limits<int>::max();
bool preserveDiacriticRepresentationsFlag = false;
bool printDebuggingInformationFlag = false;

void stream_error(const char* e)
{
  throw std::ios_base::failure((std::string("Error: malformed input stream: ")+e).c_str());
}
void stream_error(std::string e) {stream_error(e.c_str());}


bool print_usage(void)
{
  std::cerr <<
    "\n" <<
    "Usage: " << PACKAGE_NAME << " [-a [-p|-x]|-g|-n|-t] [-W] [-n N] [-v|-q|-s] transducer_file [input_file [output_file]]\n" <<
    "Perform a transducer lookup on a text stream, tokenizing on the fly\n" <<
    "\n" <<
    "  -a, --analysis              Morphological analysis (default)\n" <<
    "  -g, --generation            Morphological generation\n" <<
    "  -n, --non-marked-gen        Morph. generation without unknown word marks\n" <<
    "  -t  --tokenize              Tokenize the input stream into symbols (for debugging)\n" <<
    "  -p  --apertium              Apertium output format for analysis (default)\n" <<
    "  -x, --xerox                 Xerox output format for analysis\n" <<
    "  -W, --show-weights          Print final analysis weights (if any)\n" <<
    "  -N N, --analyses=N          Output no more than N analyses\n" <<
    "                              (if the transducer is weighted, the N best analyses)\n" <<
    "  -v, --verbose               Be verbose\n" <<
    "  -q, --quiet                 Don't be verbose (default)\n" <<
    "  -s, --silent                Same as quiet\n" <<
    "  -V, --version               Print version information\n" <<
    "  -h, --help                  Print this help message\n" <<
    "\n" <<
    "Report bugs to " << PACKAGE_BUGREPORT << "\n" <<
    "\n";
  return true;
}

bool print_version(void)
{
  std::cerr <<
    "\n" <<
    PACKAGE_STRING << std::endl <<
    __DATE__ << " " __TIME__ << std::endl <<
    "copyright (C) 2009 University of Helsinki\n";
  return true;
}

bool print_short_help(void)
{
  print_usage();
  return true;
}

int main(int argc, char **argv)
{
  int cmd = 0;
  
  while (true)
  {
    static struct option long_options[] =
    {
      // first the hfst-mandated options
      {"help",          no_argument,       0, 'h'},
      {"version",       no_argument,       0, 'V'},
      {"verbose",       no_argument,       0, 'v'},
      {"quiet",         no_argument,       0, 'q'},
      {"silent",        no_argument,       0, 's'},
      // the hfst-proc-specific options
      {"analysis",      no_argument,       0, 'a'},
      {"generation",    no_argument,       0, 'g'},
      {"non-marked-gen",no_argument,       0, 'n'},
      {"tokenize",      no_argument,       0, 't'},
      {"apertium",      no_argument,       0, 'p'},
      {"xerox",         no_argument,       0, 'x'},
      {"show-weights",  no_argument,       0, 'W'},
      {"analyses",      required_argument, 0, 'N'},
      {0,               0,                 0,  0 }
    };
      
    int option_index = 0;
    int c = getopt_long(argc, argv, "hVvqsagntpxWN:", long_options, &option_index);

    if (c == -1) // no more options to look at
      break;

    switch (c)
    {
    case 'h':
      print_usage();
      return EXIT_SUCCESS;
      break;
      
    case 'V':
      print_version();
      return EXIT_SUCCESS;
      break;
      
    case 'v':
  #ifdef DEBUG
      printDebuggingInformationFlag = true;
      preserveDiacriticRepresentationsFlag = true;
  #endif
      
      verboseFlag = true;
      break;
      
    case 'q':
    case 's':
  #ifdef DEBUG
      printDebuggingInformationFlag = false;
      preserveDiacriticRepresentationsFlag = false;
  #endif
      verboseFlag = false;
      displayWeightsFlag = true;
      break;
    
    case 'a':
    case 'g':
    case 'n':
    case 't':
      if(cmd==0)
        cmd = c;
      else
      {
        std::cerr << "Multiple operation modes given" << std::endl;
        print_short_help();
        return EXIT_FAILURE;
      }
      break;
      
    case 'p':
      outputType = Apertium;
      break;
    case 'x':
      outputType = xerox;
      break;
      
    case 'W':
      displayWeightsFlag = true;
      break;
      
    case 'N':
      maxAnalyses = atoi(optarg);
      if (maxAnalyses < 1)
        {
          std::cerr << "Invalid or no argument for analyses count\n";
          return EXIT_FAILURE;
        }
      break;
      
    default:
      std::cerr << "Invalid option\n\n";
      print_short_help();
      return EXIT_FAILURE;
      break;
    }
  }
  
  std::istream* input = &std::cin;
  std::ostream* output = &std::cout;
  int fst_arg = optind, in_arg = optind+1, out_arg = optind+2;
  
  if(optind == (argc-2))
    out_arg = -1;
  else if(optind == (argc-1))
    in_arg = out_arg = -1;
  else if(optind != (argc-3))
  {
    print_short_help();
    return EXIT_FAILURE;
  }
  
  std::ifstream in(argv[(fst_arg)], std::ios::in | std::ios::binary);
  if(!in)
  {
    std::cerr << "Could not open transducer file " << argv[(optind)] << std::endl;
    return EXIT_FAILURE;
  }
  
  if(in_arg != -1)
  {
    input = new std::ifstream(argv[in_arg], std::ios::in | std::ios::binary);
    if(!(*input))
    {
      std::cerr << "Could not open input file " << argv[in_arg] << std::endl;
      return EXIT_FAILURE;
    }
  }
  
  if(out_arg != -1)
  {
    output = new std::ofstream(argv[out_arg], std::ios::out | std::ios::binary);
    if(!(*output))
    {
      std::cerr << "Could not open output file " << argv[out_arg] << std::endl;
      return EXIT_FAILURE;
    }
  }
  
  try
  {
    AbstractTransducer* t = AbstractTransducer::load_transducer(in);
    if(printDebuggingInformationFlag)
      std::cout << "Transducer successfully loaded" << std::endl;
    in.close();
    
    TokenIOStream token_stream(*input, *output, t->get_alphabet());
    
    switch(cmd)
    {
      case 't':
        t->do_tokenize(token_stream);
        break;
      case 'g':
        t->do_generation(token_stream, gm_unknown);
        break;
      case 'n':
        t->do_generation(token_stream, gm_clean);
      case 'a':
      default:
        OutputFormatter* output_formatter = (outputType==xerox)?
                    (OutputFormatter*)new XeroxOutputFormatter(token_stream):
                    (OutputFormatter*)new ApertiumOutputFormatter(token_stream);
         
        t->do_analysis(token_stream, *output_formatter);
    
        delete output_formatter;
        break;
    }
    
    delete t;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }
  
  if(in_arg != -1)
  {
    dynamic_cast<std::ifstream*>(input)->close();
    delete input;
  }
  if(out_arg != -1)
  {
    dynamic_cast<std::ofstream*>(output)->close();
    delete output;
  }
  
  return EXIT_SUCCESS;
}

