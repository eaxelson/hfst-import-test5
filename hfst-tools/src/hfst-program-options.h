#include <cstdio>

// All programs
void print_common_program_options(FILE *file);
void print_report_bugs(FILE *file);
void print_more_info(FILE *file, const char *toolname);

// One transducer to one transducer:
//   compatible
//   determinize
//   head
//   invert
//   minimize
//   project
//   push-weights
//   remove-epsilons
//   repeat
//   reverse
//   symbols
//   tail
//   unweighted2weighted
//   weighted2unweighted
void print_common_unary_program_options(FILE *file);
void print_common_unary_program_parameter_instructions(FILE *file);

// One transducer to text:
//   fst2txt
//   fst2strings
void print_common_unary_string_program_options(FILE *file);

// Text to one transducer: txt2fst
// Text to transducer(s): strings2fst
//	  "  -i, --input=FILENAME  	Read input text-file from FILENAME\n"
//	  "  -o, --output=FILENAME 	Write output transducer(s) to FILENAME\n"
//	  "  -R, --symbols=FILENAME 	Read symbol table from FILENAME\n"
//	  "  -D, --do-not-write-symbols 	Do not write symbol table with the output transducer(s)\n"
//	  "  -W, --write-symbols-to=FILENAME 	Write symbol table to file FILENAME\n"


// Two transducers to one transducer: compose, concatenate, conjunct, disjunct, 
void print_common_binary_program_options(FILE *file);
void print_common_binary_program_parameter_instructions(FILE *file);

// Write separate commandline helps: compare, split, strings2fst, summarize, txt2fst
