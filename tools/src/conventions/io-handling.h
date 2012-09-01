//! @file io-handling.h
//! @brief Hfst tool conventions for input and output reading.

#ifndef GUARD_io_handling_h
#define GUARD_io_handling_h 1

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <HfstDataTypes.h>

/**
 * different types of hfst tools for some automagic functions.
 */
enum hfst_tool_io
  {
    AUTOM_IN_AUTOM_OUT, //!< normal automaton operation 
    FILE_IN_AUTOM_OUT,  //!< something that creates automaton from data
    AUTOM_IN_FILE_OUT,  //!< something that prints data about automaton
    FILE_IN_FILE_OUT,   //!< something that doesn't use automata
    NO_AUTOMAGIC_IO     //!< disable all io-handling.h
  };
/**
 * different input counts for tools.
 */
enum hfst_input_count
  {
    READ_NONE,          //!< this tool simply does not walk into mordor
    READ_ONE,           //!< this tool reads from one file or stream
    READ_TWO,           //!< this tool reads from two streams
    READ_MANY,          //!< this tool reads from n streams
    NO_AUTOMAGIC_FIlES  //!< disable all io-handling.h
  };
/**
 * variable determining current tools operating mode.
 */
extern hfst_tool_io hfst_iomode;
/**
 * variable determining input file count, possible values 0, 1, 2, more.
 */
extern hfst_input_count inputs;
/**
 * @brief set to output file's name for programs that output into a file.
 */
extern char* outfilename;
/**
 * @brief file handle to output target for programs that output into a file.
 */
extern FILE* outfile;
/**
 * @brief stream pointer to output target for programs that write automata.
 */
extern hfst::HfstOutputStream* outstream;
/**
 * @brief whether output was given on commandline or implicitly stdout.
 */
extern bool output_named;
/**
 * @brief name of input file for tools that read input
 */
extern char* inputfilename;
/**
 * @brief file handle for input file that read with c functions.
 */
extern FILE* inputfile;
/**
 * @brief pointer to input stream for tools that read automata
 */
extern hfst::HfstInputStream* instream;
/**
 * @brief whether input has been given from commandline or implicitly stdin.
 */
extern bool input_named;
/**
 * @brief name of the left input file for tools that read two inputs
 */
extern char* firstfilename;
/**
 * @brief handle for the left input file for tools with c functions.
 */
extern FILE* firstfile;
/**
 * @brief first stream for programs that read two automata.
 */
extern hfst::HfstInputStream* firststream;
/**
 * @brief whether first file is named in commandline or implicitly stdin.
 */
extern bool first_named;
/**
 * @brief name of the right input file for tools that read two inputs.
 */
extern char *secondfilename;
/**
 * @brief handle to the second input for tools that use c functions.
 */
extern FILE *secondfile;
/**
 * @brief second stream for programs that read two automata.
 */
extern hfst::HfstInputStream* secondstream;
/**
 * @brief whether second input file is named or implicitly stdin.
 */
extern bool second_named;
/**
 * names of the files for tools that read n inputs.
 */
extern char** inputfilenames;
/**
 * @brief handles to files for tools that use c functions.
 */
extern FILE** inputfiles;
/**
 * @brief streams for programs that read n automata
 */
extern hfst::HfstInputStream** instreams;
/**
 * @brief number of inputs for tools that read more than 2.
 */
extern size_t inputs_named;

/**
 * @brief whether any of inputs comes from stdin
 */
extern bool is_input_stdin;
/**
 * @brief whether output is going to stdout.
 */
extern bool is_output_stdout;
/**
 * automaton format used currently
 */
extern hfst::ImplementationType format;

/**
 * @brief open transducer streams for common transducer handling programs.
 */
void hfst_open_streams();

#endif
// vim: set ft=cpp.doxygen:
