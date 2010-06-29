//! @file string.cc
//!
//! @brief Parsing HWFST tokens from UTF-8 strings
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


#include <map>
#include <string>
#include <cassert>
#include <cstdio>

using std::string;
using std::map;

#include "../hfst.h"
#include "w_letter-trie.h"


namespace HWFST {

map<KeyTable*, LetterTrie*> theMagicLetterTries;

Key
stringToKey(const string& s, KeyTable* kt, bool addUnknown)
{
	if (!is_symbol(s.c_str()))
	{
		if (!addUnknown)
		{
			assert(is_symbol(s.c_str()) || addUnknown);
			return 0;
		}
		Symbol sy = define_symbol(s.c_str());
		Key k = get_unused_key(kt);
		associate_key(k, kt, sy);
	}
	return HWFST::get_key(HWFST::get_symbol(s.c_str()),
						 kt);
}

vector<Key>*
stringUtf8ToKeyVector(const string& s, KeyTable* kt, bool addUnknown)
{
	char* p = strdup(s.c_str());
	char* p_start = p;
	vector<Key>* keys = new vector<Key>;
	if (theMagicLetterTries.find(kt) == theMagicLetterTries.end())
	{
		LetterTrie* theMagicLetterTrie = new LetterTrie(kt);
		KeySet* init_keys = get_key_set(kt);
		for (KeySet::iterator k = init_keys->begin();
			 k != init_keys->end(); ++k)
		{
			theMagicLetterTrie->add_string(get_symbol_name(get_key_symbol(*k, kt)));
		}
		delete init_keys;
        theMagicLetterTries[kt] = theMagicLetterTrie;
	}
	while (*p != '\0')
	{
		Key k = theMagicLetterTries[kt]->find_key(&p, addUnknown);
		keys->push_back(k);
	}
	free(p_start);
	return keys;
}

KeySet*
stringUtf8ToKeySet(const string& s, KeyTable* kt, bool addUnknown)
{
	KeyVector* keys = stringUtf8ToKeyVector(s, kt, addUnknown);
	KeySet* keyset = create_empty_key_set();
	for (KeyVector::const_iterator k = keys->begin();
		 k != keys->end(); ++k)
	{
		insert_key(*k, keyset);
	}
	delete keys;
	return keyset;
}

KeyVector*
stringAsciiToKeyVector(const string& s, KeyTable* kt, bool addUnknown)
{
	const char *p = s.c_str();
	KeyVector* kv = new KeyVector;
	while (*p != '\0')
	{
		char* p_dup = (char*)malloc(sizeof(char)*2);
		p_dup[0] = p[0];
		p_dup[1] = '\0';
		string c = string(p_dup);
		Key k = stringToKey(c, kt, addUnknown);
		kv->push_back(k);
		p++;
		free(p_dup);
	}
	return kv;
}

KeySet*
stringAsciiToKeySet(const string& s, KeyTable* kt, bool addUnknown)
{
	const char *p = s.c_str();
	KeySet* ks = create_empty_key_set();
	while (*p != '\0')
	{
		char* p_dup = (char*)malloc(sizeof(char)*2);
		p_dup[0] = p[0];
		p_dup[1] = '\0';
		string c = string(p_dup);
		Key k = stringToKey(c, kt, addUnknown);
		insert_key(k, ks);
		p++;
		free(p_dup);
	}
	return ks;
}

KeyVector*
stringSeparatedToKeyVector(const string& s, KeyTable* kt, const string& sep,
		bool addUnknown)
{
	KeyVector* kv = new KeyVector;
	char* cstr = strdup(s.c_str());
	char* token = strtok(cstr, sep.c_str());
	while (token)
	{
		string c = string(token);
		Key k = stringToKey(c, kt, addUnknown);
		kv->push_back(k);
		token = strtok(NULL, sep.c_str());
	}
	return kv;
}

string*
keyToString(Key k, KeyTable* kt)
{
	string* s;
	if (is_key(k, kt))
	{
		Symbol sy = get_key_symbol(k ,kt);
		s = new string(get_symbol_name(sy));
	}
	else
	{
		char* num = static_cast<char*>(malloc(sizeof(char)*32));
		(void)sprintf(num, "%d", k);
		s = new string("");
		s->append("@").append(num).append("@");
		free(num);
	}
	return s;
}

string*
keyVectorToString(KeyVector* kv, KeyTable* kt)
{
	string* s = new string;
	for (KeyVector::const_iterator k = kv->begin();
			k != kv->end();
			++k)
	{
		string* keystring = keyToString(*k, kt);
		s->append(*keystring);
		delete keystring;
	}
	return s;
}

} // namespace HWFST

