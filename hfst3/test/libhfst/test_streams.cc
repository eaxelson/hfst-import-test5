/*
   Test file for HfstTransducer constructors, destructor, operator=
   and member functions set_name, get_name and get_type.
*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;

int main(int argc, char **argv) 
{

  const unsigned int TYPES_SIZE=4;
  const ImplementationType types [] = {SFST_TYPE, 
				       TROPICAL_OFST_TYPE, 
				       LOG_OFST_TYPE, 
				       FOMA_TYPE};

  for (unsigned int i=0; i<TYPES_SIZE; i++)
    {

      /* From AT&T format. */
      verbose_print("Construction from AT&T format", types[i]);

      unsigned int transducers_read=0;
      FILE * file = fopen("test_transducers.att", "rb");
      try 
	{
	  while (not feof(file))
	    {
	      HfstTransducer t(file, types[i], "<eps>");
	      transducers_read++;
	    }
	}
      catch (hfst::exceptions::NotValidAttFormatException e)
	{
	  assert(transducers_read == 4);
	}


      /* From HfstInputStream. 
	 Tests also functions get_type, set_name and get_name. */
      /*verbose_print("Construction from HfstInputStream", types[i]);
      HfstOutputStream out("testfile.hfst", foobar.get_type());
      foobar.set_name("foobar");
      out << foobar;
      out.close();
      HfstInputStream in("testfile.hfst");
      HfstTransducer foobar_stream(in);
      in.close();
      assert(foobar.compare(foobar_stream));
      assert(foobar_stream.get_name().compare("foobar") == 0);
      assert(foobar_stream.get_type() == types[i]);*/

    }

}
