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
#ifndef _HFST_OUTPUTSTREAM_H_
#define _HFST_OUTPUTSTREAM_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "HfstTransducer.h"

/** @file HfstOutputStream.h
 \brief Declaration of class HfstOutputStream. */

namespace hfst
{


  /** \brief A stream for writing binary transducers. 

      An example:
\verbatim
#include "HfstOutputStream.h"

      ...

      HfstOutputStream out("testfile", FOMA_TYPE);
      out << foma_transducer1 
          << foma_transducer2 
          << foma_transducer3;
      out.close();
\endverbatim

An HFST transducer in binary format consist of an HFST header
and the transducer of the backend implementation. 
The HFST header has the following structure:
       
- the first four chars identify an HFST header: "HFST"
- the fifth char is a separator: "\0"
- the sixth and seventh char tell the length of the rest of the header (beginning after the eighth char)
- the eighth char is a separator and is not counted to the header length: "\0"
- the rest of the header consists of pairs of attributes and their values
that are each separated by a char "\0"

HFST version 3.0 header must contain at least the attributes 'version', 'type'
and 'name' and their values. Currently the accepted values are
'3.0' for the attribute 'version', 'SFST_TYPE', 'FOMA_TYPE', 
'TROPICAL_OPENFST_TYPE', 'LOG_OPENFST_TYPE', 'HFST_OL_TYPE' and 'HFST_OLW_TYPE'
for the attribute 'type' and any string (including the empty string) for 
the attribute 'name'.

An example:

\verbatim
"HFST\0"
"\0\x1c\0"
"version\0"  "3.0\0"
"type\0"     "FOMA\0"
"name\0"     "\0"
\endverbatim

This is the header of a version 3.0 HFST transducer whose implementation 
type is foma and whose name is not defined, i.e. is the empty string "". 
The two bytes "\0\x1c" that form the length field tell that the length of
the rest of the header (i.e. the sequence of bytes
"version\03.0\0type\0FOMA\0name\0\0") is 0 * 256 + 28 * 1 = 28 bytes.

  **/
  class HfstOutputStream
  {
  protected:
    union StreamImplementation
    {
#if HAVE_OPENFST
      hfst::implementations::LogWeightOutputStream * log_ofst;
      hfst::implementations::TropicalWeightOutputStream * tropical_ofst;
#endif
#if HAVE_SFST
      hfst::implementations::SfstOutputStream * sfst;
#endif
#if HAVE_FOMA
      hfst::implementations::FomaOutputStream * foma;
#endif

#if HAVE_MY_TRANSDUCER_LIBRARY
      hfst::implementations::MyTransducerLibraryOutputStream * 
        my_transducer_library;
#endif

      hfst::implementations::HfstOlOutputStream * hfst_ol;
    };
    ImplementationType type; // type of the stream implementation
    bool hfst_format;  // whether an hfst header is written before every transducer
    StreamImplementation implementation; // backend implementation

    // write data to stream
    void write(const std::string &s);
    void write(const std::vector<char> &s);
    void write(const char &c);

    // append string s to vector str and a '\0'
    static void append(std::vector<char> &str, const std::string &s);

    // append obligatory HFST header data to \a header
    void append_hfst_header_data(std::vector<char> &header);
    // append implementation-specific header data collected from \a transducer to \a header
    void append_implementation_specific_header_data(std::vector<char> &header, HfstTransducer &transducer);


  public:

    /** \brief Create a stream to standard output for writing binary transducers of type \a type. 
	\a hfst_format defines whether transducers are written in hfst format or as such in their backend format. */
    HfstOutputStream(ImplementationType type, bool hfst_format=true);

    /** \brief Open a stream to file \a filename for writing binary transducers of type \a type. 
	\a hfst_format defines whether transducers are written in hfst format or as such in their backend format.

	If the file exists, it is overwritten. */
    HfstOutputStream(const std::string &filename, ImplementationType type, bool hfst_format=true);

    /** \brief Destructor. */
    ~HfstOutputStream(void);  

    /** \brief Write the transducer \a transducer in binary format to the stream. 

	By default, all transducers must have the same type, else an
	hfst::exceptions::TransducerHasWrongTypeException is thrown. */
    HfstOutputStream &operator<< (HfstTransducer &transducer);

    /** \brief Close the stream. 

	If the stream points to standard output, nothing is done. */
    void close(void);
  };




}



#endif
