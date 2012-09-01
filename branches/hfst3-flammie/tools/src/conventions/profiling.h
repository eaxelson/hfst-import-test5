//! @file profiling.h
//! @brief Hfst tool conventions for input and output reading.
//! all to an object that could never be more than a singleton.

#ifndef GUARD_profiling_h
#define GUARD_profiling_h 1

#if HAVE_CONFIG_H
#  include <config.h>
#endif

/**
 * @brief name for profiling file.
 */
extern char* profile_file_name;
/**
 * @brief set @a profile_fle to target of profiling info writes
 */
extern FILE* profile_file;
#if HAVE_CLOCK
/**
 * @brief set @a profile_start to @c clock() when starting profiling.
 */
extern clock_t profile_start;
/**
 * @brief end time for last profiling run.
 */
extern clock_t profile_end;
#endif
#if HAVE_GETRUSAGE
/**
 * @brief usage data for profiling.
 */
extern struct rusage* profile_usage;
#endif

/**
 * @brief print profiling results to @a stream in neat TSV format.
 *  The results of the profiling match the fields of struct rusage from
 *  sys/resource.h, except for first field which is accumulated @c clock() time.
 */
void hfst_print_profile_line();

#endif
// vim: set ft=cpp.doxygen:

