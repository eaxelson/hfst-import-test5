//! @file xymbol-table.cc
//!
//! @brief Really ugly hacky implementation to get rid of HFST::SymbolTables.
//!
//! @todo Remove xymbol table hacks, like all the rest of xymbol stuff,
//! really

#include <string>

using std::string;

#include <hfst2/hfst.h>

#include "xymbol-table.h"

// icky globals
HFST::KeyTable* magicKeyTableWithEverything;
HWFST::KeyTable* wagicKeyTableWithEverything;

extern bool weighted;

void
initialiseLexcSymbolTablesAndSets()
{
	// create global data structures
	magicKeyTableWithEverything = HFST::create_key_table();
	string eps("@0@");
	HFST::Symbol sy = HFST::define_symbol(eps.c_str());
	HFST::associate_key(0, magicKeyTableWithEverything, sy);
	string zero("@ZERO@");
	sy = HFST::define_symbol(zero.c_str());
	HFST::associate_key(1, magicKeyTableWithEverything, sy);
	// W as well
	wagicKeyTableWithEverything = HWFST::create_key_table();
	HWFST::Symbol wy = HWFST::define_symbol(eps.c_str());
	HWFST::associate_key(0, wagicKeyTableWithEverything, wy);
	wy = HWFST::define_symbol(zero.c_str());
	HWFST::associate_key(1, wagicKeyTableWithEverything, wy);
}

void
destructLexcSymbolTablesAndSets()
{
	delete magicKeyTableWithEverything;
	delete wagicKeyTableWithEverything;
}

// vim: set ft=cpp.doxygen:
