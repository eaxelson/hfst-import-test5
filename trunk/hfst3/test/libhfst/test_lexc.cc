#include "HfstTransducer.h"

int main(int argc, char **argv) 
{
  using namespace hfst;
  using namespace hfst::lexc;

  FILE * testfile = fopen("test_lexc.lexc", "wb");
  fprintf(testfile, "LEXICON Root\n\n"
	  "A;\n"
	  "B;\n\n"
	  "LEXICON A\n\n"
	  "cat #;\n"
	  "dog #;\n\n"
	  "LEXICON B\n\n"
	  "mouse #;\n"
	  );
  fclose(testfile);

  const unsigned int TYPES_SIZE=3;//4;
  const ImplementationType types [] = {SFST_TYPE, 
				       TROPICAL_OPENFST_TYPE, 
				       /*LOG_OPENFST_TYPE,*/ 
				       FOMA_TYPE};

  /* For all transducer implementation types, perform the following tests: */
  for (unsigned int i=0; i<TYPES_SIZE; i++)
    {
      LexcCompiler compiler(types[i]);
      compiler.parse("test_lexc.lexc");
      HfstTransducer * parsed = compiler.compileLexical();

      HfstTokenizer tok;
      HfstTransducer cat("cat", tok, types[i]);
      HfstTransducer dog("dog", tok, types[i]);
      HfstTransducer mouse("mouse", tok, types[i]);

      HfstTransducer animals(types[i]);
      animals.disjunct(cat);
      animals.disjunct(dog);
      animals.disjunct(mouse);

      assert(animals.compare(*parsed));
      delete parsed;
    }

  remove("test_lexc.lexc");

}
