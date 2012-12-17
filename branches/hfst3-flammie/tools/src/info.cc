//! @file info.cc
//!
//! @brief HFST debug and version info tool
//!
//! @author HFST Team


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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>


#include <set>
#include <string>

using std::set;
using std::string;

#include <hfst.hpp>

#include "conventions/commandline.h"


static long min_version = -1L;
static long exact_version = -1L;
static long max_version = -1L;
static set<string> required_features;

static
long
parse_version_string(const char* s)
  {
    char* endptr;
    long major = strtoul(s, &endptr, 10L);
    if (*endptr == '\0')
      {
        return (major * 10000 * 10000);
      }
    else if (*endptr != '.')
      {
        error(EXIT_FAILURE, 0, "cannot parse version string from %s", endptr);
      }
    s = endptr + 1;
    long minor = strtoul(s, &endptr, 10L);
    if (*endptr == '\0')
      {
        return (major * 10000 * 10000) + (minor * 10000);
      }
    else if (*endptr != '.')
      {
        error(EXIT_FAILURE, 0, "cannot parse version string from %s", endptr);
      }
    s = endptr + 1;
    long patch = strtoul(s, &endptr, 10L);
    if (*endptr == '\0')
      {
        return (major * 10000 * 10000) + (minor * 10000) + patch;
      }
    else
      {
        error(EXIT_FAILURE, 0, "cannot parse version string from %s", endptr);
      }
    return -1L;
  }


void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "show or test HFST versions and features\n"
            "\n", program_name);

    print_common_program_options();
    fprintf(message_out, "Test features:\n"
            "  -a, --atleast-version=MVER   require at least MVER version "
            "of HFST\n"
            "  -e, --exact-version=EVER     require exactly EVER version "
            "of HFST\n"
            "  -m, --max-version=UVER       require at most UVER version "
            "of HFST\n"
            "  -f, --requirefeature=FEAT    require named FEAT support "
            "from HFST\n");
    fprintf(message_out, "\n");
    fprintf(message_out, "MVER, EVER or UVER version vectors must be composed "
            "of one to three full stop separated runs of digits.\n"
            "FEAT should be name of feature supported by HFST, such as "
            "SFST, foma or openfst\n"
            "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
  }


void
parse_options(int argc, char** argv)
  {
    // use of this function requires options are settable on global scope
    while (true)
      {
        static const struct option long_options[] =
        {
          HFST_GETOPT_COMMON_LONG,
          {"atleast-version", required_argument, 0, 'a'},
          {"exact-version", required_argument, 0, 'e'},
          {"max-version", required_argument, 0, 'm'},
          {"require-feature", required_argument, 0, 'f'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             "a:e:f:m:",
                             long_options, &option_index);
        if (-1 == c)
          {
            break;
          }
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        switch (c)
          {
            case 'a':
            min_version = parse_version_string(optarg);
            break;
          case 'e':
            exact_version = parse_version_string(optarg);
            break;
          case 'm':
            max_version = parse_version_string(optarg);
            break;
          case 'f':
            required_features.insert(optarg);
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
check_options()
  {
    if ((min_version == -1L) && (max_version == -1L) && (exact_version == -1L)
        && (required_features.size() == 0) && (verbose == false))
      {
        verbose = true;
        verbose_printf("No tests selected; printing known data\n");
      }
  }

int main(int argc, char * argv[])
  {
    hfst_init_commandline(argv[0], "0.1", "HfstInfo",
                          NO_AUTOMAGIC_IO, NO_AUTOMAGIC_FIlES);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options();
    parse_options_getenv();
    if (min_version != -1L)
      {
        verbose_printf("Requiring current version %ld to be greater than %ld\n",
                       HFST_LONGVERSION, min_version);
        if (HFST_LONGVERSION < min_version)
          {
            hfst_error(EXIT_FAILURE, 0, "Version requirements not met");
          }
      }
    if (exact_version != -1L)
      {
        verbose_printf("Requiring current version %ld to be exactly %ld\n",
                       HFST_LONGVERSION, exact_version);
        if (HFST_LONGVERSION != exact_version)
          {
            hfst_error(EXIT_FAILURE, 0, "Version requirements not met");
          }
      }
    if (max_version != -1L)
      {
        verbose_printf("Requiring current version %ld to be greater than %ld\n",
                       HFST_LONGVERSION, max_version);
        if (HFST_LONGVERSION < max_version)
          {
            hfst_error(EXIT_FAILURE, 0, "Version requirements not met");
          }
      }
    for (set<string>::const_iterator f = required_features.begin();
         f != required_features.end();
         ++f)
      {
        if ((*f == "sfst") || (*f == "SFST") || (*f == "HAVE_SFST"))
          {
            verbose_printf("Requiring SFST support from library");
#   ifndef HAVE_SFST
            hfst_error(EXIT_FAILURE, 0, "Required SFST support not present");
#   endif
          }
        else if ((*f == "foma") || (*f == "FOMA") || (*f == "HAVE_FOMA"))
          {
            verbose_printf("Requiring foma support from library");
#   ifndef HAVE_FOMA
            hfst_error(EXIT_FAILURE, 0, "Required foma support not present");
#   endif
          }
        else if ((*f == "openfst") || (*f == "OPENFST") || (*f == "HAVE_OPENFST"))
          {
            verbose_printf("Requiring OpenFst support from library");
#   ifndef HAVE_OPENFST
            hfst_error(EXIT_FAILURE, 0, "Required OpenFst support not present");
#   endif
          }
        else if ((*f == "glib") || (*f == "USE_GLIB_UNICODE"))
          {
            verbose_printf("Requiring Unicode parsed by Glib");
#   ifndef USE_GLIB_UNICODE
            hfst_error(EXIT_FAILURE, 0,
                  "Required GLIB-based Unicode handling not presesnt");
#   endif
          }
        else
          {
            hfst_error(EXIT_FAILURE, 0, "Required %s support is unrecognised "
                  "and therefore assumed to be missing", f->c_str());
          }
      }
    verbose_printf("HFST info version: %s\n"
            "HFST packaging: %s <%s> <mailto:%s>\n"
            "HFST version: %s\n"
            "HFST long version: %lu\n"
            "HFST configuration revision: %s\n",
            hfst_tool_version,
            PACKAGE_STRING, PACKAGE_URL, PACKAGE_BUGREPORT,
            HFST_STRING,
            HFST_LONGVERSION,
            HFST_REVISION);
#   if HAVE_OPENFST
    verbose_printf("OpenFst supported\n");
#   endif
#   if HAVE_SFST
    verbose_printf("SFST supported\n");
#   endif
#   if HAVE_FOMA
    verbose_printf("foma supported\n");
#   endif
#   if USE_GLIB_UNICODE
    verbose_printf("Unicode support: glib\n");
#   elif USE_ICU_UNICODE
    verbose_printf("Unicode support: ICU\n");
#   else
    verbose_printf("Unicode support: no (hfst)\n");
#   endif

  return EXIT_SUCCESS;
}
