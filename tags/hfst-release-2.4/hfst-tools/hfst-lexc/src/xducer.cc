//!
//! @file xducer.cc
//!
//! @brief Implementation of a silly HFST-HWFST bridge.
//!
//! @author Tommi A. Pirinen

//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, version 3 of the License.
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
#include <cassert>
#include <string>
#include <map>
#include <queue>

using std::string;
using std::map;
using std::queue;

#include <hfst2/hfst.h>

#if NESTED_BUILD
#include <hfst2/string/string.h>
#include <hfst2/regexp/regexp.h>
#endif
#if !NESTED_BUILD
#include <hfst2/string.h>
#include <hfst2/regexp.h>
#endif

#include "xducer.h"
#include "xymbol.h"
#include "xymbol-table.h"
#include "xymbol-bridges.h"

extern bool weighted;

Xducer::Xducer() :
	keyPair_(0),
	weyPair_(0),
	sfstValid_(false),
	openfstValid_(false),
	isTrie_(false),
	isKeyPair_(false)
{
	if (weighted)
	{
		w_ = true;
	}
	else
	{
		w_ = false;
	}
}

Xducer::~Xducer()
{
	if (sfstValid_)
	{
		HFST::delete_transducer(sfstDucer_);
		sfstValid_ = false;
	}
	if (openfstValid_)
	{
		HWFST::delete_transducer(openfstDucer_);
		openfstValid_ = false;
	}
}

// we do not guarantee tokeniserness, trieness, etc. on copy
Xducer::Xducer(const Xducer& ref) :
	keyPair_(0),
	weyPair_(0),
	isTrie_(false),
	isKeyPair_(false)
{
	w_ = ref.w_;
	if (w_ == false)
	{
		if (ref.isValid())
		{
			sfstDucer_ = ref.copySfstDucer();
			sfstValid_ = true;
		}
		else
		{
			sfstValid_ = false;
		}
		openfstValid_ = false;
	}
	else if (w_ == true)
	{
		if (ref.isValid())
		{
			openfstDucer_ = ref.copyOpenfstDucer();
			openfstValid_ = true;
		}
		else
		{
			openfstValid_ = false;
		}
		sfstValid_ = false;
	}
}

Xducer&
Xducer::operator=(const Xducer& rhs)
{
	isTrie_ = false;
	isKeyPair_ = false;
	w_ = rhs.w_;
	if (sfstValid_)
	{
		HFST::delete_transducer(sfstDucer_);
	}
	if (openfstValid_)
	{
		HWFST::delete_transducer(openfstDucer_);
	}
	if (w_ == false)
	{
		if (rhs.isValid())
		{
			sfstDucer_ = rhs.copySfstDucer();
			sfstValid_ = true;
		}
		else
		{
			sfstValid_ = false;
		}
		openfstValid_ = false;
	}
	else if (w_ == true)
	{
		if (rhs.isValid())
		{
			openfstDucer_ = rhs.copyOpenfstDucer();
			openfstValid_ = true;
		}
		else
		{
			openfstValid_ = false;
		}
		sfstValid_ = false;
	}
	return *this;
}

Xducer::Xducer(const Xymbol& x)
{
	sfstValid_ = false;
	openfstValid_ = false;
	XymbolPair xp = XymbolPair(x, x);
	w_ = weighted;
	if (w_ == false)
	{
		keyPair_ = xymbolPairToKeyPair(xp);
		sfstDucer_ = HFST::define_transducer(keyPair_);
		sfstValid_ = true;
	}
	else if (w_ == true)
	{
		weyPair_ = xymbolPairToWeyPair(xp);
		openfstDucer_ = HWFST::define_transducer(weyPair_);
		openfstValid_ = true;
	}
	isTrie_ = true;
	isKeyPair_ = true;
}

Xducer::Xducer(const XymbolPair& xp)
{
	sfstValid_ = false;
	openfstValid_ = false;
	w_ = weighted;
	if (w_ == false)
	{
		keyPair_ = xymbolPairToKeyPair(xp);
		sfstDucer_ = HFST::define_transducer(keyPair_);
		sfstValid_ = true;
	}
	else if (w_ == true)
	{
		weyPair_ = xymbolPairToWeyPair(xp);
		openfstDucer_ = HWFST::define_transducer(weyPair_);
		openfstValid_ = true;
	}
	isTrie_ = true;
	isKeyPair_ = true;
}

Xducer::Xducer(const XymbolPairVector& chars)
{
	sfstValid_ = false;
	openfstValid_ = false;
	w_ = weighted;
	makeEpsilon();
	for (XymbolPairVector::const_iterator x = chars.begin();
		x != chars.end(); ++x)
	{
		Xducer cducer;
		cducer = cducer.make(*x);
		concatenate(cducer);
		minimise(); //XXX: should we?
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
}

Xducer::Xducer(const XymbolVector& chars)
{
	sfstValid_ = false;
	openfstValid_ = false;
	w_ = weighted;
	makeEpsilon();
	for (XymbolVector::const_iterator x = chars.begin();
		x != chars.end(); ++x)
	{
		Xducer cducer;
		cducer = cducer.make(*x);
		concatenate(cducer);
		minimise(); // XXX: should we?
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
}

Xducer::Xducer(const XymbolPairSet& chars)
{
	sfstValid_ = false;
	openfstValid_ = false;
	w_ = weighted;
	makeEpsilon();
	for (XymbolPairSet::const_iterator c = chars.begin();
		c != chars.end(); ++c)
	{
		Xducer cducer;
		cducer = cducer.make(*c);
		disjunct(cducer);
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
}

Xducer::Xducer(const XymbolSet& chars)
{
	sfstValid_ = false;
	openfstValid_ = false;
	w_ = weighted;
	makeEmpty();
	for (XymbolSet::const_iterator x = chars.begin();
		x != chars.end(); ++x)
	{
		Xducer cducer = Xducer(*x);
		disjunct(cducer);
		minimise(); // XXX: should we?
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
}

// pseudo constructors
Xducer&
Xducer::make(const Xymbol& c)
{
	if (c == (Xymbol(static_cast<long unsigned int>(0))))
	{
		return makeEpsilon();
	}
	else
	{
		return make(XymbolPair(c, c));
	}
	isTrie_ = true;
	isKeyPair_ = true;

}

Xducer&
Xducer::make(const XymbolPair& xp)
{
	if (w_ == false)
	{
		keyPair_ = xymbolPairToKeyPair(xp);
		sfstDucer_ = HFST::define_transducer(keyPair_);
		sfstValid_ = true;
	}
	else if (w_ == true)
	{
		weyPair_ = xymbolPairToWeyPair(xp);
		openfstDucer_ = HWFST::define_transducer(weyPair_);
		openfstValid_ = true;
	}
	isTrie_ = true;
	isKeyPair_ = true;
	return *this;
}

Xducer&
Xducer::make(const XymbolPairVector& chars)
{
	makeEpsilon();
	for (XymbolPairVector::const_iterator x = chars.begin();
		x != chars.end(); ++x)
	{
		Xducer cducer;
		cducer = cducer.make(*x);
		concatenate(cducer);
		minimise(); //XXX: should we?
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
	return *this;
}

Xducer&
Xducer::make(const XymbolVector& chars)
{
	makeEpsilon();
	for (XymbolVector::const_iterator x = chars.begin();
		x != chars.end(); ++x)
	{
		Xducer cducer;
		cducer = cducer.make(*x);
		concatenate(cducer);
		minimise(); // XXX: should we?
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
	return *this;
}

Xducer&
Xducer::make(const XymbolPairSet& chars)
{
	makeEpsilon();
	for (XymbolPairSet::const_iterator c = chars.begin();
		c != chars.end(); ++c)
	{
		Xducer cducer;
		cducer = cducer.make(*c);
		disjunct(cducer);
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
	return *this;
}

Xducer&
Xducer::make(const XymbolSet& chars)
{
	makeEmpty();
	for (XymbolSet::const_iterator x = chars.begin();
		x != chars.end(); ++x)
	{
		Xducer cducer;
		cducer = cducer.make(*x);
		disjunct(cducer);
		minimise(); // XXX: should we?
	}
	isTrie_ = true;
	isKeyPair_ = false;
	keyPair_ = 0;
	weyPair_ = 0;
	return *this;
}

// assertions
bool
Xducer::isOperableWith(const Xducer& other) const
{
	return ( (other.isValid() && this->isValid()) &&
		(this->isWeighted() == other.isWeighted()) );
}

void
Xducer::invalidate()
{
	sfstValid_ = false;
	openfstValid_ = false;
}

bool
Xducer::isValid() const
{
	if (w_ == false)
	{
		return sfstValid_;
	}
	else if (w_ == true)
	{
		return openfstValid_;
	}
	else
	{
		return false;
	}
	return false;
}

// getters
bool
Xducer::isEmpty() const
{
	assert (isValid());
	if (w_ == false)
	{
		return HFST::is_empty(sfstDucer_);
	}
	else if (w_ == true)
	{
		return HWFST::is_empty(openfstDucer_);
	}
	else
	{
		assert((w_ == false) || (w_ != true));
		return false;
	}
}

bool
Xducer::isWeighted() const
{
	return w_;
}

bool 
Xducer::isTrie() const
{
	return isTrie_;
}

// setters
void
Xducer::setWeighted(bool weightedness)
{
	w_ = weightedness;
}

Xducer&
Xducer::safelyDisjunct(const Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::disjunct(sfstDucer_,
									   other.copySfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::disjunct(openfstDucer_,
										   other.copyOpenfstDucer());
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::safelyConjunct(const Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::intersect(sfstDucer_, other.copySfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::intersect(openfstDucer_, other.copyOpenfstDucer());
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::safelyCompose(const Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::compose(sfstDucer_, other.copySfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::compose(openfstDucer_, other.copyOpenfstDucer());
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::safelyConcatenate(const Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::concatenate(sfstDucer_, other.copySfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::concatenate(openfstDucer_, other.copyOpenfstDucer());
	}
	isTrie_ = false;
	return *this;
}

// unsafe versions that do not copy
Xducer&
Xducer::disjunct(Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::disjunct(sfstDucer_, other.getSfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::disjunct(openfstDucer_, other.getOpenfstDucer());
	}
	other.invalidate();
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::disjunctTrie(Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	assert(isTrie() && other.isTrie());
	if (w_ == false)
	{
		sfstDucer_ = HFST::disjunct_transducers_as_tries(sfstDucer_, other.getSfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::disjunct_transducers_as_tries(openfstDucer_, other.getOpenfstDucer());
	}
	other.invalidate();
	return *this;
}

Xducer&
Xducer::addToTrie(const XymbolPairVector& path)
{
	assert(isValid());
	assert(isTrie());
	if (w_ == false)
	{
		HFST::KeyPairVector* hath = xymbolPairVectorToKeyPairVector(path);
		sfstDucer_ = HFST::disjunct_as_trie(sfstDucer_, hath);
		for (HFST::KeyPairVector::const_iterator kp = hath->begin();
			 kp != hath->end(); ++kp)
		{
			delete *kp;
		}
		delete hath;
	}
	else if (w_ == true)
	{
		HWFST::KeyPairVector* hath = xymbolPairVectorToWeyPairVector(path);
		openfstDucer_ = HWFST::disjunct_as_trie(openfstDucer_, hath);
		for (HWFST::KeyPairVector::const_iterator kp = hath->begin();
			 kp != hath->end(); ++kp)
		{
			delete *kp;
		}
		delete hath;
	}
	return *this;
}

Xducer&
Xducer::addToTrie(const XymbolPairVector& path, double weight)
{
	assert(isValid());
	assert(isTrie());
	if (w_ == false)
	{
		// no weights
		addToTrie(path);
	}
	else if (w_ == true)
	{
		HWFST::KeyPairVector* hath = xymbolPairVectorToWeyPairVector(path);
		openfstDucer_ = HWFST::disjunct_as_trie(openfstDucer_, hath, weight);
		for (HWFST::KeyPairVector::const_iterator kp = hath->begin();
			 kp != hath->end(); ++kp)
		{
			delete *kp;
		}
		delete hath;
	}
	return *this;
}

Xducer&
Xducer::conjunct(Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::intersect(sfstDucer_, other.getSfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::intersect(openfstDucer_, other.getOpenfstDucer());
	}
	other.invalidate();
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::compose(Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::compose(sfstDucer_, other.getSfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::compose(openfstDucer_, other.getOpenfstDucer());
	}
	other.invalidate();
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::concatenate(Xducer& other)
{
	assert(isValid() && isOperableWith(other));
	if (w_ == false)
	{
		sfstDucer_ = HFST::concatenate(sfstDucer_, other.getSfstDucer());
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::concatenate(openfstDucer_, other.getOpenfstDucer());
	}
	other.invalidate();
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::repeat(size_t n)
{
	return repeat(n, n);
}

Xducer&
Xducer::repeat(size_t n, size_t k)
{
	assert(isValid());
	if (w_ == false)
	{
		HFST::TransducerHandle sfstCopy = HFST::copy(sfstDucer_);
		HFST::delete_transducer(sfstDucer_);
		sfstDucer_ = HFST::create_epsilon_transducer();
		for (size_t i = 0; i < n; i++)
		{
			sfstDucer_ = HFST::concatenate(sfstDucer_, 
										HFST::copy(sfstCopy));
		}
		sfstCopy = HFST::optionalize(sfstCopy);
		for (size_t i = n; i < k; i++)
		{
			sfstDucer_ = HFST::concatenate(sfstDucer_, 
										HFST::copy(sfstCopy));
		}
		HFST::delete_transducer(sfstCopy);
	
	}
	else if (w_ == true)
	{
		HWFST::TransducerHandle openfstCopy = HWFST::copy(openfstDucer_);
		HWFST::delete_transducer(openfstDucer_);
		openfstDucer_ = HWFST::create_epsilon_transducer();
		for (size_t i = 0; i < n; i++)
		{
			openfstDucer_ = HWFST::concatenate(openfstDucer_,
											HWFST::copy(openfstCopy));
		}
		openfstCopy = HWFST::copy(openfstCopy);
		for (size_t i = n; i < k; i++)
		{
			openfstDucer_ = HWFST::concatenate(openfstDucer_,
											HWFST::copy(openfstCopy));
		}
		HWFST::delete_transducer(openfstCopy);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::repeatStar()
{
	assert(isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::repeat_star(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::repeat_star(openfstDucer_);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::repeatPlus()
{
	assert(isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::repeat_plus(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::repeat_plus(openfstDucer_);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::projectUpper()
{
	assert(isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::extract_input_language(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::extract_output_language(openfstDucer_);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::projectLower()
{
	assert(isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::extract_output_language(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::extract_output_language(openfstDucer_);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::revert()
{
	assert(isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::reverse(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::reverse(openfstDucer_);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::invert()
{
	assert(isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::invert(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::invert(openfstDucer_);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::makeEmpty()
{
	if (w_ == false)
	{
		sfstDucer_ = HFST::create_empty_transducer();
		sfstValid_ = true;
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::create_empty_transducer();
		openfstValid_ = true;
	}
	isTrie_ = true;
	return *this;
}

Xducer&
Xducer::makeEpsilon()
{
	if (w_ == false)
	{
		sfstDucer_ = HFST::create_epsilon_transducer();
		sfstValid_ = true;
	}
	else
	{
		openfstDucer_ = HWFST::create_epsilon_transducer();
		openfstValid_ = true;
	}
	isTrie_ = false;
	return *this;
}


Xducer&
Xducer::substitute(const Xymbol& x1, const Xymbol& x2)
{
	assert (isValid());
	if (w_ == false)
	{
		HFST::Key k1 = x1.getKey();
		HFST::Key k2 = x2.getKey();
		sfstDucer_ = HFST::substitute_key(sfstDucer_, k1, k2);
	}
	else if (w_ == true)
	{
		HWFST::Key k1 = x1.getWey();
		HWFST::Key k2 = x2.getWey();
		openfstDucer_ = HWFST::substitute_key(openfstDucer_, k1, k2);
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::substitute(const XymbolPair& xp1, const XymbolPair& xp2)
{
	assert (isValid());
	if (w_ == false)
	{
		HFST::KeyPair* kp1 = xymbolPairToKeyPair(xp1);
		HFST::KeyPair* kp2 = xymbolPairToKeyPair(xp2);
		sfstDucer_ = HFST::substitute_with_pair(sfstDucer_, kp1, kp2);
		delete kp1;
		delete kp2;
	}
	else if (w_ == true)
	{
		HWFST::KeyPair* kp1 = xymbolPairToWeyPair(xp1);
		HWFST::KeyPair* kp2 = xymbolPairToWeyPair(xp2);
		openfstDucer_ = HWFST::substitute_with_pair(openfstDucer_, kp1, kp2);
		delete kp1;
		delete kp2;
	}
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::substitute(const XymbolSet& xs, const Xymbol& x)
{
	assert (isValid());
	#if 0
	if (w_ == false)
	{
		HFST::KeySet* ks = xymbolSetToKeySet(xs);
		HFST::Key k = x.getKey();
		sfstDucer_ = HFST::substitute_key(sfstDucer_, ks, k);
		delete ks;
	}
	else if (w_ == true)
	{
		HWFST::KeySet* ks = xymbolSetToWeySet(xs);
		HWFST::Key k = x.getWey();
		openfstDucer_ = HWFST::substitute_key(openfstDucer_, ks, k);
		delete ks;
	}
	#else
	for (XymbolSet::const_iterator xi = xs.begin();
		 xi != xs.end(); ++xi)
	{
		substitute(*xi, x);
	}
	#endif
	isTrie_ = false;
	return *this;
}

Xducer&
Xducer::substitute(const XymbolPair& xp, const Xducer& repl)
{
	assert(isValid() && isOperableWith(repl));
	if (w_ == false)
	{
		HFST::KeyPair* kp = xymbolPairToKeyPair(xp);
		HFST::TransducerHandle supl = repl.copySfstDucer();
		sfstDucer_ = HFST::substitute_with_transducer(sfstDucer_, kp, supl);
		HFST::delete_transducer(supl);
	}
	else if (w_ == true)
	{
		HWFST::KeyPair* kp = xymbolPairToWeyPair(xp);
		HWFST::TransducerHandle opl = repl.copyOpenfstDucer();
		openfstDucer_ = 
			HWFST::substitute_with_transducer(openfstDucer_, kp, opl);
		HWFST::delete_transducer(opl);
	}
	return *this;
}

Xducer&
Xducer::read(std::istream& in)
{
	if (w_ == false)
	{
		sfstDucer_ = HFST::read_transducer(in);
		sfstValid_ = true;
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::read_transducer(in);
		openfstValid_ = true;
	}
	isTrie_ = false;
	return *this;
}

void
Xducer::write(std::ostream& out) const
{
	if (w_ == false)
	{
#	if 1
	  HFST::write_transducer(HFST::copy(sfstDucer_), 
							   magicKeyTableWithEverything, out);
#	elif 0
		HFST::write_transducer(sfstDucer_, 
							   magicKeyTableWithEverything, out);
#	endif
	}
	else if (w_ == true)
	{
#	if 1
	  	  
	HWFST::write_transducer(HWFST::copy(openfstDucer_),
								wagicKeyTableWithEverything, out);
#	elif 0
	HWFST::write_transducer(openfstDucer_,
								wagicKeyTableWithEverything, out);
#	endif
	}
}


Xducer&
Xducer::compileFromXre(const string& xre, const XymbolPairSet& xps)
{
	if (w_ == false)
	{
		HFST::KeyPairSet* kps = xymbolPairSetToKeyPairSet(xps);
		sfstDucer_ = HFST::compile_xre(xre.c_str(), kps, kps,
									   magicKeyTableWithEverything);
		sfstValid_ = true;
	}
	else if (w_ == true)
	{
		HWFST::KeyPairSet* kps = xymbolPairSetToWeyPairSet(xps);
		openfstDucer_ = HWFST::compile_xre(xre.c_str(), kps, kps,
										   wagicKeyTableWithEverything);
		openfstValid_ = true;
	}
	isTrie_ = false;
	return *this;
}
Xducer&
Xducer::crossproduct(const XymbolSet& chars)
{
	makeEmpty();
	for (XymbolSet::const_iterator x1 = chars.begin();
		 x1 != chars.end(); ++x1)
	{
		for (XymbolSet::const_iterator x2 = chars.begin();
			 x2 != chars.end(); ++x2)
		{
			Xducer cducer;
			cducer = cducer.make(XymbolPair(*x1, *x2));
			disjunct(cducer);
		}
	}
	isTrie_ = true;
	return *this;
}

Xducer&
Xducer::minimise()
{
	assert (isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::minimize(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::minimize(openfstDucer_);
	}
	return *this;
}

Xducer&
Xducer::determinise()
{
	assert (isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::determinize(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::determinize(openfstDucer_);
	}
	return *this;
}

Xducer&
Xducer::removeEpsilons()
{
	assert (isValid());
	if (w_ == false)
	{
		sfstDucer_ = HFST::remove_epsilons(sfstDucer_);
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::remove_epsilons(openfstDucer_);
	}
	return *this;
}

Xducer&
Xducer::setFinalWeights(double weight)
{
	assert (isValid());
	if (w_ == false)
	{
		// do nothing, sfst does not weigh
		// XXX: maybe: if weight = x set ! final
	}
	else if (w_ == true)
	{
		openfstDucer_ = HWFST::add_weight(openfstDucer_, weight);
	}
	return *this;
}

const Xducer&
Xducer::print(std::ostream& out) const
{
	assert (isValid());
	if (w_ == false)
	{
		HFST::print_transducer(sfstDucer_, magicKeyTableWithEverything, out);
	}
	else if (w_ == true)
	{
		HWFST::print_transducer(openfstDucer_, wagicKeyTableWithEverything, out);
	}
	return *this;
}

size_t
Xducer::computeStateCount() const
{
	assert(false);
	return 0;
}

size_t
Xducer::computeArcCount() const
{
	assert(false);
	return 0;
}

size_t
Xducer::computePathCount() const
{
	assert(false);
	return 0;
}


// HFST stuff (should be private)
HFST::TransducerHandle
Xducer::copySfstDucer() const
{
	assert(isValid());
	if (w_ == false)
	{
		HFST::TransducerHandle copied = HFST::copy(sfstDucer_);
		return copied;
	}
	else
	{
		assert(w_ == false);
		return 0;
	}
}

HFST::TransducerHandle
Xducer::getSfstDucer()
{
	assert(isValid());
	if (w_ == false)
	{
		return sfstDucer_;
	}
	else
	{
		assert(w_ == false);
		return 0;
	}

}

HWFST::TransducerHandle
Xducer::copyOpenfstDucer() const
{
	assert(isValid());
	if (w_ == true)
	{
		HWFST::TransducerHandle copied = HWFST::copy(openfstDucer_);
		return copied;
	}
	else
	{
		assert(w_ == true);
		return 0;
	}
}

HFST::TransducerHandle
Xducer::getOpenfstDucer()
{
	assert(isValid());
	if (w_ == true)
	{
		return openfstDucer_;
	}
	else
	{
		assert(w_ == true);
		return 0;
	}
}

static
bool
hfst_state_comp(HFST::State s1, HFST::State s2)
{
	return s1.node < s2.node;
}

static
bool
hwfst_state_comp(HWFST::State s1, HWFST::State s2)
{
	return s1.state_id < s2.state_id;
}

Xducer&
Xducer::removeLexcJoiners(const Xymbol& initial, const Xymbol& final)
{
	assert(isValid());
	if (!w_)
	{
	  // rebuild from scratch
	  HFST::TransducerHandle nu = HFST::create_epsilon_transducer();
	  HFST::TransducerHandle old = sfstDucer_;
	  HFST::State oldStart = HFST::get_initial_state(old);
	  HFST::State nuStart = HFST::get_initial_state(nu);
	  // save knowledge of lexicon starts and
	  map<HFST::Key,HFST::State> oldStarts;
	  // save all states
	  bool(*hscpt)(HFST::State,HFST::State) = hfst_state_comp;
	  map<HFST::State,HFST::State,bool(*)(HFST::State,HFST::State)> rebuildMap(hscpt);
	  queue<HFST::State> agenda;
	  // we are at initial state, go through the joiners
	  for (HFST::TransitionIterator ti = HFST::begin_ti(old, oldStart);
			  !HFST::is_end_ti(ti); HFST::next_ti(ti))
	  {
		HFST::Transition tr = HFST::get_ti_transition(ti);
		HFST::KeyPair* kp = HFST::get_transition_keypair(tr);
		HFST::Key upper = HFST::get_input_key(kp);
		HFST::Key lower = HFST::get_output_key(kp);
		assert(upper == lower);
		// joiner leads to lexicon start state
		HFST::State oldState = HFST::get_transition_to(tr);
		if (upper == initial.getKey())
		{
			// transition to start state
			HFST::KeyPair* eps = HFST::define_keypair(0, 0);
			HFST::State nustate = HFST::create_state(nu);
			(void)HFST::define_transition(nu, nuStart, eps, nustate);
			oldStart = oldState;
			nuStart = nustate;
		}
		// record start states by joiner key
		oldStarts[upper] = oldState;
	  }
	  // rebuilt from target of Root (BFS style yay!)
	  rebuildMap[oldStart] = nuStart;
	  agenda.push(oldStart);
	  while (!agenda.empty())
	  {
		HFST::State oldState = agenda.front();
		assert(rebuildMap.find(oldState) != rebuildMap.end());
		HFST::State nuState = rebuildMap[oldState];
		for (HFST::TransitionIterator ti = HFST::begin_ti(old, oldState);
				!HFST::is_end_ti(ti); HFST::next_ti(ti))
		{
			HFST::Transition tr = HFST::get_ti_transition(ti);
			HFST::KeyPair* oldKp = HFST::get_transition_keypair(tr);
			HFST::Key upper = HFST::get_input_key(oldKp);
			HFST::Key lower = HFST::get_output_key(oldKp);
			HFST::State oldTarget = HFST::get_transition_to(tr);
			HFST::KeyPair* nuKp = 0;
			HFST::State nuTarget;
			if (upper == final.getKey())
			{
				// end joiner means end state
				nuKp = HFST::define_keypair(0, 0);
				nuTarget = HFST::create_state(nu);
				HFST::set_final_state(nuTarget, nu);
			}
			else if (oldStarts.find(upper) != oldStarts.end())
			{
				// this is a joiner, epsilon to start state
				nuKp = HFST::define_keypair(0, 0);
				if (rebuildMap.find(oldStarts[upper]) != rebuildMap.end())
				{
					nuTarget = rebuildMap[oldStarts[upper]];
				}
				else
				{
					nuTarget = HFST::create_state(nu);
					rebuildMap[oldStarts[upper]] = nuTarget;
					agenda.push(oldStarts[upper]);
				}
			}
			else
			{
				// regular transition, copy and follow
				nuKp = HFST::define_keypair(upper, lower);
				if (rebuildMap.find(oldTarget) != rebuildMap.end())
				{
					// target state has been built
					nuTarget = rebuildMap[oldTarget];
				}
				else
				{
					// new target: create and map
					nuTarget = HFST::create_state(nu);
					rebuildMap[oldTarget] = nuTarget;
					agenda.push(oldTarget);
				}
			}
			// actually build the transition
			HFST::define_transition(nu, nuState, nuKp, nuTarget);
		} // each transition
		agenda.pop();
	  } // while agenda
	  sfstDucer_ = nu;
	  HFST::delete_transducer(old);
	} // if unweighted
	else
	{
		// weighted
	  // rebuild from scratch
	  HWFST::TransducerHandle nu = HWFST::create_epsilon_transducer();
	  HWFST::TransducerHandle old = openfstDucer_;
	  HWFST::State oldStart = HWFST::get_initial_state(old);
	  HWFST::State nuStart = HWFST::get_initial_state(nu);
	  // save knowledge of lexicon starts and
	  map<HWFST::Key,HWFST::State> oldStarts;
	  // save all states
	  bool(*hscpt)(HWFST::State,HWFST::State) = hwfst_state_comp;
	  map<HWFST::State,HWFST::State,bool(*)(HWFST::State,HWFST::State)> rebuildMap(hscpt);
	  queue<HWFST::State> agenda;
	  // we are at initial state, go through the joiners
	  for (HWFST::TransitionIterator ti = HWFST::begin_ti(old, oldStart);
			  !HWFST::is_end_ti(ti); HWFST::next_ti(ti))
	  {
		HWFST::Transition tr = HWFST::get_ti_transition(ti);
		HWFST::KeyPair* kp = HWFST::get_transition_keypair(tr);
		HWFST::Key upper = HWFST::get_input_key(kp);
		HWFST::Key lower = HWFST::get_output_key(kp);
		float weight = HWFST::get_transition_weight(tr);
		assert(upper == lower);
		// joiner leads to lexicon start state
		HWFST::State oldState = HWFST::get_transition_to(tr);
		if (upper == initial.getWey())
		{
			// transition to start state
			HWFST::KeyPair* eps = HWFST::define_keypair(0, 0);
			HWFST::State nustate = HWFST::create_state(nu);
			(void)HWFST::define_transition(nu, nuStart, eps, nustate, weight);
			oldStart = oldState;
			nuStart = nustate;
		}
		// record start states by joiner key
		oldStarts[upper] = oldState;
	  }
	  // rebuilt from target of Root (BFS style yay!)
	  rebuildMap[oldStart] = nuStart;
	  agenda.push(oldStart);
	  while (!agenda.empty())
	  {
		HWFST::State oldState = agenda.front();
		assert(rebuildMap.find(oldState) != rebuildMap.end());
		HWFST::State nuState = rebuildMap[oldState];
		for (HWFST::TransitionIterator ti = HWFST::begin_ti(old, oldState);
				!HWFST::is_end_ti(ti); HWFST::next_ti(ti))
		{
			HWFST::Transition tr = HWFST::get_ti_transition(ti);
			HWFST::KeyPair* oldKp = HWFST::get_transition_keypair(tr);
			HWFST::Key upper = HWFST::get_input_key(oldKp);
			HWFST::Key lower = HWFST::get_output_key(oldKp);
			HWFST::State oldTarget = HWFST::get_transition_to(tr);
			float weight = HWFST::get_transition_weight(tr);
			HWFST::KeyPair* nuKp = 0;
			HWFST::State nuTarget;
			if (upper == final.getWey())
			{
				// end joiner means end state
				nuKp = HWFST::define_keypair(0, 0);
				nuTarget = HWFST::create_state(nu);
				weight += HWFST::get_final_weight(oldTarget, old);
				HWFST::set_final_state(nuTarget, nu, weight);
				weight = 0;
			}
			else if (oldStarts.find(upper) != oldStarts.end())
			{
				nuKp = HWFST::define_keypair(0, 0);
				weight += HWFST::get_final_weight(oldTarget, old);
				if (rebuildMap.find(oldStarts[upper]) != rebuildMap.end())
				{
					nuTarget = rebuildMap[oldStarts[upper]];
				}
				else
				{
					nuTarget = HWFST::create_state(nu);
					rebuildMap[oldStarts[upper]] = nuTarget;
					agenda.push(oldStarts[upper]);
				}
			
			}
			else
			{
				// regular transition, copy and follow
				nuKp = HWFST::define_keypair(upper, lower);
				if (rebuildMap.find(oldTarget) != rebuildMap.end())
				{
					// target state has been built
					nuTarget = rebuildMap[oldTarget];
				}
				else
				{
					// new target: create and map
					nuTarget = HWFST::create_state(nu);
					rebuildMap[oldTarget] = nuTarget;
					agenda.push(oldTarget);
				}
			}
			// actually build the transition
			HWFST::define_transition(nu, nuState, nuKp, nuTarget, weight);
		} // each transition
		agenda.pop();
	  } // while agenda
	  openfstDucer_ = nu;
	  HWFST::delete_transducer(old);
	}
	return *this;
}


