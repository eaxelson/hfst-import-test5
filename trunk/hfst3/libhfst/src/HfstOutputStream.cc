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
#include "HfstOutputStream.h"

namespace hfst
{
  HfstOutputStream::HfstOutputStream(ImplementationType type, bool hfst_format):
    type(type), hfst_format(hfst_format)
  { 
    if (not HfstTransducer::is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch(type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = 
	  new hfst::implementations::SfstOutputStream();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = 
	  new hfst::implementations::TropicalWeightOutputStream();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  new hfst::implementations::LogWeightOutputStream();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma = 
	  new hfst::implementations::FomaOutputStream();
	break;
#endif
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
  // FIX: HfstOutputStream takes a string parameter, HfstInputStream a const char*
  HfstOutputStream::HfstOutputStream(const std::string &filename,ImplementationType type, bool hfst_format):
    type(type), hfst_format(hfst_format)
  { 
    if (not HfstTransducer::is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch(type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = 
	  new hfst::implementations::SfstOutputStream(filename.c_str());
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	if (filename.compare("") == 0) // FIX: this should be done in TropicalWeight layer
	  implementation.tropical_ofst = 
	    new hfst::implementations::TropicalWeightOutputStream();
	else
	  implementation.tropical_ofst = 
	    new hfst::implementations::TropicalWeightOutputStream(filename.c_str());
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  new hfst::implementations::LogWeightOutputStream(filename.c_str());
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma = 
	  new hfst::implementations::FomaOutputStream(filename.c_str());
	break;
#endif
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
#if HAVE_SFST
      case SFST_TYPE:
	delete implementation.sfst;
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	delete implementation.tropical_ofst;
	break;
      case LOG_OFST_TYPE:
	delete implementation.log_ofst;
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	delete implementation.foma;
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	delete implementation.hfst_ol;
	break;
      default:
	assert(false);
      }
  }

  void HfstOutputStream::append(std::vector<char> &str, const std::string &s)
  {
    for (unsigned int i=0; i<s.length(); i++)
      str.push_back(s[i]);
    str.push_back('\0');
  }

  void HfstOutputStream::write(const std::string &s)
  {
    for (unsigned int i=0; i<s.length(); i++)
      write(s[i]);
  }

  void HfstOutputStream::write(const std::vector<char> &s)
  {
    for (unsigned int i=0; i<s.size(); i++)
      write(s[i]);
  }

  void HfstOutputStream::write(const char &c)
  {
    switch(type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst->write(c);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->write(c);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst->write(c);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma->write(c);
	break;
#endif
#if HAVE_HFST_OL
	implementation.hfst_ol->write(c);
	break;
#endif
      default:
	assert(false);
      }
  }

  void HfstOutputStream::append_hfst_header_data(std::vector<char> &header)
  {
    append(header, "version");
    append(header, "3.0");
    append(header, "type");

    std::string type_value;

    switch(type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	type_value=std::string("SFST");
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	type_value=std::string("TROPICAL_OPENFST");
	break;
      case LOG_OFST_TYPE:
	type_value=std::string("LOG_OPENFST");
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	type_value=std::string("FOMA");
	break;
#endif
      case HFST_OL_TYPE:
	type_value=std::string("HFST_OL");
	break;
      case HFST_OLW_TYPE:
	type_value=std::string("HFST_OLW");
	break;
      default:
	assert(false);
      }

    append(header, type_value);
  }

  void HfstOutputStream::append_implementation_specific_header_data(std::vector<char> &header, HfstTransducer &transducer)
  {
    switch(type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst->append_implementation_specific_header_data(header, transducer.implementation.sfst);
	break;
#endif
      default:
	break;
      }
  }

  HfstOutputStream &HfstOutputStream::operator<< (HfstTransducer &transducer)
  {
    if (type != transducer.type)
      { throw hfst::exceptions::TransducerHasWrongTypeException(); }

    if (hfst_format) {
      const int MAX_HEADER_LENGTH=65535;
      std::vector<char> header;
      append_hfst_header_data(header);
      append(header, "name");
      append(header, transducer.name);
      append_implementation_specific_header_data(header, transducer);

      write("HFST");
      write('\0');

      int header_length = (int)header.size();
      if (header_length > MAX_HEADER_LENGTH) {
	fprintf(stderr, "ERROR: transducer header is too long\n");
	exit(1);
      }	
      // write header length using two bytes
      write((char)header_length/256);
      write((char)header_length%256);

      write('\0');
      write(header);
    }

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst->write_transducer
	  (transducer.implementation.sfst);
	return *this;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->write_transducer
	  (transducer.implementation.tropical_ofst);
	return *this;    
      case LOG_OFST_TYPE:
	implementation.log_ofst->write_transducer
	  (transducer.implementation.log_ofst);
	return *this;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma->write_transducer
	  (transducer.implementation.foma);
	return *this;
#endif
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

  void HfstOutputStream::close(void) {
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst->close();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->close();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst->close();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma->close();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol->close();
	break;
      default:
	assert(false);
      }
  }

}
