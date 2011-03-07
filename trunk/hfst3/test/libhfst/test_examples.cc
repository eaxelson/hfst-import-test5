/*
   Test file for ...
*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;

using hfst::implementations::HfstBasicTransition;
using hfst::implementations::HfstBasicTransducer;

int main(int argc, char **argv) 
{

  const unsigned int TYPES_SIZE=4;
  const ImplementationType types [] = {SFST_TYPE, 
				       TROPICAL_OPENFST_TYPE, 
				       LOG_OPENFST_TYPE, 
				       FOMA_TYPE};

  HfstBasicTransducer tr1;
  tr1.add_state(1);
  tr1.set_final_weight(1, 0);
  tr1.add_transition
    (0, HfstBasicTransition(1, "@_UNKNOWN_SYMBOL_@", "foo", 0) );
  // tr1 is [ @_UNKNOWN_SYMBOL_@:foo ]
  
  HfstBasicTransducer tr2;
  tr2.add_state(1);
  tr2.add_state(2);
  tr2.set_final_weight(2, 0);
  tr2.add_transition
    (0, HfstBasicTransition(1, "@_IDENTITY_SYMBOL_@", 
			    "@_IDENTITY_SYMBOL_@", 0) );
  tr2.add_transition
    (1, HfstBasicTransition(2, "bar", "bar", 0) );
  // tr2 is [ [ @_IDENTITY_SYMBOL_@:@_IDENTITY_SYMBOL_@ ] [ bar:bar ] ]

  // The disjunction should be
  HfstBasicTransducer disj;
  disj.add_state(1);  
  disj.add_state(2);  
  disj.set_final_weight(2, 0);

  disj.add_transition
    (0, HfstBasicTransition(1, "@_IDENTITY_SYMBOL_@", 
			    "@_IDENTITY_SYMBOL_@", 0) );
  disj.add_transition
    (0, HfstBasicTransition(1, "foo", "foo", 0) );

  disj.add_transition
    (0, HfstBasicTransition(2, "@_UNKNOWN_SYMBOL_@", "foo", 0) );
  disj.add_transition
    (0, HfstBasicTransition(2, "bar", "foo", 0) );

  disj.add_transition
    (1, HfstBasicTransition(2, "bar", "bar", 0) );


  FILE * ofile = fopen("testfile.att", "wb");
  fprintf(ofile, "0 1 a b 0.4\n");
  fprintf(ofile, "1 c d\n");
  fclose(ofile);


  // For all transducer implementation types, perform the following tests: */
  for (unsigned int i=0; i<TYPES_SIZE; i++)
    {
      verbose_print("expanding unknowns", types[i]);
      
      HfstTransducer Tr1(tr1, types[i]);
      HfstTransducer Tr2(tr2, types[i]);
      HfstTransducer Disj(disj, types[i]);

      Tr1.disjunct(Tr2).minimize();
      // Tr1 is expanded to [ @_UNKNOWN_SYMBOL_@:foo | bar:foo ]
      // Tr2 is expanded to 
      // [ [ @_IDENTITY_SYMBOL_@:@_IDENTITY_SYMBOL_@ | foo:foo ] [ bar:bar ] ]
 
      assert(Tr1.compare(Disj));


      verbose_print("testing NotValidAttFormatException", types[i]);

      FILE * ifile = fopen("testfile.att", "rb");
      try {
	HfstTransducer t(ifile, types[i], "@_EPSILON_SYMBOL_@");
	assert(false);
      }
      catch (NotValidAttFormatException e)
	{
	  assert(true);
	}
      fclose(ifile);

      

    }

}

