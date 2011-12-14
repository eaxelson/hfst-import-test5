#include <cstdio>
#include <iostream>
#include <assert.h>
#include <time.h>
#include "hfst/HfstTransducer.h"
#include "hfst/HfstInputStream.h"
#include "hfst/HfstOutputStream.h"
#include "hfst/implementations/HfstTransitionGraph.h"




using namespace hfst;
using namespace std;
using namespace implementations;
using namespace rules;


//enum ReplaceType {REPL_UP, REPL_DOWN, REPL_RIGHT, REPL_LEFT};


//ImplementationType TYPE = TROPICAL_OPENFST_TYPE;
ImplementationType TYPE = SFST_TYPE;
//ImplementationType TYPE = FOMA_TYPE;


HfstTransducer removeMarkers( HfstTransducer &tr )
{
	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");

	tr.substitute(StringPair(LeftMarker, LeftMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();
	tr.substitute(StringPair(RightMarker, RightMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();


	tr.remove_from_alphabet(LeftMarker);
	tr.remove_from_alphabet(RightMarker);

	tr.minimize();

	//printf("tr without markers: \n");
	//tr.write_in_att_format(stdout, 1);

	return tr;
}

// tmp = t.1 .o. Constr .o. t.1
// (t.1 - tmp.2) .o. t
HfstTransducer constraintComposition( HfstTransducer t, HfstTransducer Constraint )
{


	HfstTokenizer TOK;

	String leftMarker("@_LM_@");
	String rightMarker("@_RM_@");
	TOK.add_multichar_symbol(leftMarker);
	TOK.add_multichar_symbol(rightMarker);

//	HfstTransducer input("a@_LM_@a@_RM_@abaa", TOK, TYPE);
//	printf("----composition: \n");


	HfstTransducer tInputPr(t);
	tInputPr.input_project().minimize();

//printf("tInputPr: \n");
//tInputPr.write_in_att_format(stdout, 1);

	HfstTransducer tmp(tInputPr);

	tmp.compose(Constraint).minimize();
//printf("first composition \n");
//tmp.write_in_att_format(stdout, 1);
	tmp.compose(tInputPr).minimize();


//printf("tmp: \n");
//tmp.write_in_att_format(stdout, 1);

//HfstTransducer test(tmp);
//test.compose(input).minimize();

//printf("tmp compose input: \n");
//test.write_in_att_format(stdout, 1);

	tmp.output_project().minimize();
	tInputPr.subtract(tmp).minimize();

	tInputPr.compose(t);

	t = tInputPr;

	return t;
}



HfstTransducer expandContextsWithMapping (HfstTransducerPairVector ContextVector,
								HfstTransducer mappingWithBracketsAndTmpBoundary,
								HfstTransducer identityExpanded,
								ReplaceType replType)
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	String leftMarker("@_LM_@");
	String rightMarker("@_RM_@");
	String tmpMarker("@_TMPM_@");
	String leftMarker2("@_LM2_@");
	String rightMarker2("@_RM2_@");


	TOK.add_multichar_symbol(leftMarker);
	TOK.add_multichar_symbol(rightMarker);
	TOK.add_multichar_symbol(leftMarker2);
	TOK.add_multichar_symbol(rightMarker2);
	TOK.add_multichar_symbol(tmpMarker);

	HfstTransducer leftBracket(leftMarker, TOK, TYPE);
	HfstTransducer rightBracket(rightMarker, TOK, TYPE);
	HfstTransducer leftBracket2(leftMarker2, TOK, TYPE);
	HfstTransducer rightBracket2(rightMarker2, TOK, TYPE);
	HfstTransducer tmpBracket(tmpMarker, TOK, TYPE);

	HfstTransducer unionContextReplace(TYPE);

	for ( unsigned int i = 0; i < ContextVector.size(); i++ )
	{


			// Expand context with mapping
			// Cr' = (Rc .*) << Markers (<,>,|) .o. [I:I | <a:b>]*
			// Cr = Cr|Cr'
			// (same for left context)

			// Lc = (*. Lc) << {<,>}
			HfstTransducer tmp(identityExpanded);
			tmp.concatenate(ContextVector[i].first).minimize();
			ContextVector[i].first = tmp;
			ContextVector[i].first.insert_freely(leftBracket).minimize();
			ContextVector[i].first.insert_freely(rightBracket).minimize();
			ContextVector[i].first.insert_freely(leftBracket2).minimize();
			ContextVector[i].first.insert_freely(rightBracket2).minimize();

			// Rc =  (Rc .*) << {<,>}
			ContextVector[i].second.concatenate(identityExpanded).minimize();
			ContextVector[i].second.insert_freely(leftBracket).minimize();
			ContextVector[i].second.insert_freely(rightBracket).minimize();
			ContextVector[i].second.insert_freely(leftBracket2).minimize();
			ContextVector[i].second.insert_freely(rightBracket2).minimize();


			HfstTransducer leftContextExpanded(TYPE);
			HfstTransducer rightContextExpanded(TYPE);

			/* RULE:	LC:		RC:
			 * up		up		up
			 * left		up		down
			 * right	down	up
			 * down		down	down
			 */

			// both contexts are in upper language
			if ( replType == REPL_UP)
			{
				//printf("Repl up \n\n");
				// compose them with [I:I | <a:b>]*
				leftContextExpanded = ContextVector[i].first;
				rightContextExpanded = ContextVector[i].second;

				leftContextExpanded.compose(identityExpanded).minimize();
				rightContextExpanded.compose(identityExpanded).minimize();
			}
			// left context is in lower language, right in upper ( // )
			if ( replType == REPL_RIGHT )
			{
				//printf("Repl right \n\n");
					// compose them with [I:I | <a:b>]*

				// left compose opposite way
				leftContextExpanded = identityExpanded;
				rightContextExpanded = ContextVector[i].second;

				leftContextExpanded.compose(ContextVector[i].first).minimize();
				rightContextExpanded.compose(identityExpanded).minimize();
			}
			// right context is in lower language, left in upper ( \\ )
			if ( replType == REPL_LEFT )
			{
				// compose them with [I:I | <a:b>]*
				leftContextExpanded = ContextVector[i].first;
				rightContextExpanded = identityExpanded;

				leftContextExpanded.compose(identityExpanded).minimize();
				rightContextExpanded.compose(ContextVector[i].second).minimize();
			}
			if ( replType == REPL_DOWN )
			{
				// compose them with [I:I | <a:b>]*
				leftContextExpanded = identityExpanded;
				rightContextExpanded = identityExpanded;

				leftContextExpanded.compose(ContextVector[i].first).minimize();
				rightContextExpanded.compose(ContextVector[i].second).minimize();
			}

			ContextVector[i].first.disjunct(leftContextExpanded).minimize();
			ContextVector[i].second.disjunct(rightContextExpanded).minimize();


		// put mapping between (expanded) contexts
		HfstTransducer oneContextReplace(ContextVector[i].first);
		oneContextReplace.concatenate(mappingWithBracketsAndTmpBoundary).
					concatenate(ContextVector[i].second);

		unionContextReplace.disjunct(oneContextReplace).minimize();
	}
	return unionContextReplace;
}


/*
 * unconditional replace, in multiple contexts
 * first: (.* T<a:b>T .*) - [( .* L1 T<a:b>T R1 .*) u (.* L2 T<a:b>T R2 .*)...],
 * 						where .* = [I:I (+ {tmpMarker (T), <,>} in alphabet) | <a:b>]*
 * then: remove tmpMarker from transducer and alphabet, and do negation:
 * 		.* - result from upper operations
 *
 * Also note that context is expanded in this way:
 * 		Cr' = (Rc .*) << Markers (<,>,|) .o. [I:I | <a:b>]*
 * 		Cr = Cr | Cr'
 * 		(same for left context, (.* Cl))
*/


HfstTransducer bracketedReplace(  HfstTransducer mapping1,
		 	 	 	 	 	 	 HfstTransducer mapping2,
		 	 	 	 	 	 	 HfstTransducerPairVector ContextVector1,
		 	 	 	 	 		HfstTransducerPairVector ContextVector2,
		 	 	 	 	 		ReplaceType replType,
                                  bool optional)
{



	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	String leftMarker("@_LM_@");
	String rightMarker("@_RM_@");

	String leftMarker2("@_LM2_@");
	String rightMarker2("@_RM2_@");


	String tmpMarker("@_TMPM_@");
	TOK.add_multichar_symbol(leftMarker);
	TOK.add_multichar_symbol(rightMarker);
	TOK.add_multichar_symbol(leftMarker2);
		TOK.add_multichar_symbol(rightMarker2);
	TOK.add_multichar_symbol(tmpMarker);

	HfstTransducer leftBracket(leftMarker, TOK, TYPE);
	HfstTransducer rightBracket(rightMarker, TOK, TYPE);

	HfstTransducer leftBracket2(leftMarker2, TOK, TYPE);
	HfstTransducer rightBracket2(rightMarker2, TOK, TYPE);

	HfstTransducer tmpBracket(tmpMarker, TOK, TYPE);


	// Surround mapping with brackets
	HfstTransducer mappingWithBrackets1(leftBracket);
	mappingWithBrackets1.concatenate(mapping1).concatenate(rightBracket).minimize();

	// Surround mapping with brackets
	HfstTransducer mappingWithBrackets2(leftBracket);
	mappingWithBrackets2.concatenate(mapping2).concatenate(rightBracket).minimize();



	// non - optional
	// mapping = T<a:b>T u T<2a:a>2T

	HfstTransducer mappingProject1(mapping1);
	mappingProject1.input_project().minimize();

	HfstTransducer mappingProject2(mapping1);
	mappingProject2.input_project().minimize();

	HfstTransducer tmpMapping1(leftBracket2);
	tmpMapping1.concatenate(mappingProject1).concatenate(rightBracket2).minimize();
	HfstTransducer mappingWithBracketsNonOptional1(tmpMapping1);

	HfstTransducer tmpMapping2(leftBracket2);
	tmpMapping2.concatenate(mappingProject2).concatenate(rightBracket2).minimize();
	HfstTransducer mappingWithBracketsNonOptional2(tmpMapping1);

	if ( optional != true )
	{
		// mappingWithBrackets...... expanded
		mappingWithBrackets1.disjunct(mappingWithBracketsNonOptional1).minimize();
		mappingWithBrackets2.disjunct(mappingWithBracketsNonOptional2).minimize();
	}




	//printf("mappingWithBrackets1: \n");
	//mappingWithBrackets1.minimize().write_in_att_format(stdout, 1);

	//printf("mappingWithBrackets2: \n");
	//mappingWithBrackets2.minimize().write_in_att_format(stdout, 1);




	// Surround mapping with brackets with tmp boudaries
	HfstTransducer mappingWithBracketsAndTmpBoundary1(tmpBracket);
	mappingWithBracketsAndTmpBoundary1.concatenate(mappingWithBrackets1).concatenate(tmpBracket).minimize();


	HfstTransducer mappingWithBracketsAndTmpBoundary2(tmpBracket);
	mappingWithBracketsAndTmpBoundary2.concatenate(mappingWithBrackets2).concatenate(tmpBracket).minimize();

	//printf("mappingWithBracketsAndTmpBoundary1: \n");
	//mappingWithBracketsAndTmpBoundary1.minimize().write_in_att_format(stdout, 1);

	//printf("mappingWithBracketsAndTmpBoundary2: \n");
	//mappingWithBracketsAndTmpBoundary2.minimize().write_in_att_format(stdout, 1);




	// Identity pair
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );

	HfstTransducer identityExpanded(identityPair);
	identityExpanded.insert_to_alphabet(leftMarker);
	identityExpanded.insert_to_alphabet(rightMarker);
	identityExpanded.insert_to_alphabet(leftMarker2);
	identityExpanded.insert_to_alphabet(rightMarker2);
	identityExpanded.insert_to_alphabet(tmpMarker);

	identityExpanded.disjunct(mappingWithBrackets1).disjunct(mappingWithBrackets2).minimize();
	identityExpanded.repeat_star().minimize();

	//printf("identityExpanded: \n");
	//identityExpanded.write_in_att_format(stdout, 1);


	//return identityExpanded;


	// .* |<a:b>| :*
	HfstTransducer bracketedReplace1(identityExpanded);
	bracketedReplace1.concatenate(mappingWithBracketsAndTmpBoundary1).concatenate(identityExpanded).minimize();


	//printf("bracketedReplace1: \n");
	//bracketedReplace1.write_in_att_format(stdout, 1);


	HfstTransducer bracketedReplace2(identityExpanded);
	bracketedReplace2.concatenate(mappingWithBracketsAndTmpBoundary2).concatenate(identityExpanded).minimize();



	HfstTransducer bracketedReplace(bracketedReplace1);
	bracketedReplace.disjunct(bracketedReplace2).minimize();

	//printf("bracketedReplace: \n");
	//bracketedReplace.write_in_att_format(stdout, 1);







// Mapping in Context



	HfstTransducer unionContextReplace1(TYPE);
	unionContextReplace1 = expandContextsWithMapping (ContextVector1,
									 mappingWithBracketsAndTmpBoundary1,
									 identityExpanded,
									 replType);

	HfstTransducer unionContextReplace2(TYPE);
	unionContextReplace2 = expandContextsWithMapping (ContextVector2,
										 	 	 	 	 mappingWithBracketsAndTmpBoundary2,
										 	 	 	 	 identityExpanded,
										 	 	 	 	 replType);
	HfstTransducer unionContextReplace(unionContextReplace1);
	unionContextReplace.disjunct(unionContextReplace2).minimize();










	// subtract all mappings in contexts from replace without contexts
		HfstTransducer replaceWithoutContexts(bracketedReplace);
		replaceWithoutContexts.subtract(unionContextReplace).minimize();

	// remove tmpMaprker
	replaceWithoutContexts.substitute(StringPair(tmpMarker, tmpMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();
	replaceWithoutContexts.remove_from_alphabet(tmpMarker);
	replaceWithoutContexts.minimize();




	//printf("before final negation: \n");
	//replaceWithoutContexts.write_in_att_format(stdout, 1);

		// final negation
		HfstTransducer uncondidtionalTr(identityExpanded);
		uncondidtionalTr.subtract(replaceWithoutContexts).minimize();

	//printf("uncondidtionalTr: \n");
	//uncondidtionalTr.write_in_att_format(stdout, 1);

		return uncondidtionalTr;

}

// (help function)
// returns: [ B:0 | 0:B | ?-B ]*
// which is used in some constraints
HfstTransducer constraintsRightPart()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Identity pair (normal)
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );


	HfstTransducer leftBracket(LeftMarker, TOK, TYPE);
	HfstTransducer rightBracket(RightMarker, TOK, TYPE);

	// Create Right Part
	HfstTransducer B(leftBracket);
	B.disjunct(rightBracket).minimize();

	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", LeftMarker, TOK, TYPE);
	HfstTransducer LeftMarkToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);

	HfstTransducer epsilonToBrackets(epsilon);
	epsilonToBrackets.cross_product(B);

	HfstTransducer bracketsToEpsilon(B);
	bracketsToEpsilon.cross_product(epsilon);

	HfstTransducer identityPairMinusBrackets(identityPair);
	identityPairMinusBrackets.subtract(B).minimize();//.repeat_plus().minimize();

	HfstTransducer rightPart(epsilonToBrackets);
	rightPart.disjunct(bracketsToEpsilon).disjunct(identityPairMinusBrackets).minimize().repeat_star().minimize();

	return rightPart;
}




// ?* [ BL:0 (?-B)+ BR:0 ?* ]+
HfstTransducer mostBracketsPlusConstraint( HfstTransducer uncondidtionalTr )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String leftMarker("@_LM_@");
	String rightMarker("@_RM_@");
	String leftMarker2("@_LM2_@");
	String rightMarker2("@_RM2_@");
	TOK.add_multichar_symbol(leftMarker);
	TOK.add_multichar_symbol(rightMarker);
	TOK.add_multichar_symbol(leftMarker2);
	TOK.add_multichar_symbol(rightMarker2);

	HfstTransducer leftBracket(leftMarker, TOK, TYPE);
	HfstTransducer rightBracket(rightMarker, TOK, TYPE);

	HfstTransducer leftBracket2(leftMarker2, TOK, TYPE);
	HfstTransducer rightBracket2(rightMarker2, TOK, TYPE);


	// Identity (normal)
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );
	HfstTransducer identity (identityPair);
	identity.repeat_star().minimize();

	HfstTransducer identityPlus (identityPair);
	identityPlus.repeat_plus().minimize();


	HfstTransducer identityStar (identityPair);
	identityStar.repeat_star().minimize();


	// epsilon
	String epsilon("@_EPSILON_SYMBOL_@");

	// BL:0 ( <1 : 0, <2 : 0)
	HfstTransducer leftBracketToEpsilon(leftMarker, epsilon, TOK, TYPE );
	HfstTransducer leftBracket2ToEpsilon(leftMarker2, epsilon, TOK, TYPE );
	HfstTransducer allLeftBracketsToEpsilon(leftBracketToEpsilon);
	allLeftBracketsToEpsilon.disjunct(leftBracket2ToEpsilon).minimize();


//	printf("allLeftBracketsToEpsilon: \n");
//	allLeftBracketsToEpsilon.write_in_att_format(stdout, 1);

	// BR:0 ( >1 : 0, >2 : 0)
	HfstTransducer rightBracketToEpsilon(rightMarker, epsilon, TOK, TYPE );
	HfstTransducer rightBracket2ToEpsilon(rightMarker2, epsilon, TOK, TYPE );
	HfstTransducer allRightBracketsToEpsilon(rightBracketToEpsilon);
	allRightBracketsToEpsilon.disjunct(rightBracket2ToEpsilon).minimize();

	// B (B1 | B2)
	HfstTransducer B(leftBracket);
	B.disjunct(rightBracket).minimize();
	B.disjunct(leftBracket2).minimize();
	B.disjunct(rightBracket2).minimize();



	// (? - B)+
	HfstTransducer identityPairMinusBracketsPlus(identityPair);
	identityPairMinusBracketsPlus.subtract(B).minimize().repeat_plus().minimize();

	// repeatingPart ( BL:0 (?-B)+ BR:0 ?* )+
	HfstTransducer repeatingPart(allLeftBracketsToEpsilon);
	repeatingPart.concatenate(identityPairMinusBracketsPlus).minimize();
	repeatingPart.concatenate(allRightBracketsToEpsilon).minimize();
	repeatingPart.concatenate(identityStar).minimize();
	repeatingPart.repeat_plus().minimize();
//printf("middlePart: \n");
//middlePart.write_in_att_format(stdout, 1);



	HfstTransducer Constraint(identityStar);
	Constraint.concatenate(repeatingPart).minimize();


//printf("Constraint: \n");
//Constraint.write_in_att_format(stdout, 1);


	//// Compose with unconditional replace transducer
	// tmp = t.1 .o. Constr .o. t.1
	// (t.1 - tmp.2) .o. t

	HfstTransducer retval(TYPE);
	retval = constraintComposition(uncondidtionalTr, Constraint);


//printf("After composition: \n");
//retval.write_in_att_format(stdout, 1);

	return retval;

}
// ?* B2 ?*
HfstTransducer removeB2Constraint( HfstTransducer t )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	String leftMarker2("@_LM2_@");
	String rightMarker2("@_RM2_@");

	TOK.add_multichar_symbol(leftMarker2);
	TOK.add_multichar_symbol(rightMarker2);

	HfstTransducer leftBracket2(leftMarker2, TOK, TYPE);
	HfstTransducer rightBracket2(rightMarker2, TOK, TYPE);


	// Identity (normal)
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );
	HfstTransducer identity (identityPair);
	identity.repeat_star().minimize();

	HfstTransducer identityStar (identityPair);
	identityStar.repeat_star().minimize();

	// B (B2)
	HfstTransducer B(leftBracket2);
	B.disjunct(rightBracket2).minimize();


	HfstTransducer Constraint(identityStar);
	Constraint.concatenate(B).minimize();
	Constraint.concatenate(identityStar).minimize();


	//// Compose with unconditional replace transducer
	// tmp = t.1 .o. Constr .o. t.1
	// (t.1 - tmp.2) .o. t

	HfstTransducer retval(TYPE);
	retval = constraintComposition(t, Constraint);


//printf("Remove B2 After composition: \n");
//retval.write_in_att_format(stdout, 1);

	return retval;

}
// to avoid repetition in empty replace rule
HfstTransducer noRepetitionConstraint( HfstTransducer t )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	String leftMarker2("@_LM2_@");
		String rightMarker2("@_RM2_@");

		TOK.add_multichar_symbol(leftMarker2);
		TOK.add_multichar_symbol(rightMarker2);



	HfstTransducer leftBracket(LeftMarker, TOK, TYPE);
	HfstTransducer rightBracket(RightMarker, TOK, TYPE);

	HfstTransducer leftBracket2(leftMarker2, TOK, TYPE);
	HfstTransducer rightBracket2(rightMarker2, TOK, TYPE);


	HfstTransducer leftBrackets(leftBracket);
	leftBrackets.disjunct(leftBracket2).minimize();

	HfstTransducer rightBrackets(rightBracket);
	rightBrackets.disjunct(rightBracket2).minimize();

	// Identity (normal)
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );

	HfstTransducer identityStar (identityPair);
	identityStar.repeat_star().minimize();


	HfstTransducer Constraint(identityStar);
	Constraint.concatenate(leftBrackets).
			concatenate(rightBrackets).
			concatenate(leftBrackets).
			concatenate(rightBrackets).
			concatenate(identityStar).minimize();


	//// Compose with unconditional replace transducer
	// tmp = t.1 .o. Constr .o. t.1
	// (t.1 - tmp.2) .o. t

	HfstTransducer retval(TYPE);
	retval = constraintComposition(t, Constraint);


	return retval;

}




// ?* <:0 [B:0]* [I-B] [ B:0 | 0:B | ?-B ]*
HfstTransducer leftMostConstraint( HfstTransducer uncondidtionalTr )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	HfstTransducer leftBracket(LeftMarker, TOK, TYPE);
	HfstTransducer rightBracket(RightMarker, TOK, TYPE);


	// Identity (normal)
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );
	HfstTransducer identity (identityPair);
	identity.repeat_star().minimize();


	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	HfstTransducer rightPart(TYPE);
	rightPart = constraintsRightPart();


	// epsilon
	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, TYPE);
	// B
	HfstTransducer B(leftBracket);
	B.disjunct(rightBracket).minimize();
	// (B:0)*
	HfstTransducer bracketsToEpsilonStar(B);
	bracketsToEpsilonStar.cross_product(epsilon).minimize().repeat_star().minimize();

	// (I-B) and (I-B)+
	HfstTransducer identityPairMinusBrackets(identityPair);
	identityPairMinusBrackets.subtract(B).minimize();

	HfstTransducer identityPairMinusBracketsPlus(identityPairMinusBrackets);
	identityPairMinusBracketsPlus.repeat_plus().minimize();


	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);

	HfstTransducer Constraint(identity);

	// ?* <:0 [B:0]* [I-B] [ B:0 | 0:B | ?-B ]*
	Constraint.concatenate(LeftBracketToEpsilon).
			concatenate(bracketsToEpsilonStar).
			concatenate(identityPairMinusBrackets).
			concatenate(rightPart).
			minimize();

//printf("Constraint: \n");
//Constraint.write_in_att_format(stdout, 1);


	//// Compose with unconditional replace transducer
	// tmp = t.1 .o. Constr .o. t.1
	// (t.1 - tmp.2) .o. t

	HfstTransducer retval(TYPE);
	retval = constraintComposition(uncondidtionalTr, Constraint);

//printf("Constraint: \n");
//Constraint.write_in_att_format(stdout, 1);
	return retval;

}


// Longest match
// it should be composed to left most transducer........
// ?* < [?-B]+ 0:> [ ? | 0:< | <:0 | 0:> | B ] [ B:0 | 0:B | ?-B ]*
HfstTransducer longestMatchLeftMostConstraint( HfstTransducer uncondidtionalTr )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	HfstTransducer leftBracket(LeftMarker, TOK, TYPE);
	HfstTransducer rightBracket(RightMarker, TOK, TYPE);

	// Identity
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );
	HfstTransducer identity(identityPair);
	identity.repeat_star().minimize();

	// epsilon
	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, TYPE);
	// B
	HfstTransducer B(leftBracket);
	B.disjunct(rightBracket).minimize();
	// (B:0)*
	HfstTransducer bracketsToEpsilonStar(B);
	bracketsToEpsilonStar.cross_product(epsilon).minimize().repeat_star().minimize();

	// (I-B) and (I-B)+
	HfstTransducer identityPairMinusBrackets(identityPair);
	identityPairMinusBrackets.subtract(B).minimize();

	HfstTransducer identityPairMinusBracketsPlus(identityPairMinusBrackets);
	identityPairMinusBracketsPlus.repeat_plus().minimize();



	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	HfstTransducer rightPart(TYPE);
	rightPart = constraintsRightPart();



	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", RightMarker, TOK, TYPE);
	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", LeftMarker, TOK, TYPE);

	//[ ? | 0:< | <:0 | 0:> | B ]
	HfstTransducer nonClosingBracketInsertion(identityPair);
	nonClosingBracketInsertion.
			disjunct(epsilonToLeftBracket).
			disjunct(LeftBracketToEpsilon).
			disjunct(epsilonToRightBracket).
			disjunct(B).
			minimize();
	//	printf("nonClosingBracketInsertion: \n");
	//	nonClosingBracketInsertion.write_in_att_format(stdout, 1);

	// ?* < [?-B]+ 0:> [ ? | 0:< | <:0 | 0:> | B ] [?-B]+ [ B:0 | 0:B | ?-B ]*
	HfstTransducer Constraint(identity);
	Constraint.concatenate(leftBracket).
			concatenate(identityPairMinusBracketsPlus).
			concatenate(epsilonToRightBracket).
			concatenate(nonClosingBracketInsertion).
			minimize().
			concatenate(identityPairMinusBracketsPlus).
			concatenate(rightPart).
			minimize();

	//printf("Constraint Longest Match: \n");
	//Constraint.write_in_att_format(stdout, 1);


	//uncondidtionalTr should be left most for the left most longest match
	HfstTransducer retval(TYPE);
	retval = constraintComposition(uncondidtionalTr, Constraint);


	return retval;

}

/*
HfstTransducer parallel_rules(	HfstTransducer mapping1,
								HfstTransducer mapping2,
						bool optional)
{


	HfstTransducer retval(TYPE);


	retval = bracketedReplace(mapping1, mapping2, optional);


	if ( !optional )
		{
			retval = mostBracketsPlusConstraint(retval);
			retval = removeB2Constraint(retval);

		}


	retval = removeMarkers( retval );
	return retval;
}
*/

HfstTransducer parallel_rules(	HfstTransducer mapping1,
								HfstTransducer mapping2,
								HfstTransducerPairVector ContextVector1,
								HfstTransducerPairVector ContextVector2,
								ReplaceType replType,
								bool optional)
{
	HfstTransducer retval(TYPE);


	retval = bracketedReplace(mapping1, mapping2, ContextVector1, ContextVector2, replType, optional );


	if ( !optional )
		{
			retval = mostBracketsPlusConstraint(retval);
			retval = removeB2Constraint(retval);

		}


	retval = removeMarkers( retval );
	return retval;
}

HfstTransducer parallel_rules_epenthesis(	HfstTransducer mapping1,
								HfstTransducer mapping2,
								HfstTransducerPairVector ContextVector1,
								HfstTransducerPairVector ContextVector2,
								ReplaceType replType,
								bool optional)
{
	HfstTransducer retval(TYPE);


	retval = bracketedReplace(mapping1, mapping2, ContextVector1, ContextVector2, replType, optional );

	printf("retval after bracketed repl: \n");
	retval.write_in_att_format(stdout, 1);

	// it can't have more than one epsilon repetition in a row

	retval = noRepetitionConstraint( retval );

	printf("retval after no repetition constraint: \n");
	retval.write_in_att_format(stdout, 1);



	if ( !optional )
	{
			retval = mostBracketsPlusConstraint(retval);


			printf("retval after mostBracketsPlusConstraint: \n");
			retval.write_in_att_format(stdout, 1);



			retval = removeB2Constraint(retval);

			printf("retval after removeB2Constraint: \n");
					retval.write_in_att_format(stdout, 1);

	}


	retval = removeMarkers( retval );


	printf("retval after remove markers: \n");
	retval.write_in_att_format(stdout, 1);


	return retval;
}


HfstTransducer parallel_rules_leftmost_longest_match(	HfstTransducer mapping1,
								HfstTransducer mapping2,
								HfstTransducerPairVector ContextVector1,
								HfstTransducerPairVector ContextVector2,
								ReplaceType replType,
								bool optional)
{
	HfstTransducer retval(TYPE);


	retval = bracketedReplace(mapping1, mapping2, ContextVector1, ContextVector2, replType, optional );



	retval = leftMostConstraint(retval);

	//printf("leftMostConstraint: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = longestMatchLeftMostConstraint( retval );

	//printf("longestMatchLeftMostConstraint: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = removeMarkers( retval );

	return retval;
}

//---------------------------------
//	REPLACE FUNCTIONS - TESTS
//---------------------------------

// [..] -> p || m _ k
void test6()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping

	HfstTransducer leftMapping1("a", TOK, TYPE);

	HfstTransducer rightMapping1("b", TOK, TYPE);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("b", TOK, TYPE);

	HfstTransducer rightMapping2("c", TOK, TYPE);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);

	//printf("mapping1: \n");
	//mapping1.write_in_att_format(stdout, 1);

	//printf("mapping2: \n");
	//mapping2.write_in_att_format(stdout, 1);


	//
	/*
	 *    0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ]]
	 */

	// Context
	HfstTransducerPair Context1(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));
	HfstTransducerPair Context2(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));

	HfstTransducerPairVector ContextVector1;
	ContextVector1.push_back(Context1);

	HfstTransducerPairVector ContextVector2;
	ContextVector2.push_back(Context2);

/*
	// pair mapping, contextVector
	pair <HfstTransducer, HfstTransducerPairVector> rulePair1 (mapping1, ContextVector1);
	pair <HfstTransducer, HfstTransducerPairVector> rulePair2 (mapping2, ContextVector2);

	vector<pair <HfstTransducer, HfstTransducerPairVector> > ruleVector;

	ruleVector.push_back(rulePair1);
	ruleVector.push_back(rulePair2);



	*/





	HfstTransducer input1("aab", TOK, TYPE);

	//HfstTransducer result1("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@aa",TOK, TYPE);


	HfstTransducer replaceTr(TYPE);
	HfstTransducer tmp2(TYPE);

	// epsilon
	replaceTr = parallel_rules(mapping1, mapping2,  ContextVector1, ContextVector2, REPL_UP, false);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
printf("Replace leftmost tr2: \n");
tmp2.write_in_att_format(stdout, 1);
	//assert(tmp2.compare(result1));

}

// 0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ]]
// TODO:
void test7b()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping

	HfstTransducer leftMapping1("@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer rightMapping1("b", TOK, TYPE);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("a", TOK, TYPE);
	HfstTransducer rightMapping2("c", TOK, TYPE);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);

	//printf("mapping1: \n");
	//mapping1.write_in_att_format(stdout, 1);

	//printf("mapping2: \n");
	//mapping2.write_in_att_format(stdout, 1);


	//
	/*
	 *    0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ]]
	 */

	// Context
	HfstTransducerPair Context1(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));
	HfstTransducerPair Context2(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));

	HfstTransducerPairVector ContextVector1;
	ContextVector1.push_back(Context1);

	HfstTransducerPairVector ContextVector2;
	ContextVector2.push_back(Context2);

/*
	// pair mapping, contextVector
	pair <HfstTransducer, HfstTransducerPairVector> rulePair1 (mapping1, ContextVector1);
	pair <HfstTransducer, HfstTransducerPairVector> rulePair2 (mapping2, ContextVector2);

	vector<pair <HfstTransducer, HfstTransducerPairVector> > ruleVector;

	ruleVector.push_back(rulePair1);
	ruleVector.push_back(rulePair2);



	*/


	HfstTransducer input1("a", TOK, TYPE);

	//HfstTransducer result1("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@aa",TOK, TYPE);


	HfstTransducer replaceTr(TYPE);
	HfstTransducer tmp2(TYPE);

	// epsilon
	replaceTr = parallel_rules_epenthesis(mapping1, mapping2,  ContextVector1, ContextVector2, REPL_UP, false);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
printf("Replace leftmost tr2: \n");
tmp2.write_in_att_format(stdout, 1);
	//assert(tmp2.compare(result1));

}


void test7c()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping

	HfstTransducer leftMapping1("a", TOK, TYPE);
	leftMapping1.repeat_plus().minimize();

	HfstTransducer rightMapping1("X", TOK, TYPE);
	HfstTransducerPair mappingPair1(leftMapping1, rightMapping1);

	HfstTransducer leftMapping2("b", TOK, TYPE);
	leftMapping2.repeat_plus().minimize();

	HfstTransducer rightMapping2("Y", TOK, TYPE);
	HfstTransducerPair mappingPair2(leftMapping2, rightMapping2);


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);

	//printf("mapping1: \n");
	//mapping1.write_in_att_format(stdout, 1);

	//printf("mapping2: \n");
	//mapping2.write_in_att_format(stdout, 1);


	//
	/*
	 *    0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ]]
	 */

	// Context
	HfstTransducerPair Context1(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));
	HfstTransducerPair Context2(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));

	HfstTransducerPairVector ContextVector1;
	ContextVector1.push_back(Context1);

	HfstTransducerPairVector ContextVector2;
	ContextVector2.push_back(Context2);

/*
	// pair mapping, contextVector
	pair <HfstTransducer, HfstTransducerPairVector> rulePair1 (mapping1, ContextVector1);
	pair <HfstTransducer, HfstTransducerPairVector> rulePair2 (mapping2, ContextVector2);

	vector<pair <HfstTransducer, HfstTransducerPairVector> > ruleVector;

	ruleVector.push_back(rulePair1);
	ruleVector.push_back(rulePair2);



	*/


	HfstTransducer input1("aaabbb", TOK, TYPE);

	//HfstTransducer result1("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@aa",TOK, TYPE);


	HfstTransducer replaceTr(TYPE);
	HfstTransducer tmp2(TYPE);

	// epsilon
	replaceTr = parallel_rules_leftmost_longest_match(mapping1, mapping2,  ContextVector1, ContextVector2, REPL_UP, false);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
printf("Replace leftmost tr2: \n");
tmp2.write_in_att_format(stdout, 1);
	//assert(tmp2.compare(result1));

}




int main()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// tmp
printf("--test6---\n\n");
test6();
printf("--test7b---\n\n");
test7b();
printf("--test7c---\n\n");
test7c();
	return 0;
}
