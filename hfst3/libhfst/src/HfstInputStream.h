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
#ifndef _HFST_INPUTSTREAM_H_
#define _HFST_INPUTSTREAM_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "HfstTransducer.h"

/** @file HfstInputStream.h 
    \brief Declaration of class HfstInputStream.
 */

namespace hfst
{
  /** \brief A stream for reading binary transducers. 

      An example:
\verbatim
#include "HfstInputStream.h"

...

HfstInputStream *in;
try {
  in = new HfstInputStream("testfile");
} catch (FileNotReadableException e) {
    printf("ERROR: File does not exist.\n");
    exit(1);
}
int n=0;
while (not in->is_eof()) {
  if (in->is_bad()) {
    printf("ERROR: Stream cannot be read.\n");
    exit(1); 
  }
  HfstTransducer t(*in);
  printf("One transducer succesfully read.\n");
  n++;
}
printf("\nRead %i transducers in total.\n", n);
in->close();
delete in;
\endverbatim

For documentation on the HFST binary transducer format, see #hfst::HfstOutputStream.

      @see HfstTransducer::HfstTransducer(HfstInputStream &in) **/
  class HfstInputStream
  {
  protected:

    union StreamImplementation
    {
#if HAVE_SFST
      hfst::implementations::SfstInputStream * sfst;
#endif
#if HAVE_OPENFST
      hfst::implementations::TropicalWeightInputStream * tropical_ofst;
      hfst::implementations::LogWeightInputStream * log_ofst;
#endif
#if HAVE_FOMA
      hfst::implementations::FomaInputStream * foma;
#endif

#if HAVE_MFSTL
      hfst::implementations::MfstlInputStream * mfstl;
#endif

      hfst::implementations::HfstOlInputStream * hfst_ol;
    };

    StreamImplementation implementation; // the backend inplementation
    ImplementationType type;             // implementation type
    std::string name;                    // name of next transducer, given in the hfst header
    unsigned int bytes_to_skip;          // how many bytes have been already read by the function
                                         // processing the hfst header
    std::string filename;                // the name of the file, if stdin, name is ""

    /* A special case where an OpenFst transducer has no symbol tables but an
       SFST alphabet is appended at the end. Should not occur very often, but
       possible when converting old transducers into version 3.0. transducers.. */
    bool hfst_version_2_weighted_transducer;
 
    /* the stream that the reading operations use
       this stream is used when reading the first transducer, when the type of the transducer is not known
       and thus there is no backend implementation whose reading functions could be used
       if input_stream==NULL, the backend implementation is used */
    std::istream * input_stream;

    /* Basic stream operators, work on input_stream (if not NULL) or on the stream implementation. */
    char stream_get(); // extract one character from the stream
    void stream_unget(char c); // return character c to the stream
    bool stream_eof(); // whether the stream is at end
    std::string stream_getstring(); // get a string from the stream
    char stream_peek();     // return the next character in the stream without extracting it
    /* The stream implementation ignores n bytes. */
    void ignore(unsigned int n);

    /* The type of a transducer not supported directly by HFST version 3.0 
       but which can occur in conversion functions. */
    enum TransducerType { 
      HFST_VERSION_2_WEIGHTED, /* See the above variable. */
      HFST_VERSION_2_UNWEIGHTED_WITHOUT_ALPHABET, /* An SFST transducer with no alphabet,
						     not supported. */
      HFST_VERSION_2_UNWEIGHTED, /* Old header + ordinary SFST transducer. */               
      OPENFST_, /* An OpenFst transducer, can cause problems if it does not have symbol tables. */
      SFST_,  /* An SFST transducer. */
      FOMA_, /* A foma transducer. */
      MFSTL_, /* A new type */
      ERROR_TYPE_ /* Transducer type not recognized. */
    };

    /* Read a transducer from the stream. */
    void read_transducer(HfstTransducer &t);
    /* Type of next transducer in the stream. */
    ImplementationType stream_fst_type();

    // methods used by function stream_fst_type
    TransducerType guess_fst_type(int &bytes_read);
    bool read_hfst_header(int &bytes_read);
    bool read_library_header(int &bytes_read);
    int get_header_size(int &bytes_read);                        
    StringPairVector get_header_data(int header_size);                
    void process_header_data(StringPairVector &header_data, bool warnings=false); 
    bool set_implementation_specific_header_data(StringPairVector &data, unsigned int index);


    bool read_library_header_old(int &bytes_read);
    ImplementationType get_fst_type_old(int &bytes_read); 

  public:

    /** \brief Create a stream to standard in for reading binary transducers. */
    HfstInputStream(void);

    /** \brief Open a stream to file \a filename for reading binary transducers. 

	@pre The file exists. Otherwise, an exception is thrown.
	@throws hfst::exceptions::FileNotReadableException */
    HfstInputStream(const std::string &filename);

    /** Delete the stream. */
    ~HfstInputStream(void);

    /** \brief Close the stream.

	If the stream points to standard in, nothing is done. */
    void close(void);

    /** \brief Whether the stream is at the end. */
    bool is_eof(void);
    /** \brief Whether badbit is set. */
    bool is_bad(void);
    /** \brief Whether the state of the stream is good for input operations. */
    bool is_good(void);
    
    /** \brief The type of the first transducer in the stream. 

	By default, all transducers in a stream have the same type, else
	an hfst::exceptions::TransducerTypeMismatchException is thrown. */
    ImplementationType get_type(void) const;

    friend class HfstTransducer;
  };


}



#endif
