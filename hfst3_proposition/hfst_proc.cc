#include "hfst_proc.h"

void endProgram(char *name);

void 
endProgram(char *name)
{
  std::cout << basename(name) << ": process a stream with a finite-state transducer" << std::endl;
  std::cout << "USAGE: " << basename(name) << " [-w] [-z] [-a|-g|-n] fst_file [input_file [output_file]]" << std::endl;
  std::cout << "Options:" << std::endl;
#if HAVE_GETOPT_LONG
  std::cout << "  -a, --analysis:         morphological analysis (default behaviour)" << std::endl;
  std::cout << "  -g, --generation:       morphological generation" << std::endl;
  std::cout << "  -n, --non-marked-gen    morph. generation without unknown word marks" << std::endl;
  std::cout << "  -z, --null-flush:       flush output on the null character " << std::endl;
  std::cout << "  -w, --dictionary-case:  use dictionary case instead of surface case" << std::endl;
  std::cout << "  -v, --version:          version" << std::endl;
  std::cout << "  -h, --help:             show this help" << std::endl;
#else
  std::cout << "  -a:   morphological analysis (default behaviour)" << std::endl;
  std::cout << "  -g:   morphological generation" << std::endl;
  std::cout << "  -n:   morph. generation without unknown word marks" << std::endl;
  std::cout << "  -z:   flush output on the null character " << std::endl;
  std::cout << "  -w:   use dictionary case instead of surface case" << std::endl;
  std::cout << "  -v:   version" << std::endl;
  std::cout << "  -h:   show this help" << std::endl;
#endif
  exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[])
{
  int cmd = 0;
  HFSTApertiumApplicator proc;

#if HAVE_GETOPT_LONG
  static struct option long_options[]=
    {
      {"analysis",        0, 0, 'a'},
      {"generation",      0, 0, 'g'},
      {"non-marked-gen",  0, 0, 'n'},
      {"null-flush",      0, 0, 'z'},
      {"dictionary-case", 0, 0, 'w'},
      {"version",	  0, 0, 'v'},
      {"case-sensitive",  0, 0, 'c'},
      {"help",            0, 0, 'h'}
    };
#endif    

  while(true)
  {
#if HAVE_GETOPT_LONG
    int option_index;
    int c = getopt_long(argc, argv, "agnzwvh", long_options, &option_index);
#else
    int c = getopt(argc, argv, "agnzwvh");
#endif    

    if(c == -1)
    {
      break;
    }

    switch(c)
    {
    case 'a':
    case 'g':
    case 'n':
      if(cmd == 0)
      {
	cmd = c;
      }
      else
      {
	endProgram(argv[0]);
      }
      break;


    case 'z':
      // Set up null flush mode here
      break;

    case 'w':
      // Setup dictionary case mode here
      break;

    case 'v':
      std::cout << basename(argv[0]) << " version " << PACKAGE_VERSION << std::endl;
      exit(EXIT_SUCCESS);
      break;
    case 'h':
    default:
      endProgram(argv[0]);
      break;
    }
  }

  FILE *input = stdin, *output = stdout;
  
  if(optind == (argc - 3))
  {
    FILE *in = fopen(argv[optind], "rb");
    if(in == NULL || ferror(in))
    {
      endProgram(argv[0]);
    }
    
    input = fopen(argv[optind+1], "rb");
    if(input == NULL || ferror(input))
    {
      endProgram(argv[0]);
    }
    
    output= fopen(argv[optind+2], "wb");
    if(output == NULL || ferror(output))
    {
      endProgram(argv[0]);
    }
    
    // Load FST here 
    fclose(in);
  }
  else if(optind == (argc -2))
  {
    FILE *in = fopen(argv[optind], "rb");
    if(in == NULL || ferror(in))
    {
      endProgram(argv[0]);
    }
    
    input = fopen(argv[optind+1], "rb");
    if(input == NULL || ferror(input))
    {
      endProgram(argv[0]);
    }
    
    // load FST here
    fclose(in);
  }   
  else if(optind == (argc - 1))
  {
    FILE *in = fopen(argv[optind], "rb");
    if(in == NULL || ferror(in))
    {
      endProgram(argv[0]);
    }
    // load FST here
    fclose(in);
  }
  else
  {
    endProgram(argv[0]);
  }

#ifdef _MSC_VER
  	_setmode(_fileno(input), _O_U8TEXT);
	_setmode(_fileno(output), _O_U8TEXT);
#endif


  try
  {
    switch(cmd)
    {
      case 'n':
        // Init generation (no word marks) here
        break;
      case 'g':
	// Init generation (with word marks) here
        break;
      case 'a':
      default:
        // Init analysis here
        break;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what();
    // If null flush turned on, flush output here

    exit(1);
  }

  fclose(input);
  fclose(output); 

  return EXIT_SUCCESS;
}

/*****************************************************************************
 * HFSTApertiumApplicator class methods below this line
 *****************************************************************************/


HFSTApertiumApplicator::HFSTApertiumApplicator()
{
  return;
}

HFSTApertiumApplicator::~HFSTApertiumApplicator()
{
  return;
}

void 
HFSTApertiumApplicator::load(FILE *input)
{
  HFSTTransducerHeader header(input);

  if(header.probeFlag(hf_uw_input_epsilon_cycles) || 
     header.probeFlag(hf_input_epsilon_cycles))
  {
    std::cerr << "Transducer has epsilon cycles, these are not supported." << std::endl;
    exit(-1);
  }

  return;
}

void 
HFSTApertiumApplicator::setDictionaryCaseMode(bool const value)
{
  return;
}

void 
HFSTApertiumApplicator::setNullFlush(bool const value)
{
  return;
}

void 
HFSTApertiumApplicator::initAnalysis()
{
  return;
}

void 
HFSTApertiumApplicator::initGeneration()
{
  return;
}

void 
HFSTApertiumApplicator::analysis(FILE *input, FILE *output)
{
  return;
}

void 
HFSTApertiumApplicator::generation(FILE *input, FILE *output, GenerationMode mode)
{
  // The generation method reads an input stream of disambiguated lexical units
  // and outputs the forms generated by the transducer. There are a number of 
  // generation modes (see hfst_proc.h 'enum GenerationMode'). 
  //
  // Disambiguated lexical units output by the final stage of transfer look like:
  // 
  //   ^lemma1<tag1><tag2><tag3>$[ <b>]^lemma2<tag1><tag2>$[ <\/b>]
  //   
  // Where '^' marks the beginning of a new lexical unit and '$' marks the end. Tags
  // are enclosed in '<' and '>'. The unescaped characters '[' and ']' mark the 
  // beginning and end of superblanks (blocks of formatting).
  // 
  // So, the generation should in this case output:
  // 
  //   surfaceform1[ <b>]surfaceform2[ <\/b>]
  // 

  return;
}

/*****************************************************************************
 * HFSTTransducerHeader class methods below this line
 *****************************************************************************/

HFSTTransducerHeader::HFSTTransducerHeader(FILE *transducer)
{
  return;
}

HFSTTransducerHeader::~HFSTTransducerHeader()
{
  return;
}

void
HFSTTransducerHeader::readProperty(bool &property, FILE *transducer)
{
  unsigned int value = 0;
  unsigned int ret = 0;
  
  ret = fread(&value, sizeof(unsigned int), 1, transducer);
  if(value == 0)
  {
    property = false;
    return;
  }
  else 
  {
    property = true;
    return;
  }
  std::cerr << "Could not parse transducer: " << ferror(transducer) << std::endl;
  exit(1);
}

bool
HFSTTransducerHeader::probeFlag(HeaderFlag flag)
{
  switch(flag) 
  {
  case hf_weighted: 
    return weighted;
  case hf_cyclic: 
    return cyclic;
  case hf_minimised: 
    return minimised;
  case hf_deterministic: 
    return deterministic;
  case hf_input_epsilon_cycles:
    return has_input_epsilon_cycles;
  case hf_uw_input_epsilon_cycles:
    return has_unweighted_input_epsilon_cycles;
  }
  return false;
}
