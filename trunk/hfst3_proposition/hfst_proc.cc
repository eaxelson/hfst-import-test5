
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <libgen.h>

#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#endif

#define PACKAGE_VERSION "0.0.1"

using namespace std;

void 
endProgram(char *name)
{
  cout << basename(name) << ": process a stream with a finite-state transducer" << endl;
  cout << "USAGE: " << basename(name) << " [-w] [-z] [-a|-g|-n] fst_file [input_file [output_file]]" << endl;
  cout << "Options:" << endl;
#if HAVE_GETOPT_LONG
  cout << "  -a, --analysis:         morphological analysis (default behaviour)" << endl;
  cout << "  -g, --generation:       morphological generation" << endl;
  cout << "  -n, --non-marked-gen    morph. generation without unknown word marks" << endl;
  cout << "  -z, --null-flush:       flush output on the null character " << endl;
  cout << "  -w, --dictionary-case:  use dictionary case instead of surface case" << endl;
  cout << "  -v, --version:          version" << endl;
  cout << "  -h, --help:             show this help" << endl;
#else
  cout << "  -a:   morphological analysis (default behaviour)" << endl;
  cout << "  -g:   morphological generation" << endl;
  cout << "  -n:   morph. generation without unknown word marks" << endl;
  cout << "  -z:   flush output on the null character " << endl;
  cout << "  -w:   use dictionary case instead of surface case" << endl;
  cout << "  -v:   version" << endl;
  cout << "  -h:   show this help" << endl;
#endif
  exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[])
{
  int cmd = 0;

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
      cout << basename(argv[0]) << " version " << PACKAGE_VERSION << endl;
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
  catch (exception& e)
  {
    cerr << e.what();
    // If null flush turned on, flush output here

    exit(1);
  }

  fclose(input);
  fclose(output); 

  return EXIT_SUCCESS;
}

