#include <cstdlib>
#include "escape.h"


namespace COMMON {


  /* Escape 's', if needed, and append it to 'escaped'. Characters
     '\n', '\t', '\\', '\r', '\v', '\f', '\a' and '\b' are escaped
     with strings "\\n", "\\t", "\\\\", "\\r", "\\v", "\\f", "\\a"
     and "\\b". If 'space' is true ' ' is escaped with "\\x20". ("\ "?)
     If 'colon' is true ':' is escaped with "\\:". If 'brackets'
     is true, '<' and '>' are escaped with "\\<" and "\\>". */
     
  void escape_and_add( const char *s, std::string& escaped,
		       bool space, bool colon, bool brackets) {

    for (int i=0; s[i] != '\0'; i++) {
      bool was_escaped=false;
      const char *esc=NULL;
      switch(s[i]) {
      case '\n': esc = "\\n"; was_escaped=true; break;
      case '\t': esc = "\\t"; was_escaped=true; break;
      case '\\': esc = "\\\\"; was_escaped=true; break;
      case '\r': esc = "\\r"; was_escaped=true; break;
      case '\v': esc = "\\v"; was_escaped=true; break;
      case '\f': esc = "\\f"; was_escaped=true; break;
      case '\a': esc = "\\a"; was_escaped=true; break;
      case '\b': esc = "\\b"; was_escaped=true; break;
      }
      if ( space && (s[i] == ' ' ))
	esc = "\\x20"; was_escaped=true;  // esc = "\\ ";
      if ( colon && (s[i] == ':' ))
	esc = "\\:"; was_escaped=true;
      if ( brackets ) {
	if( s[i] == '<' )
	  esc = "\\<"; was_escaped=true;
	if ( s[i] == '>' )
	  esc = "\\>"; was_escaped=true;
      }
      if(was_escaped)
	escaped + esc;
      else
	escaped + s[i];
    }
  }


  void escape_and_print( const char *s, FILE *outfile,
			 bool space, bool colon, bool brackets) {

    for (int i=0; s[i]!='\0'; i++) {
      if(s[i]=='\n') fputs("\\n", outfile);
      else if(s[i]=='\t') fputs("\\t", outfile);
      else if(s[i]=='\\') fputs("\\\\", outfile);
      else if(s[i]=='\r') fputs("\\r", outfile);
      else if(s[i]=='\v') fputs("\\v", outfile);
      else if(s[i]=='\f') fputs("\\f", outfile);
      else if(s[i]=='\a') fputs("\\a", outfile);
      else if(s[i]=='\b') fputs("\\b", outfile);

      else {
	if ( space && (s[i] == ' ' ))
	  fputs("\\x20", outfile);  // fputs("\\ ", outfile); 
	else if ( colon && (s[i] == ':' ))
	  fputs("\\:", outfile); 
	else if ( brackets ) {
	  if( s[i] == '<' )
	    fputs("\\<", outfile); 
	  else if ( s[i] == '>' )
	    fputs("\\>", outfile);
	}
	else
	  fputc(s[i], outfile);
      }
    }
  }

  void escape_and_print( const char *s, ostream &os,
			 bool space, bool colon, bool brackets) {
    //fprintf(stderr, "(escape_and_print: input argument is \"%s\")", s);

    for (int i=0; s[i]!='\0'; i++) {
      if(s[i]=='\n') os << "\\n";
      else if(s[i]=='\t') os << "\\t";
      else if(s[i]=='\\') os << "\\\\";
      else if(s[i]=='\r') os << "\\r";
      else if(s[i]=='\v') os << "\\v";
      else if(s[i]=='\f') os << "\\f";
      else if(s[i]=='\a') os << "\\a";
      else if(s[i]=='\b') os << "\\b";

      else {
	if ( space && (s[i] == ' ' ))
	  os << "\\x20";  //  os << "\\ "; 
	else if ( colon && (s[i] == ':' ))
	  os << "\\:"; 
	else if ( brackets ) {
	  if( s[i] == '<' )
	    os << "\\<"; 
	  else if ( s[i] == '>' )
	    os << "\\>";
	}
	else
	  os << s[i];
      }
    }
  }


  /* Unescape 's', if needed, and append it to 'unescaped'. Strings
     "\\n", "\\t", "\\\\", "\\r", "\\v", "\\f", "\\a" and "\\b" are
     escaped with characters '\n', '\t', '\\', '\r', '\v', '\f',
     '\a' and '\b'. */

  void unescape_and_add( char *s, std::string& unescaped) {

    for (int i=0; s[i] != '\0'; i++) {
      if (s[i] == '\\') {
	i++;
	char esc;
	char next=s[i];

	if (next=='n') esc = '\n';
	else if (next=='t') esc = '\t'; 
	else if (next=='\\') esc = '\\'; 	
	else if (next=='r') esc = '\r'; 
	else if (next=='v') esc = '\v'; 
	else if (next=='f') esc = '\f'; 
	else if (next=='a') esc = '\a'; 
	else if (next=='b') esc = '\b'; 
	else if (next==':') esc = ':'; 
	else if (next=='<') esc = '<'; 
	else if (next=='>') esc = '>'; 
	// else if (next==' ') esc = ' '; 
	else if (next=='x') { 
	  if( s[i+1] == '2' && s[i+2] == '0' ) {
	    esc = ' ';
	    i = i+2;
	  }
	  else {
	    fprintf(stderr, "\nERROR: unknown escape sequence in string '%s'.\n", s);
	    exit(1); 
	  }
	}
	else {
	  fprintf(stderr, "\nERROR: unknown escape sequence in string '%s'.\n", s);
	  exit(1); 
	}
	unescaped.push_back(esc);
      }

      else {
	unescaped.push_back(s[i]);
      }
    }
  }


}



  /* Extra functions that might be useful in the future.

  void unescape_hex( char *s_escaped, std::string& s ) {

    for ( int i=0; s_escaped[i] != '\0'; i++ ) {
      if ( s_escaped[i] == '\\' &&
	   s_escaped[i+1] == 'u' ) {
	char number[5];
	for (int j=0; j<4; j++)
	  number[j] = s_escaped[i+1+j];
	number[4] = '\0';					
	unsigned long ul = std::strtoul(number, NULL, 16);
	s = s + (char)(unsigned short)ul;
	i = i+5;
      }
      else
	s = s + s_escaped[i];
    }        
  }


  void escape_hex( char *s, std::string& s_escaped ) {
    
    for (int i=0; s[i] != '\0'; i++) {
      if ((unsigned short)s[i] <= 32) {  // a character from 0 to 32
	// hexadecimal conversion
	unsigned short first_number = s[i]/16 + 48;   // \xX0, from 0 to 2
	unsigned short second_number = s[i]%16;  // \x0X, from 0 to F
	if (second_number < 10)   // from 0 to 9
	  second_number = second_number + 48; 
	else   // from A to F
	  second_number = second_number + 65; 
	s_escaped = s_escaped + '\\' + 'x' +
	  (char)first_number + (char)second_number;
	}
      else if(s[i] == '\\') { // the backslash
	s_escaped = s_escaped + '\\' + '\\';
      }
      else
	s_escaped = s_escaped + s[i];
    }
  }  

  */
