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
HFST::KeySet* _xre_negation_sigma;
HFST::KeySet* _xre_creation_sigma;
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
	char *qstart = strchr((char*) s, '"') + 1;
	char *qend = strrchr((char*) s, '"');
	char* qpart = strdup(qstart);
	*(qpart+ (size_t) (qend - qstart)) = '\0';
	return qpart;
}

HFST::KeyPairSet*
_xre_creation_pi()
{
	HFST::KeyPairSet* pi = new HFST::KeyPairSet;
	for (HFST::KeySet::const_iterator k = _xre_creation_sigma->begin();
			k != _xre_creation_sigma->end();
			++k)
	{
		for (HFST::KeySet::const_iterator k2 = _xre_creation_sigma->begin();
				k2 != _xre_creation_sigma->end();
				++k2)
		{
			pi->insert(HFST::define_keypair(*k, *k2));
		}
	}
	return pi;
}

HFST::KeyPairSet*
_xre_negation_pi()
{
	HFST::KeyPairSet* pi = new HFST::KeyPairSet;
	for (HFST::KeySet::const_iterator k = _xre_negation_sigma->begin();
			k != _xre_negation_sigma->end();
			++k)
	{
		for (HFST::KeySet::const_iterator k2 = _xre_negation_sigma->begin();
				k2 != _xre_negation_sigma->end();
				++k2)
		{
			pi->insert(HFST::define_keypair(*k, *k2));
		}
	}
	return pi;
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

void
_xre_new_key(HFST::Key k)
{
	if (_xre_keys->find(k) == _xre_keys->end())
	{
		HFST::insert_key(k, _xre_keys);
		HFST::insert_key(k, _xre_creation_sigma);
		HFST::insert_key(k, _xre_negation_sigma);
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

void _xre_initialise(const char* xre, HFST::KeyPairSet* negation_sigma,
					 HFST::KeyPairSet* creation_sigma, HFST::KeyTable* keytable,
					 bool weighted)
{
	_xre_transducer_ = (HFST::TransducerHandle)malloc(sizeof(HFST::TransducerHandle));
	_xre_data = strdup(xre);
	_xre_len = strlen(_xre_data);
	_xre_negation_sigma = new HFST::KeySet();
	for (HFST::KeyPairSet::const_iterator kp= negation_sigma->begin();
			kp != negation_sigma->end();
			++kp)
	{
		_xre_negation_sigma->insert(HFST::get_input_key(*kp));
		_xre_negation_sigma->insert(HFST::get_output_key(*kp));
	}
	_xre_creation_sigma = new HFST::KeySet();
	for (HFST::KeyPairSet::const_iterator kp= creation_sigma->begin();
			kp != creation_sigma->end();
			++kp)
	{
		_xre_creation_sigma->insert(HFST::get_input_key(*kp));
		_xre_creation_sigma->insert(HFST::get_output_key(*kp));
	}
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
	_xre_negation_sigma = HFST::create_empty_key_set();
	_xre_creation_sigma = HFST::create_empty_key_set();
	_xre_keys = HFST::create_empty_key_set();
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
		for (KeySet::const_iterator ck = _xre_creation_sigma->begin();
			 ck != _xre_creation_sigma->end(); ++ck)
		{
			id_set->insert(HFST::define_keypair(*ck, *ck));
		}
		rv = HFST::define_transducer(id_set);
	}
	else if ( k1 == ANY_KEY )
	{
		// ?:k2
		HFST::KeyPairSet* any_set = HFST::create_empty_keypair_set();
		for (KeySet::const_iterator ck = _xre_creation_sigma->begin();
			 ck != _xre_creation_sigma->end(); ++ck)
		{
				any_set->insert(HFST::define_keypair(*ck, k2));
		}
		rv = HFST::define_transducer(any_set);
	}
	else if ( k2 == ANY_KEY )
	{
		// k1:?
		HFST::KeyPairSet* any_set = HFST::create_empty_keypair_set();
		for (KeySet::const_iterator ck = _xre_creation_sigma->begin();
			 ck != _xre_creation_sigma->end(); ++ck)
		{
			any_set->insert(HFST::define_keypair(k1, *ck));
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

