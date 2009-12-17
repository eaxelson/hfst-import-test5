#ifndef __STRING_MANIPULATION_TESTS_H
#define __STRING_MANIPULATION_TESTS_H

#include <cstring>
#include <cstdlib>
#include "../src/string_manipulation.h"

bool not_equal(const char * str1,
	       const char * str2);
bool string_copy_test(void);
bool unquote_test(void);
bool unescape_test(void);
bool str2int_test(void);
int main(int argc, char * argv[]);

#endif
