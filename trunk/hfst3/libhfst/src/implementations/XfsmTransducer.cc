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
      IY_VERBOSE = 0; // suppress messages
    }


  // ---------- XfsmInputStream functions ----------


    /** Create an XfsmInputStream that reads from stdin. */
    XfsmInputStream::XfsmInputStream(void)
    {
      HFST_THROW_MESSAGE
        (FunctionNotImplementedException,
         "XfsmInputStream::XfsmInputStream() not supported");
    }

    /** Create an XfsmInputStream that reads from file \a filename. */
    XfsmInputStream::XfsmInputStream(const std::string &filename_):
      filename(std::string(filename_)), net_list(NULL), list_size(-1), list_pos(-1)
    {
      if (filename == std::string())
        { HFST_THROW_MESSAGE
            (FunctionNotImplementedException,
             "XfsmInputStream::XfsmInputStream(\"\") not supported");
        }
      else {
        FILE * input_file = fopen(filename.c_str(),"r");
        if (input_file == NULL)
          { 
            HFST_THROW(StreamNotReadableException); }
        fclose(input_file);

        char * fn = strdup(filename.c_str());
        FST_CNTXTptr fst_cntxt = get_default_cfsm_context();
        net_list = load_nets(fn, fst_cntxt);
        free(fn);
        if (net_list == NULL)
          { HFST_THROW(StreamNotReadableException); }
        list_size = NV_len(net_list);
        if (list_size <= 0)
          { HFST_THROW(HfstFatalException); }
        list_pos = 0;
      }
    }

    /** Close the stream. */
    void XfsmInputStream::close(void)
    {
      free_nv_and_nets(net_list);
      list_size = -1;
      list_pos = -1;
      return;
    }
  
    bool XfsmInputStream::is_eof(void)
    {
      return (list_pos >= list_size);
    }
  
    bool XfsmInputStream::is_bad(void)
    {
      return is_eof();
    }
    
    bool XfsmInputStream::is_good(void)
    {
      return not is_bad();
    };
    
    NETptr XfsmInputStream::read_transducer()
    {
      if (is_eof())
        return NULL;
      NETptr retval = nv_get(net_list, list_pos);
      if (retval == NULL)
        { HFST_THROW(StreamNotReadableException); }
      ++list_pos;
      return XfsmTransducer::copy(retval);
    };


  // ---------- XfsmOutputStream functions ----------

    XfsmOutputStream::XfsmOutputStream(void)
    {
      throw "XfsmOutputStream::XfsmOutputStream() not supported";
    }

    XfsmOutputStream::XfsmOutputStream(const std::string &str):
      filename(std::string(str)), net_list(NULL)
    {
      if (filename != std::string()) {
        FILE * ofile = fopen(filename.c_str(), "wb");
        if (ofile == NULL) {
          HFST_THROW(StreamNotReadableException);
        }
        fclose(ofile); /* XFSM api only has a function save_net(char * filename). */
      }
      else {
        throw "XfsmOutputStream::XfsmOutputStream(\"\") not supported";
      }
    }

    void XfsmOutputStream::close(void)
    {
      /* ofile is closed already as we use filenames. */
    }

    void XfsmOutputStream::flush()
    {
      if (net_list != NULL)
        {
          FST_CNTXTptr cptr = get_default_cfsm_context();
          char * fn = strdup(filename.c_str());
          if (save_nets(net_list, fn, cptr) != 0)
            HFST_THROW_MESSAGE
              (HfstFatalException,
               "an error happened when writing an xfsm transducer");
          free(fn);
        }
      free_nv_and_nets(net_list);
      net_list = NULL;
    }

    /* Writing is delayed and done when flush() is called. */
    void XfsmOutputStream::write_transducer(NETptr transducer) 
    {
      if (net_list == NULL) 
        {
          net_list = make_nv(0);
        }
      nv_add(XfsmTransducer::copy(transducer), net_list);
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

    NETptr XfsmTransducer::copy(NETptr t) 
    {
      return copy_net(t);
    }

    bool XfsmTransducer::minimize_even_if_already_minimal_ = false;

    void XfsmTransducer::set_minimize_even_if_already_minimal(bool value)
    {
      minimize_even_if_already_minimal_ = value;
    }

    NETptr XfsmTransducer::minimize(NETptr t)
    {
      if (minimize_even_if_already_minimal_) 
        {
          NET_minimized(t) = 0; 
        }
      if (minimize_net(t) == 1)
        {
          HFST_THROW(HfstFatalException);
        }
      return t;
    }

    void XfsmTransducer::write_in_att_format(NETptr t, const char * filename)
    {
      HFST_THROW(HfstFatalException);
    }

    void XfsmTransducer::write_in_prolog_format(NETptr t, const char * filename)
    {
      char * f = strdup(filename);
      if (write_prolog(t, f) != 0)
        HFST_THROW(HfstFatalException);
      free(f);
    }

    NETptr XfsmTransducer::prolog_file_to_xfsm_transducer(const char * filename)
    {
      char * f = strdup(filename);
      NETptr retval = read_prolog(f);
      if (retval == NULL)
        HFST_THROW(HfstFatalException);
      free(f);
      return retval;
    }

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
