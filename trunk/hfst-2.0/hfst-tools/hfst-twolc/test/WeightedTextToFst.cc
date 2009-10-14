#include <hfst2/hfst.h>
#include <iostream>
#include <cstdlib>

int main(int argc,char * argv[])
{
  if (argc != 2)
    {
      std::cerr << "Error: "
		<< argv[0] << " takes exactly one argument (a text fst file)!" 
		<< std::endl << std::endl;
      exit(1);
    }
  char * text_file_name = argv[1];
  FILE * text_file = fopen(text_file_name,"r");
  if (text_file == NULL)
    {
      std::cerr << "Error: "
	<< "The input file can't be opened!" 
	<< std::endl << std::endl;
      exit(1);
    }
  fclose(text_file);
  try
    {
      HWFST::KeyTable * kt = HWFST::create_key_table();
      Symbol eps = HWFST::define_symbol("__HFST_TWOLC_EPSILON_SYMBOL");
      HWFST::associate_key(HWFST::Epsilon,
			   kt,
			   eps);
      ifstream input_stream(text_file_name,std::ios::in);
      HWFST::TransducerHandle t = 
	HWFST::read_transducer_text(input_stream,
				    kt);
      ofstream output_stream("text_file.out",std::ios::out);
      HWFST::write_transducer(t,kt,output_stream);
    }
  catch (const char * err)
    {
      std::cerr << err << std::endl << std::endl;
      exit(1);
    }
}
