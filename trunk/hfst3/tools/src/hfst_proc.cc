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
      proc.setNullFlush(true);
      break;

    case 'w':
      proc.setDictionaryCaseMode(true);
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
    
    proc.loadTransducer(in);
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
    
    proc.loadTransducer(in);
    fclose(in);
  }   
  else if(optind == (argc - 1))
  {
    FILE *in = fopen(argv[optind], "rb");
    if(in == NULL || ferror(in))
    {
      endProgram(argv[0]);
    }
    proc.loadTransducer(in);
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
        // Run generation here 'runTransducer(C)'
        proc.generation(input, output, gm_clean);
        break;
      case 'g':
	// Init generation (with word marks) here
        // Run generation here 'runTransducer(C)'
        proc.generation(input, output, gm_unknown);
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
HFSTApertiumApplicator::loadTransducer(FILE *input)
{
  std::cerr << "HFSTApertiumApplicator::loadTransducer" << std::endl;
  transducer.loadTransducer(input);

}
void 
HFSTApertiumApplicator::setDictionaryCaseMode(bool const value)
{
  dictionaryCase = value;
}

void 
HFSTApertiumApplicator::setNullFlush(bool const value)
{
  nullFlush = value;
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
  //   ^lemma1<tag1><tag2><tag3>$[ <b>]^lemma2<tag1><tag2>$[<\/b>].
  //   
  // Where '^' marks the beginning of a new lexical unit and '$' marks the end. Tags
  // are enclosed in '<' and '>'. The unescaped characters '[' and ']' mark the 
  // beginning and end of superblanks (blocks of formatting).
  // 
  // So, the generation should in this case output:
  // 
  //   surfaceform1[ <b>]surfaceform2[<\/b>].
  // 

  return;
}

/*****************************************************************************
 * HFSTTransducerHeader class methods below this line
 *****************************************************************************/

HFSTTransducerHeader::HFSTTransducerHeader()
{
  return;
}

HFSTTransducerHeader::~HFSTTransducerHeader()
{
  return;
}

void
HFSTTransducerHeader::readHeader(FILE *transducer)
{
  std::cerr << "HFSTTransducerHeader::readHeader()" << std::endl;

  // Read the header, these operations are ordered.

  size_t val; // TODO: Catch possible errors here.

  val = fread(&number_of_input_symbols, sizeof(SymbolNumber), 1, transducer);
  val = fread(&number_of_symbols, sizeof(SymbolNumber), 1, transducer);

  val = fread(&transition_index_table_size, sizeof(TransitionTableIndex), 1, transducer);
  val = fread(&transition_target_table_size, sizeof(TransitionTableIndex), 1, transducer);

  val = fread(&number_of_states, sizeof(StateIdNumber), 1, transducer);

  // TODO: Check what is _really_ in the header, is 'number_of_transitions' really 0 / 1?
  val = fread(&number_of_transitions, sizeof(TransitionNumber), 1, transducer);

  std::cerr << number_of_states << " " << number_of_transitions << std::endl;

  readProperty(weighted, transducer);
  readProperty(deterministic, transducer);
  readProperty(input_deterministic, transducer);
  readProperty(minimised, transducer);
  readProperty(cyclic, transducer);

  readProperty(has_epsilon_epsilon_transitions, transducer);
  readProperty(has_input_epsilon_transitions, transducer);
  readProperty(has_input_epsilon_cycles, transducer);
  readProperty(has_unweighted_input_epsilon_cycles, transducer);
}

void
HFSTTransducerHeader::readProperty(bool &property, FILE *transducer)
{
  unsigned int value = 0;
  unsigned int ret = 0;
  
  ret = fread(&value, sizeof(unsigned int), 1, transducer); // TODO: Check return value
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
  case hf_input_deterministic: 
    return input_deterministic;
  case hf_input_epsilon_cycles:
    return has_input_epsilon_cycles;
  case hf_epsilon_epsilon_transitions:
    return has_epsilon_epsilon_transitions;
  case hf_input_epsilon_transitions:
    return has_input_epsilon_transitions;
  case hf_uw_input_epsilon_cycles:
    return has_unweighted_input_epsilon_cycles;
  }
  return false;
}

SymbolNumber
HFSTTransducerHeader::symbolCount()
{
  return number_of_symbols;
}

/*****************************************************************************
 * HFSTTransducer class methods below this line
 *****************************************************************************/

HFSTTransducer::HFSTTransducer()
{
  return;
}

HFSTTransducer::~HFSTTransducer()
{
  return;
}

void 
HFSTTransducer::loadTransducer(FILE *input)
{
  std::cerr << "HFSTTransducer::loadTransducer()" << std::endl;
  header.readHeader(input);
  alphabet.readAlphabet(input, header.symbolCount());

  if(header.probeFlag(hf_uw_input_epsilon_cycles) || 
     header.probeFlag(hf_input_epsilon_cycles))
  {
    std::cerr << "Transducer has epsilon cycles, these are not supported." << std::endl;
    exit(-1);
  }

  if(alphabet.getStateSize() == 0) 
  {
    // If the state size is zero there are no flag diacritics to handle

    if(header.probeFlag(hf_weighted) == false)
    {
      // No flags, no weights, all analyses       
      
    }
    else if(header.probeFlag(hf_weighted) == true)
    {
      // No flags, weights, all analyses
    }
  }
  else
  {
    // Handle flag diacritics
  }

  return;
}

/*****************************************************************************
 * HFSTTransducerAlphabet class methods below this line
 *****************************************************************************/

HFSTTransducerAlphabet::HFSTTransducerAlphabet()
{
  key_table = new KeyTable;
}

HFSTTransducerAlphabet::~HFSTTransducerAlphabet()
{

}

void
HFSTTransducerAlphabet::readAlphabet(FILE *transducer, SymbolNumber symbol_number)
{
  std::cerr << "HFSTTransducerAlphabet::readAlphabet" << std::endl;

  number_of_symbols = symbol_number;
  feature_number = 0;
  value_number = 1; 
  value_bucket[std::string()] = 0; // Empty value = neutral TODO: what does 'neutral' mean ?
   
  for(SymbolNumber k = 0; k < number_of_symbols; ++k) 
  {
    getNextSymbol(transducer, k);
  }
  // Assume the first symbol is epsilon which we don't want to print
  // TODO: Why ?
  key_table->operator[](0) = "";
  
}

void
HFSTTransducerAlphabet::getNextSymbol(FILE *transducer, SymbolNumber key)
{
  int byte;
  char *sym = line;
  while((byte = fgetc(transducer)) != 0)
  {
    if (byte == EOF)
    {
      std::cerr << "Could not parse transducer: " << ferror(transducer) << std::endl;
      exit(1);
    }
    *sym = byte;
    ++sym;
  }
  *sym = 0;

  // CONVERSION CONTINUES HERE
}

SymbolNumber
HFSTTransducerAlphabet::getStateSize()
{
  return feature_bucket.size();
}

/*****************************************************************************
 * FlagDiacriticOperation class methods below this line
 *****************************************************************************/

FlagDiacriticOperation::FlagDiacriticOperation(FlagDiacriticOperator op, SymbolNumber feat, ValueNumber val)
{
  operation = op;
  feature = feat;
  value = val;
}
