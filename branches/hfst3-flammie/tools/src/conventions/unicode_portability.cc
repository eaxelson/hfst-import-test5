//! @file unicode_portability.cc
//! @brief portability implementations for utf8 and unicode ctype functions.

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>

#if HAVE_ERROR_H
#  include <error.h>
#endif

#if USE_GLIB_UNICODE
#  include <glib.h>
#endif
#if USE_HFST_UNICODE
#  include "utils/utf8.h"
#  include "utils/ucd_data.h"
ucd_data* ucd_ = 0;
static
void
maybe_init_ucd_data()
  {
    if (ucd_ == 0)
      {
        ucd_ = init_ucd_data();
      }
    assert(ucd_ != 0);
  }
#endif

#include <stdio.h>
#include <string.h>
#include "conventions/unicode_portability.h"
#include "conventions/portability.h"


ssize_t 
hfst_utf8chars(const char* s, char*** chars)
  {
    ssize_t utf8len = 0;
#   if USE_GLIB_UNICODE
    char **rv = (char**)malloc(sizeof(char*) * strlen(s));
    const gchar* p = s;
    gunichar utf8 = 0;
    while ((utf8 = g_utf8_get_char_validated(p, -1)) != 0)
      {
        if ((utf8 == -1) || (utf8 == -2))
          {
            error(EXIT_FAILURE, 0, "Cannot convert UTF-8 %s into characters "
                  "near %s", s, p);
            chars = 0;
            return -1;
          }
        gint charlen = g_unichar_to_utf8(utf8, NULL);
        rv[utf8len] = (char*)calloc(sizeof(char), charlen + 1);
        g_unichar_to_utf8(utf8, rv[utf8len]);
        p = g_utf8_next_char(p);
        utf8len++;
      }
    *chars = rv;
    return utf8len;
#   elif USE_ICU_UNICODE
#       error no ICU implementation yet
#   elif USE_HFST_UNICODE
    utf8len = strtoutf8(s, chars);
    return utf8len;
#   endif
    return -1;
  }

bool
hfst_utf8validate(const char* s)
  {
#   if USE_GLIB_UNICODE
    const gchar* endptr = 0;
    if (g_utf8_validate(s, -1, &endptr))
      {
        return true;
      }
    else
      {
        error(EXIT_FAILURE, 0, "Broken UTF-8 in %s near %s", s, endptr);
        return false;
      }
#   elif USE_HFST_UNICODE
    if (utf8validate(s))
      {
        return true;
      }
    else
      {
        error(EXIT_FAILURE, 0, "Broken UTF-8 in %s", s);
        return false;
      }
#   endif
    return false;
  }

char*
hfst_utf8toupper(const char* c)
  {
    char* upper = 0;
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when uppercasing",
              c);
        return NULL;
      }
    gunichar rv = g_unichar_toupper(guc);
    gint charlen = g_unichar_to_utf8(rv, NULL);
    upper = (char*)calloc(sizeof(char), charlen + 1);
    g_unichar_to_utf8(rv, upper);
    return upper;
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    uint32_t cp = ucd_simple_uppercase_mapping(ucd_, utf8tocp(c));
    if (cp != 0)
      {
        upper = cptoutf8(cp);
        return upper;
      }
    else
      {
        return strdup(c);
      }
#   endif
    return NULL;
  }

char*
hfst_utf8tolower(const char* c)
  {
    char* lower = 0;
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when lowercasing",
              c);
        return NULL;
      }
    gunichar rv = g_unichar_tolower(guc);
    gint charlen = g_unichar_to_utf8(rv, NULL);
    lower = (char*)calloc(sizeof(char), charlen + 1);
    g_unichar_to_utf8(rv, lower);
    return lower;
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    uint32_t cp = ucd_simple_lowercase_mapping(ucd_, utf8tocp(c));
    if (cp != 0)
      {
        lower = cptoutf8(cp);
        return lower;
      }
    else
      {
        return strdup(c);
      }
#   endif
    return NULL;
  }

char*
hfst_utf8totitle(const char* c)
  {
    char* title = 0;
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    gunichar rv = g_unichar_totitle(guc);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when titlecasing",
              c);
        return NULL;
      }
    gint charlen = g_unichar_to_utf8(rv, NULL);
    title = (char*)calloc(sizeof(char), charlen + 1);
    g_unichar_to_utf8(rv, title);
    return title;
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    uint32_t cp = ucd_simple_titlecase_mapping(ucd_, utf8tocp(c));
    if (cp != 0)
      {
        title = cptoutf8(cp);
        return title;
      }
    else
      {
        return strdup(c);
      }
#   endif
    return NULL;
  }

bool
hfst_utf8isupper(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "uppercasedness",
              c);
        return NULL;
      }
    return g_unichar_isupper(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return ucd_is_general_category(ucd_, utf8tocp(c), "Lu");
#   endif
    return false;
  }

bool 
hfst_utf8islower(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "lowercasedness",
              c);
        return NULL;
      }
    return g_unichar_islower(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return ucd_is_general_category(ucd_, utf8tocp(c), "Ll");
#   endif
    return false;
  }

bool 
hfst_utf8istitle(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "lowercasedness",
              c);
        return NULL;
      }
    return g_unichar_istitle(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return ucd_is_general_category(ucd_, utf8tocp(c), "Lt");
#   endif
    return false;
  }

bool 
hfst_utf8isspace(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "spaceness",
              c);
        return NULL;
      }
    return g_unichar_isspace(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return (ucd_is_separator(ucd_, utf8tocp(c)) || 
            (*c == '\n') || (*c == '\r'));
#   endif
    return false;
  }

bool
hfst_utf8isalpha(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "alphabetness",
              c);
        return NULL;
      }
    return g_unichar_isalpha(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return ucd_is_letter(ucd_, utf8tocp(c));
#   endif
    return false;
  }

bool
hfst_utf8isdigit(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "digitness",
              c);
        return NULL;
      }
    return g_unichar_isdigit(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return ucd_is_number(ucd_, utf8tocp(c));
#   endif
    return false;
  }

bool
hfst_utf8isalnum(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "alnumness",
              c);
        return NULL;
      }
    return g_unichar_isalnum(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return ((ucd_is_letter(ucd_, utf8tocp(c))) || 
        (ucd_is_number(ucd_, utf8tocp(c))));
#   endif
    return false;
  }

bool
hfst_utf8ispunct(const char* c)
  {
#   if USE_GLIB_UNICODE
    gunichar guc = g_utf8_get_char_validated(c, -1);
    if ((guc == -1) || (guc == -2))
      {
        error(EXIT_FAILURE, 0, "Invalid UTF-8 character %s when determining "
              "punctness",
              c);
        return NULL;
      }
    return g_unichar_ispunct(guc);
#   elif USE_HFST_UNICODE
    maybe_init_ucd_data();
    return ucd_is_punctuation(ucd_, utf8tocp(c));
#   endif
    return false;
  }

#if defined UNIT_TEST_UNICODE_PORTABILITY

#include <stdio.h>
#include <assert.h>

int
main(int, char**)
  {
    fprintf(stdout, "Unit tests for %s at %u\n", __FILE__, __LINE__);
    const char* utf8string = "1AaÄä–𠆫";
    const char* one = "1";
    const char* A = "A";
    const char* a = "a";
    const char* Auml = "Ä";
    const char* auml = "ä";
    const char* ndash = "–";
    const char* cjk281ab = "𠆫";
    fprintf(stdout, "hfst_utf8validate...");
    assert(hfst_utf8validate(utf8string) == true);
    assert(hfst_utf8validate(one) == true);
    assert(hfst_utf8validate(A) == true);
    assert(hfst_utf8validate(a) == true);
    assert(hfst_utf8validate(Auml) == true);
    assert(hfst_utf8validate(auml) == true);
    assert(hfst_utf8validate(ndash) == true);
    assert(hfst_utf8validate(cjk281ab) == true);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "utf8chars...");
    char** utf8s;
    ssize_t utf8len = hfst_utf8chars(utf8string, &utf8s);
    assert(utf8len == 7);
    for (unsigned int i = 0; i < utf8len; i++)
      {
        assert(utf8s[i] != NULL);
        assert(hfst_utf8validate(utf8s[i]) == true);
        free(utf8s[i]);
      }
    free(utf8s);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8toupper...");
    char* rv = hfst_utf8toupper(one);
    assert(strcmp(rv, one) == 0);
    free(rv);
    rv = hfst_utf8toupper(A);
    assert(strcmp(rv, A) == 0);
    free(rv);
    rv = hfst_utf8toupper(a);
    assert(strcmp(rv, A) == 0);
    free(rv);
    rv = hfst_utf8toupper(Auml);
    assert(strcmp(rv, Auml) == 0);
    free(rv);
    rv = hfst_utf8toupper(auml);
    assert(strcmp(rv, Auml) == 0);
    free(rv);
    rv = hfst_utf8toupper(ndash);
    assert(strcmp(rv, ndash) == 0);
    free(rv);
    rv = hfst_utf8toupper(cjk281ab);
    assert(strcmp(rv, cjk281ab) == 0);
    free(rv);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8tolower...");
    rv = hfst_utf8tolower(one);
    assert(strcmp(rv, one) == 0);
    free(rv);
    rv = hfst_utf8tolower(A);
    assert(strcmp(rv, a) == 0);
    free(rv);
    rv = hfst_utf8tolower(a);
    assert(strcmp(rv, a) == 0);
    free(rv);
    rv = hfst_utf8tolower(Auml);
    assert(strcmp(rv, auml) == 0);
    free(rv);
    rv = hfst_utf8tolower(auml);
    assert(strcmp(rv, auml) == 0);
    free(rv);
    rv = hfst_utf8tolower(ndash);
    assert(strcmp(rv, ndash) == 0);
    free(rv);
    rv = hfst_utf8tolower(cjk281ab);
    assert(strcmp(rv, cjk281ab) == 0);
    free(rv);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8totitle...");
    rv = hfst_utf8totitle(one);
    assert(strcmp(rv, one) == 0);
    free(rv);
    rv = hfst_utf8totitle(A);
    assert(strcmp(rv, A) == 0);
    free(rv);
    rv = hfst_utf8totitle(a);
    assert(strcmp(rv, A) == 0);
    free(rv);
    rv = hfst_utf8totitle(Auml);
    assert(strcmp(rv, Auml) == 0);
    free(rv);
    rv = hfst_utf8totitle(auml);
    assert(strcmp(rv, Auml) == 0);
    free(rv);
    rv = hfst_utf8totitle(ndash);
    assert(strcmp(rv, ndash) == 0);
    free(rv);
    rv = hfst_utf8totitle(cjk281ab);
    assert(strcmp(rv, cjk281ab) == 0);
    free(rv);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8isupper...");
    assert(hfst_utf8isupper(one) == false);
    assert(hfst_utf8isupper(A) == true);
    assert(hfst_utf8isupper(a) == false);
    assert(hfst_utf8isupper(Auml) == true);
    assert(hfst_utf8isupper(auml) == false);
    assert(hfst_utf8isupper(ndash) == false);
    assert(hfst_utf8isupper(cjk281ab) == false);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8islower...");
    assert(hfst_utf8islower(one) == false);
    assert(hfst_utf8islower(A) == false);
    assert(hfst_utf8islower(a) == true);
    assert(hfst_utf8islower(Auml) == false);
    assert(hfst_utf8islower(auml) == true);
    assert(hfst_utf8islower(ndash) == false);
    assert(hfst_utf8islower(cjk281ab) == false);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8istitle...");
    assert(hfst_utf8istitle(one) == false);
    assert(hfst_utf8istitle(A) == false);
    assert(hfst_utf8istitle(a) == false);
    assert(hfst_utf8istitle(Auml) == false);
    assert(hfst_utf8istitle(auml) == false);
    assert(hfst_utf8istitle(ndash) == false);
    assert(hfst_utf8istitle(cjk281ab) == false);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8isspace...");
    assert(hfst_utf8isspace(one) == false);
    assert(hfst_utf8isspace(A) == false);
    assert(hfst_utf8isspace(a) == false);
    assert(hfst_utf8isspace(Auml) == false);
    assert(hfst_utf8isspace(auml) == false);
    assert(hfst_utf8isspace(ndash) == false);
    assert(hfst_utf8isspace(cjk281ab) == false);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8isalpha...");
    assert(hfst_utf8isalpha(one) == false);
    assert(hfst_utf8isalpha(A) == true);
    assert(hfst_utf8isalpha(a) == true);
    assert(hfst_utf8isalpha(Auml) == true);
    assert(hfst_utf8isalpha(auml) == true);
    assert(hfst_utf8isalpha(ndash) == false);
    assert(hfst_utf8isalpha(cjk281ab) == true);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8isdigit...");
    assert(hfst_utf8isdigit(one) == true);
    assert(hfst_utf8isdigit(A) == false);
    assert(hfst_utf8isdigit(a) == false);
    assert(hfst_utf8isdigit(Auml) == false);
    assert(hfst_utf8isdigit(auml) == false);
    assert(hfst_utf8isdigit(ndash) == false);
    assert(hfst_utf8isdigit(cjk281ab) == false);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8isalnum...");
    assert(hfst_utf8isalnum(one) == true);
    assert(hfst_utf8isalnum(a) == true);
    assert(hfst_utf8isalnum(a) == true);
    assert(hfst_utf8isalnum(auml) == true);
    assert(hfst_utf8isalnum(auml) == true);
    assert(hfst_utf8isalnum(ndash) == false);
    assert(hfst_utf8isalnum(cjk281ab) == true);
    fprintf(stdout, "ok.\n");
    fprintf(stdout, "hfst_utf8ispunct...");
    assert(hfst_utf8ispunct(one) == false);
    assert(hfst_utf8ispunct(a) == false);
    assert(hfst_utf8ispunct(a) == false);
    assert(hfst_utf8ispunct(auml) == false);
    assert(hfst_utf8ispunct(auml) == false);
    assert(hfst_utf8ispunct(ndash) == true);
    assert(hfst_utf8ispunct(cjk281ab) == false);
    fprintf(stdout, "ok.\n");
#   if USE_HFST_UNICODE
    terminate_ucd_data(ucd_);
#   endif
    return EXIT_SUCCESS;
  }
#endif
