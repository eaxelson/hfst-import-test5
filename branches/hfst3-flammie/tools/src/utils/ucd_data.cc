//! @file unicode_data.cc
//! @brief Unicode database in C structure.


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

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "conventions/portability.h"
#include "utils/ucd_data.h"

// last reasonable unicode is:
//  U+E01EF VARIATION SELECTOR 256
// (Unicode 6)
// after that all there is are PUA selectors which are encoded in functions.
#define UCD_DATA_SIZE 0xE01EF
#define MIN(a, b) (a < b) ? a : b
static
uint32_t
hextouint32_t(const char* hexdigits)
  {
    char* endptr;
    uint32_t rv = (uint32_t)strtoul(hexdigits, &endptr, 16);
    if (*endptr != '\0')
      {
        return 0;
      }
    return rv;
  }

static
char**
explode_ucd(const char* line)
  {
    char** ucd_fields = (char**)malloc(sizeof(char*) * 16);
    const char* f_start = line;
    const char* f_end = line;
    for (int i = 0; i < 15; i++)
      {
        f_end = f_start;
        assert(f_end != 0);
        while ((*f_end != ';') && (*f_end != '\n') && (*f_end != '\0'))
          {
            f_end++;
          }
        ucd_fields[i] = strndup(f_start, f_end - f_start);
        f_start = f_end + 1;
      }
    return ucd_fields;
  }

ucd_data*
init_ucd_data()
  {
    ucd_data* ucd = (ucd_data*)(malloc(sizeof(ucd_data) * 
                                                UCD_DATA_SIZE));
    FILE* ucd_file = NULL;
#if defined UCD_DATA_PATH
    ucd_file = fopen(UCD_DATA_PATH, "r");
#endif
    if (ucd_file == NULL)
      {
        ucd_file = fopen("/usr/share/hfst/UnicodeData.txt", "r");
      }
    if (ucd_file == NULL)
      {
        ucd_file = fopen("/usr/local/share/hfst/UnicodeData.txt", "r");
      }
    if (ucd_file == NULL)
      {
        char* home = getenv("HOME");
        if (home != NULL)
          {
            char* home_ucd = (char*)malloc(sizeof(char) * strlen(home) +
                                         strlen("/share/hfst/UnicodeData.txt")
                                         + 1);
            int rv = sprintf(home_ucd, "%s/share/hfst/UnicodeData.txt", home);
            if (rv == (strlen(home) + strlen("/share/hfst/UnicodeData.txt")))
              {
                ucd_file = fopen(home_ucd, "r");
              }
            free(home_ucd);
          }
      }
    if (ucd_file == NULL)
      {
        // nothing found, return NULL
        return NULL;
      }
    char* line = 0;
    size_t linelen = 0;
    uint32_t previous_cp = 0;
    char** previous_fields = 0;
    while (getline(&line, &linelen, ucd_file) != -1)
      {
        char** fields = explode_ucd(line);
        assert(fields != NULL);
        // 0. codepoint
        uint32_t cp = hextouint32_t(fields[0]);
        if (cp >= UCD_DATA_SIZE)
          {
            break;
          }
        if (cp > (previous_cp + 1))
          {
            for (unsigned i = previous_cp; i < cp; i++)
              {
                if (strstr(previous_fields[1], "First>") != NULL)
                  {
                    ucd[i].general_category = 
                        strdup(ucd[previous_cp].general_category);
                    ucd[i].simple_uppercase_mapping = 
                        ucd[previous_cp].simple_uppercase_mapping;
                    ucd[i].simple_lowercase_mapping = 
                        ucd[previous_cp].simple_lowercase_mapping;
                    ucd[i].simple_titlecase_mapping =
                        ucd[previous_cp].simple_titlecase_mapping;
                  }
                else
                  {
                    // fill unknown gaps with defaults
                    ucd[i].general_category = strdup("Cn");
                    ucd[i].simple_uppercase_mapping = 0;
                    ucd[i].simple_lowercase_mapping = 0;
                    ucd[i].simple_titlecase_mapping = 0;
                  }
              }
          }
        // 2. general category
        ucd[cp].general_category = strdup(fields[2]);
        // 12. simple uppercase mapping
        ucd[cp].simple_uppercase_mapping = hextouint32_t(fields[12]);
        // 13. simple lowercase mapping
        ucd[cp].simple_lowercase_mapping = hextouint32_t(fields[13]);
        // 14. simple titlecase mapping
        ucd[cp].simple_titlecase_mapping = hextouint32_t(fields[14]);
        if (ucd[cp].simple_titlecase_mapping == 0)
          {
            // as per note in UAX# TR 44
            ucd[cp].simple_titlecase_mapping = ucd[cp].simple_uppercase_mapping;
          }
        if (previous_fields != 0)
          {
            for (unsigned int i = 0; i < 15; i++)
              {
                free(previous_fields[i]);
              }
          }
        free(previous_fields);
        previous_fields = fields;
        previous_cp = cp;
      }
    free(line);
    return ucd;
  }

void
terminate_ucd_data(ucd_data* ucd)
  {
    for (unsigned int i = 0; i < UCD_DATA_SIZE; i++)
      {
        free(ucd[i].general_category);
      }
    free(ucd);
  }

bool
ucd_is_letter(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (*(ucd[ccp].general_category) == 'L')
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

bool
ucd_is_number(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (*(ucd[ccp].general_category) == 'N')
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

bool
ucd_is_mark(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (*(ucd[ccp].general_category) == 'M')
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

bool
ucd_is_symbol(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (*(ucd[ccp].general_category) == 'S')
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

bool
ucd_is_separator(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (*(ucd[ccp].general_category) == 'Z')
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

bool
ucd_is_punctuation(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (*(ucd[ccp].general_category) == 'P')
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

bool
ucd_is_control(const ucd_data* ucd, uint32_t cp) 
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (*(ucd[ccp].general_category) == 'C')
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

bool
ucd_is_general_category(const ucd_data* ucd, uint32_t cp, 
                        const char* general_category)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    if (strcmp(ucd[ccp].general_category, general_category) == 0)
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

uint32_t
ucd_simple_uppercase_mapping(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    return (ucd[ccp].simple_uppercase_mapping);
  }

uint32_t
ucd_simple_lowercase_mapping(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    return (ucd[ccp].simple_lowercase_mapping);
  }

uint32_t
ucd_simple_titlecase_mapping(const ucd_data* ucd, uint32_t cp)
  {
    uint32_t ccp = MIN(cp, UCD_DATA_SIZE - 1);
    return (ucd[ccp].simple_titlecase_mapping);
  }

#if defined UNIT_TEST_UCD_DATA
#include <assert.h>

int
main(int, char**)
  {
    fprintf(stdout, "unit tests for %s at %u:\n", __FILE__, __LINE__);
    fprintf(stdout, "init_ucd_data...");
    ucd_data* ucd = init_ucd_data();
    fprintf(stdout, "ok\n");
    assert(ucd != NULL);
    uint32_t one = 0x31;
    uint32_t A = 0x41;
    uint32_t a = 0x61;
    uint32_t Auml = 0xC4;
    uint32_t auml = 0xE4;
    uint32_t ndash = 0x2013;
    uint32_t cjk281ab = 0x281ab;
    fprintf(stdout, "ucd_is_letter:");
    assert(ucd_is_letter(ucd, one) == false);
    assert(ucd_is_letter(ucd, A) == true);
    assert(ucd_is_letter(ucd, a) == true);
    assert(ucd_is_letter(ucd, Auml) == true);
    assert(ucd_is_letter(ucd, auml) == true);
    assert(ucd_is_letter(ucd, ndash) == false);
    assert(ucd_is_letter(ucd, cjk281ab) == true);
    fprintf(stdout, "ok\n");
    fprintf(stdout, "ucd_is_number:");
    assert(ucd_is_number(ucd, one) == true);
    assert(ucd_is_number(ucd, A) == false);
    assert(ucd_is_number(ucd, a) == false);
    assert(ucd_is_number(ucd, Auml) == false);
    assert(ucd_is_number(ucd, auml) == false);
    assert(ucd_is_number(ucd, ndash) == false);
    assert(ucd_is_number(ucd, cjk281ab) == false);
    fprintf(stdout, "ok\n");
    fprintf(stdout, "ucd_is_mark:");
    assert(ucd_is_mark(ucd, one) == false);
    assert(ucd_is_mark(ucd, A) == false);
    assert(ucd_is_mark(ucd, a) == false);
    assert(ucd_is_mark(ucd, Auml) == false);
    assert(ucd_is_mark(ucd, auml) == false);
    assert(ucd_is_mark(ucd, ndash) == false);
    assert(ucd_is_mark(ucd, cjk281ab) == false);
    fprintf(stdout, "ok\n");
    fprintf(stdout, "ucd_is_symbol:");
    assert(ucd_is_symbol(ucd, one) == false);
    assert(ucd_is_symbol(ucd, A) == false);
    assert(ucd_is_symbol(ucd, a) == false);
    assert(ucd_is_symbol(ucd, Auml) == false);
    assert(ucd_is_symbol(ucd, auml) == false);
    assert(ucd_is_symbol(ucd, ndash) == false);
    assert(ucd_is_symbol(ucd, cjk281ab) == false);
    fprintf(stdout, "ok\n");
    fprintf(stdout, "ucd_is_separator:");
    assert(ucd_is_separator(ucd, one) == false);
    assert(ucd_is_separator(ucd, A) == false);
    assert(ucd_is_separator(ucd, a) == false);
    assert(ucd_is_separator(ucd, Auml) == false);
    assert(ucd_is_separator(ucd, auml) == false);
    assert(ucd_is_separator(ucd, ndash) == false);
    assert(ucd_is_separator(ucd, cjk281ab) == false);
    fprintf(stdout, "ok\n");
    fprintf(stdout, "ucd_is_general_category:");
    assert(ucd_is_general_category(ucd, one, "Nd") == true);
    assert(ucd_is_general_category(ucd, A, "Lu") == true);
    assert(ucd_is_general_category(ucd, a, "Ll") == true);
    assert(ucd_is_general_category(ucd, Auml, "Lu") == true);
    assert(ucd_is_general_category(ucd, auml, "Ll") == true);
    assert(ucd_is_general_category(ucd, ndash, "Pd") == true);
    assert(ucd_is_general_category(ucd, cjk281ab, "Lo") == true);
    fprintf(stdout, "ok\n");

    fprintf(stdout, "ucd_simple_uppercase_mapping:");
    assert(ucd_simple_uppercase_mapping(ucd, one) == 0);
    assert(ucd_simple_uppercase_mapping(ucd, A) == 0);
    assert(ucd_simple_uppercase_mapping(ucd, a) == A);
    assert(ucd_simple_uppercase_mapping(ucd, Auml) == 0);
    assert(ucd_simple_uppercase_mapping(ucd, auml) == Auml);
    assert(ucd_simple_uppercase_mapping(ucd, ndash) == 0);
    assert(ucd_simple_uppercase_mapping(ucd, cjk281ab) == 0);
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "ucd_simple_lowercase_mapping:");
    assert(ucd_simple_lowercase_mapping(ucd, one) == 0);
    assert(ucd_simple_lowercase_mapping(ucd, A) == a);
    assert(ucd_simple_lowercase_mapping(ucd, a) == 0);
    assert(ucd_simple_lowercase_mapping(ucd, Auml) == auml);
    assert(ucd_simple_lowercase_mapping(ucd, auml) == 0);
    assert(ucd_simple_lowercase_mapping(ucd, ndash) == 0);
    assert(ucd_simple_lowercase_mapping(ucd, cjk281ab) == 0);
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "ucd_simple_titlecase_mapping:");
    assert(ucd_simple_titlecase_mapping(ucd, one) == 0);
    assert(ucd_simple_titlecase_mapping(ucd, A) == 0);
    assert(ucd_simple_titlecase_mapping(ucd, a) == A);
    assert(ucd_simple_titlecase_mapping(ucd, Auml) == 0);
    assert(ucd_simple_titlecase_mapping(ucd, auml) == Auml);
    assert(ucd_simple_titlecase_mapping(ucd, ndash) == 0);
    assert(ucd_simple_titlecase_mapping(ucd, cjk281ab) == 0);
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "ucd_terminate...");
    terminate_ucd_data(ucd);
    fprintf(stdout, " ok.\n");
    return EXIT_SUCCESS;
  }
#endif // UNIT_TEST_UCD_DATA
