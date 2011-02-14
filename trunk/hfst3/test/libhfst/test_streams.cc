/*
   Test file for HfstInputStream, HfstOutputStream and functions that
   write or read AT&T format.
*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;

int main(int argc, char **argv) 
{

  const unsigned int TYPES_SIZE=4;
  const ImplementationType types [] = {SFST_TYPE, 
				       TROPICAL_OPENFST_TYPE, 
				       LOG_OPENFST_TYPE,
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

      /* To AT&T format. */
      verbose_print("Writing in AT&T format", types[i]);
      
      FILE * ofile = fopen("transducer2.att", "wb");

      HfstTransducer t1("foo", "bar", types[i]);
      HfstTransducer t2("baz", "@_EPSILON_SYMBOL_@", types[i]);
      t2.concatenate(t1);
      t2.minimize();
      t2.write_in_att_format(ofile, true);
      fclose(ofile);
      assert(system("diff transducer2.att transducer.att") == 0);
      remove("transducer2.att");


      /* From HfstInputStream. */      
      verbose_print("Writing to HfstOutputStream", types[i]);

      HfstTransducer tr1("foo", types[i]);
      HfstTransducer tr2("bar", "foo", types[i]);
      HfstTransducer tr3("a", types[i]);
      HfstTransducer tr4("b", "c", types[i]);

      HfstOutputStream out("testfile.hfst", types[i]);
      out << tr1;
      out << tr2;
      out << tr3;
      out << tr4;
      out.close();

      verbose_print("Construction from HfstInputStream", types[i]);

      HfstInputStream in("testfile.hfst");
      std::vector<HfstTransducer> transducers;
      transducers_read=0;
      while (not in.is_eof())
	{
	  HfstTransducer tr(in);   
	  transducers.push_back(tr);
	  transducers_read++;
	}
      in.close();


      assert(transducers_read == 4);

      assert(transducers[0].compare(tr1));
      assert(transducers[1].compare(tr2));
      assert(transducers[2].compare(tr3));
      assert(transducers[3].compare(tr4));

    }

}
