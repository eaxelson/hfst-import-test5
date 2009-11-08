//! @file string-munging.cc
//!
//! @brief Implementation of some string handling in HFST lexc.
//!
//! @author Antoine Trux
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
#include <vector>

#include <cstdlib>
#include <cassert>
#include <cstring>

#ifdef DEBUG
#include <cstdio>
#endif

#include "string-munging.h"

using std::string;


// string mangling
static
string&
replace_all(string& haystack, const string& needle, const string& replacement)
{
        size_t last_needle = haystack.find(needle);
        size_t needle_len = needle.length();
        while(last_needle != string::npos)
        {
                haystack.replace(last_needle, needle_len, replacement);
                last_needle = haystack.find(needle);
        }
        return haystack;
}


string&
stripPercents(string& s)
{
	string& stripped = s;
	stripped = replace_all(stripped, "%%", "<__REALPERCENT>");
	stripped = replace_all(stripped, "%", "");
	stripped = replace_all(stripped, "<__REALPERCENT>", "%");
	return stripped;
}

string&
addPercents(string& s)
{
	string& added = s;
	added = replace_all(added, "%", "%%");
	added = replace_all(added, "<", "%<");
	added = replace_all(added, ">", "%>");
	return added;
}

string&
joinerEncode(string& s)
{
	string& lxs = s;
	lxs = lxs.insert(0, LEXC_JOINER_START);
	lxs = lxs.append(LEXC_JOINER_END);
	return lxs;
}

string&
joinerDecode(string& s)
{
	assert(s.length() >= 4);
	string& decoded = s;
	size_t jStart = strlen(LEXC_JOINER_START);
	size_t jEnd = strlen(LEXC_JOINER_END);
	decoded = decoded.substr(jStart, (s.length() - (jStart + jEnd)));
	return decoded;
}

string&
xreDefinitionEncode(string& s)
{
	string& lxs = s;
	lxs = lxs.insert(0, LEXC_DFN_START);
	lxs = lxs.append(LEXC_DFN_END);
	return lxs;
}

string&
xreDefinitionDecode(string& s)
{
	assert(s.length() >= 4);
	string& decoded = s;
	size_t jStart = strlen(LEXC_DFN_START);
	size_t jEnd = strlen(LEXC_DFN_END);
	decoded = decoded.substr(jStart, (s.length() - (jStart + jEnd)));
	return decoded;
}

const char*
skip_non_spaces(const char* pString)
{
       while (!isspace(*pString) && (*pString != '\0'))
               ++pString;
       return pString;
}

const char*
skip_spaces(const char* pString)
{
       while (isspace(*pString) && (*pString != '\0'))
               ++pString;
       return pString;
}


// vim: set ft=cpp.doxygen:

