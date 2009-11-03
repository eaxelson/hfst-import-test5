//! @file hfst-lexc.cc
//!
//! @brief A user interface for hfst-lexc
//!
//! @author Tommi A. Pirinen
//!

//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cstdio>
#include <cerrno>
#include <getopt.h>

#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif

#include "flex-utils.h"
#include "lexc.h"
#include "lexcio.h"
#include "xducer.h"
#include "xymbol-table.h"

// global variables controlling program behaviour:
vector<string> infilenames;
string outfilename;
string symbolfilename;
FILE** infiles;
FILE* outfile;
FILE* symbolfile;
bool isInfilesStdin = true;
bool isOutfileStdout = true;
unsigned int verbosity = PRINT_UNQUIET;
#if TIMING
std::map<std::string, clock_t> timings_accumulated;
std::map<std::string, clock_t> timings_starts;
std::map<std::string, clock_t> timings_ends;
#endif
#if YYDEBUG
extern int hlexcdebug;
#endif
bool weighted = false;

// The compiler
LexcCompiler* lexc;

// oblig. declarations
extern FILE* hlexcin;
int hlexcparse(void);
extern int hlexcnerrs;

static void print_usage();
static void print_short_help();
static void print_version();

static
void
set_default_options()
{
	verbosity = PRINT_UNQUIET;
}

static
int
parse_options(int argc, char** argv)
{
	outfilename = "-";
	while(true)
	{
		static struct option long_options[] = 
		{
#			if DEBUG
			{"debug", no_argument, 0, 'd'},
#			endif
			{"help", no_argument, 0, 'h'},
			{"output", required_argument, 0, 'o'},
			{"quiet", no_argument, 0, 'q'},
#			if TIMING
			{"time", no_argument, 0, 't'},
#			endif
			{"unweighted", no_argument, 0, 'u'},
			{"verbose", no_argument, 0, 'v'},
			{"version", no_argument, 0, 'V'},
			{"weighted", no_argument, 0, 'w'},
#			if YYDEBUG
			{"yydebug", no_argument, 0, 'y'},
#			endif
			{"xerox", no_argument, 0, 'X'},
			{0, 0, 0, 0}
		};
		int option_index = 0;
		char c = getopt_long (argc, argv, "dho:qtuvVwyX",
						 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
		switch (c)
		{
		case 'h':
			print_usage();
			return EXIT_SUCCESS;
			break;
		case 'q':
			verbosity = PRINT_QUIET;
			break;
		case 'v':
			verbosity |= PRINT_VERBOSE;
			break;
		case 'X':
			verbosity = PRINT_XEROXLIKE | PRINT_UNQUIET;
			break;
		case 'u':
			weighted = false;
			break;
		case 'w':
			weighted = true;
			break;
		case 'V':
			print_version();
			return EXIT_SUCCESS;
			break;
		case 'o':
			outfilename = optarg;
			isOutfileStdout = false;
			message_out = stderr;
			break;
#if TIMING
		case 't':
			verbosity |= PRINT_TIMING;
			break;
#endif
#if DEBUG
		case 'd':
			verbosity |= PRINT_DEBUG;
			break;
#endif
#if YYDEBUG
		case 'y':
			hlexcdebug = 1;
			break;
#endif
		case '?':
			print_short_help();
			return EXIT_FAILURE;
			break;
		default:
			print_short_help();
			return EXIT_FAILURE;
			break;
		}
	}
	// rest of options are input files (stdin if none)
	// open input files
	if (argc - optind > 0)
	{
		// process filenames
		infiles = (FILE**)malloc(sizeof(FILE*) * (argc - optind + 1));
		infilenames = vector<string>(argc - optind);
		int i = 0;
		while (optind < argc)
		{
			infilenames[i] = argv[optind];
			if (infilenames[i] == "-")
			{
				infiles[i] = stdin;
			}
			else
			{
				infiles[i] = lexc_fopen(infilenames[i].c_str(), "r");
				if (infiles[i] == NULL)
				{
					return EXIT_FAILURE;
				}
			}
			i++;
			optind++;
			isInfilesStdin = false;
		}
	}
	else
	{
		// no filenames, use stdin
		infiles = (FILE**)malloc(sizeof(FILE*) * 1);
		infilenames = vector<string>(1);
		infilenames[0] = "-";
		infiles[0] = stdin;
		isInfilesStdin = true;
	}

	if (outfilename != "-")
	{
		// N.B. write_to_file handles file opening and closing but we still
		// check at this point if oufile is writable and stuff
		if (!lexc_writable(outfilename.c_str()))
		{		
			return EXIT_FAILURE;
		}
		isOutfileStdout = false;
	}
	else
	{
		outfilename = _("<stdout>");
		outfile = stdout;
		isOutfileStdout = true;
		message_out = stderr;
	}
	

	return 42;
}

// standard usage message here
static
void
print_usage()
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	lexc_printf(PRINT_ALWAYS, 0,
			_("Usage: %s [OPTIONS] [INFILE...]\n"
		   "Compile lexc files into an HFST transducer\n"
		   "\n"
		   "  -h, --help                   Print this help message\n"
		   "  -V, --version                Print version info\n"
		   "  -v, --verbose                Print verbosely while compiling\n"
		   "  -q, --quiet                  Do not print verbosely while compiling\n"
		   "  -o, --output=OUTFILE         Write output to OUTFILE\n"
		   "  -w, --weighted               Use weighted transducer format\n"
		   "  -u, --unweighted             Use unweighted transducer format\n"
#		if TIMING
		   "  -t, --time                   Time all stuff\n"
#		endif
#		if DEBUG
		   "  -d, --debug                  Print very verbosely\n"
#		endif
#		if YYDEBUG
		   "  -y, --hlexcdebug             Enable Yacc's debug state\n"
#		endif
		   "  -X, --xerox                  Imitate Xerox lexc messages\n"
		   "\n"
		   "If either INFILE... or OUTFILE is missing or -, standard streams "
		   "will be used.\n"
		   "If output is stdout, -q is implied.\n"
		   "If SYMFILE is missing, symbol table will be written with "
		   "transducer binary, if possible.\n"
		   "\n"
		   "Examples:\n"
		   "  hfst-lexc -o file.hfst file.hlexc  compiles single lexicon with"
		   "default options\n"
		   "  hfst-lexc -w -v -o file.hwfst file1.hlexc file2.hlexc  compile weighted lexicons from multiple sources\n"
		   "\n"
		   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n"),
		   PACKAGE);
}

// standard version info here
static
void
print_version()
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	lexc_printf(PRINT_ALWAYS, 0,
			_("%s\n"
		"copyright (C) 2009 University of Helsinki,\n"
		"License GPLv3: GNU GPL version 3 "
		"<http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law.\n"),
		PACKAGE_STRING);
}

// standard short help here
static
void
print_short_help()
{
	lexc_printf(PRINT_ALWAYS, 0, 
			_("Try ``%s --help'' for more information.\n"), "hfst-lexc");
}


static
int
cli_main(int argc, char* argv[])
{
	int exitCode = parse_options(argc, argv);
	if ((exitCode == EXIT_SUCCESS) || (exitCode == EXIT_FAILURE))
	{
		return exitCode;
	}
	// Be informative
	lexc_printf(PRINT_VERBOSE, 0, _("printing verbosely\n"));
	lexc_printf(PRINT_DEBUG, 0, _("debug enabled\n"));
	lexc_printf(PRINT_TIMING, 0, _("timing enabled\n"));
	if (weighted)
	{
		lexc_printf(PRINT_VERBOSE, 0, _("building weighted transducer\n"));
	}
	else
	{
		lexc_printf(PRINT_VERBOSE, 0, _("building unweighted transducer\n"));
	}
	// initialise rest, as we now know weightedness
	lexc = new LexcCompiler();
	lexc->setFinalLexiconName("#");
	// read, parse, lex files now
	lexc_timer_start("grand-total");
	for (unsigned int i = 0; i < infilenames.size(); i++)
	{
		token_reset_positions();
		if ((isInfilesStdin) || (infilenames[i] == "-"))
		{
			lexc_printf(PRINT_VERBOSE, 0, _("\nReading from <stdin>\n"));
			set_infile_name(_("<stdin>"));
			hlexcin = stdin;
			hlexcparse();
		}
		else
		{
			lexc_printf(PRINT_VERBOSE, 0, _("\nreading from %s\n"),
					infilenames[i].c_str());
			set_infile_name(infilenames[i].c_str());
			hlexcin = infiles[i];
			hlexcparse();
			fclose(infiles[i]);
		}
		if (hlexcnerrs > 0) {
			lexc_printf(PRINT_ERROR, 8, 
					_("There were parsing errors, aborting.\n"));
			return EXIT_FAILURE;
		}
	}
	// Process the lexicons now                                                 
	lexc_printf(PRINT_VERBOSE, 0, _("Compiling...\n"));
	Xducer result = lexc->compileLexical();
	// Write result
	if (isOutfileStdout)
	{
		lexc_printf(PRINT_VERBOSE, 0, _("Saving result to standard output\n"));
		result.write(std::cout);
	}
	else
	{
		lexc_printf(PRINT_VERBOSE, 0, _("Saving result to %s\n"),
				 outfilename.c_str());
		std::filebuf fbout;
		fbout.open(outfilename.c_str(), std::ios::out);
		std::ostream outstream(&fbout);
		result.write(outstream);
	}
	// destruct global gunk
	delete lexc;
	lexc_timer_end("grand-total");
	return EXIT_SUCCESS;
}

// oh, a main function here
int
main(int argc, char* argv[])
{
	// initialize ugly global structures :-/
	initialiseLexcSymbolTablesAndSets();
	lexc_timer_initialize("grand-total");
	lexc_timer_initialize("string-compile");
	lexc_timer_initialize("trie-union");
	lexc_timer_initialize("xre-compile");
	lexc_timer_initialize("xre-union");
	lexc_timer_initialize("fill-sigma");
	lexc_timer_initialize("morphotaxing");
	lexc_timer_initialize("minimise");
	// set "default" options
	set_default_options();
	// parse options with getopt_long; place them to global variables
	int rv;
	rv = cli_main(argc, argv);
	lexc_timing_printf("grand-total", _("Whole process"));
	lexc_timing_printf("string-compile", _(" * String parsing"));
	lexc_timing_printf("trie-union", _(" * String trie building"));
	lexc_timing_printf("xre-compile", _(" * Regular expression compiling"));
	lexc_timing_printf("xre-union", _(" * Regular expression joining"));
	lexc_timing_printf("fill-sigma", _(" * Completing sigma set"));
	lexc_timing_printf("morphotaxing", _(" * Applying morphotax"));
	lexc_timing_printf("minimise", _(" * All the minimising"));
	destructLexcSymbolTablesAndSets();
	return rv;
}
// vim: set ft=cpp.doxygen:
