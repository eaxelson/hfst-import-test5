//! @file xymbol-bridges.cc
//! 
//! @brief Kludges to encapsulate Xymbols in STL containers and conversions
//! between HFST and these.

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <hfst2/hfst.h>

#if NESTED_BUILD
#include <hfst2/string/string.h>
#endif
#if !NESTED_BUILD
#include <hfst2/string.h>
#endif

#include "xymbol.h"
#include "xducer.h"
#include "xymbol-table.h"
#include "xymbol-bridges.h"
// misc. legacy functions that wait for conversion to OOP constructors and
// ops
XymbolPair*
xymbolToIdentityXymbolPair(const Xymbol& k)
{
	return new XymbolPair(k, k);
}

XymbolPairSet*
xymbolSetToIdentityXymbolPairSet(const XymbolSet& ks)
{
	XymbolPairSet* xps = new XymbolPairSet();
	for (XymbolSet::const_iterator x = ks.begin(); 
		 x != ks.end(); ++x)
	{
		xps->insert(XymbolPair(*x, *x));
	}
	return xps;
}

XymbolPairVector*
xymbolVectorToIdentityXymbolPairVector(const XymbolVector& xv)
{
	XymbolPairVector* xpv = new XymbolPairVector;
	for (XymbolVector::const_iterator x = xv.begin();
		 x != xv.end(); ++x)
	{
		xpv->push_back(XymbolPair(*x, *x));
	}
	return xpv;
}

XymbolSet*
xymbolPairSetToXymbolSet(const XymbolPairSet& xps)
{
	XymbolSet* xs = new XymbolSet;
	for (XymbolPairSet::const_iterator xp = xps.begin();
		 xp != xps.end(); ++xp)
	{
		xs->insert(xp->first);
		xs->insert(xp->second);
	}
	return xs;
}


HFST::KeySet*
xymbolSetToKeySet(const XymbolSet& xs)
{
	HFST::KeySet* ks = HFST::create_empty_key_set();
	for (XymbolSet::const_iterator x = xs.begin();
		 x != xs.end(); ++x)
	{
		HFST::insert_key(x->getKey(), ks);
	}
	return ks;
}

HWFST::KeySet*
xymbolSetToWeySet(const XymbolSet& xs)
{
	HWFST::KeySet* ks = HWFST::create_empty_key_set();
	for (XymbolSet::const_iterator x = xs.begin();
		 x != xs.end(); ++x)
	{
		HWFST::insert_key(x->getWey(), ks);
	}
	return ks;
}

HFST::KeyVector*
xymbolVectorToKeyVector(const XymbolVector& xv)
{
	HFST::KeyVector* kv = new HFST::KeyVector;
	for (XymbolVector::const_iterator x = xv.begin();
		x != xv.end(); ++x)
	{
		kv->push_back(x->getKey());
	}
	return kv;
}

HWFST::KeyVector*
xymbolVectorToWeyVector(const XymbolVector& xv)
{
	HWFST::KeyVector* kv = new HWFST::KeyVector;
	for (XymbolVector::const_iterator x = xv.begin();
		x != xv.end(); ++x)
	{
		kv->push_back(x->getWey());
	}
	return kv;
}

HFST::KeyPair*
xymbolPairToKeyPair(const XymbolPair& xp)
{
	return HFST::define_keypair(xp.first.getKey(), xp.second.getKey());
}

HWFST::KeyPair*
xymbolPairToWeyPair(const XymbolPair& xp)
{
	return HWFST::define_keypair(xp.first.getWey(), xp.second.getWey());
}

HFST::KeyPairSet*
xymbolPairSetToKeyPairSet(const XymbolPairSet& xps)
{
	HFST::KeyPairSet* kps = HFST::create_empty_keypair_set();
	for (XymbolPairSet::const_iterator xp = xps.begin();
		 xp != xps.end(); ++xp)
	{
		HFST::insert_keypair(xymbolPairToKeyPair(*xp), kps);
	}
	return kps;
}

HWFST::KeyPairSet*
xymbolPairSetToWeyPairSet(const XymbolPairSet& xps)
{
	HWFST::KeyPairSet* kps = HWFST::create_empty_keypair_set();
	for (XymbolPairSet::const_iterator xp = xps.begin();
		 xp != xps.end(); ++xp)
	{
		HWFST::insert_keypair(xymbolPairToWeyPair(*xp), kps);
	}
	return kps;
}

HFST::KeyPairVector*
xymbolPairVectorToKeyPairVector(const XymbolPairVector& xpv)
{
	HFST::KeyPairVector* kpv = new HFST::KeyPairVector;
	for (XymbolPairVector::const_iterator xp = xpv.begin();
		 xp != xpv.end(); ++xp)
	{
		HFST::KeyPair* kp = xymbolPairToKeyPair(*xp);
		kpv->push_back(kp);
	}
	return kpv;
}

HWFST::KeyPairVector*
xymbolPairVectorToWeyPairVector(const XymbolPairVector& xpv)
{
	HWFST::KeyPairVector* kpv = new HWFST::KeyPairVector;
	for (XymbolPairVector::const_iterator xp = xpv.begin();
		 xp != xpv.end(); ++xp)
	{
		HWFST::KeyPair* kp = xymbolPairToWeyPair(*xp);
		kpv->push_back(kp);
	}
	return kpv;
}

Xymbol*
keyToXymbol(HFST::Key k)
{
	return new Xymbol(static_cast<unsigned long int>(k));
}

Xymbol* 
weyToXymbol(HWFST::Key k)
{
	return new Xymbol(static_cast<unsigned long int>(k));
}

XymbolPair*
keyPairToXymbolPair(HFST::KeyPair* k)
{
	Xymbol upper = Xymbol(static_cast<unsigned long int>(HFST::get_input_key(k)));
	Xymbol lower = Xymbol(static_cast<unsigned long int>(HFST::get_output_key(k)));
	return new XymbolPair(upper, lower);
}

XymbolPair*
weyPairToXymbolPair(HWFST::KeyPair* k)
{
	Xymbol upper = Xymbol(static_cast<unsigned long int>(HWFST::get_input_key(k)));
	Xymbol lower = Xymbol(static_cast<unsigned long int>(HWFST::get_output_key(k)));
	return new XymbolPair(upper, lower);
}

XymbolSet*
keySetToXymbolSet(HFST::KeySet* ks)
{
	XymbolSet* xs = new XymbolSet();
	for (HFST::KeySet::const_iterator k = ks->begin();
		 k != ks->end(); ++k)
	{
		xs->insert(Xymbol(static_cast<unsigned long int>(*k)));
	}
	return xs;
}

XymbolSet*
weySetToXymbolSet(HWFST::KeySet* ks)
{
	XymbolSet* xs = new XymbolSet();
	for (HWFST::KeySet::const_iterator k = ks->begin();
		 k != ks->end(); ++k)
	{
		xs->insert(Xymbol(static_cast<unsigned long int>(*k)));
	}
	return xs;

}

XymbolVector*
keyVectorToXymbolVector(HFST::KeyVector* kv)
{
	XymbolVector* xv = new XymbolVector();
	for (HFST::KeyVector::const_iterator k = kv->begin();
		 k != kv->end(); ++k)
	{
		xv->push_back(Xymbol(static_cast<unsigned long int>(*k)));
	}
	return xv;
}

XymbolVector*
weyVectorToXymbolVector(HWFST::KeyVector* kv)
{
	XymbolVector* xv = new XymbolVector();
	for (HWFST::KeyVector::const_iterator k = kv->begin();
		 k != kv->end(); ++k)
	{
		xv->push_back(Xymbol(static_cast<unsigned long int>(*k)));
	}
	return xv;
}

XymbolPairSet*
keyPairSetToXymbolPairSet(HFST::KeyPairSet* kps)
{
	XymbolPairSet* xps = new XymbolPairSet();
	for (HFST::KeyPairSet::const_iterator kp = kps->begin();
		 kp != kps->end(); ++kp)
	{
		xps->insert(*keyPairToXymbolPair(*kp));
	}
	return xps;
}

XymbolPairSet*
weyPairSetToXymbolPairSet(HWFST::KeyPairSet* kps)
{
	XymbolPairSet* xps = new XymbolPairSet();
	for (HWFST::KeyPairSet::const_iterator kp = kps->begin();
		 kp != kps->end(); ++kp)
	{
		xps->insert(*weyPairToXymbolPair(*kp));
	}
	return xps;
}

XymbolPairVector*
keyPairVectorToXymbolPairVector(HFST::KeyPairVector* kpv)
{
	XymbolPairVector* xpv = new XymbolPairVector();
	for (HFST::KeyPairVector::const_iterator kp = kpv->begin();
		 kp != kpv->end(); ++kp)
	{
		XymbolPair* xp = keyPairToXymbolPair(*kp);
		xpv->push_back(*xp);
		delete xp;
	}
	return xpv;
}

XymbolPairVector*
weyPairVectorToXymbolPairVector(HWFST::KeyPairVector* kpv)
{
	XymbolPairVector* xpv = new XymbolPairVector();
	for (HWFST::KeyPairVector::const_iterator kp = kpv->begin();
		 kp != kpv->end(); ++kp)
	{
		XymbolPair* xp = weyPairToXymbolPair(*kp);
		xpv->push_back(*xp);
		delete xp;
	}
	return xpv;
}

XymbolVector*
xymbolVectorFromUtf8String(const std::string& s)
{
	XymbolVector* xv = new XymbolVector();
	if (!weighted)
	{
		HFST::KeyVector* kv = HFST::stringUtf8ToKeyVector(s, 
			magicKeyTableWithEverything, true);
		for (HFST::KeyVector::const_iterator k = kv->begin();
			 k != kv->end(); ++k)
		{
			xv->push_back(Xymbol(static_cast<unsigned long int>(*k)));
		}
		delete kv;
	}
	else if (weighted)
	{
		HWFST::KeyVector* kv = HWFST::stringUtf8ToKeyVector(s, 
			wagicKeyTableWithEverything, true);
		for (HWFST::KeyVector::const_iterator k = kv->begin();
			 k != kv->end(); ++k)
		{
			xv->push_back(Xymbol(static_cast<unsigned long int>(*k)));
		}
		delete kv;
	}
	return xv;
}

XymbolSet*
xymbolSetFromUtf8String(const std::string& s)
{
	XymbolSet* xs = NULL;
	if (!weighted)
	{
		HFST::KeySet* ks = HFST::stringUtf8ToKeySet(s, 
			magicKeyTableWithEverything, true);
		xs = keySetToXymbolSet(ks);
		delete ks;
	}
	else if (weighted)
	{
		HWFST::KeySet* ks = HWFST::stringUtf8ToKeySet(s, 
			wagicKeyTableWithEverything, true);
		xs = weySetToXymbolSet(ks);
		delete ks;
	}
	return xs;
}


XymbolPairVector*
xymbolVectorsAlignLeft(const XymbolVector& upper, const XymbolVector& lower)
{
	XymbolPairVector* xpv = new XymbolPairVector();
	XymbolVector::const_iterator upperIt = upper.begin();
	XymbolVector::const_iterator lowerIt = lower.begin();
	while ((upperIt != upper.end()) || (lowerIt != lower.end()))
	{
		if (upperIt == upper.end() && lowerIt == lower.end())
		{
			break;
		}
		else if (upperIt == upper.end())
		{
			xpv->push_back(XymbolPair(Xymbol(static_cast<long unsigned int>(0)), *lowerIt));
			++lowerIt;
		}
		else if (lowerIt == lower.end())
		{
			xpv->push_back(XymbolPair(*upperIt, Xymbol(static_cast<long unsigned int>(0))));
			++upperIt;
		}
		else
		{
			xpv->push_back(XymbolPair(*upperIt, *lowerIt));
			++lowerIt;
			++upperIt;
		}
	}
	return xpv;
}

XymbolSet*
xymbolSetFromXducer(Xducer& x)
{
	XymbolSet* xs;
	if (!weighted)
	{
		HFST::KeySet* ks = HFST::define_key_set(x.getSfstDucer());
		xs = keySetToXymbolSet(ks);
		delete ks;
	}
	else if (weighted)
	{
		HWFST::KeySet* ks = HWFST::define_key_set(x.getOpenfstDucer());
		xs = weySetToXymbolSet(ks);
		delete ks;
	}
	return xs;
}

// vim: set ft=cpp.doxygen:
