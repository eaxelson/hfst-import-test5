#include <iostream>
#include <cstdlib>
int main(void)
{
  int exit_code =
    system("cat test_grammar54 | "
	   "../src/htwolcpre  -r -o test_grammar.out -s |"
	   "../src/htwolcpre2 -r -o test_grammar.out -s | "
	   "../src/htwolcpre3 -r -o test_grammar.out -s ");
  if (exit_code != 0)
    {
      exit(exit_code);
    }
  exit_code =
    system("./TextToFst test_grammar54.txt_fst");
  if (exit_code != 0)
    {
      exit(exit_code);
    }
  exit_code = 
    system("./CompareFsts");
  if (exit_code != 0)
    {
      exit(1);
    }
  exit_code = 
    system("rm -f test_grammar.out")
    or
    system("rm -f text_file.out");
  exit(exit_code);
}
