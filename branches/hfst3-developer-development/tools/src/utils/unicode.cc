//! @file unicode.cc
//! @brief HFST implementations for string handling and ctype functions


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

#include <assert.h>
#include <string.h>

#include "utils/unicode.h"
#include "utils/unicode_data.h"
#include "conventions/portability.h"

#ifndef NDEBUG
#include <stdio.h>
#endif

uint32_t
utf8tocp(const char* c)
  {
    unsigned char u = static_cast<unsigned char>(*c);
    uint32_t cp = 0;
    if (u <= 127)
      {
        cp = u;
      }
    else if ( (u & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16))
      {
        cp = (u & (1 + 2 + 4)) << 18;
        u = static_cast<unsigned char>(*(c+1));
        assert(u != 0);
        cp += (u & (1 + 2 + 4 + 8 + 16 + 32)) << 12;
        u = static_cast<unsigned char>(*(c+2));
        assert(u != 0);
        cp += (u & (1 + 2 + 4 + 8 + 16 + 32)) << 6;
        u = static_cast<unsigned char>(*(c+3));
        assert(u != 0);
        cp += (u & (1 + 2 + 4 + 8 + 16 + 32));
      }
    else if ( (u & (128 + 64 + 32)) == (128 + 64 + 32))
      {
        cp = (u & (1 + 2 + 4 + 8)) << 12;
        u = static_cast<unsigned char>(*(c+1));
        assert(u != 0);
        cp += (u & (1 + 2 + 4 + 8 + 16 + 32)) << 6;
        u = static_cast<unsigned char>(*(c+2));
        assert(u != 0);
        cp += (u & (1 + 2 + 4 + 8 + 16 + 32));
      }
    else if ( (u & (128 + 64)) == (128 + 64))
      {
        // e.g. Ä = 1100 0011 10100001
        //          0000 0011 00100001 // shift high byte 6 left and add
        //                    11100001
        cp = (u & (1 + 2 + 4 + 8 + 16)) << 6;
        u = static_cast<unsigned char>(*(c+1));
        assert(u != 0);
        cp += (u & (1 + 2 + 4 + 8 + 16 + 32));
      }
    else
      {
        cp = 0;
      }
    return cp;
  }

char*
cptoutf8(uint32_t cp)
  {
    char* rv;
    if (cp <= 127)
      {
        rv = static_cast<char*>(malloc(sizeof(char) * 2));
        rv[0] = static_cast<char>(cp);
        rv[1] = 0;
      }
    else if (cp <= 0x7FF)
      {
        rv = static_cast<char*>(malloc(sizeof(char) * 3));
        rv[1] = static_cast<char>((cp & (1 + 2 + 4 + 8 + 16 + 32)) + 128);
        rv[0] = static_cast<char>( ((cp & (64 + 128 + 256 + 512 + 1024)) >> 6) + 64 + 128);
        rv[2] = 0;
      }
    else if (cp <= 0xFFFF)
      {
        rv = static_cast<char*>(malloc(sizeof(char) * 4));
        rv[2] = static_cast<char>((cp & (1 + 2 + 4 + 8 + 16 + 32)) + 128);
        rv[1] = static_cast<char>( ((cp & (64 + 128 + 256 + 512 + 1024 + 2048)) >> 6) + 128);
        rv[0] = static_cast<char>( ((cp & (4096 + 8192 + 16384 + 32768)) >> 12) +32 + 64 + 128);
        rv[3] = 0;
      }
    else if (cp <= 0x10FFFF)
      {
        rv = static_cast<char*>(malloc(sizeof(char) * 5));
        rv[3] = static_cast<char>((cp & (1 + 2 + 4 + 8 + 16 + 32)) + 128);
        rv[2] = static_cast<char>( ((cp & (64 + 128 + 256 + 512 + 1024 + 2048)) >> 6) + 128);
        rv[1] = static_cast<char>( ((cp & (4096 + 8192 + 16384 + 32768 + 65536 + 131072)) >> 12) + 128);
        rv[0] = static_cast<char>( ((cp & (262144 + 524288 + 1048576)) >> 18) + 16 + 32 + 64 + 128);
        rv[4] = 0;
      }
    else
      {
        rv = 0;
      }
    return rv;
  }


char*
utf8tok_r(const char* str, const char **saveptr)
  {
    if (*saveptr == NULL)
      {
        *saveptr = str;
      }
    if (*saveptr == NULL)
      {
        return NULL;
      }
    unsigned char c = static_cast<unsigned char>(**saveptr);
    unsigned short u8len = 1;
    if (c == 0)
      {
        return NULL;
      }
    else if (c <= 127)
      {
        u8len = 1;
      }
    else if ( (c & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16) )
      {
        u8len = 4;
      }
    else if ( (c & (128 + 64 + 32 )) == (128 + 64 + 32) )
      {
        u8len = 3;
      }
    else if ( (c & (128 + 64 )) == (128 + 64))
      {
        u8len = 2;
      }
    else
      {
        return NULL;
      }
    char* rv = strndup(*saveptr, u8len);
    *saveptr += u8len;
    return rv;
  }


size_t
strtoutf8(const char* s, char*** buf)
  {
    const char* saveptr = 0;
    char** utf8s = static_cast<char**>(malloc(sizeof(char*) * strlen(s)));
    char* nextu8 = utf8tok_r(s, &saveptr);
    size_t i = 0;
    while (nextu8)
      {
        utf8s[i] = nextu8;
        i++;
        nextu8 = utf8tok_r(NULL, &saveptr);
      }
    if (*saveptr != '\0')
      {
        return 0;
      }
    *buf = utf8s;
    return i;
  }

char* utf8toupper(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].upper != 0)
      {
        return cptoutf8(unicode[cp].upper);
      }
    else
      {
        return strdup(c);
      }
    return NULL;
  }

char* utf8tolower(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].lower != 0)
      {
        return cptoutf8(unicode[cp].lower);
      }
    else
      {
        return strdup(c);
      }
    return NULL;
  }

char*
utf8totitle(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].title != 0)
      {
        return cptoutf8(unicode[cp].title);
      }
    else
      {
        return strdup(c);
      }
    return NULL;
  }

bool
utf8isupper(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].category != 0)
      {
        return (strcmp(unicode[cp].category, "Lu") == 0);
      }
    else if (('A' <= cp) && (cp <= 'Z'))
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
utf8islower(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].category != 0)
      {
        return (strcmp(unicode[cp].category, "Ll") == 0);
      }
    else if (('a' <= cp) && (cp <= 'z'))
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
utf8istitle(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].category != 0)
      {
        return (strcmp(unicode[cp].category, "Lt") == 0);
      }
    else
      {
        return false;
      }
    return false;
  }

bool
utf8isalpha(const char* c)
  {
    if (utf8isupper(c) || utf8islower(c) || utf8istitle(c))
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
utf8isdigit(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].category != 0)
      {
        return (strcmp(unicode[cp].category, "Nd") == 0);
      }
    else if (('0' <= cp) && (cp <= '9'))
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
utf8isalnum(const char* c)
  {
    if (utf8isalpha(c) || utf8isdigit(c))
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
utf8isspace(const char* c)
  {
    uint32_t cp = utf8tocp(c);
    if (unicode[cp].category != 0)
      {
        return (strcmp(unicode[cp].category, "Zs") == 0);
      }
    else if (' ' == cp)
      {
        return true;
      }
    else
      {
        return false;
      }
    return false;
  }

#if UNIT_TEST

#include <stdio.h>
#include <string.h>
#include <assert.h>

int
main(void)
  {
    fprintf(stdout, "unit tests for %s at %u:\n", __FILE__, __LINE__);
    init_unicode_data();
    fprintf(stdout, "utf8tocp:");
    assert(utf8tocp("1") == 49);
    assert(utf8tocp("A") == 65);
    assert(utf8tocp("a") == 97);
    assert(utf8tocp("Ä") == 196);
    assert(utf8tocp("ä") == 228);
    assert(utf8tocp("–") == 8211);
    assert(utf8tocp("𠆫") == 131499);
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "cptoutf8:");
    assert(strcmp(cptoutf8(49), "1") == 0);
    assert(strcmp(cptoutf8(65), "A") == 0);
    assert(strcmp(cptoutf8(97), "a") == 0);
    assert(strcmp(cptoutf8(196), "Ä") == 0);
    assert(strcmp(cptoutf8(228), "ä") == 0);
    assert(strcmp(cptoutf8(8211), "–") == 0);
    assert(strcmp(cptoutf8(131499), "𠆫") == 0);
    fprintf(stdout, " ok.\n");
    char* s = strdup("1AaÄä–𠆫");
    const char* saveptr;
    fprintf(stdout, "utf8tok_r:");
    char* u8 = utf8tok_r(s, &saveptr);
    assert(u8 != NULL);
    while (u8 != NULL)
      {
        free(u8);
        u8 = utf8tok_r(NULL, &saveptr);
      }
    assert(saveptr != NULL);
    assert(*saveptr == '\0');
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "strtoutf8:");
    char** u8s = 0;
    size_t u8s_len = strtoutf8(s, &u8s);
    assert(u8s != NULL);
    for (size_t i = 0; i < u8s_len; i++)
      {
        assert(u8s[i] != NULL);
        free(u8s[i]);
      }
    free(u8s);
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "utf8toXXX:");
    assert(strcmp(utf8toupper("1"), "1") == 0);
    assert(strcmp(utf8toupper("a"), "A") == 0);
    assert(strcmp(utf8toupper("A"), "A") == 0);
    assert(strcmp(utf8toupper("ä"), "Ä") == 0);
    assert(strcmp(utf8toupper("Ä"), "Ä") == 0);
    assert(strcmp(utf8toupper("–"), "–") == 0);
    assert(strcmp(utf8toupper("𠆫"), "𠆫") == 0);
    assert(strcmp(utf8tolower("1"), "1") == 0);
    assert(strcmp(utf8tolower("a"), "a") == 0);
    assert(strcmp(utf8tolower("A"), "a") == 0);
    assert(strcmp(utf8tolower("ä"), "ä") == 0);
    assert(strcmp(utf8tolower("Ä"), "ä") == 0);
    assert(strcmp(utf8tolower("–"), "–") == 0);
    assert(strcmp(utf8tolower("𠆫"), "𠆫") == 0);
    assert(strcmp(utf8totitle("1"), "1") == 0);
    assert(strcmp(utf8totitle("a"), "A") == 0);
    assert(strcmp(utf8totitle("A"), "A") == 0);
    assert(strcmp(utf8totitle("ä"), "Ä") == 0);
    assert(strcmp(utf8totitle("Ä"), "Ä") == 0);
    assert(strcmp(utf8totitle("–"), "–") == 0);
    assert(strcmp(utf8totitle("𠆫"), "𠆫") == 0);
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "utf8isXXX:");
    assert(!utf8isupper("1"));
    assert(!utf8isupper("a"));
    assert(utf8isupper("A"));
    assert(!utf8isupper("ä"));
    assert(utf8isupper("Ä"));
    assert(!utf8isupper("–"));
    assert(!utf8isupper("𠆫"));
    assert(!utf8islower("1"));
    assert(utf8islower("a"));
    assert(!utf8islower("A"));
    assert(utf8islower("ä"));
    assert(!utf8islower("Ä"));
    assert(!utf8islower("–"));
    assert(!utf8islower("𠆫"));
    assert(!utf8istitle("1"));
    assert(!utf8istitle("A"));
    assert(!utf8istitle("a"));
    assert(!utf8istitle("Ä"));
    assert(!utf8istitle("ä"));
    assert(!utf8istitle("–"));
    assert(!utf8istitle("𠆫"));
    assert(!utf8isalpha("1"));
    assert(utf8isalpha("A"));
    assert(utf8isalpha("a"));
    assert(utf8isalpha("Ä"));
    assert(utf8isalpha("ä"));
    assert(!utf8isalpha("–"));
    assert(!utf8isalpha("𠆫"));
    assert(utf8isdigit("1"));
    assert(!utf8isdigit("A"));
    assert(!utf8isdigit("a"));
    assert(!utf8isdigit("Ä"));
    assert(!utf8isdigit("ä"));
    assert(!utf8isdigit("–"));
    assert(!utf8isdigit("𠆫"));
    assert(utf8isalnum("1"));
    assert(utf8isalnum("A"));
    assert(utf8isalnum("a"));
    assert(utf8isalnum("Ä"));
    assert(utf8isalnum("ä"));
    assert(!utf8isalnum("𠆫"));
    assert(!utf8isalnum("–"));
    assert(!utf8isspace("1"));
    assert(!utf8isspace("A"));
    assert(!utf8isspace("a"));
    assert(!utf8isspace("Ä"));
    assert(!utf8isspace("ä"));
    assert(!utf8isspace("–"));
    assert(!utf8isspace("𠆫"));
    fprintf(stdout, " ok.\n");
  }
#endif
