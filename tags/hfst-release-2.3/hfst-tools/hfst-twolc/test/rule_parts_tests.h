#include <iostream>
#include "../src/string_manipulation.h"
#include "../src/rule_parts.h"

bool RuleContextContainer_test(void);
void insert_symbols(SymbolMap &symbol_map,
		    char ** string_symbols);
RuleContextVector * 
make_context_vector(char ** context_symbols);
bool equal(RuleContextVector * v1,
	   RuleContextVector * v2);
int main(void);
