#include "HfstTransducer.h"

namespace hfst
{
  void HfstInputStream::read_transducer(HfstTransducer &t)
  {
    switch (type)
      {
      case SFST_TYPE:
	t.implementation.sfst =
	  this->implementation.sfst->read_transducer(t.key_table);
	break;
      case TROPICAL_OFST_TYPE:
	t.implementation.tropical_ofst =
	  this->implementation.tropical_ofst->read_transducer(t.key_table);
	break;
      case LOG_OFST_TYPE:
	t.implementation.log_ofst =
	  this->implementation.log_ofst->read_transducer(t.key_table);
	break;
	  case ERROR_TYPE:
	  case UNSPECIFIED_TYPE:
	  default:
		throw hfst::exceptions::NotTransducerStreamException();
		break;
	  }
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
    if (0 == strcmp(fst_type,"TROPICAL_OFST_TYPE"))
      { return TROPICAL_OFST_TYPE; }
    if (0 == strcmp(fst_type,"LOG_OFST_TYPE"))
      { return LOG_OFST_TYPE; }
    return ERROR_TYPE;
  }

  int HfstInputStream::read_library_header(std::istream &in) 
  {
    char id[6];
    in.getline(id,6,0);
    if (strcmp(id,"HFST3") != 0)
      { return -1; }
    if (in.eof())
      { return -1; }
    char c;
    in.get(c);
    if (1 != in.gcount())
      { return -1; }
    return c;
  }

  ImplementationType HfstInputStream::stream_fst_type(std::istream &in)
  { 
    if (not in.good())
      { throw hfst::implementations::FileNotReadableException(); }
    int library_version;
    if (-1 == (library_version = read_library_header(in)))
      { return ERROR_TYPE; }
    switch (library_version)
      {
      case 0:
	return read_version_3_0_fst_type(in);
	break;
      default:
	return ERROR_TYPE;
	break;
      }
    return ERROR_TYPE;
  }

  HfstInputStream::HfstInputStream(void)
  {
    try { type = stream_fst_type(std::cin); }
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
    default:
      throw hfst::exceptions::NotTransducerStreamException();
    }
  }

  HfstInputStream::HfstInputStream(const char* filename)
  {
    try { 
      std::ifstream in(filename);
      type = stream_fst_type(in); 
    }
    catch (hfst::implementations::FileNotReadableException e)
      { throw e; }

    if (type == ERROR_TYPE)
      { throw hfst::implementations::NotTransducerStreamException(); }

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
    default:
      assert(false);
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
	  case LOG_OFST_TYPE:
	return implementation.log_ofst->is_open();
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
      default:
	assert(false);
	return false;
      }
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
