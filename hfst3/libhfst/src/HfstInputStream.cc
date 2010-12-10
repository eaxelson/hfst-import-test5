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
#include "HfstTransducer.h"
#include "HfstInputStream.h"

namespace hfst
{
  void debug_error(const char *msg)
  {
#if PRINT_DEBUG_MESSAGES
    fprintf(stderr, "%s\n", msg);
#endif
  }

  void HfstInputStream::ignore(unsigned int n)
  {
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	this->implementation.sfst->ignore(n);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	this->implementation.tropical_ofst->ignore(n);
	break;
      case LOG_OFST_TYPE:
	this->implementation.log_ofst->ignore(n);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	this->implementation.foma->ignore(n);
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	this->implementation.mfstl->ignore(n);
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	  this->implementation.hfst_ol->ignore(n);
	  break;

      default:
	assert(false);
	break;
      }
  }

  char HfstInputStream::stream_get()
  {
    if (input_stream != NULL)
      return (char) input_stream->get();
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	return this->implementation.sfst->stream_get();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	return this->implementation.tropical_ofst->stream_get();
	break;
      case LOG_OFST_TYPE:
	return this->implementation.log_ofst->stream_get();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	return this->implementation.foma->stream_get();
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	return this->implementation.mfstl->stream_get();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	  return this->implementation.hfst_ol->stream_get();
	  break;

      default:
	assert(false);
	break;
      }
  }

  void HfstInputStream::stream_unget(char c)
  {
    if (input_stream != NULL) {
      input_stream->putback(c);
      return;
    }
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	this->implementation.sfst->stream_unget(c);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	this->implementation.tropical_ofst->stream_unget(c);
	break;
      case LOG_OFST_TYPE:
	this->implementation.log_ofst->stream_unget(c);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	this->implementation.foma->stream_unget(c);
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	this->implementation.mfstl->stream_unget(c);
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	  return this->implementation.hfst_ol->stream_unget(c);
	  break;
      default:
	assert(false);
	break;
      }
  }

  char HfstInputStream::stream_peek()
  {
    char c = stream_get();
    stream_unget(c);
    return c;
  }

  std::string HfstInputStream::stream_getstring()
  {
    std::string retval("");
    while (true) {
      char c = stream_get();
      if (stream_eof())
	throw hfst::exceptions::NotTransducerStreamException();
      if (c == '\0')
	break;
      retval.append(1,c);
    }
    return retval;
  }

  bool HfstInputStream::stream_eof()
  {
    if (input_stream != 0)
      return input_stream->eof();
    return is_eof();
  }

  bool HfstInputStream::set_implementation_specific_header_data(StringPairVector& /* data */, unsigned int /* index*/)
  {
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	//return this->implementation.sfst->set_implementation_specific_header_data(data, index);
	break;
#endif
      default:
	break;
      }
    return false;
  }

  void HfstInputStream::read_transducer(HfstTransducer &t)
  {
    if (input_stream != NULL) { // first transducer in the stream
      input_stream = NULL;
      // if header bytes have been read from a file, skip these bytes
      if (strcmp(filename.c_str(), "") != 0)
	ignore(bytes_to_skip);
	}
    else {
      ImplementationType stype = stream_fst_type();
      if (stype != type) {
	throw hfst::exceptions::TransducerTypeMismatchException();
      }
    }

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	t.implementation.sfst =
	  this->implementation.sfst->read_transducer();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	{
	  t.implementation.tropical_ofst =
	    this->implementation.tropical_ofst->read_transducer();

	  // A special case: HFST version 2 transducer
	  if (hfst_version_2_weighted_transducer) // an SFST alphabet follows
	    {
	      stream_get(); // UTF8
	      
	      // read the symbol mapping
	      unsigned short n=0;
	      n = n + (unsigned short)stream_get() * 1;
	      n = n + (unsigned short)stream_get() * 256;
	      
	      //fprintf(stderr, "alphabet size is %i\n", (int)n );

	      // special symbol-to-number mappings
	      std::vector<std::pair<unsigned short, std::string> > special_cases;

	      // normal symbol-to-number mappings
	      std::vector<std::pair<unsigned short, std::string> > symbol_mappings;

	      unsigned short max_number=0;

	      for( unsigned i=0; i<n; i++) {

		max_number++;

		unsigned short symbol_number=0;
		symbol_number = symbol_number + (unsigned short)stream_get() * 1;
		symbol_number = symbol_number + (unsigned short)stream_get() * 256;
		
		std::string symbol_string("");
		char c = stream_get();
		while (c != '\0') {
		  symbol_string = symbol_string + std::string(1,c);
		  c = stream_get();
		}

		//fprintf(stderr, "read number %hu and symbol %s\n", symbol_number, symbol_string.c_str());

		// epsilon, unknown and identity numbers must be handled separately
		if (symbol_number == 0 || symbol_number == 1 || symbol_number == 2) {
		  special_cases.push_back(std::pair<unsigned short, std::string>(symbol_number,symbol_string));
		}
		else {
		  symbol_mappings.push_back(std::pair<unsigned short, std::string>(symbol_number, symbol_string));
		}
	      }

	      max_number--;

	      // handle special symbol cases
	      for (unsigned int i=0; i<special_cases.size(); i++) {
		if (special_cases[i].first == 0) {
		}
		else {
		  //fprintf(stderr, "substituting number %hu with %hu...\n", special_cases[i].first, max_number+1);
		  fst::StdVectorFst * tmp = 
		    t.tropical_ofst_interface.substitute(t.implementation.tropical_ofst, 
							 special_cases[i].first, 
							 (unsigned short)++max_number);
		  t.implementation.tropical_ofst = tmp;
		  //delete t.implementation.tropical_ofst;

		  symbol_mappings.push_back(std::pair<unsigned short, std::string>(
					      max_number, special_cases[i].second));
		  //fprintf(stderr, "...substituted\n");
		}
	      }

	      t.tropical_ofst_interface.set_symbol_table(t.implementation.tropical_ofst, symbol_mappings);

	      // skip the character pairs
	      unsigned short to_skip=0;
	      to_skip = to_skip + (unsigned short)stream_get() * 1;
	      to_skip = to_skip + (unsigned short)stream_get() * 256;
	      unsigned int to_skip_ = 4 * (unsigned int)to_skip;

	      //fprintf(stderr, "skipping %i bytes\n", (int)to_skip_);

	      for( unsigned int i=0; i<to_skip_; i++)
		stream_get();

	    }
	break;
	}
      case LOG_OFST_TYPE:
	t.implementation.log_ofst =
	  this->implementation.log_ofst->read_transducer();
	if (hfst_version_2_weighted_transducer) // this should not happen
	  { 
	    fprintf(stderr, "ERROR: not transducer stream\n");
	    exit(1);
	  }
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	t.implementation.foma =
	  this->implementation.foma->read_transducer();
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	t.implementation.mfstl =
	  this->implementation.mfstl->read_transducer();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	t.implementation.hfst_ol =
	  this->implementation.hfst_ol->read_transducer(false);  // FIX: has_header
	if(t.get_type() != type) // weights need to be added or removed
	{ t.convert(type); }
	break;
      case ERROR_TYPE:
	//case UNSPECIFIED_TYPE:
      default:
	debug_error("#1");
	throw hfst::exceptions::NotTransducerStreamException();
	break;
  }
  
  t.set_name(name);
  
}

  HfstInputStream::TransducerType HfstInputStream::guess_fst_type(int &bytes_read)
  {
    bytes_read=0;

    char c = stream_peek();

    switch(c)
      {
      case (char)0xd6: // OpenFst
	{
	  char chars_read[26];
	  for (unsigned int i=0; i<26; i++) {
	    chars_read[i]=(char)stream_get();
	    if ( stream_eof() )
	      throw hfst::exceptions::NotTransducerStreamException();
	  }
	  for (int i=25; i>=0; i--)
	    stream_unget(chars_read[i]);

	  if (chars_read[18]=='s') // standard
	    return OPENFST_; //OPENFST_TROPICAL_;
	  else if (chars_read[18]=='l') // log
	    return OPENFST_; //OPENFST_LOG_;
	  else
	    throw hfst::exceptions::NotTransducerStreamException();
	  //return OPENFST_;
	  break;
	}
      case '#':  // foma
	return FOMA_;
	break;
      case 'a':  // SFST
	return SFST_;
	break;
#ifdef HAVE_MFSTL
	  case 'f':  // Mfstl (replace 'f' with the first char in a binary MyFst)
	    return MFSTL_;
	    break;
#endif
      case 'P':
	{
	  // extract HFST version 2 header
	  (void)stream_get();
	  int i1 = (int)stream_get();
	  int i2 = (int)stream_get();
	  unsigned int prop_length= i1*256 + i2;
	  bytes_read=2;
	  if (prop_length != 2) {
	    debug_error("#2");
	    throw hfst::exceptions::NotTransducerStreamException();
	  }
	  char c2=stream_get();
	  bytes_read++;
	  if (c2 == 'A')
	    return HFST_VERSION_2_UNWEIGHTED_WITHOUT_ALPHABET;
	  if (c2 == 'a') {
	    stream_unget(c2);
	    bytes_read--;
	    return HFST_VERSION_2_UNWEIGHTED;
	  }
	  else {
	    	debug_error("#3");
	    throw hfst::exceptions::NotTransducerStreamException();
	  }
	  break;
	}
      case 'A':
	{
	  (void)stream_get();
	  bytes_read=1;
	  char c2 = stream_peek();
	  if (c2 == 'a')
	    return HFST_VERSION_2_UNWEIGHTED_WITHOUT_ALPHABET;
	  if (c2 == (char)0xd6)
	    return HFST_VERSION_2_WEIGHTED;
	  else
	    return ERROR_TYPE_;
	  break;
	}
      default:
	return ERROR_TYPE_;
      }
    return ERROR_TYPE_;
  }

  void HfstInputStream::process_header_data(StringPairVector &header_data, bool warnings)
  {
    if (header_data.size() < 2)
      throw hfst::exceptions::TransducerHeaderException();

    // (1) first pair "version", "3.0"
    if ( not ( ( strcmp("version", header_data[0].first.c_str()) == 0 ) &&
	       ( strcmp("3.0", header_data[0].second.c_str()) == 0 ) ) )
      throw hfst::exceptions::TransducerHeaderException();

    // (2) second pair "type", (valid type field)
    if ( not ( strcmp("type", header_data[1].first.c_str()) == 0 ) )
      throw hfst::exceptions::TransducerHeaderException();

    if (strcmp("SFST", header_data[1].second.c_str()) == 0 )
      type = SFST_TYPE;
    else if (strcmp("FOMA", header_data[1].second.c_str()) == 0 )
      type = FOMA_TYPE;
    else if (strcmp("TROPICAL_OPENFST", header_data[1].second.c_str()) == 0 )
      type = TROPICAL_OFST_TYPE;
    else if (strcmp("LOG_OPENFST", header_data[1].second.c_str()) == 0 )
      type = LOG_OFST_TYPE;
#if HAVE_MFSTL
    else if (strcmp("MFSTL", header_data[1].second.c_str()) == 0 )
      type = MFSTL_TYPE;
#endif
    else if (strcmp("HFST_OL", header_data[1].second.c_str()) == 0 )
      type = HFST_OL_TYPE;
    else if (strcmp("HFST_OLW", header_data[1].second.c_str()) == 0 )
      type = HFST_OLW_TYPE;
    else
      throw hfst::exceptions::TransducerHeaderException();

    if (header_data.size() == 2)
      return;

    // (3) third optional pair "name", (string)
    if (header_data[2].first.compare("name") == 0) {
      name = header_data[2].second;
    }
    // (3) an optional pair "minimal", ("true"|"false") if type == SFST_TYPE
    // FIX: forward this information to SfstInputStream's transducer
    else {
      if (not (set_implementation_specific_header_data(header_data, 2)) && warnings)
	fprintf(stderr, "Warning: Transducer header has extra data that cannot be used.\n");
    }

    if (header_data.size() == 3)
      return;
    
    if (not (set_implementation_specific_header_data(header_data, 3)) && warnings)
      fprintf(stderr, "Warning: Transducer header has extra data that cannot be used.\n");
  }


  /* Try to read a hfst header. If successful, return true and the number of bytes read.
     If not, return false and 0. Throw a NotTransducerStreamException if the header cannot
     be parsed after a field "HFST3" or "HFST". Throw a TransducerHeaderException if 
     the header data cannot be parsed. */
  bool HfstInputStream::read_hfst_header(int &bytes_read)
  {
    char c =stream_peek();

    if (c != 'H') {
      bytes_read=0;
      return false;
    }
    int header_bytes=0;
    // try to read an HFST version 3.0 header
    if (read_library_header(header_bytes)) 
      {
      int size_bytes=0;
      int header_size = get_header_size(size_bytes); // throws error
      StringPairVector header_info = 
	get_header_data(header_size);
      process_header_data(header_info, false);           // throws error

      bytes_read = header_bytes + size_bytes + header_size;
      return true;
      }
    header_bytes=0;
    // try to read a pre-release HFST version 3.0 header
    if (read_library_header_old(header_bytes)) 
      {
      int type_bytes=0;
      type = get_fst_type_old(type_bytes); // throws error
      if (type == ERROR_TYPE) {
	throw hfst::exceptions::NotTransducerStreamException();
      }
      bytes_read = header_bytes + type_bytes;

      return true;
      }
    return false;
  }

  ImplementationType HfstInputStream::get_fst_type_old(int &bytes_read)
  {
    std::string fst_type = stream_getstring();
    if (stream_eof()) {
      	debug_error("#5");
      throw hfst::exceptions::NotTransducerStreamException();
    }
    if (fst_type.compare("SFST_TYPE") == 0)
      { bytes_read=10; return SFST_TYPE; }
    if (fst_type.compare("FOMA_TYPE") == 0)
      { bytes_read=10; return FOMA_TYPE; }
    if (fst_type.compare("TROPICAL_OFST_TYPE") == 0)
      { bytes_read=19; return TROPICAL_OFST_TYPE; }
    if (fst_type.compare("LOG_OFST_TYPE") == 0)
      { bytes_read=14; return LOG_OFST_TYPE; }
    if (fst_type.compare("HFST_OL_TYPE") == 0)
      { bytes_read=13; return HFST_OL_TYPE; }
    if (fst_type.compare("HFST_OLW_TYPE") == 0)
      { bytes_read=14; return HFST_OLW_TYPE; }
    return ERROR_TYPE;
  }

  bool HfstInputStream::read_library_header_old(int &bytes_read) 
  {
    const char *id = "HFST3";
    
    for (int i=0; i<6; i++) {
      char c = stream_get();
      if(c != id[i]) { /* No match */
        stream_unget(c);
        if(i > 0) {
          for(int j=i-1; j>=0; j--) {
            stream_unget(id[j]); 
	  }
        }
	bytes_read=0;
        return false;
      }
    }
    bytes_read=6;
    return true;
  }

  bool HfstInputStream::read_library_header(int &bytes_read)
  {
    const char *id = "HFST";
    
    for (int i=0; i<5; i++) {
      char c = stream_get();
      if(c != id[i]) { /* No match */
	//fprintf(stderr, "No match: %c != %c\n", c, id[i]);
        stream_unget(c);
        if(i > 0) {
          for(int j=i-1; j>=0; j--) {
	    stream_unget(id[j]);
          }
        }
	bytes_read=0;
        return false;
      }
    }
    bytes_read=5;
    return true;
  }

  int HfstInputStream::get_header_size(int &bytes_read)
  {
    int header_size=0;
    int c1 = stream_get();
    int c2 = stream_get();
    int c = stream_get();
    if (c != 0) {
      debug_error("#6");
      throw hfst::exceptions::NotTransducerStreamException();
    }
    header_size = (c1 * 256) + c2;
    bytes_read=3;

    return header_size;
  }

  StringPairVector HfstInputStream::get_header_data(int header_size)
  {
    StringPairVector retval;
    int bytes_read=0;

    while(true) 
      {
	std::string str1 = stream_getstring();
	std::string str2 = stream_getstring();

	bytes_read = bytes_read + (int)str1.length() + (int)str2.length() + 2; 

	if (bytes_read > header_size) {
	  debug_error("#7");
	  fprintf(stderr, "%i > %i\n", bytes_read, header_size);
	  throw hfst::exceptions::NotTransducerStreamException();
	}
	if (stream_eof()) {
	  debug_error("#8");
	  throw hfst::exceptions::NotTransducerStreamException();
	}

	retval.push_back(std::pair<std::string, std::string>
			 (str1, str2) );
	if (bytes_read == header_size)
	  break;
      }
    return retval;
  }
  

  /* The implementation type of the first transducer in the stream. */
  ImplementationType HfstInputStream::stream_fst_type()
  { 
    int bytes_read=0;

    // whether the stream contains an HFST version 3.0 transducer
    if (read_hfst_header(bytes_read)) {
      bytes_to_skip=bytes_read;
      return type;
    }

    // whether the stream contains an HFST version <3.0 transducer
    // or an SFST, OpenFst or a foma transducer
    TransducerType transducer_type = guess_fst_type(bytes_read);
    bytes_to_skip=bytes_read;

    switch (transducer_type)
      {
      case HFST_VERSION_2_WEIGHTED:
	hfst_version_2_weighted_transducer=true;
	return TROPICAL_OFST_TYPE;
	break;
      case HFST_VERSION_2_UNWEIGHTED_WITHOUT_ALPHABET:
	fprintf(stderr, "ERROR: version 2 HFST transducer with no alphabet cannot be processed\n"
		"Add an alphabet with HFST version 2 tool hfst-symbols\n" );
	return ERROR_TYPE;
	break;
      case HFST_VERSION_2_UNWEIGHTED:
	return SFST_TYPE;
	break;
      case OPENFST_:
	return TROPICAL_OFST_TYPE; // FIX: can be log or something else
	break;
      case SFST_:
	return SFST_TYPE;
	break;
      case FOMA_:
	return FOMA_TYPE;
	break;
#if HAVE_MFSTL
      case MFSTL_:
	return MFSTL_TYPE;
	break;
#endif
      case ERROR_TYPE_:
      default:
	return ERROR_TYPE;
      }
  }

  /* Open a transducer stream to stdout.
     The implementation type of the stream is defined by 
     the type of the first transducer in the stream. */
  HfstInputStream::HfstInputStream(void):
    bytes_to_skip(0), filename(std::string()), hfst_version_2_weighted_transducer(false)
  {
    try { 
      input_stream = &std::cin;
      type = stream_fst_type();
    }
    catch (hfst::implementations::FileNotReadableException e)
      { throw e; }
    if ( not HfstTransducer::is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (type)
    {
#if HAVE_SFST
    case SFST_TYPE:
      implementation.sfst = new hfst::implementations::SfstInputStream;
      break;
#endif
#if HAVE_OPENFST
    case TROPICAL_OFST_TYPE:
      implementation.tropical_ofst = 
	new hfst::implementations::TropicalWeightInputStream;
      break;
    case LOG_OFST_TYPE:
      implementation.log_ofst = 
	new hfst::implementations::LogWeightInputStream;
      break;
#endif
#if HAVE_FOMA
    case FOMA_TYPE:
      implementation.foma = new hfst::implementations::FomaInputStream;
      break;
#endif
#if HAVE_MFSTL
    case MFSTL_TYPE:
      implementation.mfstl = new hfst::implementations::MfstlInputStream;
      break;
#endif
    case HFST_OL_TYPE:
      implementation.hfst_ol =
        new hfst::implementations::HfstOlInputStream(false);
      break;
    case HFST_OLW_TYPE:
      implementation.hfst_ol = 
        new hfst::implementations::HfstOlInputStream(true);
      break;
    default:
      debug_error("#9");
      throw hfst::exceptions::NotTransducerStreamException();
    }
  }

  // FIX: HfstOutputStream takes a string parameter, HfstInputStream a const char*
  HfstInputStream::HfstInputStream(const char* filename):
    bytes_to_skip(0), filename(std::string(filename)), hfst_version_2_weighted_transducer(false)
  {
    try { 
      if (strcmp("",filename) != 0) {
	std::ifstream ifs(filename);
	input_stream = &ifs;
	type = stream_fst_type();
      }
      else {
	input_stream = &std::cin;
	type = stream_fst_type();
      }
    }
    catch (hfst::implementations::FileNotReadableException e)
      { throw e; }
    if ( not HfstTransducer::is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (type)
    {
#if HAVE_SFST
    case SFST_TYPE:
      implementation.sfst = new hfst::implementations::SfstInputStream(filename);
      break;
#endif
#if HAVE_OPENFST
    case TROPICAL_OFST_TYPE:
      if (strcmp(filename,"") == 0)  // FIX: this should be done in TropicalWeight layer
	implementation.tropical_ofst = 
	  new hfst::implementations::TropicalWeightInputStream();
      else
	implementation.tropical_ofst = 
	  new hfst::implementations::TropicalWeightInputStream(filename);
      break;
    case LOG_OFST_TYPE:
      implementation.log_ofst = 
	new hfst::implementations::LogWeightInputStream(filename);
      break;
#endif
#if HAVE_FOMA
    case FOMA_TYPE:
      implementation.foma = new hfst::implementations::FomaInputStream(filename);
      break;
#endif
#if HAVE_MFSTL
    case MFSTL_TYPE:
      implementation.mfstl = new hfst::implementations::MfstlInputStream(filename);
      break;
#endif
    case HFST_OL_TYPE:
      implementation.hfst_ol = new hfst::implementations::HfstOlInputStream(filename, false);
      break;
    case HFST_OLW_TYPE:
      implementation.hfst_ol = new hfst::implementations::HfstOlInputStream(filename, true);
      break;
    default:
      debug_error("#10");
      throw hfst::implementations::NotTransducerStreamException();
    }
  }

  HfstInputStream::~HfstInputStream(void)
  { 
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	delete implementation.sfst;
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	delete implementation.tropical_ofst;
	break;
      case LOG_OFST_TYPE:
	delete implementation.log_ofst;
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	delete implementation.foma;
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	delete implementation.mfstl;
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	delete implementation.hfst_ol;
	break;
      case ERROR_TYPE:
	//case UNSPECIFIED_TYPE:
      default:
	debug_error("#11");
	throw hfst::exceptions::NotTransducerStreamException();
      }
  }

  void HfstInputStream::close(void)
  {
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst->close();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst->close();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst->close();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma->close();
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	implementation.mfstl->close();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol->close();
	break;
      default:
	assert(false);
      }
  }

  bool HfstInputStream::is_eof(void)
  {
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	return implementation.sfst->is_eof();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	return implementation.tropical_ofst->is_eof();
	break;
      case LOG_OFST_TYPE:
	return implementation.log_ofst->is_eof();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	return implementation.foma->is_eof();
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	return implementation.mfstl->is_eof();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	return implementation.hfst_ol->is_eof();
      default:
	assert(false);
	return false;
      }
  }

  bool HfstInputStream::is_bad(void)
  {
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	return implementation.sfst->is_bad();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	return implementation.tropical_ofst->is_bad();
	break;
      case LOG_OFST_TYPE:
	return implementation.log_ofst->is_bad();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	return implementation.foma->is_bad();
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	return implementation.mfstl->is_bad();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	return implementation.hfst_ol->is_bad();
	break;
      default:
	assert(false);
	return false;
      }
  }

  bool HfstInputStream::is_good(void)
  {
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	return implementation.sfst->is_good();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	return implementation.tropical_ofst->is_good();
	break;
      case LOG_OFST_TYPE:
	return implementation.log_ofst->is_good();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	return implementation.foma->is_good();
	break;
#endif
#if HAVE_MFSTL
      case MFSTL_TYPE:
	return implementation.mfstl->is_good();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	return implementation.hfst_ol->is_good();
	break;
      default:
	assert(false);
	return false;
      }
  }
  
  ImplementationType HfstInputStream::get_type(void) const
  {
    return type;
  }
}

#ifdef DEBUG_MAIN_STREAM
int main(void)
{
  hfst::HfstInputStream sfst_stdin_input(hfst::SFST_TYPE);
  sfst_stdin_input.open();
  sfst_stdin_input.close();
  hfst::HfstInputStream ofst_stdin_input(hfst::TROPICAL_OFST_TYPE);
  ofst_stdin_input.open();
  ofst_stdin_input.close();
  // This succeeds only if the files stream_test and
  // stream_test_ne exist
  hfst::HfstInputStream sfst_file_input("stream_test",hfst::SFST_TYPE);
  sfst_file_input.open();
  sfst_file_input.close();
  assert(sfst_file_input.is_eof());
  hfst::HfstInputStream sfst_ne_file_input("stream_test_ne",hfst::SFST_TYPE);
  sfst_ne_file_input.open();
  assert(not sfst_ne_file_input.is_eof());
  sfst_ne_file_input.close();
  hfst::HfstInputStream ofst_file_input("stream_test",hfst::TROPICAL_OFST_TYPE);
  ofst_file_input.open();
  ofst_file_input.close();
  assert(ofst_file_input.is_eof());
  hfst::HfstInputStream ofst_ne_file_input("stream_test_ne",hfst::TROPICAL_OFST_TYPE);
  ofst_ne_file_input.open();
  assert(not ofst_ne_file_input.is_eof());
  ofst_ne_file_input.close();
}
#endif
