
using namespace hfst;
using namespace implementations;
using namespace hfst::xeroxRules;



// empty language replacements
// a -> ~[?*]
void test10a( ImplementationType type )
{
	HfstTokenizer TOK;

	// Mapping
	HfstTransducerPair mappingPair(HfstTransducer("a", TOK, type), HfstTransducer(type));

	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Rule
	Rule rule(mappingPairVector);

	// result
	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	HfstTransducer result1(identityPair);
	result1.repeat_star().minimize();
	result1.insert_to_alphabet("a");


	HfstTransducer replaceTr(type);
	replaceTr = replace(rule, false);

	//printf("replaceTr: \n");
	//replaceTr.write_in_att_format(stdout, 1);

	//printf("result1: \n");
	//result1.write_in_att_format(stdout, 1);

	assert(replaceTr.compare(result1));
}
// empty language replacements
// ~[?*] -> a
/* TODO
void test10b( ImplementationType type )
{
	HfstTokenizer TOK;

	// Mapping
	HfstTransducerPair mappingPair( HfstTransducer(type), HfstTransducer("a", TOK, type));

	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Rule
	Rule rule(mappingPairVector);

	// result
	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	HfstTransducer result1(identityPair);
	result1.repeat_star().minimize();
	result1.insert_to_alphabet("a");


	HfstTransducer replaceTr(type);
	replaceTr = replace(rule, false);

printf("replaceTr: \n");
replaceTr.write_in_att_format(stdout, 1);

printf("result1: \n");
result1.write_in_att_format(stdout, 1);

	assert(replaceTr.compare(result1));
}
*/


// replace left d <- ca || ca_c  ( input: c a c a c a c )
void test9a( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping
	HfstTransducerPair mappingPair(HfstTransducer("d@_EPSILON_SYMBOL_@", TOK, type), HfstTransducer("ca", TOK, type));

	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);


	// Context
	HfstTransducerPair Context(HfstTransducer("ca", TOK, type), HfstTransducer("c", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	Rule rule(mappingPairVector, ContextVector, REPL_UP);

	HfstTransducer input1("cacacac", TOK, type);
	HfstTransducer result1("cad@_EPSILON_SYMBOL_@d@_EPSILON_SYMBOL_@c", "cacacac", TOK, type);


	HfstTransducer replaceTr(type);
	replaceTr = replace_left(rule, false);

	HfstTransducer tmp2(type);
	tmp2 = replaceTr;
	tmp2.compose(input1).minimize();
	//printf("abba optional: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}


// replace left b <- a ,, a <- b
void test9b( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	// Mapping
	HfstTransducerPair mappingPair1(HfstTransducer("b", TOK, type), HfstTransducer("a", TOK, type));
	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPair mappingPair2(HfstTransducer("a", TOK, type), HfstTransducer("b", TOK, type));
	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);


	Rule rule1(mappingPairVector1);
	Rule rule2(mappingPairVector2);

	vector<Rule> ruleVector;

	ruleVector.push_back(rule1);
	ruleVector.push_back(rule2);



	HfstTransducer input1("abba", TOK, type);
	HfstTransducer result1("baab", "abba", TOK, type);


	HfstTransducer replaceTr(type);
	replaceTr = replace_left(ruleVector, false);

	//printf("tr : \n");
	//replaceTr.write_in_att_format(stdout, 1);

	HfstTransducer tmp2(type);
	tmp2 = replaceTr;
	tmp2.compose(input1).minimize();
	//printf("abba: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}


/*
// ab->x  ab_a
void test8( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	// Mapping
	HfstTransducer mapping1("a","b", TOK, type);
	HfstTransducer mapping2("b","a", TOK, type);
	mapping1.disjunct(mapping2).minimize();


	HfstTransducer input1("abba", TOK, type);
	HfstTransducer result1("abba", "baab", TOK, type);

	Rule rule(mapping1);

	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(rule, false);


	//printf("transducer: \n");
	//replaceTr.write_in_att_format(stdout, 1);

	//HfstOutputStream output( replaceTr.get_type() );
	//output << replaceTr;

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("abba optional: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}
*/


// ab->x  ab_a
void test1( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("ab", TOK, type);
	HfstTransducer rightMapping("x", TOK, type);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context(HfstTransducer("ab", TOK, type), HfstTransducer("a", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);


	HfstTransducer input1("abababa", TOK, type);

	HfstTransducer result1("abababa", TOK, type);
	HfstTransducer r1tmp("abababa", "abx@_EPSILON_SYMBOL_@aba", TOK, type);
	HfstTransducer r2tmp("abababa", "ababx@_EPSILON_SYMBOL_@a", TOK, type);
	HfstTransducer r3tmp("abababa", "abx@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@a", TOK, type);
	result1.disjunct(r1tmp).disjunct(r2tmp).minimize().disjunct(r3tmp).minimize();


	Rule rule(mappingPairVector, ContextVector, REPL_UP);

	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(rule, true);

	//printf("test 1 optional replaceTr: \n");
	//replaceTr.write_in_att_format(stdout, 1);

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("test 1 optional: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));


	//replace up non optional
	// Left most optional
	replaceTr = replace(rule, false);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("left most: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(r3tmp));
}




// a -> x
void test1b( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	HfstTransducer rightMapping("x", TOK, type);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("aaana", TOK, type);

//	HfstTransducer result1("aaana", TOK, type);


	HfstBasicTransducer bt;
	bt.add_transition(0, HfstBasicTransition(1, "a", "a", 0) );
	bt.add_transition(0, HfstBasicTransition(1, "a", "x", 0) );
	bt.add_transition(1, HfstBasicTransition(2, "a", "a", 0) );
	bt.add_transition(1, HfstBasicTransition(2, "a", "x", 0) );
	bt.add_transition(2, HfstBasicTransition(3, "a", "a", 0) );
	bt.add_transition(2, HfstBasicTransition(3, "a", "x", 0) );
	bt.add_transition(3, HfstBasicTransition(4, "n", "n", 0) );
	bt.add_transition(4, HfstBasicTransition(5, "a", "a", 0) );
	bt.add_transition(4, HfstBasicTransition(5, "a", "x", 0) );
	bt.set_final_weight(5, 0);

	HfstTransducer result1(bt, type);
	HfstTransducer result2("aaana", "xxxnx", TOK, type);


	Rule rule(mappingPairVector, ContextVector, REPL_UP);

	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(rule, true);

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("aaana optional: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));


	// non optional
	replaceTr = replace(rule, false);

	//printf("test 1b replaceTr: \n");
	//replaceTr.write_in_att_format(stdout, 1);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("test 1b: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));


	//	printf(".... Left most longest match replace ....\n");
	// Left most longest match Constraint test
	replaceTr = replace_leftmost_longest_match(rule);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("leftmost longest match: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));



	// replace_leftmost_shortest_match
	// Left most shortest match Constraint test
	replaceTr = replace_leftmost_shortest_match(rule);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("shortest match 1: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));
}



// ? -> x
void test1c( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");
	TOK.add_multichar_symbol("@_IDENTITY_SYMBOL_@");

	// Mapping
	HfstTransducer leftMapping("@_IDENTITY_SYMBOL_@", TOK, type);
	HfstTransducer rightMapping("x", TOK, type);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	HfstTransducer input1("s", TOK, type);

	HfstTransducer result1("s", "x", TOK, type);


	Rule rule(mappingPairVector);


	HfstTransducer replaceTr(type);
	replaceTr = replace(rule, false);


	//printf("test1c replaceTr: \n");
	//replaceTr.write_in_att_format(stdout, 1);



	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("test1c: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}



// a+ @-> x || a _ a
// a+ @-> x // a _ a
// a+ @-> x \\ a _ a
// a+ @-> x \/ a _ a
void test2a( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");
	TOK.add_multichar_symbol("@_IDENTITY_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	leftMapping.repeat_plus().minimize();
	HfstTransducer rightMapping("x", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context(HfstTransducer("a", TOK, type), HfstTransducer("a", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);


	HfstTransducer input1("aaaa", TOK, type);
	HfstTransducer input2("aaaaabaaaa", TOK, type);
	HfstTransducer input3("aaaaabaaaacaaaa", TOK, type);

	// results:

	HfstTransducer result1("aaaa", TOK, type);
	HfstTransducer r1tmp("aaaa","ax@_EPSILON_SYMBOL_@a",TOK, type);
	HfstTransducer r2tmp("aaaa","axaa",TOK, type);
	HfstTransducer r3tmp("aaaa","aaxa",TOK, type);
	HfstTransducer r4tmp("aaaa","axxa",TOK, type);

	result1.disjunct(r1tmp).minimize().disjunct(r2tmp).minimize().disjunct(r3tmp).minimize();

	HfstTransducer result8(result1);
	result8.disjunct(r4tmp).minimize();

	HfstTransducer result2(r1tmp);
	result2.disjunct(r4tmp).minimize();




	HfstTransducer result3(r1tmp);

	HfstTransducer result9(r1tmp);
	result9.disjunct(r2tmp).minimize();

	HfstTransducer result10(r1tmp);
	result10.disjunct(r3tmp).minimize();

	HfstTransducer result11(result10);
	result11.disjunct(r2tmp).minimize();


	HfstTransducer result4("aaaaabaaaa","ax@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@abax@_EPSILON_SYMBOL_@a",TOK, type);
	HfstTransducer result5("aaaaabaaaa","axxxabaxxa",TOK, type);

	HfstTransducer result6("aaaaabaaaacaaaa","ax@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@abax@_EPSILON_SYMBOL_@acax@_EPSILON_SYMBOL_@a",TOK, type);
	HfstTransducer result7("aaaaabaaaacaaaa","axxxabaxxacaxxa",TOK, type);





	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPairVector, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPairVector, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPairVector, ContextVector, REPL_DOWN);





	HfstTransducer replaceTrUp(type);
	HfstTransducer replaceTrLeft(type);
	HfstTransducer replaceTrRight(type);
	HfstTransducer replaceTrDown(type);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);


	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result8));

	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
	//printf("Unconditional optional replace L: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("Unconditional optional replace R: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("Unconditional optional replace D: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));





	// Non optional replacements

	HfstTransducer a(type);

	//a = replace(ContextVector, mappingPair, REPL_UP, false);
	replaceTrUp 	= replace(ruleUp, false);
	replaceTrLeft 	= replace(ruleLeft, false);
	replaceTrRight	= replace(ruleRight, false);
	replaceTrDown	= replace(ruleDown, false);

	//printf("a: \n");
	//replaceTrUp.write_in_att_format(stdout, 1);

	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("input 1 repl U: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));

	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
	//printf("non opt repl Left: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result10));

	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("non opt repl R: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result9));

	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("non opt repl D: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result11));


	//	printf(".... Left most longest match replace ....\n");
	// Left most longest match Constraint test

	HfstTransducer replaceTr(type);
	replaceTr = replace_leftmost_longest_match(ruleUp);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//	printf("leftmost longest match: \n");
	//	tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));

	tmp2 = input2;
	tmp2.compose(replaceTr).minimize();
	//printf("input2 longest m: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));

	tmp2 = input3;
	tmp2.compose(replaceTr).minimize();
	//printf("input2 longest m: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result6));



	// replace_leftmost_shortest_match
	// Left most shortest match Constraint test

	replaceTr = replace_leftmost_shortest_match(ruleUp);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//	printf("shortest match 1: \n");
	//	tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(r4tmp));

	tmp2 = input2;
	tmp2.compose(replaceTr).minimize();
	//	printf("shortest match 2: \n");
	//	tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result5));

	tmp2 = input3;
	tmp2.compose(replaceTr).minimize();
	//printf("shortest match 2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result7));
}

// longest & shortest, left & right
// a+ b+ | b+ a+ @-> x
// input aabbaa
void test2b( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer aPlus("a", TOK, type);
	aPlus.repeat_plus().minimize();
	HfstTransducer bPlus("b", TOK, type);
	bPlus.repeat_plus().minimize();

	// a+ b+
	HfstTransducer mtmp1(aPlus);
	mtmp1.concatenate(bPlus).minimize();
	// b+ a+
	HfstTransducer mtmp2(bPlus);
	mtmp2.concatenate(aPlus).minimize();
	// a+ b+ | b+ a+ -> x
	HfstTransducer leftMapping(mtmp1);
	leftMapping.disjunct(mtmp2).minimize();
	HfstTransducer rightMapping("x", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	HfstTransducer input1("aabbaa", TOK, type);

	HfstTransducer result1("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@aa",TOK, type);
	HfstTransducer result2("aabbaa", "aax@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@",TOK, type);
	HfstTransducer result3("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@a",TOK, type);
	HfstTransducer result4("aabbaa", "ax@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@",TOK, type);




	Rule ruleUp(mappingPairVector);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	// leftmost longest match
	replaceTr = replace_leftmost_longest_match(ruleUp);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

	// rightmost longest match
	replaceTr = replace_rightmost_longest_match(ruleUp);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace rmost tr: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));

	// leftmost shortest match
	replaceTr = replace_leftmost_shortest_match(ruleUp);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));

	// rightmost shortest match
	replaceTr = replace_rightmost_shortest_match(ruleUp);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace r tr: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));


	// in context
	// // a+ b+ | b+ a+ @-> x \/ _ x ;
	// input: aabbaax

	HfstTransducer input2("aabbaax", TOK, type);
	HfstTransducer result5("aabbaax", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@x",TOK, type);
	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type), HfstTransducer("x", TOK, type));
	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	Rule ruleDown(mappingPairVector, ContextVector, REPL_DOWN);

	// leftmost longest match in context
	replaceTr = replace_leftmost_longest_match( ruleDown );
	tmp2 = input2;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace r tr: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result5));

}



// a+ @-> x || c _

void test2c( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");
	TOK.add_multichar_symbol("@_IDENTITY_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	leftMapping.repeat_plus().minimize();
	HfstTransducer rightMapping("x", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context(HfstTransducer("c", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);


	HfstTransducer input1("caav", TOK, type);

	// results:

	HfstTransducer result1("caav", "cx@_EPSILON_SYMBOL_@v", TOK, type);


	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);

	HfstTransducer replaceTr = replace_leftmost_longest_match(ruleUp);

	HfstTransducer tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("leftmost longest match: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));


}

// test multiple contexts
// a -> b ||  x _ x ;
void test3a( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);


	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	HfstTransducer rightMapping("b", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context( HfstTransducer("x",TOK, type),  HfstTransducer("x",TOK, type));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("xaxax", TOK, type);

	HfstTransducer result1("xaxax", TOK, type);
	HfstTransducer r1tmp("xaxax", "xbxax", TOK, type);
	HfstTransducer r2tmp("xaxax", "xaxbx", TOK, type);
	HfstTransducer r3tmp("xaxax", "xbxbx", TOK, type);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();

	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}

// test multiple contexts
// a b -> b ||  x_y, y_z
void test3b( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	leftMapping.repeat_plus().minimize();
	HfstTransducer rightMapping("b", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context( HfstTransducer("x",TOK, type),  HfstTransducer("y",TOK, type));
	HfstTransducerPair Context2( HfstTransducer("y",TOK, type),  HfstTransducer("z", TOK, type));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);
	ContextVector.push_back(Context2);

	HfstTransducer input1("axayaz", TOK, type);

	HfstTransducer result1("axayaz", TOK, type);
	HfstTransducer r1tmp("axayaz", "axbybz", TOK, type);
	HfstTransducer r2tmp("axayaz", "axbyaz", TOK, type);
	HfstTransducer r3tmp("axayaz", "axaybz", TOK, type);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();


	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();

	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));
}

// test multiple contexts
// a+ -> x  || x x _ y y, y _ x
void test3c( ImplementationType type )

{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	leftMapping.repeat_plus().minimize();
	HfstTransducer rightMapping("x", TOK, type);


	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context( HfstTransducer("xx",TOK, type),  HfstTransducer("yy",TOK, type));
	HfstTransducerPair Context2( HfstTransducer("y",TOK, type),  HfstTransducer("x", TOK, type));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);
	ContextVector.push_back(Context2);

	HfstTransducer input1("axxayyax", TOK, type);

	HfstTransducer result1("axxayyax", TOK, type);
	HfstTransducer r1tmp("axxayyax", "axxayyxx", TOK, type);
	HfstTransducer r2tmp("axxayyax", "axxxyyax", TOK, type);
	HfstTransducer r3tmp("axxayyax", "axxxyyxx", TOK, type);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();

	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();

	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}
// test multiple contexts
// a -> b ;
void test3d( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	HfstTransducer rightMapping("b", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context( HfstTransducer("@_EPSILON_SYMBOL_@",TOK, type),
								HfstTransducer("@_EPSILON_SYMBOL_@",TOK, type));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("xaxax", TOK, type);

	HfstTransducer result1("xaxax", TOK, type);
	HfstTransducer r1tmp("xaxax", "xbxax", TOK, type);
	HfstTransducer r2tmp("xaxax", "xaxbx", TOK, type);
	HfstTransducer r3tmp("xaxax", "xbxbx", TOK, type);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();


	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}


// b -> a  || _a (r: bbaa)
// b -> a  \\ _a (r:aaaa)
// b -> a  // _a (r:bbaa)
// b -> a  \/ _a (r:aaaa)
// input: bbba
void test4a( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("b", TOK, type);
	HfstTransducer rightMapping("a", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type), HfstTransducer("a", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("bbba", TOK, type);


	// results:
	HfstTransducer result1("bbba", "bbaa", TOK, type);
	HfstTransducer result2("bbba", "aaaa", TOK, type);
	HfstTransducer r1Tmp("bbba", "baaa", TOK, type);
	HfstTransducer result3(input1);
	result3.disjunct(result1).minimize();


	HfstTransducer result4(result3);
	result4.disjunct(result2).minimize().disjunct(r1Tmp).minimize();


	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPairVector, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPairVector, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPairVector, ContextVector, REPL_DOWN);


	HfstTransducer replaceTrUp(type);
	HfstTransducer replaceTrLeft(type);
	HfstTransducer replaceTrRight(type);
	HfstTransducer replaceTrDown(type);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);



	// Unconditional optional replace
	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("Unconditional optional replace 4a: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));


	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
	//printf("Unconditional optional replace 4a L: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));


	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("Unconditional optional replace 4a L: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));

	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("Unconditional optional replace 4a down: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));




	// Non optional
	replaceTrUp 	= replace(ruleUp, false);
	replaceTrLeft 	= replace(ruleLeft, false);
	replaceTrRight	= replace(ruleRight, false);
	replaceTrDown	= replace(ruleDown, false);

	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("Replace up: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
//printf("Replace left: \n");
//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));

	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("Replace right: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));


	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("Replace down: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));

}


// b -> a  || a _ (r: aabb)
// b -> a  \\ a _ (r: aabb)
// b -> a  // a _ (r: aaaa)
// b -> a  \/ a _ (r: aaaa)
// input: abbb
void test4b( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("b", TOK, type);
	HfstTransducer rightMapping("a", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context(HfstTransducer("a", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type) );

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);


	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPairVector, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPairVector, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPairVector, ContextVector, REPL_DOWN);


	HfstTransducer replaceTrUp(type);
	HfstTransducer replaceTrLeft(type);
	HfstTransducer replaceTrRight(type);
	HfstTransducer replaceTrDown(type);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);

	HfstTransducer input1("abbb", TOK, type);

	// results:
	HfstTransducer result1("abbb", "aabb", TOK, type);
	HfstTransducer result2("abbb", "aaaa", TOK, type);
	HfstTransducer r1Tmp("abbb", "aaab", TOK, type);
	HfstTransducer result3(input1);
	result3.disjunct(result1).minimize();

	HfstTransducer result4(result3);
	result4.disjunct(r1Tmp).minimize().disjunct(result2).minimize();


	// Unconditional optional replace
	HfstTransducer tmp2(type);

	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("Unconditional optional replace 4b: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));

	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
	//printf("Unconditional optional replace 4b: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));

	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("Unconditional optional replace 4b: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));

	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("Unconditional optional replace 4b: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));





	// Non optional
	replaceTrUp 	= replace(ruleUp, false);
	replaceTrLeft 	= replace(ruleLeft, false);
	replaceTrRight	= replace(ruleRight, false);
	replaceTrDown	= replace(ruleDown, false);


	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("Replace up: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
	//printf("Replace left: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("Replace right: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));


	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("Replace down: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));

}

void test4c( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping
	HfstTransducer leftMapping("ab", TOK, type);
	HfstTransducer rightMapping("x", TOK, type);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);

	// Context
	HfstTransducerPair Context( HfstTransducer("ab",TOK, type),  HfstTransducer("a",TOK, type));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("abababa", TOK, type);

	HfstTransducer result1("abababa", "abababa", TOK, type);
	HfstTransducer r2tmp("abababa", "ababx@_EPSILON_SYMBOL_@a", TOK, type);
	HfstTransducer r3tmp("abababa", "abx@_EPSILON_SYMBOL_@aba", TOK, type);
	HfstTransducer r4tmp("abababa", "abx@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@a", TOK, type);

	HfstTransducer result2(result1);
	result2.disjunct(r2tmp).disjunct(r3tmp).minimize();

	HfstTransducer result3(result2);
	result3.disjunct(r4tmp).minimize();

	HfstTransducer result4(r2tmp);
	result4.disjunct(r3tmp).minimize();

	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPairVector, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPairVector, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPairVector, ContextVector, REPL_DOWN);


	HfstTransducer replaceTrUp(type);
	HfstTransducer replaceTrLeft(type);
	HfstTransducer replaceTrRight(type);
	HfstTransducer replaceTrDown(type);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);

	HfstTransducer tmp2(type);

	//printf("optional 4c: \n");

	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("Replace up: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));

	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
	//printf("Replace l: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));

	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("Replace r: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));


	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("down: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));



	// non optional
	replaceTrUp 	= replace(ruleUp, false);
	replaceTrLeft 	= replace(ruleLeft, false);
	replaceTrRight	= replace(ruleRight, false);
	replaceTrDown	= replace(ruleDown, false);

	//printf("non-optional 4c: \n");

	tmp2 = input1;
	tmp2.compose(replaceTrUp).minimize();
	//printf("Replace up: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(r4tmp));

	tmp2 = input1;
	tmp2.compose(replaceTrLeft).minimize();
	//printf("Replace l: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(r2tmp));

	tmp2 = input1;
	tmp2.compose(replaceTrRight).minimize();
	//printf("Replace r: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(r3tmp));


	tmp2 = input1;
	tmp2.compose(replaceTrDown).minimize();
	//printf("Replace down 4c: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));

}

// mark up rules
void test5( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping
	HfstTransducer lmtmp("b", TOK, type);
	HfstTransducer leftMapping("a", TOK, type);
	leftMapping.disjunct(lmtmp).minimize();

	HfstTransducer empty(type);

	HfstTransducerPair mappingPair(leftMapping, empty);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);



	StringPair marks("[","]");


	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

//	HfstTransducerPair Context(HfstTransducer("m", TOK, type), HfstTransducer("k", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mba", TOK, type);

	HfstTransducer result1("m@_EPSILON_SYMBOL_@b@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@a@_EPSILON_SYMBOL_@", "m[b][a]",TOK, type);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);


	replaceTr = mark_up_replace(ruleUp, marks, false);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("test5: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}


// epenthesis rules
void test6a( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping

	HfstTransducer leftMapping("@_EPSILON_SYMBOL_@", TOK, type);

	HfstTransducer rightMapping("p", TOK, type);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);


	// Context
	HfstTransducerPair Context(HfstTransducer("m", TOK, type), HfstTransducer("k", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mk", TOK, type);

	HfstTransducer result1("m@_EPSILON_SYMBOL_@k", "mpk",TOK, type);
	HfstTransducer result2("mk", "mk",TOK, type);
	result2.disjunct(result1).minimize();



	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);

	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	// epsilon
	replaceTr = replace_epenthesis(ruleUp, false);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

	// epsilon
	replaceTr = replace_epenthesis(ruleUp, true);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));

}

void test6b( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	leftMapping.repeat_star().minimize();

//	printf("leftMapping: \n");
//	leftMapping.write_in_att_format(stdout, 1);

	HfstTransducer rightMapping("p", TOK, type);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);
	HfstTransducerPairVector mappingPairVector;
	mappingPairVector.push_back(mappingPair);


	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type),
							   HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mak", TOK, type);

	HfstTransducer result1("@_EPSILON_SYMBOL_@m@_EPSILON_SYMBOL_@a@_EPSILON_SYMBOL_@k@_EPSILON_SYMBOL_@",
							"pmpppkp", TOK, type);


	Rule ruleUp(mappingPairVector, ContextVector, REPL_UP);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	// epsilon
	replaceTr = replace_epenthesis(ruleUp, false);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}
// a -> b , b -> c
void test7a( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping

	HfstTransducer leftMapping1("a", TOK, type);
	HfstTransducer rightMapping1("b", TOK, type);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("b", TOK, type);
	HfstTransducer rightMapping2("c", TOK, type);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);

	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);

	// without context
	Rule rule1(mappingPairVector1);
	Rule rule2(mappingPairVector2);

	vector<Rule> ruleVector;

	ruleVector.push_back(rule1);
	ruleVector.push_back(rule2);


	HfstTransducer input1("aab", TOK, type);
	HfstTransducer result1("aab", "bbc",TOK, type);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	replaceTr = replace(ruleVector, false);


	//printf("Test 7a Replace tr: \n");
	//replaceTr.write_in_att_format(stdout, 1);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Test 7a Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));



}




// [. .] -> b , a -> c ;
void test7b( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping

	HfstTransducer leftMapping1("@_EPSILON_SYMBOL_@", TOK, type);
	HfstTransducer rightMapping1("b", TOK, type);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("a", TOK, type);
	HfstTransducer rightMapping2("c", TOK, type);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);

	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);


	// without context
	Rule rule1(mappingPairVector1);
	Rule rule2(mappingPairVector2);

	vector<Rule> ruleVector;

	ruleVector.push_back(rule1);
	ruleVector.push_back(rule2);


	HfstTransducer input1("a", TOK, type);

	HfstTransducer result1("@_EPSILON_SYMBOL_@a@_EPSILON_SYMBOL_@", "bcb",TOK, type);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	// epsilon
	replaceTr = replace_epenthesis( ruleVector, false);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}


// a+ @-> x , b+ @-> y
// a+ @> x , b+ @> y
// a+ -> x \/ m _ ,, b+ -> y || x _

void test7c( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	// Mapping

	HfstTransducer leftMapping1("a", TOK, type);
	leftMapping1.repeat_plus().minimize();

	HfstTransducer rightMapping1("x", TOK, type);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("b", TOK, type);
	leftMapping2.repeat_plus().minimize();

	HfstTransducer rightMapping2("y", TOK, type);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);


	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);

	//printf("mapping1: \n");
	//mapping1.write_in_att_format(stdout, 1);

	//printf("mapping2: \n");
	//mapping2.write_in_att_format(stdout, 1);

	// without context
	Rule rule1(mappingPairVector1);
	Rule rule2(mappingPairVector2);

	vector<Rule> ruleVector;

	ruleVector.push_back(rule1);
	ruleVector.push_back(rule2);


	HfstTransducer input1("aaabbb", TOK, type);
	HfstTransducer result1("aaabbb", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@y@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@",TOK, type);
	HfstTransducer result1b("aaabbb", "xxxyyy",TOK, type);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	replaceTr = replace_leftmost_longest_match( ruleVector );
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));


	replaceTr = replace_leftmost_shortest_match( ruleVector );
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1b));




	// With Contexts

	// a -> x \/ m _ ,, b -> y || x _ ;
	// input: mab
	HfstTransducer input2("mab", TOK, type);

	HfstTransducer result2("mab", "mxb",TOK, type);
	HfstTransducer result3("mab", "mxy",TOK, type);


	HfstTransducer input3("maabb", TOK, type);

	HfstTransducer result4("maabb", "mx@_EPSILON_SYMBOL_@bb",TOK, type);
	HfstTransducer result4b("maabb", "mxabb",TOK, type);
	result4.disjunct(result4b).minimize();

	HfstTransducer result5("maabb", "mx@_EPSILON_SYMBOL_@yb",TOK, type);
	HfstTransducer result5b("maabb", "mx@_EPSILON_SYMBOL_@y@_EPSILON_SYMBOL_@",TOK, type);
	result5.disjunct(result5b).disjunct(result4b).minimize();






	HfstTransducerPair Context1(HfstTransducer("m", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));
	HfstTransducerPair Context2(HfstTransducer("x", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

	HfstTransducerPairVector ContextVector1;
	ContextVector1.push_back(Context1);

	HfstTransducerPairVector ContextVector2;
	ContextVector2.push_back(Context2);

	// without context replace up
	Rule rule2aUp(mappingPairVector1, ContextVector1, REPL_UP);
	Rule rule2bUp(mappingPairVector2, ContextVector2, REPL_UP);

	vector<Rule> ruleVector2;
	ruleVector2.push_back(rule2aUp);
	ruleVector2.push_back(rule2bUp);

	replaceTr = replace( ruleVector2, false);

	tmp2 = input2;
	tmp2.compose(replaceTr).minimize();
	//printf("With context: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result2));


	tmp2 = input3;
	tmp2.compose(replaceTr).minimize();
	//printf("With context: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result4));


	// With context replace down

	Rule rule2aDown(mappingPairVector1, ContextVector1, REPL_DOWN);
	Rule rule2bDown(mappingPairVector2, ContextVector2, REPL_DOWN);

	vector<Rule> ruleVector3;
	ruleVector3.push_back(rule2aDown);
	ruleVector3.push_back(rule2bDown);

	replaceTr = replace( ruleVector3, false);

	tmp2 = input2;
	tmp2.compose(replaceTr).minimize();
	//printf("With context: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result3));

	tmp2 = input3;
	tmp2.compose(replaceTr).minimize();
	//printf("With context: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result5));

}
// TODO: change algorithm for epenthesis replace
// 0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ]
void test7d( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	// Mapping

	HfstTransducer leftMapping1("@_EPSILON_SYMBOL_@", TOK, type);
	//HfstTransducer leftMapping1("x", TOK, type);
	HfstTransducer rightMapping1("a", TOK, type);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("@_EPSILON_SYMBOL_@", TOK, type);
	//HfstTransducer leftMapping2("x", TOK, type);
	HfstTransducer rightMapping2("b", TOK, type);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);

	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);

	 //   0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ] ;


	// Context
	HfstTransducerPair Context1a(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type), HfstTransducer("ba", TOK, type));
	HfstTransducerPair Context1b(HfstTransducer("ab", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

	HfstTransducerPairVector ContextVector1;
	ContextVector1.push_back(Context1a);
	ContextVector1.push_back(Context1b);

	HfstTransducerPair Context2(HfstTransducer("a", TOK, type), HfstTransducer("a", TOK, type));

	HfstTransducerPairVector ContextVector2;
	ContextVector2.push_back(Context2);



	// without context
	Rule rule1(mappingPairVector1, ContextVector1, REPL_DOWN);
	Rule rule2(mappingPairVector2, ContextVector2, REPL_DOWN);

	vector<Rule> ruleVector;

	ruleVector.push_back(rule1);
	ruleVector.push_back(rule2);


	HfstTransducer input1("@_EPSILON_SYMBOL_@", TOK, type);
	//HfstTransducer input1("xxx", TOK, type);

	//HfstTransducer result1("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@aa",TOK, type);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	// epsilon
	replaceTr = replace_epenthesis( ruleVector, false);



	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("7d: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(input1));
}

/* markup vector
void test7e( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	// Mapping

	HfstTransducer leftMapping1("a", TOK, type);
	HfstTransducer leftMapping2("b", TOK, type);
	HfstTransducer empty(type);

	HfstTransducerPair mappingPair1(leftMapping1, empty);
	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPair mappingPair2(leftMapping2, empty);
	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);

	StringPair marks1("[","]");
	StringPair marks2("|","|");


	MarkUpRule markUpRule1(mappingPairVector1, marks1 );
	MarkUpRule markUpRule2(mappingPairVector2, marks2 );

	vector<MarkUpRule> markUpRuleVector;
	markUpRuleVector.push_back(markUpRule1);
	markUpRuleVector.push_back(markUpRule2);

	HfstTransducer input1("ab", TOK, type);

	HfstTransducer result1("@_EPSILON_SYMBOL_@a@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@b@_EPSILON_SYMBOL_@",
					"[a]|b|",TOK, type);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	replaceTr = mark_up_replace( markUpRuleVector, false);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("7e: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));
}
*/


// a -> b , b -> a
void test7f( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping

	HfstTransducer leftMapping1("a", TOK, type);
	HfstTransducer rightMapping1("b", TOK, type);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("b", TOK, type);
	HfstTransducer rightMapping2("a", TOK, type);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);

	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);


	// without context
	Rule rule1(mappingPairVector1);
	Rule rule2(mappingPairVector2);

	vector<Rule> ruleVector;

	ruleVector.push_back(rule1);
	ruleVector.push_back(rule2);


	HfstTransducer input1("aabbaa", TOK, type);
	HfstTransducer result1("aabbaa", "bbaabb",TOK, type);


	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	replaceTr = replace(ruleVector, false);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Test 7f Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));


}



// a -> b b, a -> b
void test7g( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping

	HfstTransducer leftMapping1("a", TOK, type);
	HfstTransducer rightMapping1("bb", TOK, type);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("a", TOK, type);
	HfstTransducer rightMapping2("b", TOK, type);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);


	HfstTransducerPairVector mappingPairVector1;
	mappingPairVector1.push_back(mappingPair1);

	HfstTransducerPairVector mappingPairVector2;
	mappingPairVector2.push_back(mappingPair2);



	// without context
	Rule rule1(mappingPairVector1);
	Rule rule2(mappingPairVector2);

	vector<Rule> ruleVector;

	ruleVector.push_back(rule1);
	ruleVector.push_back(rule2);


	HfstTransducer input1("a", TOK, type);
	HfstTransducer result1("a", "b",TOK, type);
	HfstTransducer resultTmp("a@_EPSILON_SYMBOL_@", "bb",TOK, type);
	result1.disjunct(resultTmp).minimize();

	HfstTransducer replaceTr(type);
	HfstTransducer tmp2(type);

	replaceTr = replace(ruleVector, false);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Test 7f Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));
}
