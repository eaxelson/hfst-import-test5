//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** \file HfstApply.cc 
    \brief HFST transducer functions that take several parameters are handled here. 
 */

#include "HfstTransducer.h"
namespace hfst
{
  // Whether weights or other information are lost in the conversion.
  bool HfstTransducer::is_safe_conversion(ImplementationType original, ImplementationType converted) {
    if (original == converted)
      return true;
    if (original == TROPICAL_OFST_TYPE && converted == LOG_OFST_TYPE) {
      return false;
    }
    if (original == LOG_OFST_TYPE && converted == TROPICAL_OFST_TYPE) {
      return false;
    }
    if (original == TROPICAL_OFST_TYPE || original == LOG_OFST_TYPE) {
      if (converted == SFST_TYPE) {
	return false;
      }
      if (converted == FOMA_TYPE) {
	return false;
      }
    }
#ifdef HAVE_FOO
    if (original == FOO_TYPE) {
      // From foo to weighted
      if ( converted == TROPICAL_OFST_TYPE || converted == LOG_OFST_TYPE) {
	return true;  // if foo supports weights
	return false; // if foo does not support weights
      }
      // From foo to unweighted
      else {
	return true;  // if foo does not support weights
	return false; // if foo supports weights
      }
    }
#endif
    return true;
  }

  HfstTransducer &HfstTransducer::apply(
#if HAVE_SFST
 SFST::Transducer * (*sfst_funct)(SFST::Transducer *), 
#endif
#if HAVE_OPENFST
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *),
#endif
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *),
#endif
#if HAVE_FOO
 FooTransducer * (*foo_funct)(FooTransducer *),
#endif
  bool foo )
    {
      (void)foo;
    switch(this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * tropical_ofst_temp =
	    tropical_ofst_funct(implementation.tropical_ofst);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * log_ofst_temp =
	    log_ofst_funct(implementation.log_ofst);
	  delete implementation.log_ofst;
	  implementation.log_ofst = log_ofst_temp;
	  break;
	}
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	{
      fsm * foma_temp =
	    foma_funct(implementation.foma);
	  this->foma_interface.delete_foma(implementation.foma);
	  implementation.foma = foma_temp;
	  break;
	}
#endif
#if HAVE_FOO
      case FOO_TYPE:
	{
	  FooTransducer * foo_temp =
	    foo_funct(implementation.foo);
	  delete implementation.foo;
	  implementation.foo = foo_temp;
	  break;
	}
#endif
	//case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (
#if HAVE_SFST
SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int n),
#endif
#if HAVE_OPENFST
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *, int n),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *, int n),
#endif
#if HAVE_FOMA
   fsm * (*foma_funct)(fsm *,int n),
#endif
#if HAVE_FOO
   FooTransducer * (*foo_funct)(FooTransducer *,int n),
#endif
   int n )
  {
    switch(this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,n);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * tropical_ofst_temp =
	    tropical_ofst_funct(implementation.tropical_ofst,n);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      case LOG_OFST_TYPE:
        {
	  hfst::implementations::LogFst * log_ofst_temp =
            log_ofst_funct(implementation.log_ofst,n);
          delete implementation.log_ofst;
          implementation.log_ofst = log_ofst_temp;
          break;
        }
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	{
      fsm * foma_temp = 
	    foma_funct(implementation.foma,n);
	  this->foma_interface.delete_foma(implementation.foma);
	  implementation.foma = foma_temp;
	  break;
    }
#endif
#if HAVE_FOO
      case FOO_TYPE:
	{
      FooTransducer * foo_temp = 
	    foo_funct(implementation.foo,n);
	  delete (implementation.foo);
	  implementation.foo = foo_temp;
	  break;
    }
#endif
	//case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (
#if HAVE_SFST
   SFST::Transducer * (*sfst_funct)(SFST::Transducer *, String, String),
#endif
#if HAVE_OPENFST
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,String, 
					      String),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,String, String),
#endif
#if HAVE_FOMA
   fsm * (*foma_funct)(fsm *, String, String),
#endif
#if HAVE_FOO
   FooTransducer * (*foo_funct)(FooTransducer *, String, String),
#endif
   String s1, String s2)
  {
    switch(this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,s1,s2);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * tropical_ofst_temp =
	    tropical_ofst_funct(implementation.tropical_ofst,s1,s2);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      case LOG_OFST_TYPE:
        {
	  hfst::implementations::LogFst * log_ofst_temp =
            log_ofst_funct(implementation.log_ofst,s1,s2);
          delete implementation.log_ofst;
          implementation.log_ofst = log_ofst_temp;
          break;
        }
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	{
      fsm * foma_temp = 
	    foma_funct(implementation.foma,s1,s2);
	  this->foma_interface.delete_foma(implementation.foma);
	  implementation.foma = foma_temp;
      break;
	}
#endif
#if HAVE_FOO
      case FOO_TYPE:
	{
      FooTransducer * foo_temp = 
	    foo_funct(implementation.foo,s1,s2);
	  delete(implementation.foo);
	  implementation.foo = foo_temp;
      break;
	}
#endif
	//case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return *this;
  }


  HfstTransducer &HfstTransducer::apply(
#if HAVE_SFST
   SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				    SFST::Transducer *),
#endif
#if HAVE_OPENFST
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					      fst::StdVectorFst *),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,hfst::implementations::LogFst *),
#endif
#if HAVE_FOMA
   fsm * (*foma_funct)(fsm *,
				    fsm *),
#endif
#if HAVE_FOO
   FooTransducer * (*foo_funct)(FooTransducer *,
				    FooTransducer *),
#endif
   HfstTransducer &another)
  {
    if (this->type != another.type)
      throw hfst::exceptions::TransducerTypeMismatchException();

    this->harmonize(another);

    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,another.implementation.sfst);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * tropical_ofst_temp =
	    tropical_ofst_funct(this->implementation.tropical_ofst,
				another.implementation.tropical_ofst);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      case LOG_OFST_TYPE:
        {
	  hfst::implementations::LogFst * log_ofst_temp =
            log_ofst_funct(implementation.log_ofst,
			   another.implementation.log_ofst);
          delete implementation.log_ofst;
          implementation.log_ofst = log_ofst_temp;
          break;
        }
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	{
	  fsm * foma_temp = 
	    foma_funct(implementation.foma,another.implementation.foma);
	  delete implementation.foma;
	  implementation.foma = foma_temp;
	  break;
	}
#endif
#if HAVE_FOO
      case FOO_TYPE:
	{
	  FooTransducer * foo_temp = 
	    foo_funct(implementation.foo,another.implementation.foo);
	  delete implementation.foo;
	  implementation.foo = foo_temp;
	  break;
	}
#endif
	//case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }

    return *this;
  }
}
