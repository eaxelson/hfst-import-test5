#include <cstring>
#include "HfstOlTransducer.h"

namespace hfst { namespace implementations
{
  HfstOlInputStream::HfstOlInputStream(bool weighted):
    i_stream(),input_stream(std::cin), weighted(weighted)
  {}
  HfstOlInputStream::HfstOlInputStream(const char * filename, bool weighted):
    filename(filename),i_stream(filename),input_stream(i_stream),weighted(weighted)
  {}
  
  /* Skip the identifier string "HFST_OL_TYPE" or "HFST_OLW_TYPE" */
  void HfstOlInputStream::skip_identifier_version_3_0(void)
  { input_stream.ignore((weighted?14:13)); }

  void HfstOlInputStream::skip_hfst_header(void)
  {
    input_stream.ignore(6);
    skip_identifier_version_3_0();
  }
  
  void HfstOlInputStream::open(void) {}
  void HfstOlInputStream::close(void)
  {
    if (filename != string())
      { i_stream.close(); }
  }
  bool HfstOlInputStream::is_open(void) const
  { 
    if (filename != string())
      { return i_stream.is_open(); }
    return true;
  }
  bool HfstOlInputStream::is_eof(void) const
  {
    return input_stream.peek() == EOF;
  }
  bool HfstOlInputStream::is_bad(void) const
  {
    if (filename == string())
      { return std::cin.bad(); }
    else
      { return input_stream.bad(); }    
  }
  bool HfstOlInputStream::is_good(void) const
  {
    if(is_eof())
      return false;
    
    if (filename == string())
      { return std::cin.good(); }
    else
      { return input_stream.good(); }
  }
  
  bool HfstOlInputStream::is_fst(void) const
  {
    return (is_fst(input_stream)!=0);
  }
  
  int HfstOlInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    
    char buffer[24];
    size_t num_read = fread(buffer, 24, 1, f);
    unsigned int weighted = *((int*)(buffer+20));
    int res;
    if(num_read != 24)
      { res = 0; }
    else if(weighted == 0)
      { res = 1; }
    else if(weighted == 1)
      { res = 2; }
    else
      { res = 0; }
    
    for(int i=num_read-1;i>=0;i--)
      { ungetc(buffer[i], f); }
    if(num_read != 24)
      { clearerr(f); }
    
    return res;
  }
  
  int HfstOlInputStream::is_fst(istream &s)
  {
    if(!s.good())
      { return 0; }
    
    char buffer[24];
    s.read(buffer, 24);
    size_t num_read = s.gcount();
    unsigned int weighted = *((int*)(buffer+20));
    int res;
    if(num_read != 24)
      { res = 0; }
    else if(weighted == 0)
      { res = 1; }
    else if(weighted == 1)
      { res = 2; }
    else
      { res = 0; }
    
    for(int i=num_read-1;i>=0;i--)
      { s.putback(buffer[i]); }
    if(num_read != 24)
      { s.clear(); }
    
    return res;
  }

  bool HfstOlInputStream::operator() (void) const
  { return is_good(); }

  hfst_ol::Transducer * HfstOlInputStream::read_transducer(bool has_header)
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    try 
    {
      if (has_header)
        skip_hfst_header();
      
      hfst_ol::Transducer* t = new hfst_ol::Transducer(input_stream);
      //t->display();
      return t;
    }
    catch (TransducerHasWrongTypeException e)
      { throw e; }
  }
  
  
  HfstOlOutputStream::HfstOlOutputStream(bool weighted):
    filename(std::string()), output_stream(std::cout), weighted(weighted)
  {}

  HfstOlOutputStream::HfstOlOutputStream(const char * str, bool weighted):
    filename(str),o_stream(str,std::ios::out),output_stream(o_stream), weighted(weighted)
  {
    if (!output_stream)
      fprintf(stderr, "HfstOlOutputStream: ERROR: failbit set (3).\n");
  }

  void HfstOlOutputStream::write_3_0_library_header(std::ostream &out)
  {
    if (!out)
      fprintf(stderr, "HfstOlOutputStream: ERROR: failbit set (2).\n");
    out.write("HFST3",6);
    if(!weighted)
      out.write("HFST_OL_TYPE",13);
    else
      out.write("HFST_OLW_TYPE",14);
  }
  
  void HfstOlOutputStream::write_transducer(hfst_ol::Transducer * transducer) 
  {
    if (!output_stream)
      fprintf(stderr, "HfstOlOutputStream: ERROR: failbit set (1).\n");
    write_3_0_library_header(output_stream);
    transducer->write(output_stream);
  }
  
  void HfstOlOutputStream::open(void) {}
  void HfstOlOutputStream::close(void) 
  {
    if (filename != string())
      { o_stream.close(); }
  }
  
  hfst_ol::Transducer * HfstOlTransducer::create_empty_transducer(bool weighted)
  { return new hfst_ol::Transducer(weighted); }

} }
