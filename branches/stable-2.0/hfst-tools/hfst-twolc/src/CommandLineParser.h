#ifndef __COMMNAND_LINE_PARSER_H_
#define __COMMNAND_LINE_PARSER_H_
#include "../config.h"
#include <iostream>
#include <getopt.h>
#include "string_manipulation.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

class CommandLineParser
{
 protected:
  void parse_options(int argc, char * argv[]);
  char * generate_names_file_name(const char * output_file_name);

  void check_read_file(const char * file_name);
  void check_write_file(const char * file_name);
  public:
  bool help;
  bool usage;
  bool version;
  bool input_file_defined;
  bool output_file_defined;
  char * input_file_name;
  char * output_file_name;
  bool resolve_conflicts;
  bool weighted;
  bool store_names;
  bool verbose;
  bool silent;
  char * names_file_name;
  char * program_name;
  bool write_messages;
  bool write_errors;
  bool no_output;
  CommandLineParser(int argc,char * argv[], 
		    bool write_output, bool write_errs):
  help(false),usage(false),version(false),input_file_defined(false),
  output_file_defined(false),input_file_name(NULL),output_file_name(NULL),
  resolve_conflicts(false),weighted(false),store_names(false),verbose(false),
  silent(false), names_file_name(NULL),program_name(NULL),
  write_messages(write_output),write_errors(write_errs),
  no_output(false)
  { 
    parse_options(argc,argv); 
  }
  ~CommandLineParser(void)
  {
    free(program_name);
    free(input_file_name);
    free(output_file_name);
    free(names_file_name);
  }
  void print_usage(void);
  void print_help(void);
  void print_version(void);
  void display_messages(void);
  bool check_parameters(bool help,
			bool usage,
			bool version,
			bool input_file_defined,
			bool output_file_defined,
			char * input_file_name,
			char * output_file_name,
			bool resolve_conflicts,
			bool weighted,
			bool store_names,
			bool verbose,
			bool silent,
			char * names_file_name);
  void display_options(void);
};
  
#endif
