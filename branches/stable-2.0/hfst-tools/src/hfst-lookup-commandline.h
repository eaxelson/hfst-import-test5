#include <cstdlib>
#include <cstdio>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <getopt.h>

using std::istream;
using std::vector;
using std::ifstream;

static char * program_name = NULL;
static bool debug = false;
static bool verbose = false;
static bool silent = false;
static bool display_help = false;
static bool display_usage = false;
static bool display_version = false;
static char * fst_file_name = NULL;
static char * output_file_name = NULL;
static char * strings_file_name = NULL;
static bool spaces = false;
static char * alphabet_file_name = NULL;
static FILE *message_out = stdout;

// Keep track of input line number when reading from file.
static size_t line_count = 0;

/* For debug: print the values of all variables storing
   commandline arguments after getopt has been used. */
void display_all_arguments(void)
{
  fprintf(stderr,"\nArguments parsed\n");
  fprintf(stderr,"program name: %s\n",program_name);
  fprintf(stderr,"Debug: ");
  if (debug)
    {
      fprintf(stderr,"true\n");
    }
  else
    {
      fprintf(stderr,"false\n");
    }
  fprintf(stderr,"Verbose: ");
  if (verbose)
    {
      fprintf(stderr,"true\n");
    }
  else
    {
      fprintf(stderr,"false\n");
    }
  fprintf(stderr,"Display help: ");
  if (display_help)
    {
      fprintf(stderr,"true\n");
    }
  else
    {
      fprintf(stderr,"false\n");
    }
  fprintf(stderr,"Display usage: ");
  if (display_usage)
    {
      fprintf(stderr,"true\n");
    }
  else
    {
      fprintf(stderr,"false\n");
    }
  fprintf(stderr,"Display version: ");
  if (display_version)
    {
      fprintf(stderr,"true\n");
    }
  else
    {
      fprintf(stderr,"false\n");
    }
  fprintf(stderr,"Rule file name: %s\n",fst_file_name);
  fprintf(stderr,"Output file name: %s\n",output_file_name);
  fprintf(stderr,"Spaces: ");
  if (spaces)
    {
      fprintf(stderr,"true\n");
    }
  else
    {
      fprintf(stderr,"false\n");
    }
  fprintf(stderr,"Silent: ");
  if (silent)
    {
      fprintf(stderr,"true\n");
    }
  else
    {
      fprintf(stderr,"false\n");
    }
  fprintf(stderr,"Alphabet file name: %s\n",alphabet_file_name);
  fprintf(stderr,"\n");
}

/* Return a string, with length given by size_t length,
   whose every character is char c.*/
char * init(char c, size_t length)
{
  char * str = (char*)malloc(sizeof(char)*(length+1));
  for( size_t i = 0; i < length; ++i )
    str[i] = c;
  str[length] = 0;
  return str;
}

/* Return a string of spaces equal in length to the program name. */
char * get_skip(void)
{
  size_t program_name_length = strlen(program_name);
  return init(' ',program_name_length);
}

/* Display program name and possible parameters with their argument types. */
void usage(void) 
{
  fprintf(message_out,"Usage: %s [ OPTIONS ] [ FSTFILE ]\n",
	  program_name);
}

/* Display the version and legal stuff concerning this program. */
void version(void)
{
  fprintf(message_out,
	  "HFST-Lookup 0.1\n"
	  "copyright (C) 2008 University of Helsinki\n"
	  "written by Miikka Silfverberg mar 2008\n"
	  "License GPLv3: GNU GPL version 3 <http://gnu.org/licences/gpl.html>\n"
	  "This is free software: you are free to change and redistribute it\n"
	  "There is NO WARRANTY, to the extent permitted by law.\n");
}

void help(void)
{
  usage();
  fprintf(message_out,
	  "utility for doing lookup of forms.\n\n");

  
  
  fprintf(message_out,
	  "  -v, --verbose           Verbose mode, display all kind of information.\n"
	  "  -q, --quiet             Output nothing except warnings and errors.\n"
	  "  -s, --silent            Output nothing except warnings and errors.\n"
	  "  -d, --debug             Display debug information (mainly for development).\n"
	  "  -u, --usage             Display commandline options and exit.\n"
	  "  -h, --help              Display this help-messageand exit.\n"
	  "  -V, --version           Display the version of the program and exit.\n"
	  "  -i, --input F           The file with the input transducer.\n"
	  "  -o, --output F          The file, where outputstrings are written.\n"
	  "  -I, --input-strings F   The file with input strings.\n"
	  "  -S, --spaces            Give symbol-pairs in the input strings separated by\n"
	  "                          spaces.\n"
	  "  -R, --read-symbols F    Give the symbol coding file for the input\n"
	  "                          transducers in case the coding hasn't been stored with the\n"
	  "                          transducers.\n\n");
  fprintf(message_out,
         "Take a transducer and a file of strings as input. Lookup each\n"
         "string in the transducer and write the result strings into a\n"
         "file.\n\n");
  fprintf(message_out,
          "More info on at "
"<https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstLookup>\n\n"

	  "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}  
/* Parse the command line argument list. */
void parse_options(int argc, char * argv[])
{
  program_name = strdup(argv[0]);
  while (true)
    {
      static const struct option long_options[] =
	{
	  {"debug",no_argument,NULL,'d'},
	  {"verbose",no_argument,NULL,'v'},
	  {"silent",no_argument,NULL,'s'},
	  {"quiet",no_argument,NULL,'q'},
	  {"help",no_argument,NULL,'h'},
	  {"usage",no_argument,NULL,'u'},
	  {"version",no_argument,NULL,'V'},
	  {"input",required_argument,NULL,'i'},
	  {"output",required_argument,NULL,'o'},
	  {"input-strings",required_argument,NULL,'I'},
	  {"spaces",no_argument,NULL,'S'},
	  {"read-symbols",required_argument,NULL,'R'},
	  {0,0,0,0}
	};
      int option_index = 0;
      int c = getopt_long(argc,argv,":dvsqhuVi:o:I:wR:S",
			  long_options, &option_index);
      // End of argument list.
      if ( c == EOF )
	break;

      switch (c)
	{
	case 'd': /* debug */ 
	  debug = true;
	  fprintf(message_out,
		  "Debug mode.\n");
	  break;
	case 'v': /* debug */ 
	  verbose = true;
	  fprintf(message_out,
		  "Verbose mode.\n");
	  break;
	case 'h': /* help */
	  help();
	  if (verbose or debug)
	    fprintf(message_out,
		    "Display help.\n");
	  exit(0);
	  break;
	case 'u': /* help */
	  usage();
	  if (verbose or debug)
	    fprintf(message_out,
		    "Display usage.\n");
	  exit(0);
	  break;
	case 'V': /* version */
	  version();
	  if (verbose or debug)
	    fprintf(message_out,
		    "Display version.\n");
	  exit(0);
	  break;
	case 'i': /* input FILE */
	  fst_file_name = strdup(optarg);
	  if (verbose or debug)
	    fprintf(message_out,
		    "Reading rule-transducers from the file %s.\n",
		    fst_file_name);
	  break;
	case 'o': /* output FILE */
	  output_file_name = strdup(optarg);
	  if (verbose or debug)
	    fprintf(message_out,
		    "Storing test results into the file %s.\n",
		    output_file_name);
	  break;
	case 'I': /* input-strings FILE */
	  strings_file_name = strdup(optarg);
	  if (verbose or debug)
	    fprintf(message_out,
		    "Reading input-strings from the file %s.\n",
		    fst_file_name);
	  break;
	case 'S': /* spaces */ 
	  spaces = true;
	  fprintf(message_out,
		  "Input and output strings separated by spaces.\n");
	  break;
	case 's': /* silent */ 
	  silent = true;
	  break;
	case 'q': /* quiet */ 
	  silent = true;
	  break;
	case 'R': /* read-symbols FILE */
	  alphabet_file_name = strdup(optarg);
	  if (verbose or debug)
	    fprintf(message_out,
		    "Reading the alphabet from the file %s.\n",
		    alphabet_file_name);
	  break;
	case '?': /* Unknown option encountered */
	  fprintf(message_out,
		  "Error: Commandline argument %i %s is unknown.\n",
		  option_index,argv[optind]);
	  usage();
	  exit(1);
	case ':': /* Option with required argument missing encountered. */
	  fprintf(message_out,
		  "Error: Commandline argument %s is\n"
		  "missing a file name\n",argv[optind]);
	  usage();
	  exit(1);
	}      
    }
  if (NULL == output_file_name)
	 {
		 message_out = stderr;
	 }
  // Read fst_file_name as free argument
  if ((argc - optind) != 0) {
    if (fst_file_name == NULL)
      fst_file_name = strdup(argv[optind]);
  }
  
} 

istream &getline(istream &in,
		 char * input,
		 size_t input_size)
{

  if (not in)
    {
      return in;
    }
  ++line_count;
  fprintf(message_out,"Processing:input line: %zu\r",line_count);
  return in.getline(input,input_size);
}

void warn(char * warning)
{
  fprintf(message_out,
	  "\nLINE %zu: WARNING:\n"
	  "%s",
	  line_count,
	  warning);
	  
}

