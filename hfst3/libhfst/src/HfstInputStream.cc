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
  void HfstInputStream::read_transducer(HfstTransducer &t)
  {
    bool has_header = stream_has_headers && (!header_eaten);
    switch (type)
      {
      case SFST_TYPE:
	t.implementation.sfst =
	  this->implementation.sfst->read_transducer(has_header);
	break;
      case TROPICAL_OFST_TYPE:
	t.implementation.tropical_ofst =
	  this->implementation.tropical_ofst->read_transducer(has_header);
	break;
      case LOG_OFST_TYPE:
	t.implementation.log_ofst =
	  this->implementation.log_ofst->read_transducer(has_header);
	break;
      case FOMA_TYPE:
#if HAVE_FOMA
    t.implementation.foma =
	  this->implementation.foma->read_transducer(has_header);
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
    break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	t.implementation.hfst_ol =
	  this->implementation.hfst_ol->read_transducer(has_header);
	if(t.get_type() != type) // weights need to be added or removed
	  { t.convert(type); }
    break;
      case HFST2_TYPE:
#ifndef NDEBUG
        fprintf(stderr, "HFST 2 transducers must be converted before use "
                "(transducer conversion not implemented)\n");
#endif
        throw hfst::exceptions::TransducerHasWrongTypeException();
        break;
	break;
	  case ERROR_TYPE:
	  case UNSPECIFIED_TYPE:
	  default:
		throw hfst::exceptions::NotTransducerStreamException();
		break;
      }
    header_eaten = false;
  }

  ImplementationType HfstInputStream::guess_fst_type(std::istream &in)
  {
    if (hfst::implementations::SfstInputStream::is_fst(in))
      { return SFST_TYPE; }
#if HAVE_FOMA
    if (hfst::implementations::FomaInputStream::is_fst(in))
      { return FOMA_TYPE; }
#endif
    if (hfst::implementations::TropicalWeightInputStream::is_fst(in))
      { return TROPICAL_OFST_TYPE; }
    if (hfst::implementations::LogWeightInputStream::is_fst(in))
      { return LOG_OFST_TYPE; }
    int ol_type = hfst::implementations::HfstOlInputStream::is_fst(in);
    if(ol_type == 1)
      { return HFST_OL_TYPE; }
    if(ol_type == 2)
      { return HFST_OLW_TYPE; }
    // also. guess HFST2 type?
    char c = 0;
    in.get(c);
    switch (c)
      {
      case 'A':
        in.unget();
        return HFST2_TYPE;
        break;
      case 'P':
        in.get(c);
        if (c == '2')
          {
            return HFST2_TYPE;
          }
        else
          {
            return ERROR_TYPE;
          }
        break;
      default:
        // still failed
        in.unget();
        return ERROR_TYPE;
      }
    in.unget();
    return ERROR_TYPE;
  }



  
  ImplementationType HfstInputStream::read_version_3_0_fst_type
  (std::istream &in) 
  {
    char fst_type[100];
    in.getline(fst_type,100,0);
    if (in.eof())
      { return ERROR_TYPE; }
    if (0 == strcmp(fst_type,"SFST_TYPE"))
      { return SFST_TYPE; }
    if (0 == strcmp(fst_type,"FOMA_TYPE"))
      { return FOMA_TYPE; }
    if (0 == strcmp(fst_type,"TROPICAL_OFST_TYPE"))
      { return TROPICAL_OFST_TYPE; }
    if (0 == strcmp(fst_type,"LOG_OFST_TYPE"))
      { return LOG_OFST_TYPE; }
    if (0 == strcmp(fst_type,"HFST_OL_TYPE"))
      { return HFST_OL_TYPE; }
    if (0 == strcmp(fst_type,"HFST_OLW_TYPE"))
      { return HFST_OLW_TYPE; }
    return ERROR_TYPE;
  }

  /* Returns 0 on HFST3 type. */
  int HfstInputStream::read_library_header(std::istream &in) 
  {
    const char *id = "HFST3";
    
    for (int i=0; i<6; i++) {
      int c = in.get();
      if(c != id[i]) { /* No match */
        in.putback(c);
        if(i > 0) {
          for(int j=i-1; j>=0; j--) {
            in.putback(id[j]);
          }
        }
        return -1;
      }
    }
    return 0;
  }

  /* The implementation type of the first transducer in the stream. */
  ImplementationType HfstInputStream::stream_fst_type(const char *filename)
  { 
    std::istream *in;
    std::ifstream ifs(filename);
    if (strcmp(filename,"") != 0) {
      in = &ifs;
    }
    else
      in = &cin;
    
    if (not in->good())
      { throw hfst::implementations::FileNotReadableException(); }
    int library_version;
    if (0 == (library_version = read_library_header(*in))) {
      stream_has_headers = true;
      switch (library_version)
      {
      case 0:
        return read_version_3_0_fst_type(*in);
      default:
        fprintf(stderr, "stream_fst_type: returning ERROR_TYPE (1)\n");
        return ERROR_TYPE;
      }
    }
    else { /* No HFST3 header on the file */
      stream_has_headers = false;
      ImplementationType type = guess_fst_type(*in);
      if (type == ERROR_TYPE) {
        fprintf(stderr, "stream_fst_type: returning ERROR_TYPE (2)\n");
      }
      return type;
    }
  }

  /* Open a transducer stream to stdout.
     The implementation type of the stream is defined by 
     the type of the first transducer in the stream. */
  HfstInputStream::HfstInputStream(void):
    header_eaten(true)
  {
    try { type = stream_fst_type(""); }
    catch (hfst::implementations::FileNotReadableException e)
      { throw e; }

    switch (type)
    {
    case SFST_TYPE:
      implementation.sfst = new hfst::implementations::SfstInputStream;
      break;
    case TROPICAL_OFST_TYPE:
      implementation.tropical_ofst = 
	new hfst::implementations::TropicalWeightInputStream;
      break;
    case LOG_OFST_TYPE:
      implementation.log_ofst = 
	new hfst::implementations::LogWeightInputStream;
      break;
    case FOMA_TYPE:
#if HAVE_FOMA
      implementation.foma = new hfst::implementations::FomaInputStream;
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
      break;
    case HFST_OL_TYPE:
      implementation.hfst_ol =
        new hfst::implementations::HfstOlInputStream(false);
      break;
    case HFST_OLW_TYPE:
      implementation.hfst_ol = 
        new hfst::implementations::HfstOlInputStream(true);
      break;
      case HFST2_TYPE:
#ifndef NDEBUG
        fprintf(stderr, "HFST 2 transducers must be converted before use "
                "(transducer conversion not implemented)\n");
#endif
        throw hfst::exceptions::TransducerHasWrongTypeException();
        break;
    default:
      throw hfst::exceptions::NotTransducerStreamException();
    }
  }

  HfstInputStream::HfstInputStream(const char* filename):
    header_eaten(false)
  {
    try { type = stream_fst_type(filename); }
    catch (hfst::implementations::FileNotReadableException e)
      { throw e; }

    switch (type)
    {
    case SFST_TYPE:
      implementation.sfst = new hfst::implementations::SfstInputStream(filename);
      break;
    case TROPICAL_OFST_TYPE:
      implementation.tropical_ofst = 
	new hfst::implementations::TropicalWeightInputStream(filename);
      break;
    case LOG_OFST_TYPE:
      implementation.log_ofst = 
	new hfst::implementations::LogWeightInputStream(filename);
      break;
    case FOMA_TYPE:
#if HAVE_FOMA
      implementation.foma = new hfst::implementations::FomaInputStream(filename);
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
      break;
    case HFST_OL_TYPE:
      implementation.hfst_ol = new hfst::implementations::HfstOlInputStream(filename, false);
      break;
    case HFST_OLW_TYPE:
      implementation.hfst_ol = new hfst::implementations::HfstOlInputStream(filename, true);
      break;
      case HFST2_TYPE:
#ifndef NDEBUG
        fprintf(stderr, "HFST 2 transducers must be converted before use "
                "(transducer conversion not implemented)\n");
#endif
        throw hfst::exceptions::TransducerHasWrongTypeException();
        break;
    default:
      throw hfst::implementations::NotTransducerStreamException();
    }
  }

  HfstInputStream::~HfstInputStream(void)
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
#if HAVE_FOMA
	delete implementation.foma;
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	delete implementation.hfst_ol;
	break;
      case HFST2_TYPE:
#ifndef NDEBUG
        fprintf(stderr, "HFST 2 transducers must be converted before use "
                "(transducer conversion not implemented)\n");
#endif
        throw hfst::exceptions::TransducerHasWrongTypeException();
        break;
	  case ERROR_TYPE:
	  case UNSPECIFIED_TYPE:
	  default:
		throw hfst::exceptions::NotTransducerStreamException();
      }
  }

  void HfstInputStream::open(void)
  {
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
#if HAVE_FOMA
	implementation.foma->open();
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol->open();
	break;
      default:
	assert(false);
      }
  }

  void HfstInputStream::close(void)
  {
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
#if HAVE_FOMA
	implementation.foma->close();
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol->close();
	break;
      default:
	assert(false);
      }
  }

  bool HfstInputStream::is_open(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	return implementation.sfst->is_open();
	break;
      case TROPICAL_OFST_TYPE:
	return implementation.tropical_ofst->is_open();
	break;
      case LOG_OFST_TYPE:
	return implementation.log_ofst->is_open();
	break;
      case FOMA_TYPE:
#if HAVE_FOMA
	return implementation.foma->is_open();
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	return implementation.hfst_ol->is_open();
	break;
      default:
	assert(false);
	return false;
      }
  }

  bool HfstInputStream::is_eof(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	return implementation.sfst->is_eof();
	break;
      case TROPICAL_OFST_TYPE:
	return implementation.tropical_ofst->is_eof();
	break;
      case LOG_OFST_TYPE:
	return implementation.log_ofst->is_eof();
	break;
      case FOMA_TYPE:
#if HAVE_FOMA
	return implementation.foma->is_eof();
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	return implementation.hfst_ol->is_eof();
      default:
	assert(false);
	return false;
      }
  }

  bool HfstInputStream::is_bad(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	return implementation.sfst->is_bad();
	break;
      case TROPICAL_OFST_TYPE:
	return implementation.tropical_ofst->is_bad();
	break;
      case LOG_OFST_TYPE:
	return implementation.log_ofst->is_bad();
	break;
      case FOMA_TYPE:
#if HAVE_FOMA
	return implementation.foma->is_bad();
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	return implementation.hfst_ol->is_bad();
	break;
      default:
	assert(false);
	return false;
      }
  }

  bool HfstInputStream::is_good(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	return implementation.sfst->is_good();
	break;
      case TROPICAL_OFST_TYPE:
	return implementation.tropical_ofst->is_good();
	break;
      case LOG_OFST_TYPE:
	return implementation.log_ofst->is_good();
	break;
      case FOMA_TYPE:
#if HAVE_FOMA
	return implementation.foma->is_good();
#else
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
#endif
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	return implementation.hfst_ol->is_good();
	break;
      default:
	assert(false);
	return false;
      }
  }
  
  ImplementationType HfstInputStream::get_type(void) const
  {
    return type;
  }
}

#ifdef DEBUG_MAIN_STREAM
int main(void)
{
  hfst::HfstInputStream sfst_stdin_input(hfst::SFST_TYPE);
  sfst_stdin_input.open();
  sfst_stdin_input.close();
  hfst::HfstInputStream ofst_stdin_input(hfst::TROPICAL_OFST_TYPE);
  ofst_stdin_input.open();
  ofst_stdin_input.close();
  // This succeeds only if the files stream_test and
  // stream_test_ne exist
  hfst::HfstInputStream sfst_file_input("stream_test",hfst::SFST_TYPE);
  sfst_file_input.open();
  sfst_file_input.close();
  assert(sfst_file_input.is_eof());
  hfst::HfstInputStream sfst_ne_file_input("stream_test_ne",hfst::SFST_TYPE);
  sfst_ne_file_input.open();
  assert(not sfst_ne_file_input.is_eof());
  sfst_ne_file_input.close();
  hfst::HfstInputStream ofst_file_input("stream_test",hfst::TROPICAL_OFST_TYPE);
  ofst_file_input.open();
  ofst_file_input.close();
  assert(ofst_file_input.is_eof());
  hfst::HfstInputStream ofst_ne_file_input("stream_test_ne",hfst::TROPICAL_OFST_TYPE);
  ofst_ne_file_input.open();
  assert(not ofst_ne_file_input.is_eof());
  ofst_ne_file_input.close();
}
#endif
