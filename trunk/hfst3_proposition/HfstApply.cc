#include "HfstTransducer.h"
namespace hfst
{
  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *),
   fsm * (*foma_funct)(fsm *),
   ImplementationType type)
  {
    if (type != UNSPECIFIED_TYPE)
      { convert(type); }
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

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int n),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *, int n),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *, int n),
   fsm * (*foma_funct)(fsm *,int n),
   int n, ImplementationType type)
  {
    if (type != UNSPECIFIED_TYPE)
      { convert(type); }
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

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *, Key, Key),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,Key, 
					      Key),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,Key, Key),
   fsm * (*foma_funct)(fsm *, Key, Key),
   Key k1, Key k2,ImplementationType type)
  {
    if (type != UNSPECIFIED_TYPE)
      { convert(type); }
    switch(this->type)
      {
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,k1,k2);
	  delete implementation.sfst;
	  implementation.sfst = sfst_temp;
	  break;
	}
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * tropical_ofst_temp =
	    tropical_ofst_funct(implementation.tropical_ofst,k1,k2);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      case LOG_OFST_TYPE:
        {
	  hfst::implementations::LogFst * log_ofst_temp =
            log_ofst_funct(implementation.log_ofst,k1,k2);
          delete implementation.log_ofst;
          implementation.log_ofst = log_ofst_temp;
          break;
        }
      case FOMA_TYPE:
	{
	  fsm * foma_temp = 
	    foma_funct(implementation.foma,k1,k2);
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

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				    SFST::Transducer *),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					      fst::StdVectorFst *),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,hfst::implementations::LogFst *),
   fsm * (*foma_funct)(fsm *,
				    fsm *),
   HfstTransducer &another,
   ImplementationType type)
  {
    if (type != UNSPECIFIED_TYPE)
      {
	convert(type);
	if (type != another.type)
	  { another = HfstTransducer(another).convert(type); }
      }
    else if (this->type != another.type)
      { convert(another.type); }

    // added
    this->harmonize(another);

    fprintf(stderr, "harmonized\n");

    this->print();
    another.print();

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

    this->print();

    return *this;
  }
}
