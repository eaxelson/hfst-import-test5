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
#include <errno.h>

#include "hfst-commandline.h"

#ifndef HAVE_ERROR_AT_LINE
void error_at_line(int status, int errnum, const char* filename, 
                   unsigned int linenum, const char* fmt, ...)
{
  fprintf(stderr, "%s.%u: ", filename, linenum);
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  if (errnum != 0)
    {
      fprintf(stderr, "%s", strerror(errnum));
    }
  if (status != 0)
    {
      exit(status);
    }
}
#endif

// deprecated; everything's compatible
int get_compatible_fst_format(std::istream& , std::istream& ) {
    assert(false);
    return -1;
}

// specific printf's wrapped in conditions
void
debug_save_transducer(hfst::HfstTransducer t, const char* name)
{
    if (debug)
      {
        hfst::HfstOutputStream debugOut(name, t.get_type());
        //debugOut.open();
        debug_printf("*** DEBUG (%s): saving current transducer to %s\n",
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
      fprintf(stderr, "\nDEBUG: ");
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
        error(EXIT_FAILURE, errno, "%s not a weight", s);
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
        error(EXIT_FAILURE, errno, "%s not a number",  s);
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
        error(EXIT_FAILURE, errno, "%s not a weight", s);
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
        rv = hfst::TROPICAL_OFST_TYPE;
      }
    else if ((strcasecmp(s, "openfst-log") == 0) ||
             (strcasecmp(s, "ofst-log") == 0))
      {
        rv = hfst::LOG_OFST_TYPE;
      }
    else if ((strcasecmp(s, "openfst") == 0) ||
             (strcasecmp(s, "ofst") == 0))
      {
        rv = hfst::TROPICAL_OFST_TYPE;
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
        error(EXIT_FAILURE, 0, "Could not parse format name from string %s",
              s);
      }
    return rv;
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
        error(EXIT_FAILURE, errno, "Could not open %s", filename);
        return NULL;
    }
}


void
hfst_fseek(FILE* stream, long offset, int whence)
{
    errno = 0;
    if (fseek(stream, offset, whence) != 0)
    {
        error(EXIT_FAILURE, errno, "fseek failed");
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
        error(EXIT_FAILURE, errno, "ftell failed");
        return -1;
    }
}

#ifndef HAVE_STRNDUP
char*
hfst_strndup(const char* s, size_t n)
{
  char* rv = static_cast<char*>(malloc(sizeof(char)*n+1));
  if (rv == NULL)
    {
      fprintf(stderr, "strndup failed to malloc\n");
      exit(EXIT_FAILURE);
    }
  rv = static_cast<char*>(memcpy(rv, s, n));
  rv[n] = '\0';
  return rv;
}
#endif

ssize_t
hfst_getline(char** lineptr, size_t* n, FILE* stream)
{
  errno = 0;
  ssize_t rv = -1;
#if HAVE_GETLINE
  rv = getline(lineptr, n, stream);
  if ((rv < 0) && errno)
    {
      error(EXIT_FAILURE, errno, "getline failed");
    }
#else
#define MAX_LEN 4196
  size_t nn = *n;
  if (nn == 0)
    {
      nn = MAX_LEN;
    }
  if (*lineptr == NULL)
    {
      *lineptr = static_cast<char*>(malloc(nn));
      if (*lineptr == NULL)
        {
           error(EXIT_FAILURE, errno, "getline failed to malloc");
        }
    }
  *lineptr = fgets(*lineptr, nn, stream);
  if (*lineptr == NULL)
    {
      error(EXIT_FAILURE, errno, "getline fread failed");
    }
  rv = strlen(*lineptr);
#endif
  return rv;
}

// customized default printouts for HFST tools
void
hfst_set_program_name(const char* argv0, const char* version_vector,
                      const char* wikiname)
{
#if HAVE_SET_PROGRAM_NAME
  set_program_name(argv0);
#else
// this's gnulib
  const char *slash;
  const char *base;
  slash = strrchr (argv0, '/');
  base = (slash != NULL ? slash + 1 : argv0);
  if (base - argv0 >= 7 && strncmp (base - 7, "/.libs/", 7) == 0)
    {
      argv0 = base;
      if (strncmp (base, "lt-", 3) == 0)
        {
          argv0 = base + 3;
        }
    }
  program_name = hfst_strdup(argv0);
#endif
  hfst_tool_version = hfst_strdup(version_vector);
  hfst_tool_wikiname = hfst_strdup(wikiname);
}

void
print_short_help()
{
    fprintf(message_out, "Try ``%s --help'' for more information.\n",
             program_name);
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
    fprintf(message_out, "%s %s (" PACKAGE_STRING "\n"
             "Copyright (C) 2010 University of Helsinki,\n"
             "License GPLv3: GNU GPL version 3 "
             "<http://gnu.org/licenses/gpl.html>\n"
             "This is free software: you are free to change and "
             "redistribute it.\n"
             "There is NO WARRANTY, to the extent permitted by law.\n",
             program_name, hfst_tool_version);
}

void
print_report_bugs()
{
  fprintf(message_out, "Report bugs to <" PACKAGE_BUGREPORT "> "
          "or directly to our bug tracker at:\n"
          "<https://sourceforge.net/tracker/?atid=1061990&group_id=224521&func=browse>\n");
}


