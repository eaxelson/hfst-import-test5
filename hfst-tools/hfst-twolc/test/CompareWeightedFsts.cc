#include <iostream>
#include <hfst2/hfst.h>
int main(void)
{
  try
    {
      ifstream in1("text_file.out",std::ios::in); 
      ifstream in2("test_grammar.out",std::ios::in); 
      int format1 = HWFST::read_format(in1);
      int format2 = HWFST::read_format(in2);
      switch (format1)
	{
	case 1:
	  break;
	case 0:
	  std::cerr << "Error: text_file.out is unweighted!" << std::endl 
		    << std::endl;
	  break;
	case 2:
	  std::cerr << "Error: text_file.out is compact!" << std::endl 
		    << std::endl;
	  break;
	case -1:
	  std::cerr << "Error: text_file.out is empty!" << std::endl 
		    << std::endl;
	  break;
      	case -2:
	  std::cerr << "Error: text_file.out has unknown format!" << std::endl 
		    << std::endl;
	  break;
	}
      if (format1 != 1)
	{
	  exit(1);
	}
      HWFST::KeyTable * kt1 = HWFST::create_key_table();
      HWFST::TransducerHandle t1 = HWFST::read_transducer(in1,kt1);
      
      switch (format2)
	{
	case 1:
	  break;
	case 0:
	  std::cerr << "Error: test_grammar.out is unweighted!" << std::endl 
		    << std::endl;
	  break;
	case 2:
	  std::cerr << "Error: test_grammar.out is compact!" << std::endl 
		    << std::endl;
	  break;
	case -1:
	  std::cerr << "Error: test_grammar.out is empty!" << std::endl 
		    << std::endl;
	  break;
	case -2:
	  std::cerr << "Error: test_grammar.out has unknown format!" << std::endl 
		    << std::endl;
	  break;
	}
      if (format2 != 1)
	{
	  exit(1);
	}
      HWFST::KeyTable * kt2 = HWFST::create_key_table();
      HWFST::TransducerHandle t2 = HWFST::read_transducer(in2,kt2);
      t1 = HWFST::harmonize_transducer(t1,kt1,kt2);      
      if (HWFST::are_equivalent(t1,t2))
	{
	  exit(0);
	}
      else
	{
	  std::cout << "not equivalent" << std::endl;
	  exit(1);
	}
    }
  catch (const char * err)
    {
      std::cerr << err << std::endl << std::endl;
      exit(1);
    }
}
