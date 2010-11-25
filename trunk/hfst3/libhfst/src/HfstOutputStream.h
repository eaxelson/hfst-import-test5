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
      HfstOutputStream out("testfile", FOMA_TYPE);
      out << foma_transducer1 
          << foma_transducer2 
          << foma_transducer3;
      out.close();
\endverbatim
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

#if HAVE_FOO
      hfst::implementations::FooOutputStream * foo;
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

    void append_hfst_header_data(std::vector<char> &header);
    void append_implementation_specific_header_data(std::vector<char> &header, HfstTransducer &transducer);


  public:

    /** \brief Create a stream to standard out for writing binary transducers of type \a type. 
	\a hfst_format defines whether transducers are written in hfst format or as such in their backend format. */
    HfstOutputStream(ImplementationType type, bool hfst_format=true);

    /** \brief Open a stream to file \a filename for writing binary transducers of type \a type. 
	\a hfst_format defines whether transducers are written in hfst format or as such in their backend format.

	If the file exists, it is overwritten. */
    HfstOutputStream(const std::string &filename, ImplementationType type, bool hfst_format=true);

    /** \brief Delete the stream. */
    ~HfstOutputStream(void);  

    /** \brief Write the transducer \a transducer in binary format to the stream. 

	By default, all transducers must have the same type, else an
	hfst::exceptions::TransducerHasWrongTypeException is thrown. */
    HfstOutputStream &operator<< (HfstTransducer &transducer);

    /** \brief Close the stream. 

	If the stream points to standard out, nothing is done. */
    void close(void);
  };




}



#endif
