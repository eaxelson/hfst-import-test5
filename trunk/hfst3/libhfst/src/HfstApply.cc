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
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *, StringSymbol, StringSymbol),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,StringSymbol, 
					      StringSymbol),
   hfst::implementations::LogFst * (*log_ofst_funct)
   (hfst::implementations::LogFst *,StringSymbol, StringSymbol),
   fsm * (*foma_funct)(fsm *, StringSymbol, StringSymbol),
   StringSymbol s1, StringSymbol s2,ImplementationType type)
  {
    if (type != UNSPECIFIED_TYPE)
      { convert(type); }
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
    //fprintf(stderr, "apply(1): this type is %i\n", this->type);
    //fprintf(stderr, "apply(1): another type is %i\n", another.type);

    if (type != UNSPECIFIED_TYPE)
      {
	convert(type);
	if (type != another.type)
	  { another.convert(type); }
      }
    else if (this->type != another.type)
      { convert(another.type); }

    //fprintf(stderr, "apply(2): this type is %i\n", this->type);
    //fprintf(stderr, "apply(2): another type is %i\n", another.type);

    // added
    this->harmonize(another);

    //fprintf(stderr, "apply(3): this type is %i\n", this->type);
    //fprintf(stderr, "apply(3): another type is %i\n", another.type);

    HfstTransducer *result = new HfstTransducer(this->type);

    switch (this->type)
      {
      case SFST_TYPE:
	{
	  SFST::Transducer * sfst_temp = 
	    sfst_funct(implementation.sfst,another.implementation.sfst);
	  //delete implementation.sfst;
	  //implementation.sfst = sfst_temp;
	  delete result->implementation.sfst;
	  result->implementation.sfst = sfst_temp;
	  break;
	}
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * tropical_ofst_temp =
	    tropical_ofst_funct(this->implementation.tropical_ofst,
				another.implementation.tropical_ofst);
	  //delete implementation.tropical_ofst;
	  //implementation.tropical_ofst = tropical_ofst_temp;
	  delete result->implementation.tropical_ofst;
	  result->implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      case LOG_OFST_TYPE:
        {
	  hfst::implementations::LogFst * log_ofst_temp =
            log_ofst_funct(implementation.log_ofst,
			   another.implementation.log_ofst);
          //delete implementation.log_ofst;
          //implementation.log_ofst = log_ofst_temp;
	  delete result->implementation.log_ofst;
	  result->implementation.log_ofst = log_ofst_temp;
          break;
        }
      case FOMA_TYPE:
	{
	  fsm * foma_temp = 
	    foma_funct(implementation.foma,another.implementation.foma);
	  //delete implementation.foma;
	  //implementation.foma = foma_temp;
	  this->foma_interface.delete_foma(result->implementation.foma);
	  result->implementation.foma = foma_temp;
	  break;
	}
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  fprintf(stderr, "BABAAR\n");
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }

    //return *this;
    return *result;
  }
}
