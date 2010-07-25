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
#include "HfstTransducer.h"
namespace hfst
{
  bool HfstTransducer::is_safe_conversion(ImplementationType original, ImplementationType converted) {
    if (original == TROPICAL_OFST_TYPE && converted == LOG_OFST_TYPE) {
      printf("TROPICAL -> LOG\n");
      return false;
    }
    if (original == LOG_OFST_TYPE && converted == TROPICAL_OFST_TYPE) {
      printf("LOG -> TROPICAL\n");
      return false;
    }
    if (original == TROPICAL_OFST_TYPE || original == LOG_OFST_TYPE) {
      if (converted == SFST_TYPE) {
	printf("WEIGHTED -> SFST\n");
	return false;
      }
      if (converted == FOMA_TYPE) {
	printf("WEIGHTED -> FOMA\n");
	return false;
      }
    }
    return true;
  }

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *),
   fsm * (*foma_funct)(fsm *) )
  {
    switch(this->type)
      {
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
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
      case FOMA_TYPE:
	{
	  fsm * foma_temp =
	    foma_funct(implementation.foma);
	  this->foma_interface.delete_foma(implementation.foma);
	  implementation.foma = foma_temp;
	  break;
	}
      case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int n),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *, int n),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *, int n),
   fsm * (*foma_funct)(fsm *,int n),
   int n )
  {
    switch(this->type)
      {
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,n);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
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
      case FOMA_TYPE:
	{
	  fsm * foma_temp = 
	    foma_funct(implementation.foma,n);
	  this->foma_interface.delete_foma(implementation.foma);
	  implementation.foma = foma_temp;
	  break;
	}
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *, String, String),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,String, 
					      String),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,String, String),
   fsm * (*foma_funct)(fsm *, String, String),
   String s1, String s2)
  {
    switch(this->type)
      {
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,s1,s2);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
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
      case FOMA_TYPE:
	{
	  fsm * foma_temp = 
	    foma_funct(implementation.foma,s1,s2);
	  this->foma_interface.delete_foma(implementation.foma);
	  implementation.foma = foma_temp;
	  break;
	}
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return *this;
  }

  /*
  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *, KeyPair, 
				    KeyPair),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,KeyPair, 
					      KeyPair),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,KeyPair, KeyPair),
   fsm * (*foma_funct)(fsm *, KeyPair, 
				    KeyPair),
   KeyPair kp1, KeyPair kp2,ImplementationType type)
  {
    if (type != UNSPECIFIED_TYPE)
      { convert(type); }
    switch(this->type)
      {
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,kp1,kp2);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * tropical_ofst_temp =
	    tropical_ofst_funct(implementation.tropical_ofst,kp1,kp2);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      case LOG_OFST_TYPE:
        {
	  hfst::implementations::LogFst * log_ofst_temp =
            log_ofst_funct(implementation.log_ofst,kp1,kp2);
          delete implementation.log_ofst;
          implementation.log_ofst = log_ofst_temp;
          break;
        }
      case FOMA_TYPE:
	{
	  fsm * foma_temp = 
	    foma_funct(implementation.foma,kp1,kp2);
	  this->foma_interface.delete_foma(implementation.foma);
	  implementation.foma = foma_temp;
	  break;
	}
 	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
     }
    return *this;
    }*/

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				    SFST::Transducer *),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					      fst::StdVectorFst *),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,hfst::implementations::LogFst *),
   fsm * (*foma_funct)(fsm *,
				    fsm *),
   HfstTransducer &another)
  {
    if (this->type != another.type)
      throw hfst::exceptions::TransducerTypeMismatchException();

    // harmonize this according to symbol coding of another
    // and expand unknowns and identities of both transducers
    this->harmonize(another);

    switch (this->type)
      {
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,another.implementation.sfst);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
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
      case FOMA_TYPE:
	{
	  fsm * foma_temp = 
	    foma_funct(implementation.foma,another.implementation.foma);
	  delete implementation.foma;
	  implementation.foma = foma_temp;
	  break;
	}
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }

    return *this;
  }
}
