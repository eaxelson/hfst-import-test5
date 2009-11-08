#include "../src/string_manipulation.h"
#include "../src/rule_parts.h"
#include "../src/RuleModifier.h"

RuleString * read_symbols(const char ** symbols);
void remove(RuleStrings * strs);
void remove(RuleString * str);
void display(RuleStrings &strings);
void display(RuleCenter &center); 
void display(RuleRepresentationVector * representation_vector); 
bool same_representations(RuleRepresentationVector * rep1, 
			  RuleRepresentationVector * rep2);
bool RuleModifier_test(void);
int main(void);


