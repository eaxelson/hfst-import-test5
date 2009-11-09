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
      HFST::KeyTable * kt = HFST::create_key_table();
      Symbol eps = HFST::define_symbol("@0@");
      HFST::associate_key(HFST::Epsilon,
			  kt,
			  eps);
      ifstream input_stream(text_file_name,std::ios::in);
      HFST::TransducerHandle t = 
	HFST::read_transducer_text(input_stream,
				   kt,
				   false);
      ofstream output_stream("text_file.out",std::ios::out);
      HFST::write_transducer(t,kt,output_stream);
    }
  catch (const char * err)
    {
      std::cout << err << std::endl << std::endl;
      exit(1);
    }
}
