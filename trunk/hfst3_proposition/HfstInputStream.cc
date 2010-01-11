#include "HfstTransducer.h"

namespace HFST
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
      }
  }

  bool HfstInputStream::has_tropical_weight_type(std::istream &in)
  {
    if ((char)in.peek() != (char)214)
      { return false; }

    char prefix[18];
    prefix[0] = 0;
    size_t prefix_characters_read = in.readsome(prefix,18);
    if (prefix_characters_read != 18)
      {
	for (unsigned int i = 0; i < prefix_characters_read; ++i)
	  { in.putback(prefix[prefix_characters_read - i]); }
	return false;
      }
    char identifier[9];
    identifier[0] = 0;
    size_t identifier_characters_read = in.readsome(identifier,8);
    if (identifier_characters_read != 8)
      {
	for (unsigned int i = 0; i < identifier_characters_read; ++i)
	  { in.putback(identifier[identifier_characters_read - i]); }
	for (unsigned int i = 0; i < prefix_characters_read; ++i)
	  { in.putback(prefix[prefix_characters_read - i]); }
	return false;
      }
    identifier[8] = 0;
    std::cerr << "identifier: " << identifier << std::endl;
    bool result = strcmp(identifier,"standard") == 0;
    for (unsigned int i = 0; i < identifier_characters_read; ++i)
      { in.putback(identifier[identifier_characters_read - i]); }
    for (unsigned int i = 0; i < prefix_characters_read; ++i)
      { in.putback(prefix[prefix_characters_read - i]); }
    return result;
  }

  bool HfstInputStream::has_log_weight_type(std::istream &in)
  {
    if ((char)in.peek() != (char)214)
      { return false; }

    char prefix[18];
    prefix[0] = 0;
    size_t prefix_characters_read = in.readsome(prefix,18);
    if (prefix_characters_read != 18)
      {
	for (unsigned int i = 0; i < prefix_characters_read; ++i)
	  { in.putback(prefix[prefix_characters_read - i]); }
	return false;
      }
    char identifier[4];
    identifier[0] = 0;
    size_t identifier_characters_read = in.readsome(identifier,3);
    if (identifier_characters_read != 3)
      {
	for (unsigned int i = 0; i < identifier_characters_read; ++i)
	  { in.putback(identifier[identifier_characters_read - i]); }
	for (unsigned int i = 0; i < prefix_characters_read; ++i)
	  { in.putback(prefix[prefix_characters_read - i]); }
	return false;
      }
    identifier[3] = 0;
    std::cerr << "identifier: " << identifier << std::endl;
    bool result = strcmp(identifier,"log") == 0;
    for (unsigned int i = 0; i < identifier_characters_read; ++i)
      { in.putback(identifier[identifier_characters_read - i]); }
    for (unsigned int i = 0; i < prefix_characters_read; ++i)
      { in.putback(prefix[prefix_characters_read - i]); }
    return result;
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
      { throw HFST_IMPLEMENTATIONS::FileNotReadableException(); }
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
    catch (HFST_IMPLEMENTATIONS::FileNotReadableException e)
      { throw e; }

    if (type == ERROR_TYPE)
      { throw HFST_IMPLEMENTATIONS::NotTransducerStreamException(); }
    switch (type)
    {
    case SFST_TYPE:
      implementation.sfst = new HFST_IMPLEMENTATIONS::SfstInputStream;
      break;
    case TROPICAL_OFST_TYPE:
      implementation.tropical_ofst = 
	new HFST_IMPLEMENTATIONS::TropicalWeightInputStream;
      break;
    }
  }

  HfstInputStream::HfstInputStream(const char * filename)
  {
    try { 
      std::ifstream in(filename);
      type = stream_fst_type(in); 
    }
    catch (HFST_IMPLEMENTATIONS::FileNotReadableException e)
      { throw e; }

    if (type == ERROR_TYPE)
      { throw HFST_IMPLEMENTATIONS::NotTransducerStreamException(); }

    switch (type)
    {
    case SFST_TYPE:
      implementation.sfst = new HFST_IMPLEMENTATIONS::SfstInputStream(filename);
      break;
    case TROPICAL_OFST_TYPE:
      implementation.tropical_ofst = 
	new HFST_IMPLEMENTATIONS::TropicalWeightInputStream(filename);
      break;
    case LOG_OFST_TYPE:
      implementation.log_ofst = 
	new HFST_IMPLEMENTATIONS::LogWeightInputStream(filename);
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
	implementation.tropical_ofst->close();
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
	implementation.sfst->is_open();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->is_open();
	break;
      default:
	assert(false);
      }
  }

  bool HfstInputStream::is_eof(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst->is_eof();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->is_eof();
	break;
      default:
	assert(false);
      }
  }

  bool HfstInputStream::is_bad(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst->is_bad();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->is_bad();
	break;
      default:
	assert(false);
      }
  }

  bool HfstInputStream::is_good(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst->is_good();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->is_good();
	break;
      default:
	assert(false);
      }
  }
}

#ifdef DEBUG_MAIN_STREAM
int main(void)
{
  HFST::HfstInputStream sfst_stdin_input(HFST::SFST_TYPE);
  sfst_stdin_input.open();
  sfst_stdin_input.close();
  HFST::HfstInputStream ofst_stdin_input(HFST::TROPICAL_OFST_TYPE);
  ofst_stdin_input.open();
  ofst_stdin_input.close();
  // This succeeds only if the files stream_test and
  // stream_test_ne exist
  HFST::HfstInputStream sfst_file_input("stream_test",HFST::SFST_TYPE);
  sfst_file_input.open();
  sfst_file_input.close();
  assert(sfst_file_input.is_eof());
  HFST::HfstInputStream sfst_ne_file_input("stream_test_ne",HFST::SFST_TYPE);
  sfst_ne_file_input.open();
  assert(not sfst_ne_file_input.is_eof());
  sfst_ne_file_input.close();
  HFST::HfstInputStream ofst_file_input("stream_test",HFST::TROPICAL_OFST_TYPE);
  ofst_file_input.open();
  ofst_file_input.close();
  assert(ofst_file_input.is_eof());
  HFST::HfstInputStream ofst_ne_file_input("stream_test_ne",HFST::TROPICAL_OFST_TYPE);
  ofst_ne_file_input.open();
  assert(not ofst_ne_file_input.is_eof());
  ofst_ne_file_input.close();
}
#endif
