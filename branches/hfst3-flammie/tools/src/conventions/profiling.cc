//! @file profiling.c
//! implementations of the profiling functions for hfst stuff.

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <errno.h>
#if HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif
#if HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
#endif

#include "commandline.h"
#include "profiling.h"

// globals and defaults
char* profile_file_name = 0;
FILE* profile_file = 0;
#if HAVE_CLOCK
clock_t profile_start = 0;
clock_t profile_end = 0;
#endif
#if HAVE_GETRUSAGE
struct rusage* profile_usage = 0;
#endif

void
hfst_print_profile_line()
  {
    if (profile_file == 0)
      {
        return;
      }
    // print headers if new file (otherwise we append, this will get us great
    // R-able tsv)
    if (hfst_ftell(profile_file) == 0L)
      {
        fprintf(profile_file, "name");
#   if HAVE_CLOCK
        fprintf(profile_file, ", clock");
#   endif
#   if HAVE_GETRUSAGE
        fprintf(profile_file, ", utime, stime, maxrss, ixrss, idrss, isrss, "
                "minflt, majflt, nswap, inblock, oublock, msgsnd, msgrcv, "
                "nsignals, nvcsw, nivcsw");
#   endif
        fprintf(profile_file, "\n");
      }
    fprintf(profile_file, "%s", program_short_name);
#   if HAVE_CLOCK
    clock_t profile_end = clock();
    fprintf(profile_file, "\t%f", ((float)(profile_end - profile_start))
                                               / CLOCKS_PER_SEC);
#   endif
#   if HAVE_GETRUSAGE
    struct rusage* usage = static_cast<struct rusage*>
        (malloc(sizeof(struct rusage)));
    errno = 0;
    int rv = getrusage(RUSAGE_SELF, usage);
    if (rv != -1)
      {
        fprintf(profile_file, "\t%lu.%lu\t%lu.%lu"
                "\t%ld\t%ld\t%ld"
                "\t%ld"
                "\t%ld\t%ld\t%ld"
                "\t%ld\t%ld"
                "\t%ld\t%ld"
                "\t%ld"
                "\t%ld\t%ld",
                usage->ru_utime.tv_sec, usage->ru_utime.tv_usec,
                usage->ru_stime.tv_sec, usage->ru_stime.tv_usec,
                usage->ru_maxrss, usage->ru_ixrss, usage->ru_idrss,
                usage->ru_isrss,
                usage->ru_minflt, usage->ru_majflt, usage->ru_nswap,
                usage->ru_inblock, usage->ru_oublock, 
                usage->ru_msgsnd, usage->ru_msgrcv,
                usage->ru_nsignals,
                usage->ru_nvcsw, usage->ru_nivcsw);
      }
    else
      {
        fprintf(profile_file, "\tgetrusage: %s", strerror(errno));
      }
#   endif
    fprintf(profile_file, "\n");
  }

