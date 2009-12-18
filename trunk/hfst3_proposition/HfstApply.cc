#include "HfstTransducer.h"
namespace HFST
{
  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
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
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int n),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *, int n),
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
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *, Key, Key),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,Key, 
					      Key),
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
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *, KeyPair, 
				    KeyPair),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,KeyPair, 
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
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::apply
  (SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				    SFST::Transducer *),
   fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					      fst::StdVectorFst *),
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
	    tropical_ofst_funct(implementation.tropical_ofst,
				another.implementation.tropical_ofst);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = tropical_ofst_temp;
	  break;
	}
      }
    return *this;
  }
}
