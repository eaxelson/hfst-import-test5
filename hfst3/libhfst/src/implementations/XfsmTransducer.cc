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

#include "XfsmTransducer.h"
#include <cstring>

#ifndef MAIN_TEST
namespace hfst { namespace implementations {

    void XfsmTransducer::initialize_xfsm()
    {
      FST_CNTXTptr cntxt = initialize_cfsm();
      (void) set_char_encoding(cntxt, CHAR_ENC_UTF_8);
    }


  // ---------- XfsmInputStream functions ----------

    /** Create a XfsmInputStream that reads from stdin. */
  XfsmInputStream::XfsmInputStream(void)
  {
    input_file = stdin;
  }
    /** Create a XfsmInputStream that reads from file \a filename. */
    XfsmInputStream::XfsmInputStream(const std::string &filename_):
      filename(std::string(filename_))
  {
    if (filename == std::string())
      { input_file = stdin; }
    else {
      input_file = fopen(filename.c_str(),"r");
      if (input_file == NULL)
        { 
          HFST_THROW(StreamNotReadableException); }
    }
  }

    /** Close the stream. */
  void XfsmInputStream::close(void)
  {
    if (input_file == NULL)
      { return; }
    if (filename.c_str()[0] != 0)
      {
    fclose(input_file);
    input_file = NULL;
      }
  }
  
  char XfsmInputStream::stream_get() {
    return (char) fgetc(input_file); }

  short XfsmInputStream::stream_get_short() 
  {
    short i;
    assert(1 == fread(&i,sizeof(i),1,input_file));
    return i;
  }

  void XfsmInputStream::stream_unget(char c) {
    ungetc ( (int)c, input_file ); }

  bool XfsmInputStream::is_eof(void)
  {
    int c = getc(input_file);
    bool retval = (feof(input_file) != 0);
    ungetc(c, input_file);
    return retval;
  }
  
  bool XfsmInputStream::is_bad(void)
  {
    return is_eof();
  }
  
  bool XfsmInputStream::is_good(void)
  {
    return not is_bad();
  };
  
  bool XfsmInputStream::is_fst(void)
  {
    return is_fst(input_file);
  }
  
  bool XfsmInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    int c = getc(f);
    ungetc(c, f);
    return c == 31 || c == (int)'#';
  }
  
  bool XfsmInputStream::is_fst(std::istream &s)
  {
    int c = s.peek();
    return s.good() && (c == 31 || c == (int)'#');
  }

  /* Skip the identifier string "FOMA_TYPE" */
  void XfsmInputStream::skip_identifier_version_3_0(void)
  { 
    char foma_identifier[10];
    int foma_id_count = fread(foma_identifier,10,1,input_file);
    if (foma_id_count != 1)
      { 
        HFST_THROW(NotTransducerStreamException); }
    if (0 != strcmp(foma_identifier,"FOMA_TYPE"))
      {
        HFST_THROW(NotTransducerStreamException); }
  }
  
  void XfsmInputStream::skip_hfst_header(void)
  {
    char hfst_header[6];
    int header_count = fread(hfst_header,6,1,input_file);
    if (header_count != 1)
      {  
        HFST_THROW(NotTransducerStreamException); }
    try { skip_identifier_version_3_0(); }
    catch (const HfstException e)
      { throw e; }
  }

    void XfsmInputStream::ignore(unsigned int n)
    { 
      for (unsigned int i=0; i<n; i++)
    fgetc(input_file);
    }

  NETptr XfsmInputStream::read_transducer()
  {
    if (is_eof())
      return NULL;
    NETptr t = XfsmTransducer::read_net(input_file);
    if (t == NULL) {
      HFST_THROW(NotTransducerStreamException);
    }
    return t;
  };

    XfsmOutputStream::XfsmOutputStream(void)
    { /*ofile = stdout;*/ 
      throw "XfsmOutputStream::XfsmOutputStream() not supported";
    }

    XfsmOutputStream::XfsmOutputStream(const std::string &str):
      filename(std::string(str))
    {
      if (filename != std::string()) {
        ofile = fopen(filename.c_str(), "wb");
        if (ofile == NULL) {
          HFST_THROW(StreamNotReadableException);
        }
      }
      else {
        throw "XfsmOutputStream::XfsmOutputStream(\"\") not supported";
        /*ofile = stdout;*/
      }
    }

    void XfsmOutputStream::close(void)
    {
      if (filename != std::string())
        { fclose(ofile); }
    }

    void XfsmOutputStream::write(const char &c)
    {
      fputc(c,ofile);
    }

    void XfsmOutputStream::write_transducer(NETptr transducer) 
    {
      FST_CNTXTptr cptr = get_default_cfsm_context();
      char * fn = strdup(filename.c_str());
      if (save_net(transducer, fn, cptr) != 0)
        HFST_THROW_MESSAGE
          (HfstFatalException,
           "an error happened when writing an xfsm transducer");
      // free(fn) ?
    }

    static id_type hfst_symbol_to_xfsm_symbol(const std::string & symbol);
    static std::string xfsm_symbol_to_hfst_symbol(id_type id);
    static void label_id_to_symbol_pair(id_type label_id, std::string & isymbol, std::string & osymbol);
    static id_type symbol_pair_to_label_id(const std::string & isymbol, const std::string & osymbol);

    // Convert between hfst and xfsm one-side symbols.
    // The identity symbol must be handled separately.
    id_type XfsmTransducer::hfst_symbol_to_xfsm_symbol(const std::string & symbol)
    {
      if (symbol == hfst::internal_epsilon)
        return EPSILON;
      else if (symbol == hfst::internal_unknown)
        return OTHER;
      else if (symbol == hfst::internal_identity)
        throw "hfst_symbol_to_xfsm_symbol does not accept the identity symbol as its argument";
      else
        return single_to_id(symbol.c_str());
    }
    
    // Convert between xfsm and hfst one-side symbols.
    // The identity symbol must be handled separately.
    std::string XfsmTransducer::xfsm_symbol_to_hfst_symbol(id_type id)
    {
      if (id == EPSILON)
        return hfst::internal_epsilon;
      else if (id == OTHER)
        return hfst::internal_unknown;
      else {
        std::string retval("");
        LABELptr lptr = id_to_label(id);
        FAT_STR fs = lptr->content.name;
        while (*fs != '\0')
          {
            retval.append(1, *fs);
            ++fs;
          }
        return retval;
      }
    }

    // Convert between an xfsm label (symbol pair) and hfst transition symbols.
    void XfsmTransducer::label_id_to_symbol_pair(id_type label_id, std::string & isymbol, std::string & osymbol)
    {
      // (1) atomic OTHER label -> identity pair
      if (label_id == OTHER)
        {
          isymbol = hfst::internal_identity;
          osymbol = hfst::internal_identity;
        }
      else
        {
          // (2) non-atomic OTHER label -> unknown pair
          // (3) all other cases
          id_type upperid = upper_id(label_id);
          id_type lowerid = lower_id(label_id);
          isymbol = xfsm_symbol_to_hfst_symbol(upperid);
          osymbol = xfsm_symbol_to_hfst_symbol(lowerid);
        }
    }

    id_type XfsmTransducer::symbol_pair_to_label_id(const std::string & isymbol, const std::string & osymbol)
    {
      if (isymbol == hfst::internal_identity)
        {
          if (osymbol != hfst::internal_identity)
            throw "identity symbol cannot be on one side only";
          // atomic OTHER label
          return OTHER;
        }
      else
        {
          id_type input_id = hfst_symbol_to_xfsm_symbol(isymbol);
          id_type output_id = hfst_symbol_to_xfsm_symbol(osymbol);
          return id_pair_to_id(input_id, output_id);
        }
    }

    NETptr XfsmTransducer::create_xfsm_unknown_to_unknown_transducer()
    {
      NETptr result = null_net();
      STATEptr final = add_state_to_net(result, 1);
      id_type ti = id_pair_to_id(OTHER, OTHER);
      if( add_arc_to_state(result, result->start.state, ti, final, NULL, 0) == NULL )
        throw "add_arc_to_state failed";
      return result;
    }

    NETptr XfsmTransducer::create_xfsm_identity_to_identity_transducer()
    {
      NETptr result = null_net();
      STATEptr final = add_state_to_net(result, 1);
      id_type ti = OTHER;
      if( add_arc_to_state(result, result->start.state, ti, final, NULL, 0) == NULL )
        throw "add_arc_to_state failed";
      return result;
    }

    NETptr XfsmTransducer::create_empty_transducer(void) 
    {
      return null_net();
    }

    NETptr XfsmTransducer::create_epsilon_transducer(void) 
    {
      NETptr result = null_net();
      return optional_net(result, DONT_KEEP);
    }

    NETptr XfsmTransducer::define_transducer(const hfst::StringPairVector &spv) { return NULL; }

    NETptr XfsmTransducer::define_transducer(const hfst::StringPairSet &sps, bool cyclic/*=false*/) { return NULL; }

    NETptr XfsmTransducer::define_transducer(const std::vector<StringPairSet> &spsv) { return NULL; }

    NETptr XfsmTransducer::define_transducer(const std::string &symbol) { return NULL; }

    NETptr XfsmTransducer::define_transducer(const std::string &isymbol, const std::string &osymbol) { return NULL; }

    NETptr XfsmTransducer::copy(NETptr t) { return NULL; }

    NETptr XfsmTransducer::read_net(FILE * f)  { return NULL; }

  } }

#else // MAIN_TEST was defined
#include <cstdlib>
#include <cassert>
#include <iostream>
using namespace hfst::implementations;

int main(int argc, char * argv[]) 
{
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "ok" << std::endl;
    return EXIT_SUCCESS;
}
#endif // MAIN_TEST
