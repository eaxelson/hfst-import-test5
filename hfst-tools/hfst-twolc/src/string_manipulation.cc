#include "string_manipulation.h"

char * new_string(size_t lgth)
{
  char * str = (char*)(malloc((lgth+1)*sizeof(char)));
  for (size_t index = 0;
       index < lgth+1;
       ++index)
    {
      str[index] = 0;
    }
  return str;
}

char * string_copy(const char * str)
{
    char * new_str = new_string(strlen(str));

    for (size_t string_index = 0;
	 string_index < strlen(str);
	 ++string_index)
      {
	new_str[string_index] = str[string_index]; 
      }
    return new_str;
}

int strcmp_skip_0(const char * str1, 
		  const char * str2)
{
  assert(str1 != NULL); 
  assert(str2 != NULL); 
  for ( ;(*str1 != 0) and (*str2 != 0); ++str1, ++str2)
    {
      while (*str1 == '%') { ++str1; }
      while (*str2 == '%') { ++str2; }
      if (*str1 != *str2)
	{
	  return (int)(*str1) - (int)(*str2);
	}
    }
  return (int)(*str1) - (int)(*str2);
}


bool empty_string(const char * str)
{
  return strlen(str) == 0;
}

char * unescape(const char * text)
{

  assert(text != NULL);

  char * unescaped = new_string(strlen(text));
  const char * next_escape;

  while((next_escape = strchr(text,ESCAPE_CHAR)) != NULL)
    {
      strncat(unescaped, text, next_escape - text);
      text = next_escape + 1;
      strncat(unescaped,text,1);
      if (empty_string(text))
	{
	  break;
	}
      else 
	{
	  ++text;
	}
    }

  return strcat(unescaped,text);
}

char * remove_white_space(const char * str)
{
  while ((*str == ' ') or
	 (*str == '\t'))
    {
      ++str;
    }
  char * new_str = string_copy(str); 
  for (size_t i = strlen(new_str) - 1;
       i >= 0;
       --i)
    {
      if ((new_str[i] != ' ') and
	  (new_str[i] != '\t'))
	{
	  break;
	}
      if (i > 0)
	{
	  if (new_str[i-1] == ESCAPE_CHAR)
	    {
	      break;
	    }
	}
      new_str[i] = 0;
    }
  return new_str;
}

char * unescape_and_remove_white_space(const char * str)
{
  char * str_without_space = remove_white_space(str);
  char * unescaped = unescape(str_without_space);
  free(str_without_space);
  return unescaped;
}

char * unquote( const char * text ) 
{
  assert(text != NULL);
  while(('\t' == *text) or (' ' == *text))
    {
      ++text;
    }
  assert(text[0] == '\"');
  assert(text[strlen(text)-1] == '\"');

  // Skip first quote.
  ++text;
  
  // Reserve new char * for unquoted string.
  // The new string is long enough.
  char * unquoted = new_string(strlen(text));
  return strncpy(unquoted, text, strlen(text)-1);
}

int str2int(const char * string) 
{
  
  bool negative = false;
  
  if ( *string == '-' ) {
    negative = true;
    ++string;
  }
  
  int number = atoi( string );
  
  if ( negative )
    number =  -1 * number;
  
  return number;
  
}

void print_kill_symbol(void)
{
  std::cout << std::endl
	    << "__HFST_TWOLC_DIE" << std::endl;
}
