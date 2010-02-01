#include "HfstTransducer.h"

namespace hfst
{
  HfstOutputStream::HfstOutputStream(ImplementationType type):
  type(type)
  { 
    switch(type)
      {
      case SFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case TROPICAL_OFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  new hfst::implementations::LogWeightOutputStream();
	break;
      default:
	throw hfst::exceptions::SpecifiedTypeRequiredException();
	break;
      }
  }
  HfstOutputStream::HfstOutputStream(const std::string &filename,ImplementationType type):
  type(type)
  { 
    switch(type)
      {
      case SFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case TROPICAL_OFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  new hfst::implementations::LogWeightOutputStream(filename.c_str());
	break;
      default:
	throw hfst::exceptions::SpecifiedTypeRequiredException();
	break;
      }
  }
  
  HfstOutputStream::~HfstOutputStream(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	break;
      case TROPICAL_OFST_TYPE:
	break;
      case LOG_OFST_TYPE:
	delete implementation.log_ofst;
      default:
	assert(false);
      }
  }

  HfstOutputStream &HfstOutputStream::operator<< (HfstTransducer &transducer)
  {
    if (type != transducer.type)
      { throw hfst::exceptions::TransducerHasWrongTypeException(); }

    switch (type)
      {
      case SFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case TROPICAL_OFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case LOG_OFST_TYPE:
	if (transducer.anonymous)
	  {
	    implementation.log_ofst->write_transducer
	      (transducer.implementation.log_ofst);
	  }
	else
	  {
	    implementation.log_ofst->write_transducer
	      (transducer.implementation.log_ofst,transducer.key_table);
	  }
	return *this;
	break;
      default:
	assert(false);
	return *this;
      }    
  }
}
