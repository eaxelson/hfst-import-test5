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

#ifndef _XFSM_TRANSDUCER_H_
#define _XFSM_TRANSDUCER_H_

#include "HfstSymbolDefs.h"
#include "HfstExceptionDefs.h"
#include "HfstExtractStrings.h"
#include "HfstFlagDiacritics.h"
#include <stdlib.h>

#ifndef _XFSMLIB_H_
#define _XFSMLIB_H_
#include "xfsm/xfsm_api.h"
#endif
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

#include "../FormatSpecifiers.h"

/** @file XfsmTransducer.h
    \brief Declarations of functions and datatypes that form a bridge between
    HFST API and xfsm. */

namespace hfst {
  namespace implementations
{

  /* A class for reading XFSM binary transducers from a file. */
  class XfsmInputStream
  {
  private:
    /* The name of the file where transducers will be read from. */
    std::string filename;
    /* A list of transducers that will return transducers one by one when 
       read_transducer() is called. */
    NVptr net_list;
    /* The size of net_list. */
    int list_size;
    /* The position of the transducer that read_transducer() will return. */
    int list_pos;
  public:
    /* Throws an error. */
    XfsmInputStream(void);
    /* A stream that will read transducers from file \a filename. */
    XfsmInputStream(const std::string &filename);
    /* Frees the memory allocated for this XfsmInputStream. */
    void close(void);
    /* Whether there are any transducers left in net_list. */
    bool is_eof(void);
    /* Whether the stream is bad for reading, always false. */
    bool is_bad(void);
    /* Whether the stream is good for reading, always true. */
    bool is_good(void);
    /* Whether the next item returned by read_transducer() is a valid
       XFSM transducer. Basically always true, except if is_eof() is true. */
    bool is_fst(void);
    /* Delayed read, returns a transducer from net_list. */
    NETptr read_transducer();
  };

  /* A class for writing XFSM transducers in binary format to a file. */
  class XfsmOutputStream
  {
  private:
    /* The name of the file where transducers will be written. */
    std::string filename;
    /* A list of transducers to be written when flush() is called. */
    NVptr net_list;
  public:
    /* Throws an error. */
    XfsmOutputStream(void);
    /* A stream that will write transducers into file \a filename. */
    XfsmOutputStream(const std::string &filename);
    /* Writes the contents of net_list into file filename. */
    void flush();
    /* Does nothing. */
    void close();
    /* Delayed write, stores a copy of \a transducer in net_list. */
    void write_transducer(NETptr transducer); 
  };

  class XfsmTransducer {
  public:
    static void initialize_xfsm();
    static NETptr create_xfsm_unknown_to_unknown_transducer();
    static NETptr create_xfsm_identity_to_identity_transducer();
    static id_type hfst_symbol_to_xfsm_symbol(const std::string & symbol);
    static std::string xfsm_symbol_to_hfst_symbol(id_type id);
    static void label_id_to_symbol_pair(id_type label_id, std::string & isymbol, std::string & osymbol);
    static id_type symbol_pair_to_label_id(const std::string & isymbol, const std::string & osymbol);

    static NETptr create_empty_transducer(void);
    static NETptr create_epsilon_transducer(void);
    static NETptr define_transducer(const hfst::StringPairVector &spv);
    static NETptr define_transducer
      (const hfst::StringPairSet &sps, bool cyclic=false);
    static NETptr define_transducer(const std::vector<StringPairSet> &spsv);
    static NETptr define_transducer
      (const std::string &symbol);
    static NETptr define_transducer
      (const std::string &isymbol, const std::string &osymbol);
    static NETptr copy(NETptr t);
    static NETptr minimize(NETptr t);

    static void write_in_att_format(NETptr t, const char * filename);
    static void write_in_prolog_format(NETptr t, const char * filename);
    static NETptr prolog_file_to_xfsm_transducer(const char * filename);

  } ;
} }
#endif
