//! @file lexcio.cc
//!
//! @brief Implementation of basic printing for hfst-lexc text based interfaces
//!
//! @author Tommi A. Pirinen


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.


#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>
#include <map>

using std::string;
using std::map;

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <errno.h>

#if HAVE_DECL_ACCESS
#  include <unistd.h>
#endif

#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif


#include "lexcio.h"
#include "flex-utils.h"

extern char *yytext;

static bool hlexc_list_first = false;
static bool hlexc_list_end = true;

// global setting gunk
extern vector<string> infilenames;
extern FILE** infiles;
extern FILE* outfile;
extern string outfilename;
extern unsigned int verbosity;
#if TIMING
extern bool timing;
extern std::map<std::string, clock_t> timings_accumulated;
extern std::map<std::string, clock_t> timings_starts;
extern std::map<std::string, clock_t> timings_ends;
#endif
#if DEBUG
extern bool debug;
#endif
#if YYDEBUG
extern int hlexcdebug;
#endif
extern bool weighted;
extern bool isInfilesStdin;
extern bool isOutfileStdout;
FILE* message_out = stdout;

bool
lexc_writable(const char *path)
{
	FILE* readable = fopen(path, "r");
	if (readable != NULL)
	{
		fclose(readable);
#		if HAVE_DECL_ACCESS
		int rv = access(path, W_OK);
		if (rv == -1)
		{
			lexc_printf(PRINT_ERROR, 2,
						_("could not open %s for writing: %s\n"),
						path, strerror(errno));
			return false;
		}
		else
		{
			return true;
		}
#		else
		FILE* writable = lexc_fopen(path, "w");
		if (writable == NULL)
		{
			return false;
		}
		else
		{
			fclose(writable);
			return true;
		}
#		endif
	}
	else
	{
		// does not exist, assume writable
		return true;
	}
}

FILE*
lexc_fopen(const char *path, const char *mode)
{
	FILE* f = fopen(path, mode);
	if (NULL == f)
	{
		// TRANSLATORS: filename, strerror
		if (strcmp(mode, "r") == 0)
		{
			lexc_printf(PRINT_ERROR, 1,
						_("could not open %s for reading: %s\n"),
						path, strerror(errno));
			return NULL;
		}
		else if (strcmp(mode, "w") == 0)
		{
			lexc_printf(PRINT_ERROR, 2,
						_("could not open %s for writing: %s\n"),
						path, strerror(errno));
			return NULL;
		}
		else
		{
			lexc_printf(PRINT_ERROR, 1,
						_("could not open %s: %s\n"),
						path, strerror(errno));
			return NULL;
		}
	}
	return f;
}

bool
lexc_printf(unsigned int log_level, unsigned int id, 
		const char* format, ...)
{
	char* real_format = (char*)malloc(sizeof(char)*(strlen(format) +
				strlen(_("w%d: %s\n")) + 1));
	if (verbosity & log_level)
	{
		if (log_level & PRINT_WARNING_LEADER)
		{
			sprintf(real_format, _("W%d: %s"), id, format);
		}
		else if (log_level & PRINT_ERROR_LEADER)
		{
			sprintf(real_format, _("E%d: %s"), id, format);
		}
		else
		{
			real_format = strcpy(real_format, format);
		}
		va_list ap;
		va_start(ap, format);
		vfprintf(message_out, real_format, ap);
		va_end(ap);
	}
	free(real_format);
	return true;
}

bool
lexc_parser_printf(unsigned int log_level, unsigned int warn_id,
		const char* format, ...)
{
	if (verbosity & log_level)
	{
		char *filename_part = strdup_token_positions();
		char *error_token = strdup_token_part();
		char* real_format = (char*)malloc(sizeof(char) * 
				(strlen(format) + strlen(filename_part) + 
				 strlen(error_token) + 30));
		if (log_level & PRINT_WARNING_LEADER)
		{
			(void)sprintf(real_format, _("%s: W%d: %s %s\n"), 
					filename_part, warn_id,
					format, error_token);
		}
		else if (log_level & PRINT_ERROR_LEADER)
		{
			(void)sprintf(real_format, _("%s: E%d: %s %s\n"), 
					filename_part, warn_id,
					format, error_token);
		}
		else
		{
			(void)sprintf(real_format, "%s: E%d: %s %s\n", 
					filename_part, warn_id,
					format, error_token);
		}
		va_list ap;
		va_start(ap, format);
		vfprintf(message_out, real_format, ap);
		va_end(ap);
		free(filename_part);
		free(error_token);
		free(real_format);
	}
	return true;
}

bool
lexc_print_list_start(const char *text)
{
	lexc_printf(PRINT_UNQUIET, 0, _("%s:\n"), text);
	hlexc_list_first = true;
	return true;
}

bool
lexc_list_printf(const char *format, ...)
{
	char* real_format = (char*)malloc(sizeof(char)*strlen(format)+10);
	if (hlexc_list_first)
	{
		(void)strcpy(real_format, format);
	}
	else
	{
		(void)sprintf(real_format, _(", %s"), format);
	}
	va_list ap;
	va_start(ap, format);
	(void)vfprintf(message_out, real_format, ap);
	va_end(ap);
	free(real_format);
	hlexc_list_first = false;
	hlexc_list_end = false;
	return true;
}

bool
lexc_print_list_end(const char* text)
{
	if (!hlexc_list_end)
	{
		if (text != NULL)
		{
			lexc_printf(PRINT_UNQUIET, 0, _("%s\n"), text);
		}
		else
		{
			lexc_printf(PRINT_UNQUIET, 0, _("\n"));
		}
		hlexc_list_end = true;
		hlexc_list_first = true;
	}
	return true;
}

void
lexc_timer_initialize(const char* name)
{
#	if TIMING
	timings_starts[name] = 0;
	timings_ends[name] = 0;
	timings_accumulated[name] = 0;
#	else
	;
#	endif
}

void
lexc_timer_start(const char* name)
{
#	if TIMING
	assert(timings_starts.find(name) != timings_starts.end());
	timings_starts[name] = clock();
#	else
	;
#	endif
}

void
lexc_timer_end(const char* name)
{
#	if TIMING
	assert(timings_ends.find(name) != timings_ends.end());
	assert(timings_accumulated.find(name) != timings_accumulated.end());
	timings_ends[name] = clock();
	timings_accumulated[name] += timings_ends[name] - timings_starts[name];
	timings_starts[name] = clock();
#	else
	;
#	endif
}

bool
lexc_timing_printf(const char* name, const char* format, ...)
{
#	if TIMING
	assert(timings_starts.find(name) != timings_starts.end());
	assert(timings_ends.find(name) != timings_ends.end());
	assert(timings_accumulated.find(name) != timings_accumulated.end());
	if (verbosity & PRINT_TIMING)
	{
		char* real_format = (char*)malloc(sizeof(char)*strlen(format)+
				30);
		// TRANSLATORS: timing description, time, s for seconds
		(void)sprintf(real_format,
				_("%s: %f s\n"), format, 
				static_cast<double>(timings_accumulated[name])/CLOCKS_PER_SEC);
		va_list ap;
		va_start(ap, format);
		vfprintf(message_out, real_format, ap);
		va_end(ap);
		return true;
	}
#	else
	return false;
#	endif
	return true;
}

bool
lexc_xducer_printf(const Xducer& x, const char* format, ...)
{
	if (verbosity & PRINT_DEBUG)
	{
		va_list ap;
		va_start(ap, format);
		vfprintf(message_out, format, ap);
		va_end(ap);
		x.print(std::cerr);
	}
	return true;
}

bool
lexc_xymbol_set_printf(const XymbolSet& xs, const char* format, ...)
{
	if (verbosity & PRINT_DEBUG)
	{
		va_list ap;
		va_start(ap, format);
		vfprintf(message_out, format, ap);
		va_end(ap);
		for (XymbolSet::const_iterator x = xs.begin();
				x != xs.end();
				++x)
		{
			x->dumpDebugString();
		}
	}
	return true;
}
// vim: set ft=cpp.doxygen:
