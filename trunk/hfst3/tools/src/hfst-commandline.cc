/**
 * @file hfst-commandline.cc
 *
 * @brief some utils for handling common tasks in command line programs.
 * @todo maybe replace with glib?
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <errno.h>

#include "hfst-commandline.h"
#include <hfst2/hfst.h>

// defaults
bool weighted = true;
bool verbose = false;
bool debug = false;
FILE *message_out = stdout;

// deprecated; everything's compatible
int get_compatible_fst_format(std::istream& is1, std::istream& is2) {
    assert(false);
    return -1;
}


// string functions
char*
hfst_strdup(const char *s)
{
	errno = 0;
	char *rv = strdup(s);
	if (NULL == rv)
	{
		if (errno != 0)
		{
			fprintf(message_out, "strdup failed: %s\n", strerror(errno));
		}
		else
		{
			fprintf(message_out, "strdup failed\n");
		}
		exit(EXIT_FAILURE);
	}
	return rv;
}

char*
hfst_strndup(const char *s, size_t n)
{
  errno = 0;
# if HAVE_STRNDUP
  char* rv = strndup(s, n);
  if (NULL == rv)
    {
      if (errno != 0)
        {
          fprintf(message_out, "strndup failed: %s\n", strerror(errno));
        }
      else
        {
          fprintf(message_out, "strndup failed\n");
        }
      exit(EXIT_FAILURE);
    }
# else
  char* rv = static_cast<char*>(calloc(sizeof(char), n + 1));
  rv = static_cast<char*>(memcpy(rv, s, n));
# endif
  return rv;
}

double
hfst_strtoweight(const char *s)
{
	errno = 0;
	char *endptr;
	double rv = strtod(s, &endptr);
	if ((endptr == s) && (0 == rv))
	{
		if (errno != 0)
		{
			fprintf(message_out, "string %s not a weight: %s\n",  s,
					strerror(errno));
		}
		else
		{
			fprintf(message_out, "string %s not a weight\n", s);
		}
	}
	return rv;
}

int
hfst_strtonumber(const char *s, bool *infinite)
{
	errno = 0;
	char *endptr;
	double rv = strtod(s, &endptr);
	if ((endptr == s) && (0 == rv))
	{
		if (errno != 0)
		{
			fprintf(message_out, "string %s not a weight: %s\n",  s,
					strerror(errno));
		}
		else
		{
			fprintf(message_out, "string %s not a weight\n", s);
		}
	}
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

// file functions
FILE*
hfst_fopen(const char* filename, const char* mode)
{
	errno = 0;
	FILE *f = fopen(filename, mode);
	if (NULL != f)
	{
		return f;
	}
	else
	{
		if (errno != 0)
		{
			fprintf(message_out, "Could not open %s: %s\n", filename,
					strerror(errno));
		}
		else
		{
			fprintf(message_out, "Could not open %s\n", filename);
		}
		exit(EXIT_FAILURE);
	}
}


void
hfst_fseek(FILE* stream, long offset, int whence)
{
	errno = 0;
	if (fseek(stream, offset, whence) != 0)
	{
		fprintf(message_out, "fseek failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
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
		fprintf(message_out, "ftell failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

ssize_t
hfst_getline(char** lineptr, size_t* n, FILE* stream)
{
# if HAVE_GETLINE
  errno = 0;
  ssize_t rv = getline(lineptr, n, stream);
  if ((rv < 0) && errno)
    {
      fprintf(message_out, "getline failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
# else
# define GETLINE_BUFFER 65535
  size_t max_line = *n;
  ssize_t rv = 0;
  if (*lineptr == NULL)
    {
      *lineptr = static_cast<char*>(calloc(sizeof(char), GETLINE_BUFFER));
    }
  else
    {
      free(*lineptr);
      *lineptr = static_cast<char*>(calloc(sizeof(char), GETLINE_BUFFER));
    }
  max_line = GETLINE_BUFFER;
  *lineptr = fgets(*lineptr, max_line, stream);
  if (*lineptr == NULL)
    {
      rv = -1;
    }
  else
    {
      *n = rv = strlen(*lineptr);
    }
# endif
  return rv;
}

// mem functions
void*
hfst_malloc(size_t size)
{
	errno = 0;
	void* rv = malloc(size);
	if (NULL != rv)
	{
		return rv;
	}
	else
	{
		fprintf(message_out, "malloc failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}
void*
hfst_calloc(size_t nmemb, size_t size)
{
	errno = 0;
	void* rv = calloc(nmemb, size);
	if (NULL != rv)
	{
		return rv;
	}
	else
	{
		fprintf(message_out, "calloc failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void*
hfst_realloc(void* ptr, size_t size)
{
	errno = 0;
	void* rv = realloc(ptr, size);
	if (NULL != rv)
	{
		return rv;
	}
	else
	{
		fprintf(message_out, "realloc failed %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}


//
void
print_short_help(const char* program_name)
{
	fprintf(message_out, "Try ``%s --help'' for more information.\n",
			program_name);
}
