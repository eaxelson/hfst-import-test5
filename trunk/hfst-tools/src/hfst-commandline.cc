/**
 * @file hfst-commandline.cc
 *
 * @brief some utils for handling common tasks in command line programs.
 * @todo maybe replace with glib?
 */

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

/* 
 * Returns 0, 1 or 2 if transducers are of the same type and -1
 * if they are not. 
 */
int get_compatible_fst_format(std::istream& is1, std::istream& is2) {
	int type1 = HFST::read_format(is1);
	int type2 = HFST::read_format(is2);

	if (type1 == SFST_FORMAT && type2 == SFST_FORMAT)
	{
		return SFST_FORMAT;
	}
	else if (type1 == 1 && type2 == 1)
	{
		return 1;
	}
	else if (type1 == OPENFST_FORMAT && type2 == OPENFST_FORMAT)
	{
		return OPENFST_FORMAT;
	}
	else if (type1 == EOF_FORMAT && type2 == EOF_FORMAT)  // EOF = - 2
	{
		return EOF_FORMAT;
	}
	else {
		fprintf(message_out, "ERROR: Transducer type mismatch\n");
		return FORMAT_ERROR;
	}
}


// string functions
char*
hfst_strdup(const char *s)
{
	errno = 0;
	char *rv = strdup(s);
	if (NULL == s)
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
