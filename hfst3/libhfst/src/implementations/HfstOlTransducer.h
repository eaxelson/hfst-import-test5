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

#include <iostream>
#include <fstream>
#include "HfstExceptions.h"
#include "FlagDiacritics.h"
#include "optimized-lookup/transducer.h"

namespace hfst { namespace implementations
{
  using std::ostream;
  using std::ofstream;
  using std::istream;
  using std::ifstream;
  using std::string;
  using namespace hfst::exceptions;
  
  class HfstOlInputStream
  {
  private:
    std::string filename;
    ifstream i_stream;
    istream &input_stream;
    bool weighted;
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);
  public:
    HfstOlInputStream(bool weighted);
    HfstOlInputStream(const char * filename, bool weighted);
    void open(void);
    void close(void);
    bool is_open(void) const;
    bool is_eof(void) const;
    bool is_bad(void) const;
    bool is_good(void) const;
    bool is_fst(void) const;
    bool operator() (void) const;
    hfst_ol::Transducer * read_transducer(bool has_header);
    
    // 1=unweighted, 2=weighted
    static int is_fst(FILE * f);
    static int is_fst(istream &s);
  };
  
  class HfstOlOutputStream 
  {
  private:
    std::string filename;
    ofstream o_stream;
    ostream &output_stream;
    bool weighted;
    void write_3_0_library_header(std::ostream &out);
  public:
    HfstOlOutputStream(bool weighted);
    HfstOlOutputStream(const char * filename, bool weighted);
    void open(void);
    void close(void);
    void write_transducer(hfst_ol::Transducer * transducer);
  };
  
  class HfstOlTransducer
  {
   public:
    static hfst_ol::Transducer * create_empty_transducer(bool weighted);
    
    static bool is_cyclic(hfst_ol::Transducer* t);
    
    static FdTable<hfst_ol::SymbolNumber>* get_flag_diacritics(hfst_ol::Transducer* t);
  };
  
} }
