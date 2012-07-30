//! @file portability.cc
//! @brief implementations of GNU functions for worse OSes and systems.

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <errno.h>

#include "conventions/portability.h"

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
  fprintf(stderr, "\n");
  if (status != 0)
    {
      exit(status);
    }
}
#endif

#ifndef HAVE_ERROR
void
error(int status, int errnum, const char* fmt, ...)
{
  fprintf(stderr, "%s: ", program_name);
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  if (errnum != 0)
    {
      fprintf(stderr, "%s", strerror(errnum));
    }
  fprintf(stderr, "\n");
  if (status != 0)
    {
      exit(status);
    }
}
#endif
#ifndef HAVE_WARNING
void
warning(int status, int errnum, const char* fmt, ...)
{
  fprintf(stderr, "%s: warning: ", program_name);
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  if (errnum != 0)
    {
      fprintf(stderr, "%s", strerror(errnum));
    }
  fprintf(stderr, "\n");
  if (status != 0)
    {
      exit(status);
    }
}
#endif

#ifndef HAVE_STRNDUP
char*
strndup(const char* s, size_t n)
  {
    char* rv = static_cast<char*>(malloc(sizeof(char)*n+1));
    if (rv == NULL)
      {
        return rv;
      }
    rv = static_cast<char*>(memcpy(rv, s, n));
    if (rv == NULL)
      {
        return rv;
      }
    rv[n] = '\0';
    return rv;
  }
#endif

#ifndef HAVE_GETDELIM
ssize_t
getdelim(char** lineptr, size_t* n, int delim, FILE* stream)
  {
#define MAX_GETDELIM 8192
    size_t nn = *n;
    if (nn == 0)
      {
        nn = MAX_GETDELIM;
      }
    if (*lineptr == NULL)
      {
        *lineptr = static_cast<char*>(malloc(nn));
        if (*lineptr == NULL)
          {
            return -1;
          }
      }
    char* currptr = *lineptr;
    *currptr = '\0';
    size_t readin = 0;
    int readbyte = 0;
    while ((readbyte = fgetc(stream)) != EOF)
      {
        *currptr = static_cast<char>(readbyte);
        currptr++;
        readin++;
        if (readin >= nn)
          {
            currptr--;
            *currptr = '\0';
            return -1;
          }
        else if (readbyte == delim)
          {
            *currptr = '\0';
            return strlen(*lineptr);
          }
      }
    if (readbyte == EOF)
      {
        *currptr = '\0';
        return -1;
      }
    if (*lineptr == NULL)
      {
        return -1;
      }
    return strlen(*lineptr);
  }
#endif

#ifndef HAVE_GETLINE
ssize_t
getline(char** lineptr, size_t* n, FILE* stream)
  {
#define MAX_GETLINE 4096
    size_t nn = *n;
    if (nn == 0)
      {
        nn = MAX_GETLINE;
      }
    if (*lineptr == NULL)
      {
        *lineptr = static_cast<char*>(malloc(nn));
        if (*lineptr == NULL)
          {
             return -1;
          }
      }
    *lineptr = fgets(*lineptr, nn, stream);
    if (*lineptr == NULL)
      {
        return -1;
      }
    return strlen(*lineptr);
  }
#endif

#ifndef HAVE_READLINE
char*
readline(const char* prompt)
{
  fprintf(message_out, "%s", prompt);
  char* line = 0;
  size_t len = 0;
  if (getline(&line, &len, stdin) == -1)
    {
      return 0;
    }
  return line;
}
#endif

#ifndef HAVE_SET_PROGRAM_NAME
void
set_program_name(const char* argv0)
  {
#   if HAVE_DECL_PROGRAM_INVOCATION_NAME
    program_name = strdup(program_invocation_name);
#   endif
#   if HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME
    program_short_name = strdup(program_invocation_short_name);
#   endif
    if ((program_name != NULL) && (program_short_name != NULL))
      {
        return;
      }
    // bit like in gnulib, but without that horrible gnulib-tool
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
            base += 3;
          }
      }
    program_name = strdup(argv0);
    program_short_name = strdup(base);
  }
#endif

#ifndef HAVE_GETRUSAGE
int
getrusage(int who, void* usage)
  {
    return -1;
  }
#endif
