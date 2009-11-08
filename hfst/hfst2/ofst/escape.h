#include <stdio.h>
#include <fstream>
using std::ofstream;
#include <string>
using namespace std;

namespace COMMON {

  void escape_and_add( const char *s, std::string& escaped,
		       bool space, bool colon, bool brackets);
  void unescape_and_add( char *s, std::string& unescaped);

  void escape_and_print( const char *s, FILE *outfile,
			 bool space, bool colon, bool brackets);
  void escape_and_print( const char *s, ostream& os,
			 bool space, bool colon, bool brackets);

}
