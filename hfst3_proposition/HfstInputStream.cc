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
      }
  }

  ImplementationType HfstInputStream::stream_fst_type(std::istream &in)
  { 
    if (not in.good())
      { throw HFST_IMPLEMENTATIONS::FileNotReadableException(); }
    if (in.peek() == (int)'a')
      { return SFST_TYPE; }
    if ((char)in.peek() == (char)214)
      { return TROPICAL_OFST_TYPE; }
    std::cerr << "error " << in.peek() << std::endl;
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
