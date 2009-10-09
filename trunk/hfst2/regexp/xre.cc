/**
 * @file xre.cc
 *
 * @brief implements xre routines.
 */

#include <map>

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "xre.h"
#include "../hfst.h"

char *_xre_data;
size_t _xre_len;
HFST::TransducerHandle _xre_transducer_;
HFST::KeySet* _xre_keys;
HFST::KeyPairSet* _xre_negation_pi;
HFST::KeyPairSet* _xre_creation_pi;
HFST::KeyTable* _xre_key_table;
std::map<HFST::Key,HFST::TransducerHandle> _xre_definitions;
bool _xre_initialised;
bool _xre_weighted;
extern char* xretext;

int
xreerror(char *msg)
{
#ifndef NDEBUG
	fprintf(stderr, "*** XRE PARSE ERROR: %s\n", msg);
	if (strlen(_xre_data) < 60)
	{
		fprintf(stderr, "***	parsing %s [near %s]\n", _xre_data, xretext);
	}
	else
	{
		fprintf(stderr, "***	parsing %60s [near %s]...\n", 
				_xre_data, xretext);
	}
#endif
	return 0;
}

int
xre_getinput(char *buf, int maxlen)
{
	int retval = 0;
	if ( maxlen > _xre_len ) {
		maxlen = _xre_len;
	}
	memcpy(buf, _xre_data, maxlen);
	_xre_data += maxlen;
	_xre_len -= maxlen;
	retval = maxlen;
	return retval;
}

char*
xre_strip_percents(const char *s)
{
	const char *c = s;
	char *stripped = (char*)calloc(sizeof(char),strlen(s)+1);
	size_t i = 0;
	while (*c != '\0')
	{
		if (*c == '%')
		{
			if (*(c + 1) == '\0')
			{
				break;
			}
			else
			{
				stripped[i] = *(c + 1);
				i++;
				c += 2;
			}
		}
		else
		{
			stripped[i] = *c;
			i++;
			c++;
		}
	}
	stripped[i] = '\0';
	return stripped;
}

char *
xre_get_quoted(const char *s)
{
	char *qstart = strchr(s, '"') + 1;
	char *qend = strrchr(s, '"');
	char* qpart = strdup(qstart);
	*(qpart+ (size_t) (qend - qstart)) = '\0';
	return qpart;
}


HFST::Key
_xre_string_to_key(char *s)
{
	Key rv;
	if (HFST::is_symbol(s))
	{
		rv = HFST::get_key(HFST::get_symbol(s), _xre_key_table);
	}
	else
	{
		HFST::Symbol sy = HFST::define_symbol(s);
		rv = HFST::get_unused_key(_xre_key_table);
		HFST::associate_key(rv, _xre_key_table, sy);
	}
	return rv;
}

static
void
_xre_maybe_insert_keypair(HFST::KeyPairSet* kps, HFST::KeyPair* kp)
{
	if (kps->find(kp) == kps->end())
	{
		kps->insert(kp);
	}
	else
	{
		delete kp;
	}
}

void
_xre_new_key(HFST::Key k)
{
	if (_xre_keys->find(k) == _xre_keys->end())
	{
		HFST::insert_key(k, _xre_keys);
		HFST::KeyPair* idkp = HFST::define_keypair(k, k);
		_xre_maybe_insert_keypair(_xre_creation_pi, idkp);
		idkp = HFST::define_keypair(k, k);
		_xre_maybe_insert_keypair(_xre_creation_pi, idkp);
		for (KeyPairSet::const_iterator ck = _xre_creation_pi->begin();
				 ck != _xre_creation_pi->end(); ++ck)
		{
			HFST::Key inp = HFST::get_input_key(*ck);
			HFST::Key outp = HFST::get_output_key(*ck);
			HFST::KeyPair* kp = HFST::define_keypair(k, inp);
			_xre_maybe_insert_keypair(_xre_creation_pi, kp);
			kp = HFST::define_keypair(inp, k);
			_xre_maybe_insert_keypair(_xre_creation_pi, kp);
			kp = HFST::define_keypair(k, outp);
			_xre_maybe_insert_keypair(_xre_creation_pi, kp);
			kp = HFST::define_keypair(outp, k);
			_xre_maybe_insert_keypair(_xre_creation_pi, kp);
		}
		for (KeyPairSet::const_iterator ck = _xre_negation_pi->begin();
				 ck != _xre_negation_pi->end(); ++ck)
		{
			HFST::Key inp = HFST::get_input_key(*ck);
			HFST::Key outp = HFST::get_output_key(*ck);
			HFST::KeyPair* kp = HFST::define_keypair(k, inp);
			_xre_maybe_insert_keypair(_xre_negation_pi, kp);
			kp = HFST::define_keypair(inp, k);
			_xre_maybe_insert_keypair(_xre_negation_pi, kp);
			kp = HFST::define_keypair(k, outp);
			_xre_maybe_insert_keypair(_xre_negation_pi, kp);
			kp = HFST::define_keypair(outp, k);
			_xre_maybe_insert_keypair(_xre_negation_pi, kp);
		}

	}
}

void
_xre_new_transducer(HFST::TransducerHandle t)
{
	HFST::KeySet* keys = HFST::define_key_set(t);
	for (HFST::KeySet::const_iterator k = keys->begin();
		 k != keys->end(); ++k)
	{
		_xre_new_key(*k);
	}
}

// XRE data structures
void
_xre_new_data(const char* xre)
{
	if (!_xre_initialised)
	{
		_xre_initialise();
	}
	_xre_transducer_ = HFST::create_epsilon_transducer();
	_xre_data = strdup(xre);
	_xre_len = strlen(_xre_data);
}

void _xre_ensure_zero_epsilon()
{
	// FIXME
}

void _xre_initialise(const char* xre, HFST::KeyPairSet* negation_pi,
					 HFST::KeyPairSet* creation_pi, HFST::KeyTable* keytable,
					 bool weighted)
{
	_xre_transducer_ = (HFST::TransducerHandle)malloc(sizeof(HFST::TransducerHandle));
	_xre_data = strdup(xre);
	_xre_len = strlen(_xre_data);
	_xre_negation_pi = negation_pi;
	_xre_creation_pi = creation_pi;
	_xre_keys = HFST::create_empty_key_set();
	_xre_key_table = keytable;
	_xre_ensure_zero_epsilon();
	_xre_weighted = false;
	_xre_initialised = true;
}

void _xre_initialise()
{
	_xre_transducer_ = (HFST::TransducerHandle)malloc(sizeof(HFST::TransducerHandle));
	_xre_data = (char*)calloc(sizeof(char), 1);
	_xre_len = 0;
	_xre_negation_pi = HFST::create_empty_keypair_set();
	_xre_creation_pi = HFST::create_empty_keypair_set();
	_xre_keys = HFST::create_empty_key_set();
	//_xre_new_key(0);
	// does not have an effect ?
	_xre_negation_pi = HFST::insert_keypair(HFST::define_keypair(0,0), _xre_negation_pi);
	_xre_creation_pi = HFST::insert_keypair(HFST::define_keypair(0,0), _xre_creation_pi);
	//
	_xre_key_table = HFST::create_key_table();
	HFST::Symbol zeroeps = HFST::define_symbol("@0@");
	HFST::associate_key(0, _xre_key_table, zeroeps);
	_xre_ensure_zero_epsilon();
	_xre_weighted = false;
	_xre_initialised = true;
}

HFST::TransducerHandle
_xre_make_key_pair(HFST::Key k1, HFST::Key k2)
{
	HFST::TransducerHandle rv = (HFST::TransducerHandle)malloc(sizeof(HFST::TransducerHandle));
	if ( (k1 == ANY_KEY) && (k2 == ANY_KEY))
	{
		// any id pair
		HFST::KeyPairSet* id_set = HFST::create_empty_keypair_set();
		for (KeyPairSet::const_iterator ck = _xre_creation_pi->begin();
			 ck != _xre_creation_pi->end(); ++ck)
		{
			HFST::Key inp = HFST::get_input_key(*ck);
			HFST::Key outp = HFST::get_output_key(*ck);
			id_set->insert(HFST::define_keypair(inp, inp));
			id_set->insert(HFST::define_keypair(outp, outp));
		}
		rv = HFST::define_transducer(id_set);
	}
	else if ( k1 == ANY_KEY )
	{
		HFST::KeyPairSet* any_set = HFST::create_empty_keypair_set();
		for (KeyPairSet::const_iterator ck = _xre_creation_pi->begin();
			 ck != _xre_creation_pi->end(); ++ck)
		{
			HFST::Key inp = HFST::get_input_key(*ck);
			HFST::Key outp = HFST::get_output_key(*ck);
			any_set->insert(HFST::define_keypair(inp, k2));
			any_set->insert(HFST::define_keypair(outp, k2));
		}
		rv = HFST::define_transducer(any_set);
	}
	else if ( k2 == ANY_KEY )
	{
		HFST::KeyPairSet* any_set = HFST::create_empty_keypair_set();
		for (KeyPairSet::const_iterator ck = _xre_creation_pi->begin();
			 ck != _xre_creation_pi->end(); ++ck)
		{
			HFST::Key inp = HFST::get_input_key(*ck);
			HFST::Key outp = HFST::get_output_key(*ck);
			any_set->insert(HFST::define_keypair(k1, inp));
			any_set->insert(HFST::define_keypair(k1, outp));
		}
		rv = HFST::define_transducer(any_set);
	}
	else
	{
		// regular key pair
		HFST::KeyPair* kp = HFST::define_keypair(k1, k2);
		rv = HFST::define_transducer(kp);
	}
	return rv;
}

