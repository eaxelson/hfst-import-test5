#include "HfstTransducer.h"

namespace hfst
{
  HfstOutputStream::HfstOutputStream(ImplementationType type):
  type(type)
  { 
    switch(type)
      {
      case SFST_TYPE:
	implementation.sfst = 
	  new hfst::implementations::SfstOutputStream();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = 
	  new hfst::implementations::TropicalWeightOutputStream();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  new hfst::implementations::LogWeightOutputStream();
	break;
      case FOMA_TYPE:
	implementation.foma = 
	  new hfst::implementations::FomaOutputStream();
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
	implementation.sfst = 
	  new hfst::implementations::SfstOutputStream(filename.c_str());
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = 
	  new hfst::implementations::TropicalWeightOutputStream(filename.c_str());
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  new hfst::implementations::LogWeightOutputStream(filename.c_str());
	break;
      case FOMA_TYPE:
	implementation.foma = 
	  new hfst::implementations::FomaOutputStream(filename.c_str());
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
	delete implementation.sfst;
	break;
      case TROPICAL_OFST_TYPE:
	delete implementation.tropical_ofst;
	break;
      case LOG_OFST_TYPE:
	delete implementation.log_ofst;
	break;
      case FOMA_TYPE:
	delete implementation.foma;
	break;
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
	implementation.sfst->write_transducer
	  (transducer.implementation.sfst);
	return *this;
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->write_transducer
	  (transducer.implementation.tropical_ofst);
	return *this;    
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst->write_transducer
	  (transducer.implementation.log_ofst);
	return *this;
	break;
      case FOMA_TYPE:
	implementation.foma->write_transducer
	  (transducer.implementation.foma);
	return *this;
	break;
      default:
	assert(false);
	return *this;
      }
  }

  void HfstOutputStream::open(void) {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst->open();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->open();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst->open();
	break;
      case FOMA_TYPE:
	implementation.foma->open();
	break;
      default:
	assert(false);
      }
  }

  void HfstOutputStream::close(void) {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst->close();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->close();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst->close();
	break;
      case FOMA_TYPE:
	implementation.foma->close();
	break;
      default:
	assert(false);
      }
  }

}
