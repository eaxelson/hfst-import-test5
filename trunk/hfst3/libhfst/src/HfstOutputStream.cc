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
      case HFST_OL_TYPE:
	implementation.hfst_ol =
	  new hfst::implementations::HfstOlOutputStream(false);
	break;
      case HFST_OLW_TYPE:
	implementation.hfst_ol =
	  new hfst::implementations::HfstOlOutputStream(true);
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
      case HFST_OL_TYPE:
	implementation.hfst_ol =
	  new hfst::implementations::HfstOlOutputStream(filename.c_str(), false);
	break;
      case HFST_OLW_TYPE:
	implementation.hfst_ol =
	  new hfst::implementations::HfstOlOutputStream(filename.c_str(), true);
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
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	delete implementation.hfst_ol;
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
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->write_transducer
	  (transducer.implementation.tropical_ofst);
	return *this;    
      case LOG_OFST_TYPE:
	implementation.log_ofst->write_transducer
	  (transducer.implementation.log_ofst);
	return *this;
      case FOMA_TYPE:
	implementation.foma->write_transducer
	  (transducer.implementation.foma);
	return *this;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol->write_transducer
	  (transducer.implementation.hfst_ol);
	return *this;
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
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol->open();
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
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol->close();
	break;
      default:
	assert(false);
      }
  }

}
