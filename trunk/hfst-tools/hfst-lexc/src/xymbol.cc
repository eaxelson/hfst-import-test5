//!
//! @file xymbol.cc
//!
//! @brief My re-implementation of deformed bridge pattern over
//! symbols/keys/Characters/arcs/things/stuff or whatever concept du jour
//! for them is in HFST.
//! This is only done to remove my confusion over HFST concepts in single
//! file rather than spread it around all source code.
//!
//! @author Tommi A. Pirinen
//! 
//! @todo this should not exist at all, or exist implemented on some level in
//! the API.

//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, version 3 of the Licence.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#if DEBUG
#  include <cstdio>
#endif

#include <cstdlib>
#include <cassert>

#include <string>

using std::string;

#if NESTED_BUILD
#include <hfst2/string/string.h>
#endif
#if !NESTED_BUILD
#include <hfst2/string.h>
#endif

#include "xymbol.h"
#include "xymbol-table.h"

extern bool weighted;

Xymbol::Xymbol() :
	name_(""),
	key_(0),
	wey_(0),
	valid_(false),
	walid_(false)
{
#if 0
	fprintf(stderr, "Xymbol default constructed\n");
#endif
	w_ = weighted;
}


Xymbol::Xymbol(const std::string& name) :
	name_(name)
{
#	if 0
	fprintf(stderr, "Trying to construct Xymbol(`%s') W=%d\n", name.c_str(),
		weighted);
#	endif
	assert(name_ != "");
	w_ = weighted;
	if (!w_)
	{
		valid_ = true;
		key_ = HFST::stringToKey(name_, magicKeyTableWithEverything, true);
	}
	else if (w_)
	{
		walid_ = true;
		wey_ = HWFST::stringToKey(name_, wagicKeyTableWithEverything, true);
	}
}

Xymbol::Xymbol(const std::string& name, long value) :
	name_(name)
{
	w_ = weighted;
	assert(name_ != "");
#	if 0
	fprintf(stderr, "Trying to construct Xymbol(%s, %ld) W=%d\n", name.c_str(),
		value, weighted);
#	endif
	if (!w_)
	{
		key_ = value;
		symbol_ = HFST::define_symbol(name.c_str());
		HFST::associate_key(key_, magicKeyTableWithEverything, symbol_);
		valid_ = true;
	}
	else if (w_)
	{
		wey_ = value;
		wymbol_ = HWFST::define_symbol(name.c_str());
		HWFST::associate_key(wey_, wagicKeyTableWithEverything, wymbol_);
		walid_ = true;
	}
}

Xymbol::Xymbol(unsigned long value)
{
#	if 0
	fprintf(stderr, "Trying to construct Xymbol(%ld) W=%d\n", value,
		weighted);
#	endif
	w_ = weighted;
	if (!w_)
	{
		key_ = value;
		symbol_ = HFST::get_key_symbol(key_, magicKeyTableWithEverything);
		name_ = HFST::get_symbol_name(symbol_);
		valid_ = true;
	}
	else if (w_)
	{
		wey_ = value;
		wymbol_ = HWFST::get_key_symbol(wey_, wagicKeyTableWithEverything);
		name_ = HWFST::get_symbol_name(wymbol_);
		walid_ = true;
	}
}

Xymbol::Xymbol(const Xymbol& rhs) :
	name_(rhs.name_)
{
	w_ = weighted;
	if (!w_)
	{
		key_ = HFST::stringToKey(name_, magicKeyTableWithEverything, true);
		symbol_ = HFST::get_key_symbol(key_, magicKeyTableWithEverything);
		valid_ = true;
	}
	else if (w_)
	{
		wey_ = HWFST::stringToKey(name_, wagicKeyTableWithEverything, true);
		wymbol_ = HWFST::get_key_symbol(wey_, wagicKeyTableWithEverything);
		walid_ = true;
	}
}

bool
Xymbol::operator<(const Xymbol& ref) const
{
	if (!w_)
	{
		return key_ < ref.key_;
	}
	else if (w_)
	{
		return wey_ < ref.wey_;
	}
	assert(false);
	return true;
}

bool
Xymbol::operator==(const Xymbol& rhs) const
{
	if (!w_)
	{
		return key_ == rhs.key_;
	}
	else
	{
		return wey_ == rhs.wey_;
	}
}

bool
Xymbol::operator!=(const Xymbol& rhs) const
{
	return !(*this == rhs);
}

Xymbol&
Xymbol::operator=(const Xymbol& rhs)
{
	w_ = weighted;
	assert(w_ == rhs.w_);
	name_ = rhs.name_;
	if (!w_)
	{
		valid_ = true;
		key_ = HFST::stringToKey(name_, magicKeyTableWithEverything, true);
		symbol_ = HFST::get_key_symbol(key_, magicKeyTableWithEverything);
	}
	else if (w_)
	{
		walid_ = true;
		wey_ = HWFST::stringToKey(name_, wagicKeyTableWithEverything, true);
		wymbol_ = HWFST::get_key_symbol(wey_, wagicKeyTableWithEverything);
	}
	return *this;
}

HFST::Key
Xymbol::getKey() const
{
	assert(valid_);
	return key_;
}

HWFST::Key
Xymbol::getWey() const
{
	assert(walid_);
	return wey_;
}

void
Xymbol::dumpDebugString() const
{
	string debugstring = string("{");
	debugstring.append(string("name: ")).append(name_);
	if (!w_)
	{
		char* keystr = static_cast<char*>(malloc(sizeof(char)*12+1));
		snprintf(keystr, 12, "0x%X", key_);
		debugstring.append(string(", key: ")).append(string(keystr));
		free(keystr);
	}
	else if (w_)
	{
		char* keystr = static_cast<char*>(malloc(sizeof(char)*12+1));
		snprintf(keystr, 12, "0x%X", wey_);
		debugstring.append(string(", wey: ")).append(string(keystr));
		free(keystr);
	}
	debugstring.append(string("}"));
	fprintf(stderr, "%s\n", debugstring.c_str());
}

const string&
Xymbol::getName() const
{
	assert(valid_ || walid_);
	return name_;
}

// vim: set ft=cpp.doxygen:


