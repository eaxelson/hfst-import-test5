//! @file pretty-printing.h
//! @brief Hfst functions for printing neatly formatted informative messages.
//! use these to avoid messing up the output streams that may be reserved for
//! automata saving, to get all messages formatted in the same manner, and
//! to obey the user preferences on what should be printed.

#ifndef GUARD_pretty_printing_h
#define GUARD_pretty_printing_h

/** 
 * @brief set @c verbose when program should print before and after every
 *        non-trivial step it takes.
 */
extern bool verbose;
/** @brief set @c silent when program should not print anything at all. */
extern bool silent;
/** @brief set @c debug when program should dump all the intermediate results
 *         to terminal and/or save them to files in @c CWD.
 */
extern bool debug;
/** @brief set @c message_out to stream that is usable for non-error message
 *         print-outs.
 *         This @e should be stdout in all cases, except when transducer 
 *         binaries are being transmitted through @c stdout. Some programs 
 *         @e may have option to log these messages to a file instead.
 */
extern FILE* message_out;
/**
 * @brief whether we want to determine if color setting is sensible.
 */
extern bool auto_colors;
/**
 * @brief if output is colorable and it is wise to print colors there.
 */
extern bool print_colors;
#define COLOR_RESET "\033[0m"
#define COLOR_ERROR "\033[21;31m"
#define COLOR_INFO "\033[22;32m"
#define COLOR_WARNING "\033[22;33m"
#define COLOR_DEBUG "\033[22;34m"
#define COLOR_VERBOSE "\033[22;35m"
#define COLOR_BRACKET "\033{21;35m"
#define COLOR_OK "\033[21;32m"
#define COLOR_FAIL "\033[21;31m"
#define COLOR_PROGRAM_NAME "\033[22;37m"

#ifndef HAVE_DECL_PROGRAM_NAME
/**
 * @brief set to program's name for output functions
 */
extern char* program_name;
#endif
#ifndef HAVE_DECL_PROGRAM_SHORT_NAME
/**
 * @brief set to program's name without path for output functions.
 */
extern char* program_short_name;
#endif

/**
 *  @brief set @a hfst_tool_version to version specific to the tool.
 *  @sa hfst_set_program_name
 */
extern char* hfst_tool_version;

/** 
 * @brief set @a hfst_tool_wikiname to name of the kitwiki page for this tool.
 */
extern char* hfst_tool_wikiname;

/** 
 * @brief set program's name and other infos for reusable messages defined
 * below. This function must be called in beginning of main as the values are
 * used in all error messages as well.
 */
void hfst_set_program_name(const char* argv0, const char* version,
                           const char* wikipage);

/** 
 * print message @c s with parameters @c __VA_ARGS__ if @c debug is @c true,
 * and @c silent is @c false. 
 */
void debug_printf(const char* format, ...);

/** 
 * print message @c s with parameters @c __VA_ARGS__ if @c verbose is @c true,
 * and @c silent is @c false.
 */
void verbose_printf(const char* format, ...);

/**
 * print message @c s with parameters @c __VA_ARGS__ if @c silent is @c false.
 * This is meant for messages that are central to program's operation, for
 * informative messages use @c verbose_printf.
 */
void nonsilent_printf(const char* format, ...);

/**
 * print error message @a format with parameters @a VA_ARGS and exit if 
 * @a status is not 0.
 */
void hfst_error(int status, int errnum, const char* format, ...);
/**
 * print warning @a format with parameters @a VA_ARGS unless silent.
 */
void hfst_warning(const char* format, ...);
/**
 * print info @a format with parameters @a VA_ARGS unless silent.
 */
void hfst_info(const char* format, ...);
/**
 * print message @a format with parameters @a VA_ARGS if verbose.
 */
void hfst_verbose(const char* format, ...);
/**
 * print message @a format with parameters @a VA_ARGS if debugging.
 */
void hfst_debug(const char* format, ...);
/**
 * print message about oncoming read operation on @a filename if verbose.
 */
void hfst_reading(const char* filename);
/**
 * print message about starting @a nth iteration of main loop over @a name
 * while doing @a operation.
 */
void hfst_begin_processing(const char* name, size_t nth, const char* operation);
void hfst_begin_processing(const char* firstname, const char* secondname,
                           size_t nth, const char* op);
/**
 * print message about finished main loop.
 */
void hfst_end_processing();
/**
 * print message about smaller operation within main loop.
 */
void hfst_processing(const char* operation);
/**
 * print message about oncoming write operation on @a filename if verbose.
 */
void hfst_writing(const char* filename);
/**
 * close one of @c hfst_*ing operation with a message about successful finish.
 */
void hfst_done();
/**
 * close one of @c hfst_*ing operation with a message about failure.
 */
void hfst_failed();


/**
 *
 * @brief print standard usage message.
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
 * @sa http://www.gnu.org/software/womb/gnits/Help-Output.html
 */
void print_usage();

/**
 * @brief print standard version message.
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
 * @sa http://www.gnu.org/software/womb/gnits/Version-Output.html
 */
void print_version();

/**
 * @brief print standard short help message.
 * 
 * @sa http://www.gnu.org/software/womb/gnits/Help-Output.html#Help-Output
 */
void print_short_help();

#define KITWIKI_URL "https://kitwiki.csc.fi/twiki/bin/view/KitWiki/"
/**
 * @brief print link to wiki pages.
 */
void print_more_info();

/**
 * @brief print bug reporting message.
 */
void print_report_bugs();



#endif 
// vim: set ft=cpp.doxygen:
