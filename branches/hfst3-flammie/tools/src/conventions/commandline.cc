/**
 * @file hfst-commandline.cc
 *
 * @brief some utils for handling common tasks in command line programs.
 * @todo maybe replace with glib?
 */
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


#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cassert>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if HAVE_LOCALE_H
#  include <locale.h>
#endif
#if HAVE_LANGINFO_H
#  include <langinfo.h>
#endif
#if HAVE_READLINE_READLINE_H
#  include <readline/readline.h>
#endif
#if HAVE_READLINE_HISTORY_H
#  include <readline/history.h>
#endif
#if HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif
#if HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
#endif
#if HAVE_NCURSES_H
#include <ncurses.h>
#endif

#include <hfst.hpp>

#include "conventions/commandline.h"
#include "conventions/portability.h"

// specific printf's wrapped in conditions
static
void
print_color(FILE* out, const char* colors)
  {
    if (print_colors)
      {
        fprintf(out, "%s", colors);
      }
  }

static
void
print_program_name(FILE* out)
  {
    print_color(out, COLOR_PROGRAM_NAME);
    fprintf(out, "%s: ", program_invocation_name);
    print_color(out, COLOR_RESET);
  }

void
debug_save_transducer(hfst::HfstTransducer& t, const char* name)
{
    if (debug)
      {
        char* debug_name = static_cast<char*>(malloc(sizeof(char)*(strlen("DEBUG: " + strlen(name) + 1 ))));
        if ((sprintf(debug_name, "DEBUG %s", name)) > 0 ) 
          {
            t.set_name(debug_name);
          }
        else
          {
            t.set_name("DEBUG <error in sprintf>");
          }
        hfst::HfstOutputStream debugOut(name, t.get_type());
        //debugOut.open();
        debug_printf("saving current transducer to %s\n",
                 program_name, name);
        debugOut << t;
        debugOut.close();
      }
}


void
debug_printf(const char* fmt, ...)
{
  if (debug)
    {
      fprintf(stderr, "DEBUG: ");
      va_list ap;
      va_start(ap, fmt);
      vfprintf(stderr, fmt, ap);
      va_end(ap);
      fprintf(stderr, "\n");
    }
}

void
verbose_printf(const char* fmt, ...)
{
  if (verbose)
    {
      va_list ap;
      va_start(ap, fmt);
      vfprintf(message_out, fmt, ap);
      va_end(ap);
    }
}

void
hfst_error(int status, int errnum, const char* fmt, ...)
  {
    print_program_name(stderr);
    print_color(stderr, COLOR_ERROR);
    fprintf(stderr, "ERROR: ");
    print_color(stderr, COLOR_RESET);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    if (errnum != 0)
      {
        fprintf(stderr, "%s\n", strerror(errnum));
      }
    if (status != 0)
      {
        exit(status);
      }
  }

void
hfst_warning(const char* fmt, ...)
  {
    if (!silent)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_WARNING);
        fprintf(message_out, "WARN: ");
        print_color(message_out, COLOR_RESET);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(message_out, fmt, ap);
        va_end(ap);
        fprintf(message_out, "\n");
      }
  }

void
hfst_info(const char* fmt, ...)
  {
    if (!silent)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_INFO);
        fprintf(message_out, "*** ");
        print_color(message_out, COLOR_RESET);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(message_out, fmt, ap);
        va_end(ap);
        fprintf(message_out, "\n");
      }
  }

void
hfst_verbose(const char* fmt, ...)
  {
    if (!silent && verbose)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_VERBOSE);
        fprintf(message_out, "*** ");
        print_color(message_out, COLOR_RESET);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(message_out, fmt, ap);
        va_end(ap);
        fprintf(message_out, "\n");
      }
  }

// string functions
double
hfst_strtoweight(const char *s)
{
    errno = 0;
    char *endptr;
    double rv = strtod(s, &endptr);
    if (*endptr == '\0')
      {
        return rv;
      }
    else
      {
        hfst_error(EXIT_FAILURE, errno, "%s not a weight", s);
        return rv;
      }
}

int
hfst_strtonumber(const char *s, bool *infinite)
{
    *infinite = false;
    errno = 0;
    char *endptr;
    double rv = strtod(s, &endptr); 
    if (*endptr == '\0')
      {
        if (std::isinf(rv) && infinite != NULL)
        {
            *infinite = true;
            return std::signbit(rv);
        }
        else if (rv > INT_MAX)
        {
            return INT_MAX;
        }
        else if (rv < INT_MIN)
        {
            return INT_MIN;
        }
        return (int)floor(rv);
      }
    else
    {
        hfst_error(EXIT_FAILURE, errno, "%s not a number",  s);
        return rv;
    }
}

unsigned long
hfst_strtoul(char *s, int base)
{
    errno = 0;
    char *endptr;
    unsigned long rv = strtoul(s, &endptr, base);
    if (*endptr == '\0')
      {
        return rv;
      }
    else
      {
        hfst_error(EXIT_FAILURE, errno, "%s is not a valid unsigned number string",
              s);
        return rv;
      }
}

long
hfst_strtol(char *s, int base)
{
    errno = 0;
    char *endptr;
    long rv = strtol(s, &endptr, base);
    if (*endptr == '\0')
      {
        return rv;
      }
    else
      {
        hfst_error(EXIT_FAILURE, errno, "%s is not a valid signed number string", s);
        return rv;
      }
}

hfst::ImplementationType
hfst_parse_format_name(const char* s)
{
    hfst::ImplementationType rv; // = hfst::UNSPECIFIED_TYPE;
    if (strcasecmp(s, "sfst") == 0)
      {
        rv = hfst::SFST_TYPE;
      }
    else if ((strcasecmp(s, "openfst-tropical") == 0) ||
             (strcasecmp(s, "ofst-tropical") == 0))
      {
        rv = hfst::TROPICAL_OPENFST_TYPE;
      }
    else if ((strcasecmp(s, "openfst-log") == 0) ||
             (strcasecmp(s, "ofst-log") == 0))
      {
        rv = hfst::LOG_OPENFST_TYPE;
      }
    else if ((strcasecmp(s, "openfst") == 0) ||
             (strcasecmp(s, "ofst") == 0))
      {
        rv = hfst::TROPICAL_OPENFST_TYPE;
        warning(0, 0, "Ambiguous format name %s, guessing openfst-tropical",
                s);
      }
    else if (strcasecmp(s, "foma") == 0)
      {
        rv = hfst::FOMA_TYPE;
      }
    else if ((strcasecmp(s, "optimized-lookup-unweighted") == 0) ||
             (strcasecmp(s, "olu") == 0))
      {
        rv = hfst::HFST_OL_TYPE;
      }
    else if ((strcasecmp(s, "optimized-lookup-weighted") == 0) ||
             (strcasecmp(s, "olw") == 0))
      {
        rv = hfst::HFST_OLW_TYPE;
      }
    else if ((strcasecmp(s, "optimized-lookup") == 0) ||
             (strcasecmp(s, "ol") == 0))
      {
        rv = hfst::HFST_OLW_TYPE;
        warning(0, 0, "Ambiguous format name %s, guessing "
                "optimized-lookup-weighted", s);
      }
    else
      {
        hfst_error(EXIT_FAILURE, 0, "Could not parse format name from string %s",
              s);
        rv = hfst::UNSPECIFIED_TYPE;
        return rv;
      }
    return rv;
}

char*
hfst_strformat(hfst::ImplementationType format)
{
  switch (format)
    {
    case hfst::SFST_TYPE:
      return strdup("SFST (1.4 compatible)");
      break;
    case hfst::TROPICAL_OPENFST_TYPE:
      return strdup("OpenFST, std arc, tropical semiring");
      break;
    case hfst::LOG_OPENFST_TYPE:
      return strdup("OpenFST, std arc, log semiring");
      break;
    case hfst::FOMA_TYPE:
      return strdup("foma");
      break;
    case hfst::HFST_OL_TYPE:
      return strdup("Hfst's lookup optimized, unweighted");
      break;
    case hfst::HFST_OLW_TYPE:
      return strdup("Hfst's lookup optimized, weighted");
      break;
    case hfst::HFST2_TYPE:
      return strdup("Hfst 2 legacy (deprecated)");
      break;
    case hfst::ERROR_TYPE:
    case hfst::UNSPECIFIED_TYPE:
    default:
      return strdup("ERROR (not a HFST supported transducer)");
      exit(1);
    }

}
// file functions
FILE*
hfst_fopen(const char* filename, const char* mode)
{
    if (strcmp(filename, "-") == 0)
      {
        if (strcmp(mode, "r") == 0)
          {
            return stdin;
          }
        else if (strcmp(mode, "w") == 0)
          {
            return stdout;
          }
      }
    errno = 0;
    FILE *f = fopen(filename, mode);
    if (NULL != f)
    {
        return f;
    }
    else
    {
        hfst_error(EXIT_FAILURE, errno, "Could not open '%s'. ", filename);
        return NULL;
    }
}


void
hfst_fseek(FILE* stream, long offset, int whence)
{
    errno = 0;
    if (fseek(stream, offset, whence) != 0)
    {
        hfst_error(EXIT_FAILURE, errno, "fseek failed");
    }
}

unsigned long 
hfst_ftell(FILE* stream)
{
    errno = 0;
    long offset = ftell(stream);
    if (-1 != offset)
    {
        return (unsigned long)offset;
    }
    else
    {
        hfst_error(EXIT_FAILURE, errno, "ftell failed");
        return -1;
    }
}

size_t
hfst_fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
  errno = 0;
  size_t rv = fread(ptr, size, nmemb, stream);
  if ((rv < nmemb) && (ferror(stream)))
    {
      hfst_error(EXIT_FAILURE, errno, "fread failed");
    }
  return rv;
}

size_t
hfst_fwrite(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
  errno = 0;
  size_t rv = fwrite(ptr, size, nmemb, stream);
  if ((rv < nmemb) || (ferror(stream)))
    {
      hfst_error(EXIT_FAILURE, errno, "fwrite failed");
    }
  return rv;
}

FILE*
hfst_tmpfile()
{
  errno = 0;
  FILE* rv = tmpfile();
  if (NULL == rv)
    {
      hfst_error(EXIT_FAILURE, errno, "tmpfile failed");
    }
  return rv;
}

int
hfst_close(int fd)
{
  errno = 0;
  int rv = close(fd);
  if (rv == -1)
    {
      hfst_error(EXIT_FAILURE, errno, "close failed");
    }
  return rv;
}

int
hfst_open(const char* pathname, int flags)
{
  errno = 0;
  int rv = open(pathname, flags);
  if (rv == -1)
    {
      hfst_error(EXIT_FAILURE, errno, "open failed");
    }
  return rv;
}

ssize_t
hfst_read(int fd, void* buf, size_t count)
{
  if (count > SSIZE_MAX)
    {
      hfst_error(EXIT_FAILURE, 0, "cannot read %zu bytes in one read(2)", count);
    }
  errno = 0;
  ssize_t rv = read(fd, buf, count);
  if (rv == -1)
    {
      hfst_error(EXIT_FAILURE, errno, "read failed");
    }
  return rv;
}

ssize_t
hfst_write(int fd, const void* buf, size_t count)
{
  errno = 0;
  ssize_t rv = write(fd, buf, count);
  if (rv == -1)
    {
      hfst_error(EXIT_FAILURE, errno, "write failed");
    }
  return rv;
}

int
hfst_mkstemp(char* templ)
{
  errno = 0;
  int rv = mkstemp(templ);
  if (rv == -1)
    {
      hfst_error(EXIT_FAILURE, errno, "mkstemp failed");
    }
  return rv;
}

int
hfst_remove(const char* filename)
  {
    errno = 0;
    int rv = remove(filename);
    if (rv == -1)
      {
        hfst_error(EXIT_FAILURE, errno, "remove %s failed", filename);
      }
    return rv;
  }

// str functions

char*
hfst_strdup(const char* s)
  {
    char* rv = strdup(s);
    if (rv == NULL)
      {
        hfst_error(EXIT_FAILURE, errno, "strdup failed");
      }
    return rv;
  }

char*
hfst_strndup(const char* s, size_t n)
  {
    char* rv = strndup(s, n);
    if (rv == NULL)
      {
        hfst_error(EXIT_FAILURE, errno, "strndup failed");
      }
    return rv;
}


ssize_t
hfst_getdelim(char** lineptr, size_t* n, int delim, FILE* stream)
  {
    errno = 0;
    ssize_t rv = -1;
    rv = getdelim(lineptr, n, delim, stream);
    if ((rv < 0) && errno)
      {
        hfst_error(EXIT_FAILURE, errno, "getdelim failed");
      }
    return rv;
  }

ssize_t
hfst_getline(char** lineptr, size_t* n, FILE* stream)
{
  errno = 0;
  ssize_t rv = -1;
  rv = getline(lineptr, n, stream);
  if ((rv < 0) && errno)
    {
      hfst_error(EXIT_FAILURE, errno, "getline failed");
    }
  return rv;
}


char*
hfst_readline(const char* prompt)
{
  return readline(prompt);
}

char*
hfst_setlocale()
{
#if HAVE_SETLOCALE
    char* rv = setlocale(LC_ALL, "");
    if (NULL == rv)
      {
        hfst_error(EXIT_FAILURE, errno, "Unable to set locale for character "
              "settings");
      }
#if HAVE_NL_LANGINFO
    char* charset = nl_langinfo(CODESET);
    if (strcmp(charset, "UTF-8") != 0)
      {
        hfst_error(EXIT_FAILURE, 0, "Character set %s not supported; exiting to "
              "avoid data corruption\n"
              "please set up UTF-8 locale instead (e.g. LC_ALL=en_GB.utf8)",
              charset);
      }
#endif
#else
    char* rv = NULL;
#endif
    return rv;
}

// mem functions

void*
hfst_malloc(size_t s)
  {
    void* rv = malloc(s);
    if ((rv==NULL) && (s > 0))
      {
        hfst_error(EXIT_FAILURE, errno, "malloc failed");
      }
    return rv;
  }

void*
hfst_realloc(void* ptr, size_t s)
{
  void* rv = realloc(ptr, s);
  if ((rv==NULL) && (s > 0))
    {
      hfst_error(EXIT_FAILURE, errno, "realloc failed");
    }
  return rv;
}

// customized default printouts for HFST tools
void
hfst_set_program_name(const char* argv0, const char* version_vector,
                      const char* wikiname)
{
  set_program_name(argv0);
  hfst_tool_version = hfst_strdup(version_vector);
  hfst_tool_wikiname = hfst_strdup(wikiname);
}

void
hfst_init_commandline(const char* argv0, const char* version,
                      const char* wikiname)
  {
    hfst_setlocale();
    hfst_set_program_name(argv0, version, wikiname);
    if (isatty(1) && isatty(2))
      {
        print_colors = true;
      }
    else
      {
        print_colors = false;
      }
  }

void
print_short_help()
{
    fprintf(message_out, "Try ``%s --help'' for more information.\n",
             program_short_name);
}

// print web site reference
void
print_more_info()
{
    fprintf(message_out, "%s home page: \n"
            "<" KITWIKI_URL "/%s>\n",
            program_name, hfst_tool_wikiname);
    fprintf(message_out, "General help using HFST software: \n"
            "<" KITWIKI_URL "/HfstHome>\n");
}

// print version message
void
print_version()
{
  // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
    fprintf(message_out, "%s %s (" PACKAGE_STRING "; " HFST_STRING ")\n"
             "Copyright (C) 2012 University of Helsinki,\n"
             "License GPLv3: GNU GPL version 3 "
             "<http://gnu.org/licenses/gpl.html>\n"
             "This is free software: you are free to change and "
             "redistribute it.\n"
             "There is NO WARRANTY, to the extent permitted by law.\n",
             program_short_name, hfst_tool_version);
}

void
print_report_bugs()
{
  fprintf(message_out, "Report bugs to <" PACKAGE_BUGREPORT "> "
          "or directly to our bug tracker at:\n"
          "<https://sourceforge.net/tracker/?atid=1061990&group_id=224521&func=browse>\n");
}


void
extend_options_getenv(int* argc, char*** argv)
{
  char* hfstopts = getenv("HFST_OPTIONS");
  if (NULL == hfstopts)
    {
      return;
    }
  char* p = hfstopts;
  unsigned int spaces = 0;
  while (*p != '\0')
    {
      if (' ' == *p)
        {
          spaces++;
        }
      p++;
    }
  // we cannot realloc argv since it's magic
  char** new_argv = static_cast<char**>(hfst_malloc(sizeof(char*) * 
                                                    (*argc + spaces + 1)));
  new_argv = static_cast<char**>(memcpy(new_argv, *argv, sizeof(char*)**argc));
  // there's this magic stuff with *argv that we shouldn't free it still
  *argv = new_argv;
  char* new_arg = strtok(hfstopts, " ");
  while (new_arg != NULL)
    {
      char** new_arg_spot = ((*argv) + *argc);
      *new_arg_spot = hfst_strdup(new_arg);
      (*argc)++;
      new_arg = strtok(NULL, " ");
    }
}

void
hfst_print_profile_line()
  {
    if (profile_file == 0)
      {
        return;
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
