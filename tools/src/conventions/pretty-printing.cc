//! @file pretty-printing.c
//! Hfst's pretty printing functions.

#include <cstdarg>
#include <cstdio>

#include "commandline.h"
#include "pretty-printing.h"

// globals and defaults
bool verbose = false;
bool silent = false;
bool debug = false;
FILE* message_out = stdout;
bool auto_colors = true;
bool print_colors = false;
#ifndef HAVE_DECL_PROGRAM_NAME
char* program_name = 0;
#endif
#ifndef HAVE_DECL_PROGRAM_SHORT_NAME
char* program_short_name = 0;
#endif
char* hfst_tool_version = 0;
char* hfst_tool_wikiname = 0;

// helpers for all
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
    fprintf(out, "%s: ", program_short_name);
    print_color(out, COLOR_RESET);
  }

// Gnu style short help
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
    fprintf(message_out, "%s home page: "
            "<" KITWIKI_URL "/%s>\n",
            program_name, hfst_tool_wikiname);
    fprintf(message_out, "General help using HFST software: "
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
  fprintf(message_out, "Report bugs to: " PACKAGE_BUGREPORT " "
          "Report bugs via web to:"
          "https://sourceforge.net/tracker/?atid=1061990"
          "&group_id=224521&func=browse\n");
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
  if (!silent && verbose)
    {
      va_list ap;
      va_start(ap, fmt);
      vfprintf(message_out, fmt, ap);
      va_end(ap);
    }
}

void
nonsilent_printf(const char* fmt, ...)
  {
    if (!silent)
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

void
hfst_debug(const char* fmt, ...)
  {
    if (!silent && debug)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_DEBUG);
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
hfst_print_reading(const char* filename)
  {
    if (!silent && verbose)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_VERBOSE);
        fprintf(message_out, "<<< ");
        print_color(message_out, COLOR_RESET);
        fprintf(message_out, "%s... ", filename);
      }
  }

void
hfst_begin_processing(const char* name, size_t nth, const char* op)
  {
    if (!silent && verbose)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_VERBOSE);
        fprintf(message_out, "*** ");
        print_color(message_out, COLOR_RESET);
        if (nth < 2)
          {
            fprintf(message_out, "%s automaton %s...\n", op, name);
          }
        else
          {
            fprintf(message_out, "%s automaton %s, ", op, name);
            print_color(message_out, COLOR_INFO);
            fprintf(message_out, "%zu...\n", nth);
            print_color(message_out, COLOR_RESET);
          }
      }
  }

void
hfst_begin_processing(const char* firstname, const char* secondname,
                      size_t nth, const char* op)
  {
    if (!silent && verbose)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_VERBOSE);
        fprintf(message_out, "*** ");
        print_color(message_out, COLOR_RESET);
        if (nth < 2)
          {
            fprintf(message_out, "%s automata %s and %s...\n", op, 
                    firstname, secondname);
          }
        else
          {
            fprintf(message_out, "%s automata %s and %s, ", op, 
                    firstname, secondname);
            print_color(message_out, COLOR_INFO);
            fprintf(message_out, "%zu...\n", nth);
            print_color(message_out, COLOR_RESET);
          }
      }
  }

void
hfst_processing(const char* op)
  {
    if (!silent && verbose)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_VERBOSE);
        fprintf(message_out, "*** ");
        print_color(message_out, COLOR_RESET);
        fprintf(message_out, "%s... ", op);
      }
  }


void
hfst_writing(const char* filename)
  {
    if (!silent && verbose)
      {
        print_program_name(message_out);
        print_color(message_out, COLOR_VERBOSE);
        fprintf(message_out, ">>> ");
        print_color(message_out, COLOR_RESET);
        fprintf(message_out, "%s... ", filename);
      }
  }

void
hfst_done()
  {
    if (!silent && verbose)
      {
        fprintf(message_out, "\t");
        print_program_name(message_out);
        print_color(message_out, COLOR_BRACKET);
        fprintf(message_out, "[");
        print_color(message_out, COLOR_RESET COLOR_OK);
        fprintf(message_out, " OK ");
        print_color(message_out, COLOR_RESET COLOR_BRACKET);
        fprintf(message_out, "]");
        print_color(message_out, COLOR_RESET);
        fprintf(message_out, "\n");
      }
  }

void
hfst_failed()
  {
    if (!silent && verbose)
      {
        fprintf(message_out, "\t");
        print_program_name(message_out);
        print_color(message_out, COLOR_BRACKET);
        fprintf(message_out, "[");
        print_color(message_out, COLOR_RESET COLOR_FAIL);
        fprintf(message_out, "FAIL");
        print_color(message_out, COLOR_RESET COLOR_BRACKET);
        fprintf(message_out, "]");
        print_color(message_out, COLOR_RESET);
        fprintf(message_out, "\n");
      }
  }

void
hfst_end_processing()
  {
    if (!silent && verbose)
      {
        fprintf(message_out, "\n");
        print_program_name(message_out);
        print_color(message_out, COLOR_VERBOSE);
        fprintf(message_out, "*** ");
        print_color(message_out, COLOR_RESET COLOR_OK);
        fprintf(message_out, "finished succesfully.");
        print_color(message_out, COLOR_RESET);
        fprintf(message_out, "\n");
      }
  }

