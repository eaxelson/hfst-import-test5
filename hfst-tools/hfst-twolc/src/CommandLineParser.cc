#include "CommandLineParser.h"

void CommandLineParser::check_read_file(const char * file_name)
{
  FILE * f = fopen(file_name,"r");
  if (f == NULL)
    {
      if (write_errors)
	{
	  std::cerr << "Error: The input file " << file_name 
		    << " couldn't be opened!" << std::endl;
	}
      exit(1);
    }
  fclose(f);
}

void CommandLineParser::check_write_file(const char * file_name)
{
  FILE * f = fopen(file_name,"w");
  if (f == NULL)
    {
      if (write_errors)
	{
	  std::cerr << "Error: The output file " << file_name 
		    << " couldn't be opened!" << std::endl;
	}
      exit(1);
    }
  int i = fputc(1,f);
  if (i != 1)
    {
      if (write_errors)
	{
	  std::cerr << "Error: The output file " << file_name 
		    << " is write-protected!" << std::endl;
	}
      exit(1);
    }
  fclose(f);
}

void CommandLineParser::parse_options(int argc, char * argv[])
  {
    program_name = string_copy(argv[0]);
    while (true)
      {
	static const struct option long_options[] =
	  {
	    {"verbose",no_argument,NULL,'v'},
	    {"silent",no_argument,NULL,'s'},
	    {"quiet",no_argument,NULL,'q'},
	    {"help",no_argument,NULL,'h'},
	    {"usage",no_argument,NULL,'u'},
	    {"version",no_argument,NULL,'V'},
	    {"input",required_argument,NULL,'i'},
	    {"output",required_argument,NULL,'o'},
	    {"weighted",required_argument,NULL,'w'},
	    {"names",no_argument,NULL,'N'},
	    {"resolve",no_argument,NULL,'r'},
	    {0,0,0,0}
	  };
	int option_index = 0;
	int c = getopt_long(argc,argv,":vsqrhuVi:o:wN",
			    long_options, &option_index);
	// End of argument list.
	if ( c == EOF )
	  break;
	switch (c)
	  {
	  case 'v':
	    silent = false;
	    verbose = true;
	    break;
	  case 's':
	    silent = true;
	    verbose = false;
	    break;
	  case 'q':
	    silent = true;
	    verbose = false;
	    break;
	  case 'h':
	    no_output = true;
	    version = true;
	    usage = true;
	    help = true;
	    break;
	  case 'u':
	    no_output = true;
	    usage = true;
	    break;
	  case 'V':
	    no_output = true;
	    version = true;
	    break;
	  case 'i':
	    input_file_defined = true;
	    input_file_name = string_copy(optarg);
	    break;
	  case 'o':
	    output_file_defined = true;
	    output_file_name = string_copy(optarg);
	    break;
	  case 'w':
	    weighted = true;
	    break;
	  case 'N':
	    store_names = true;
	    break;
	  case 'r':
	    resolve_conflicts = true;
	    break;
	  case ':':
	    if (write_errors)
	      {
		std::cerr << std::endl;
		std::cerr << "Error: The option " << argv[optind-1] 
			  << " requires " << "a file-name argument!" 
			  << std::endl << std::endl;
	      }
	    exit(1);
	  default:
	    if (write_errors)
	      {
		std::cerr << std::endl;
		std::cerr << "Error: The option " << argv[optind-1] 
			  << " is unknown!" << std::endl << std::endl;
	      }
	    exit(1);
	  }
      }
    if (not input_file_defined)
      {
	if ((argc - optind) != 0) {
	  input_file_defined = true;
	  input_file_name = string_copy(argv[optind]);
	}
      }

    if (not input_file_defined)
      {
	if (write_messages and not silent)
	  {
	    if (not no_output)
	      {
		std::cerr << "Note: Reading from STDIN." << std::endl;
	      }
	  }
      }
    else
      {
	check_read_file(input_file_name);
      }

    if (not output_file_defined)
      {
	if (write_messages and not silent)
	  {
	    if (not no_output)
	      {
		std::cerr << "Note: Writing to STDOUT." << std::endl;
	      }
	  }
      }
    else
      {
	check_write_file(output_file_name);
      }

    if (store_names)
      {
	if (output_file_defined)
	  {
	    names_file_name = generate_names_file_name(output_file_name);
	  }
	else
	  {
	    names_file_name = generate_names_file_name("hfst_twolc_rule");
	  }
	if (write_messages and not silent)
	  {
	    std::cerr << "Note: Writing rule names to file " << names_file_name
		      << "." << std::endl;
	  }
	check_write_file(names_file_name);
      }
    display_messages();
  }

char * CommandLineParser::generate_names_file_name(const char * name)
{
  char * file_name = new_string(strlen(name)+6);
  int num = sprintf(file_name,"%s.names",name);
  if (num != (int)(strlen(name)+6))
    {
      if (write_errors)
	{
	  std::cerr << std::endl;
	  std::cerr << "Error: Expected " << strlen(name)+6 << " got " << num
		    << "." << std::endl
		    << "Wrong number of characters written to string." 
		    << std::endl
		    << "This is a bug!. Please report it to" << std::endl
		    << "hfst-bugs@helsinki." << std::endl << std::endl;
	}
      assert(false);
    }
  return file_name;
}

void CommandLineParser::print_usage(void)
{
  std::cerr << "USAGE: hfst-twolc [ OPTIONS ] [ GRAMMARFILE ]" << std::endl
            << "An input-grammar has to be given either using "  << std::endl
	    << "option -i (--input), as the last commanline" << std::endl
	    << "argument or in STDIN." << std::endl;
}

void CommandLineParser::print_version(void)
{
  std::cerr << "HFST-TwolC 2.0 [$Revision: 1.11 $]" << std::endl
	    << "copyright (C) 2009 University of Helsinki" << std::endl
	    << "written by Miikka Silfverberg" << std::endl
	    << "License GPLv3: GNU GPL version 3" << std::endl
	    << "<http://gnu.org/licences/gpl.html>" << std::endl 
	    << "This is free software: you are free to change and " 
	    << "redistribute it" << std::endl
	    << "There is NO WARRANTY, to the extent permitted by law."
	    << std::endl;
}

void CommandLineParser::print_help(void)
{
  std::cerr
    << "HFST-TwolC takes a two-level grammar as input and compiles a set of"
    << std::endl 
    << "two-level rule transducers from them." << std::endl << std::endl

    << "For further information visit" << std::endl
    << "<https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstTwolC>" << std::endl
    << "The names and types of the commandline-arguments given to hfst-twolc."
    << std::endl
    << "-h, --help              Display this help-message." << std::endl
    << "-V, --version           Display version and legal stuff." << std::endl
    << "-i, --input=FILE        A file with a hfst-twolc-grammar in " 
    << "text-format." << std::endl
    << "-o, --output=FILE       A file where the rule transducers are "
    << "stored." << std::endl
    << "-r, --resolve           Resolve conflicts between rules." << std::endl
    << "-N, --names             Save the names of the rules into the file"
    << std::endl
    << "                        inputfilename.names." << std::endl
    << "-v, --verbose           Print a lot of information about the "
    << "compilation." << std::endl << std::endl

    << "If -i (--input) is omitted, the last commandline argument is " 
    << "considered" << std::endl
    << "to be the grammar-file (or STDIN if there is no free argument)." 
    << std::endl << std::endl

    << "If -o (--output) is omitted, the resulting transducers are written "
    << "to" << std::endl 
    << "STDOUT." << std::endl << std::endl

    << "Report bugs to <hfst-bugs@helsinki.fi>" << std::endl << std::endl;
}

void CommandLineParser::display_messages(void)
{
  if (not write_messages)
    {
      return;
    }
  if (help)
    {
      print_usage();
      print_version();
      print_help();
    }
  else 
    {
      if (usage)
	{
	  print_usage();
	}
      if (version)
	{
	  print_version();
	}
    }
}

bool CommandLineParser::check_parameters(bool help_b,
					 bool usage_b,
					 bool version_b,
					 bool input_file_defined_b,
					 bool output_file_defined_b,
					 char * input_file_name_s,
					 char * output_file_name_s,
					 bool resolve_conflicts_b,
					 bool weighted_b,
					 bool store_names_b,
					 bool verbose_b,
					 bool silent_b,
					 char * names_file_name_s)
{
  if (help_b != help)
    {
      return false;
    }
  if (usage_b != usage)
    {
      return false;
    }
  if (version_b != version)
    {
      return false;
    }
  if (input_file_defined_b != input_file_defined)
    {
      return false;
    }
  if (output_file_defined_b != output_file_defined)
    {
      return false;
    }
  if (strcmp(input_file_name_s,input_file_name) != 0)
    {
      return false;
    }
  if (strcmp(output_file_name_s,output_file_name) != 0)
    {
      return false;
    }
  if (resolve_conflicts_b != resolve_conflicts)
    {
      return false;
    }
  if (weighted_b != weighted)
    {
      return false;
    }
  if (store_names_b != store_names)
    {
      return false;
    }
  if (verbose_b != verbose)
    {
      return false;
    }
  if (silent_b != silent)
    {
      return false;
    }
  if (strcmp(names_file_name_s,names_file_name) != 0)
    {
      return false;
    }

  return true;
}

void CommandLineParser::display_options(void)
{
  std::cout << "help                " << help << std::endl;
  std::cout << "usage               " << usage << std::endl;
  std::cout << "version             " << version << std::endl;
  std::cout << "input_file_defined  " << input_file_defined << std::endl;
  std::cout << "output_file_defined " << output_file_defined << std::endl;
  if (input_file_defined)
    {
      std::cout << "input_file_name     " << input_file_name << std::endl;
    }
  else
    {
      std::cout << "input_file_name      (NULL)" << std::endl;
    }
  if (output_file_defined)
    {
      std::cout << "output_file_name    " << output_file_name << std::endl;
    }
  else
    {
      std::cout << "output_file_name     (NULL)" << std::endl;
    }
  std::cout << "resolve_conflicts   " << resolve_conflicts << std::endl;
  std::cout << "weighted            " << weighted << std::endl;
  std::cout << "store_names         " << store_names << std::endl;
  std::cout << "verbose             " << verbose << std::endl;
  std::cout << "silent              " << silent << std::endl;
  if (store_names)
    {
      std::cout << "names_file_name     " << names_file_name << std::endl; 
    }
  else
    {
      std::cout << "names_file_name      (NULL)" << std::endl; 
    }
  std::cout << std::endl;
}

#ifdef DEBUG
int main(int argc, char * argv[])
{
  CommandLineParser command_line_parser(argc, argv,true);
  command_line_parser.display_options();
}
#endif
