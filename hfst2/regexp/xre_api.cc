
#include "xre.h"
#include "xre_parse.hh"
#include "xre_lex.h"

extern int xrenerrs;
extern int xreparse();

namespace HFST
{
TransducerHandle
compile_xre(const char *xre)
{
	xrenerrs = 0;
	_xre_new_data(xre);
	xreparse();
	if (0 == xrenerrs)
	{
		return _xre_transducer_;
	}
	else
	{
		return 0;
	}
}

HFST::TransducerHandle
compile_xre(const char *xre,
	HFST::KeyPairSet* negation_pi, HFST::KeyPairSet* creation_pi,
	HFST::KeyTable* kt)
{
	_xre_initialise(xre, negation_pi, creation_pi, kt, false);
	return compile_xre(xre);
}

bool
xre_add_definition(const char* name, const char* xre)
{
	char* cname = strdup(name);
	// reserve key equivalent to name :-/
	HFST::Key xre_key = _xre_string_to_key(cname);
	free(cname);
	// compile expression
	TransducerHandle defcomp = compile_xre(xre);
	if ((defcomp != 0) && (!HFST::is_empty(defcomp)))
	{
		_xre_definitions[xre_key] = defcomp;
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

HFST::TransducerHandle
xre_get_last_transducer()
{
	return _xre_transducer_;
}

HFST::KeyTable*
xre_get_last_key_table()
{
	return _xre_key_table;
}

HFST::KeySet*
xre_get_last_key_set()
{
	return HFST::define_key_set(_xre_transducer_);
}

void
xre_set_key_table(HFST::KeyTable* kt)
{
	_xre_key_table = kt;
}

void
xre_set_negation_key_pair_set(HFST::KeyPairSet* kps)
{
	assert(false);
}

void
xre_set_creation_key_pair_set(HFST::KeyPairSet* kps)
{
	assert(false);
}

}



