#include "string_manipulation_tests.h"

bool not_equal(const char * str1,
	       const char * str2)
{
  if (strcmp(str1,str2) != 0)
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool string_copy_test(void)
{
  const char * str = "";
  char * new_str = string_copy(str);
  if (not_equal(str,new_str))
    {
      return false;
    }
  free(new_str);

  str = "a";
  new_str = string_copy(str);
  if (not_equal(str,new_str))
    {
      return false;
    }
  free(new_str);
  return true;
}

bool unquote_test(void)
{
  const char * quoted = "\"ab\"";
  const char * unquoted = "ab";
  char * unquoted_quoted = unquote(quoted);
  if (not_equal(unquoted_quoted, unquoted))
    {
      return false;
    }
  free(unquoted_quoted);

  quoted = "\"\"";
  unquoted = "";
  unquoted_quoted = unquote(quoted);
  
  if (not_equal(unquoted_quoted, unquoted))
    {
      return false;
    }
  free(unquoted_quoted);

  quoted = "\"\%\"a\"";
  unquoted = "\%\"a";
  unquoted_quoted = unquote(quoted);

  if (not_equal(unquoted_quoted, unquoted))
    {
      return false;
    }
  free(unquoted_quoted);
  return true;
}

bool unescape_test(void)
{
  const char * escaped = "ab";
  const char * unescaped = "ab";
  char * unescaped_escaped = unescape(escaped);

  if (not_equal(unescaped_escaped, unescaped))
    {
      return false;
    }
  free(unescaped_escaped);

  escaped = "ab%ab";
  unescaped = "abab";
  unescaped_escaped = unescape(escaped);

  if (not_equal(unescaped_escaped, unescaped))
    {
      return false;
    }
  free(unescaped_escaped);

  escaped = "%";
  unescaped = "";
  unescaped_escaped = unescape(escaped);

  if (not_equal(unescaped_escaped, unescaped))
    {
      return false;
    }
  free(unescaped_escaped);

  escaped = "%%";
  unescaped = "%";
  unescaped_escaped = unescape(escaped);

  if (not_equal(unescaped_escaped, unescaped))
    {
      return false;
    }
  free(unescaped_escaped);

  escaped = "%a%";
  unescaped = "a";
  unescaped_escaped = unescape(escaped);

  if (not_equal(unescaped_escaped, unescaped))
    {
      return false;
    }
  free(unescaped_escaped);

  return true;
}

bool str2int_test(void)
{
  const char * str = "1";
  int val = 1;
  int computed_val = str2int(str);

  if (val != computed_val)
    {
      return false;
    }
  
  str = "-1";
  val = -1;
  computed_val = str2int(str);

  if (val != computed_val)
    {
      return false;
    }

  str = "0";
  val = 0;
  computed_val = str2int(str);

  if (val != computed_val)
    {
      return false;
    }

  str = "-0";
  val = -0;
  computed_val = str2int(str);

  if (val != computed_val)
    {
      return false;
    }
  return true;
}

int main(int argc, char * argv[])
{
  if (unquote_test() and
      unescape_test() and
      str2int_test() and
      string_copy_test())
    {
      exit(0);
    }
  else
    {
      exit(1);
    }
}
