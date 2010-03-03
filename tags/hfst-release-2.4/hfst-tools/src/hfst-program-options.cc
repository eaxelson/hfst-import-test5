#include "hfst-program-options.h"

// All programs
void print_common_program_options(FILE *file) {

  fprintf(file, "%-35s%s", "  -h, --help",    "Print help message\n");
  fprintf(file, "%-35s%s", "  -V, --version", "Print version info\n");
  fprintf(file, "%-35s%s", "  -v, --verbose", "Print verbosely while processing\n");
  fprintf(file, "%-35s%s", "  -q, --quiet",   "Do not print output\n");
  fprintf(file, "%-35s%s", "  -s, --silent",  "Alias of --quiet\n");
   
}

void print_more_info(FILE *file, const char *toolname) {

  fprintf(file, "More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/Hfst%s>\n", toolname);

}

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
void print_common_unary_program_options(FILE *file) {

  fprintf(file,	"%-35s%s", "  -i, --input=INFILE",          "Read input transducer from INFILE\n");
  fprintf(file, "%-35s%s", "  -o, --output=OUTFILE",        "Write output transducer to OUTFILE\n");
  fprintf(file, "%-35s%s", "  -R, --read-symbols=FILE",     "Read symbol table from FILE\n");
  fprintf(file, "%-35s%s", "  -D, --do-not-write-symbols",  "Do not write symbol table with the output transducer\n");
  fprintf(file, "%-35s%s", "  -W, --write-symbols-to=FILE", "Write symbol table to file FILE\n");

}

void print_common_unary_program_parameter_instructions(FILE *file) {

  fprintf(file, 
	  "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
	  "Weightedness of result depends on weightedness of INFILE\n"
	  );

}

// Two transducers to one transducer
//   compose
//   concatenate
//   conjunct
//   disjunct, 
void print_common_binary_program_options(FILE *file) {

  fprintf(file, "%-35s%s", "  -1, --input1=INFILE1",        "Read input transducer from INFILE1\n");
  fprintf(file, "%-35s%s", "  -2, --input2=INFILE2",        "Read output transducer from INFILE2\n");
  fprintf(file, "%-35s%s", "  -o, --output=OUTFILE",        "Write results to OUTFILE\n");
  fprintf(file, "%-35s%s", "  -n, --numbers",               "Use numbers instead of harmonizing transducers\n");
  fprintf(file, "%-35s%s", "",                              "(symbol table is not written with this option)\n");
  fprintf(file, "%-35s%s", "  -D, --do-not-write-symbols",  "Do not write symbol table with the output transducer\n");
  fprintf(file, "%-35s%s", "  -W, --write-symbols-to=FILE", "Write symbol table to FILE\n");
 
}

void print_common_binary_program_parameter_instructions(FILE *file) {

  fprintf(file, 
	  "If OUTFILE, or either INFILE1 or INFILE2 is missing or -,\n"
	  "standard streams will be used.\n"
	  "INFILE1, INFILE2, or both, must be specified.\n"
	  "Weightedness of result depends on weightedness of INFILE1\n"
	  "and INFILE2; both must be either weighted or unweighted.\n"
	  "Both input files must either have a symbol table or option\n"
	  "-n must be used.\n"
	  );

}

void print_report_bugs(FILE *file) {

  fprintf(file, 
	  "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n"
	  );

}
