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

  class XfsmInputStream
  {
  private:
    std::string filename;
    FILE * input_file;
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);
  public:
    XfsmInputStream(void);
    XfsmInputStream(const std::string &filename);
    void close(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    void ignore(unsigned int);
    NETptr read_transducer();

    char stream_get();
    short stream_get_short();
    void stream_unget(char c);

    static bool is_fst(FILE * f);
    static bool is_fst(std::istream &s);
  };

  class XfsmOutputStream
  {
  private:
    std::string filename;
    FILE *ofile;
    //void write_3_0_library_header(FILE *file);
  public:
    XfsmOutputStream(void);
    XfsmOutputStream(const std::string &filename);
    void close(void);
    void write(const char &c);
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
    static NETptr read_net(FILE * f);
  } ;
} }
#endif
