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

#include "utils/utf8.h"
#include "utils/ucd_data.h"
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
    if (str != NULL)
      {
        if (*saveptr == NULL)
          {
            *saveptr = str;
          }
        if (*saveptr == NULL)
          {
            return NULL;
          }
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

bool
utf8validate(const char* s)
  {
    const char* p = s;
    while (*p != '\0')
      {
        unsigned char u = static_cast<unsigned char>(*p);
        if (u <= 127)
          {
            p++;
          }
        else if ( (u & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16))
          {
            u = static_cast<unsigned char>(*(p+1));
            if ((u & (128 + 64)) != 128)
              {
                return false;
              }
            u = static_cast<unsigned char>(*(p+2));
            if ((u & (128 + 64)) != 128)
              {
                return false;
              }
            u = static_cast<unsigned char>(*(p+3));
            if ((u & (128 + 64)) != 128)
              {
                return false;
              }
            p += 4;
          }
        else if ( (u & (128 + 64 + 32)) == (128 + 64 + 32))
          {
            u = static_cast<unsigned char>(*(p+1));
            if ((u & (128 + 64)) != 128)
              {
                return false;
              }
            u = static_cast<unsigned char>(*(p+2));
            if ((u & (128 + 64)) != 128)
              {
                return false;
              }
            p += 3;
          }
        else if ( (u & (128 + 64)) == (128 + 64))
          {
            u = static_cast<unsigned char>(*(p+1));
            if ((u & (128 + 64)) != 128)
              {
                return false;
              }
            p += 2;
          }
        else
          {
            return false;
          }
      }
    return true;
  }


#if UNIT_TEST_UTF8

#include <stdio.h>
#include <string.h>
#include <assert.h>

int
main(void)
  {
    fprintf(stdout, "unit tests for %s at %u:\n", __FILE__, __LINE__);
    const char* one = "1";
    const char* A = "A";
    const char* a = "a";
    const char* Auml = "Ä";
    const char* auml = "ä";
    const char* ndash = "–";
    const char* cjk281ab = "𠆫";
    uint32_t one_cp = 49;
    uint32_t A_cp = 65;
    uint32_t a_cp = 97;
    uint32_t Auml_cp = 196;
    uint32_t auml_cp = 228;
    uint32_t ndash_cp = 8211;
    uint32_t cjk281ab_cp = 131499;
    fprintf(stdout, "utf8tocp:");
    assert(utf8tocp(one) == one_cp);
    assert(utf8tocp(A) == A_cp);
    assert(utf8tocp(a) == a_cp);
    assert(utf8tocp(Auml) == Auml_cp);
    assert(utf8tocp(auml) == auml_cp);
    assert(utf8tocp(ndash) == ndash_cp);
    assert(utf8tocp(cjk281ab) == cjk281ab_cp);
    fprintf(stdout, " ok.\n");
    fprintf(stdout, "cptoutf8:");
    char* rv = cptoutf8(one_cp);
    assert(strcmp(rv, one) == 0);
    free(rv);
    rv = cptoutf8(A_cp);
    assert(strcmp(rv, A) == 0);
    free(rv);
    rv = cptoutf8(a_cp);
    assert(strcmp(rv, a) == 0);
    free(rv);
    rv = cptoutf8(Auml_cp);
    assert(strcmp(rv, Auml) == 0);
    free(rv);
    rv = cptoutf8(ndash_cp);
    assert(strcmp(rv, ndash) == 0);
    free(rv);
    rv = cptoutf8(cjk281ab_cp);
    assert(strcmp(rv, cjk281ab) == 0);
    free(rv);
    fprintf(stdout, " ok.\n");
    char* s = strdup("1AaÄä–𠆫");
    fprintf(stdout, "utf8validate...");
    assert(utf8validate(s));
    fprintf(stdout, "ok.\n");
    const char* saveptr = 0;
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
    free(s);
    fprintf(stdout, " ok.\n");
    return EXIT_SUCCESS;
  }
#endif
