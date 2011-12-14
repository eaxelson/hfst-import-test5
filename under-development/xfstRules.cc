//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.


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



///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class Rule
{
	HfstTransducer mapping;
	HfstTransducerPairVector context;
	ReplaceType replType;

  public:
	Rule ( HfstTransducer, ReplaceType ); // mapping
	Rule ( HfstTransducer,  HfstTransducerPairVector, ReplaceType); // mapping, context
	Rule ( HfstTransducerPair, ReplaceType );
	Rule ( HfstTransducerPair, HfstTransducerPairVector, ReplaceType );
	HfstTransducer get_mapping();
	HfstTransducerPairVector get_context();
	ReplaceType get_replType();
};

Rule::Rule ( HfstTransducer a_mapping, ReplaceType a_replType )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	HfstTransducerPair contextPair(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE),
								   HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));
	HfstTransducerPairVector epsilonContext;
	epsilonContext.push_back(contextPair);


	mapping = a_mapping;
	context = epsilonContext;
	replType = a_replType;
}
Rule::Rule ( HfstTransducer a_mapping, HfstTransducerPairVector a_context, ReplaceType a_replType )
{
	mapping = a_mapping;
	context = a_context;
	replType = a_replType;
}

Rule::Rule ( HfstTransducerPair mappingPair, ReplaceType a_replType )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	HfstTransducer tmpMapping(mappingPair.first);
	tmpMapping.cross_product(mappingPair.second).minimize();


	HfstTransducerPair contextPair(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE),
									   HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));
	HfstTransducerPairVector epsilonContext;
	epsilonContext.push_back(contextPair);


	mapping = tmpMapping;
	context = epsilonContext;
	replType = a_replType;

}
Rule::Rule ( HfstTransducerPair mappingPair, HfstTransducerPairVector a_context, ReplaceType a_replType )
{
	HfstTransducer tmpMapping(mappingPair.first);
	tmpMapping.cross_product(mappingPair.second).minimize();

	mapping = tmpMapping;
	context = a_context;
	replType = a_replType;
}

HfstTransducer Rule::get_mapping()
{
	return mapping;
}

HfstTransducerPairVector Rule::get_context()
{
	return context;
}
ReplaceType Rule::get_replType()
{
	return replType;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////















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

	tmp.output_project().minimize();
	tInputPr.subtract(tmp).minimize();

	tInputPr.compose(t);

	t = tInputPr;

	return t;
}



HfstTransducer insertFreelyAllTheBrackets( HfstTransducer t )
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


	t.insert_freely(leftBracket).minimize();
	t.insert_freely(rightBracket).minimize();
	t.insert_freely(leftBracket2).minimize();
	t.insert_freely(rightBracket2).minimize();

	t.minimize();


	return t;
}


HfstTransducer expandContextsWithMapping (HfstTransducerPairVector ContextVector,
								HfstTransducer mappingWithBracketsAndTmpBoundary,
								HfstTransducer identityExpanded,
								ReplaceType replType)
{


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

			ContextVector[i].first = insertFreelyAllTheBrackets( ContextVector[i].first );

			// Rc =  (Rc .*) << {<,>}
			ContextVector[i].second.concatenate(identityExpanded).minimize();
			ContextVector[i].second = insertFreelyAllTheBrackets( ContextVector[i].second );

			/* RULE:	LC:		RC:
			 * up		up		up
			 * left		up		down
			 * right	down	up
			 * down		down	down
			 */

			HfstTransducer leftContextExpanded(TYPE);
			HfstTransducer rightContextExpanded(TYPE);

			// both contexts are in upper language
			if ( replType == REPL_UP)
			{

				// compose them with [I:I | <a:b>]*
				leftContextExpanded = ContextVector[i].first;
				rightContextExpanded = ContextVector[i].second;

				leftContextExpanded.compose(identityExpanded).minimize();
				rightContextExpanded.compose(identityExpanded).minimize();
			}
			// left context is in lower language, right in upper ( // )
			if ( replType == REPL_RIGHT )
			{
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

HfstTransducer bracketedReplace(  Rule rule, bool optional)
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

	HfstTransducer mapping( rule.get_mapping() );
	HfstTransducerPairVector ContextVector( rule.get_context() );
	ReplaceType replType( rule.get_replType() );

	// Surround mapping with brackets
	HfstTransducer tmpMapping(leftBracket);
	tmpMapping.concatenate(mapping).concatenate(rightBracket).minimize();

	HfstTransducer mappingWithBrackets(tmpMapping);

	//printf("mappingWithBrackets: \n");
	//mappingWithBrackets.minimize().write_in_att_format(stdout, 1);


	if ( optional != true )
	{
		// non - optional
		// mapping = T<a:b>T u T<2a:a>2T

		HfstTransducer mappingProject(mapping);
		mappingProject.input_project().minimize();

		HfstTransducer tmpMapping2(leftBracket2);
		tmpMapping2.concatenate(mappingProject).concatenate(rightBracket2).minimize();
		HfstTransducer mappingWithBrackets2(tmpMapping2);

		// mappingWithBrackets...... expanded
		mappingWithBrackets.disjunct(mappingWithBrackets2).minimize();
	}




	// Identity pair
	HfstTransducer identityPair = HfstTransducer::identity_pair( TYPE );

	// Identity with bracketed mapping and marker symbols and TmpMarker in alphabet
	// [I:I | <a:b>]* (+ tmpMarker in alphabet)
	HfstTransducer identityExpanded (identityPair);

	identityExpanded.insert_to_alphabet(leftMarker);
	identityExpanded.insert_to_alphabet(rightMarker);
	identityExpanded.insert_to_alphabet(tmpMarker);

	if ( optional != true )
	{
		identityExpanded.insert_to_alphabet(leftMarker2);
		identityExpanded.insert_to_alphabet(rightMarker2);
	}
	identityExpanded.disjunct(mappingWithBrackets).minimize();
	identityExpanded.repeat_star().minimize();

	//printf("identityExpanded: \n");
	//identityExpanded.write_in_att_format(stdout, 1);


// when there aren't any contexts, result is identityExpanded
	if ( ContextVector.size() == 1 )
	{
		HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, TYPE);
		if ( ContextVector[0].first.compare(epsilon) && ContextVector[0].second.compare(epsilon) )
		{
			//printf("context 1.1 je 0\n");
			return identityExpanded;
		}


	}




	// Surround mapping with tmp boudaries
	HfstTransducer mappingWithBracketsAndTmpBoundary(tmpBracket);
	mappingWithBracketsAndTmpBoundary.concatenate(mappingWithBrackets).concatenate(tmpBracket).minimize();


	// .* |<a:b>| :*
	HfstTransducer bracketedReplace(identityExpanded);
	bracketedReplace.concatenate(mappingWithBracketsAndTmpBoundary).concatenate(identityExpanded).minimize();

	//printf("bracketedReplace: \n");
	//bracketedReplace.write_in_att_format(stdout, 1);









	// Expand all contexts with mapping taking in consideration replace type
	// result is their union
	HfstTransducer unionContextReplace(TYPE);
	unionContextReplace = expandContextsWithMapping (ContextVector,
									 mappingWithBracketsAndTmpBoundary,
									 identityExpanded,
									 replType);











	// subtract all mappings in contexts from replace without contexts
	HfstTransducer replaceWithoutContexts(bracketedReplace);
	replaceWithoutContexts.subtract(unionContextReplace).minimize();


//printf("uncondidtionalTr with tmp marker: \n");
//bracketedReplace.write_in_att_format(stdout, 1);



	// remove tmpMaprker
	replaceWithoutContexts.substitute(StringPair(tmpMarker, tmpMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();
	replaceWithoutContexts.remove_from_alphabet(tmpMarker);
	replaceWithoutContexts.minimize();

	// final negation
	HfstTransducer uncondidtionalTr(identityExpanded);
	uncondidtionalTr.subtract(replaceWithoutContexts).minimize();

//printf("uncondidtionalTr: \n");
//uncondidtionalTr.write_in_att_format(stdout, 1);

	return uncondidtionalTr;

}

/*
HfstTransducer bracketedReplace(  HfstTransducerPairVector ContextVector,
                                      HfstTransducerPair mappingPair,
                                      ReplaceType replType,
                                      bool optional)
{
	// Mapping crossproduct
	HfstTransducer mapping(mappingPair.first);
	mapping.cross_product(mappingPair.second);

	HfstTransducer retval(TYPE);
	retval = bracketedReplace(ContextVector, mapping, replType, optional);
	return retval;
}
*/

//---------------------------------
//	CONSTRAINTS
//---------------------------------

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

// [ B:0 | 0:B | ?-B ]* [I-B]+  >:0 [ ?-B ]*
HfstTransducer rightMostConstraint( HfstTransducer uncondidtionalTr )
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

	HfstTransducer identityPairMinusBracketsStar(identityPairMinusBrackets);
	identityPairMinusBracketsStar.repeat_star().minimize();


	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);

	HfstTransducer Constraint(rightPart);
	// [ B:0 | 0:B | ?-B ]* [I-B]+  >:0 [ ?-B ]*

	Constraint.concatenate(identityPairMinusBracketsPlus).
			concatenate(RightBracketToEpsilon).
			concatenate(identity).
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

// Longest match RIGHT most
HfstTransducer longestMatchRightMostConstraint( HfstTransducer uncondidtionalTr )
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


	//[ ? | 0:< | >:0 | 0:> | B ]
	HfstTransducer nonClosingBracketInsertion(identityPair);
	nonClosingBracketInsertion.disjunct(epsilonToLeftBracket).
			disjunct(RightBracketToEpsilon).
			disjunct(epsilonToRightBracket).
			disjunct(B).
			minimize();


	// [ B:0 | 0:B | ?-B ]* [?-B]+ [ ? | 0:< | <:0 | 0:> | B ] 0:< [?-B]+ > ?*

	HfstTransducer Constraint(rightPart);
	Constraint.concatenate(identityPairMinusBracketsPlus).
			concatenate(nonClosingBracketInsertion).minimize().
			concatenate(epsilonToLeftBracket).
			concatenate(identityPairMinusBracketsPlus).
			concatenate(rightBracket).
			concatenate(identity).
			minimize();
	//printf("Constraint Longest Match: \n");
	//Constraint.write_in_att_format(stdout, 1);


	//uncondidtionalTr should be left most for the left most longest match
	HfstTransducer retval(TYPE);
	retval = constraintComposition(uncondidtionalTr, Constraint);


	return retval;
}

// Shortest match
// it should be composed to left most transducer........
// ?* < [?-B]+ >:0
// [?-B] or [ ? | 0:< | <:0 | >:0 | B ][?-B]+
// [ B:0 | 0:B | ?-B ]*
HfstTransducer shortestMatchLeftMostConstraint( HfstTransducer uncondidtionalTr )
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


	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	HfstTransducer rightPart(TYPE);
	rightPart = constraintsRightPart();

	// [?-B] and [?-B]+
	HfstTransducer B(leftBracket);
	B.disjunct(rightBracket).minimize();
	HfstTransducer identityPairMinusBrackets(identityPair);
	identityPairMinusBrackets.subtract(B).minimize();
	HfstTransducer identityPairMinusBracketsPlus(identityPairMinusBrackets);
	identityPairMinusBracketsPlus.repeat_plus().minimize();


	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", RightMarker, TOK, TYPE);
	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", LeftMarker, TOK, TYPE);


	// [ 0:< | <:0 | >:0 | B ][?-B]+
	HfstTransducer nonClosingBracketInsertion(epsilonToLeftBracket);
	nonClosingBracketInsertion.
			//disjunct(epsilonToLeftBracket).
			disjunct(LeftBracketToEpsilon).
			disjunct(RightBracketToEpsilon).
			disjunct(B).
			minimize();

	nonClosingBracketInsertion.concatenate(identityPairMinusBracketsPlus).minimize();

	HfstTransducer middlePart(identityPairMinusBrackets);
	middlePart.disjunct(nonClosingBracketInsertion).minimize();

//	printf("nonClosingBracketInsertion: \n");
//	nonClosingBracketInsertion.write_in_att_format(stdout, 1);

	// ?* < [?-B]+ >:0
	// [?-B] or [ ? | 0:< | <:0 | >:0 | B ][?-B]+
	//[ B:0 | 0:B | ?-B ]*
	HfstTransducer Constraint(identity);
	Constraint.concatenate(leftBracket).
			concatenate(identityPairMinusBracketsPlus).
			concatenate(RightBracketToEpsilon).
			concatenate(middlePart).minimize().
			concatenate(rightPart).
			minimize();

	//printf("Constraint Shortest Match: \n");
	//Constraint.write_in_att_format(stdout, 1);


	//uncondidtionalTr should be left most for the left most shortest match
	HfstTransducer retval(TYPE);
	retval = constraintComposition(uncondidtionalTr, Constraint);


	return retval;

}

// Shortest match
// it should be composed to left most transducer........
//[ B:0 | 0:B | ?-B ]*
// [?-B] or [?-B]+  [ ? | 0:> | >:0 | <:0 | B ]
// <:0 [?-B]+   > ?*
HfstTransducer shortestMatchRightMostConstraint( HfstTransducer uncondidtionalTr )
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


	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	HfstTransducer rightPart(TYPE);
	rightPart = constraintsRightPart();

	// [?-B] and [?-B]+
	HfstTransducer B(leftBracket);
	B.disjunct(rightBracket).minimize();
	HfstTransducer identityPairMinusBrackets(identityPair);
	identityPairMinusBrackets.subtract(B).minimize();
	HfstTransducer identityPairMinusBracketsPlus(identityPairMinusBrackets);
	identityPairMinusBracketsPlus.repeat_plus().minimize();



	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", RightMarker, TOK, TYPE);
	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, TYPE);
	HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", LeftMarker, TOK, TYPE);


	// [?-B]+ [ 0:> | >:0 | <:0 | B ]
	HfstTransducer nonClosingBracketInsertionTmp(epsilonToRightBracket);
	nonClosingBracketInsertionTmp.
			disjunct(RightBracketToEpsilon).
			disjunct(LeftBracketToEpsilon).
			disjunct(B).minimize();
	HfstTransducer nonClosingBracketInsertion(identityPairMinusBracketsPlus);
	nonClosingBracketInsertion.concatenate(nonClosingBracketInsertionTmp).minimize();

//	printf("nonClosingBracketInsertion: \n");
//	nonClosingBracketInsertion.write_in_att_format(stdout, 1);

	HfstTransducer middlePart(identityPairMinusBrackets);
	middlePart.disjunct(nonClosingBracketInsertion).minimize();


	//[ B:0 | 0:B | ?-B ]*
	// [?-B] or [?-B]+  [ ? | 0:> | >:0 | <:0 | B ]
	// <:0 [?-B]+   > ?*

	HfstTransducer Constraint(rightPart);
	Constraint.concatenate(middlePart).
			concatenate(LeftBracketToEpsilon).
			concatenate(identityPairMinusBracketsPlus).
			concatenate(rightBracket).
			concatenate(identity).
			minimize();

	//printf("Constraint Shortest Match: \n");
//Constraint.write_in_att_format(stdout, 1);


	//uncondidtionalTr should be left most for the left most longest match
	HfstTransducer retval(TYPE);
	retval = constraintComposition(uncondidtionalTr, Constraint);


	return retval;

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

// ?* [ BL:0 (?-B)* BR:0 ?* ]+
HfstTransducer mostBracketsStarConstraint( HfstTransducer uncondidtionalTr )
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



	// (? - B)*
	HfstTransducer identityPairMinusBracketsStar(identityPair);
	identityPairMinusBracketsStar.subtract(B).minimize().repeat_star().minimize();



	// repeatingPart [ BL:0 (?-B)* BR:0 ?* ]+
	HfstTransducer repeatingPart(allLeftBracketsToEpsilon);
	repeatingPart.concatenate(identityPairMinusBracketsStar).minimize();
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



//---------------------------------
//	REPLACE FUNCTIONS - INTERFACE
//---------------------------------




// replace up, left, right, down
HfstTransducer replace(	Rule rule, bool optional)
{
	HfstTransducer retval(TYPE);

	retval = bracketedReplace(rule, optional);

	//printf("replace tr: \n");
	//retval.write_in_att_format(stdout, 1);


	if ( !optional )
	{
		retval = mostBracketsPlusConstraint(retval);
		retval = removeB2Constraint(retval);

	}

	retval = removeMarkers( retval );

	return retval;
}


// left to right
HfstTransducer replace_leftmost_longest_match( Rule rule )
{

	HfstTransducer uncondidtionalTr(TYPE);
	uncondidtionalTr = bracketedReplace(rule, true);

	HfstTransducer retval (TYPE);
	retval = leftMostConstraint(uncondidtionalTr);

	//printf("leftMostConstraint: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = longestMatchLeftMostConstraint( retval );

	//printf("longestMatchLeftMostConstraint: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = removeMarkers( retval );

	return retval;
}
// right to left
HfstTransducer replace_rightmost_longest_match( Rule rule )
{

	HfstTransducer uncondidtionalTr(TYPE);
	uncondidtionalTr = bracketedReplace(rule, true);

	HfstTransducer retval (TYPE);
	retval = rightMostConstraint(uncondidtionalTr);

	//printf("rightMostConstraint: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = longestMatchRightMostConstraint( retval );

	//printf("longestMatchLeftMostConstraint: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = removeMarkers( retval );

	return retval;
}
HfstTransducer replace_leftmost_shortest_match( Rule rule)
{

	HfstTransducer uncondidtionalTr(TYPE);
	uncondidtionalTr = bracketedReplace(rule, true);

	HfstTransducer retval (TYPE);
	retval = leftMostConstraint(uncondidtionalTr);
	retval = shortestMatchLeftMostConstraint( retval );

	//printf("sh tr: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = removeMarkers( retval );


	return retval;
}
HfstTransducer replace_rightmost_shortest_match( Rule rule )
{

	HfstTransducer uncondidtionalTr(TYPE);
	uncondidtionalTr = bracketedReplace( rule, true);

	HfstTransducer retval (TYPE);
	retval = rightMostConstraint(uncondidtionalTr);
	retval = shortestMatchRightMostConstraint( retval );

	//printf("sh tr: \n");
	//retval.write_in_att_format(stdout, 1);

	retval = removeMarkers( retval );


	return retval;
}

HfstTransducer mark_up_replace(	Rule rule,
						StringPair marks,
						bool optional)
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	HfstTransducer retval(TYPE);

	HfstTransducer leftMark(marks.first, TOK, TYPE);
	HfstTransducer rightMark(marks.second, TOK, TYPE);

	HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", TOK, TYPE);
	epsilonToLeftMark.cross_product(leftMark).minimize();

	//printf("epsilonToLeftMark: \n");
	//epsilonToLeftMark.write_in_att_format(stdout, 1);


	HfstTransducer epsilonToRightMark("@_EPSILON_SYMBOL_@", TOK, TYPE);
	epsilonToRightMark.cross_product(rightMark).minimize();

	//printf("epsilonToRightMark: \n");
	//epsilonToRightMark.write_in_att_format(stdout, 1);

	// Mapping
	HfstTransducer mapping(epsilonToLeftMark);
	mapping.concatenate(rule.get_mapping()).
			minimize().
			concatenate(epsilonToRightMark).
			minimize();

	Rule newRule(mapping, rule.get_context(), rule.get_replType());
	//printf("epsilonToRightMark: \n");
	//epsilonToRightMark.write_in_att_format(stdout, 1);


	retval = replace(newRule, optional);

	return retval;
}

// replace up, left, right, down
HfstTransducer replace_epenthesis(	Rule rule,
						bool optional)
{
	HfstTransducer retval(TYPE);


	retval = bracketedReplace(rule, optional);



	//printf("bracketedReplace: \n");
	//retval.write_in_att_format(stdout, 1);

	// it can't have more than one epsilon repetition in a row
	retval = noRepetitionConstraint( retval );


	//printf("after no repet.: \n");
	//retval.write_in_att_format(stdout, 1);


	if ( !optional )
	{
		retval = mostBracketsStarConstraint(retval);

		//printf("after most bracket star: \n");
		//retval.write_in_att_format(stdout, 1);

		retval = removeB2Constraint(retval);

		//printf("after remove B2: \n");
		//retval.write_in_att_format(stdout, 1);
	}

	retval = removeMarkers( retval );

	//printf("after remove markers: \n");
	//retval.write_in_att_format(stdout, 1);
	return retval;

}


//---------------------------------
//	UNIT TESTS
//---------------------------------





// ab->x  ab_a
void test1()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("ab", TOK, TYPE);
	HfstTransducer rightMapping("x", TOK, TYPE);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context(HfstTransducer("ab", TOK, TYPE), HfstTransducer("a", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);


	HfstTransducer input1("abababa", TOK, TYPE);

	HfstTransducer result1("abababa", TOK, TYPE);
	HfstTransducer r1tmp("abababa", "abx@_EPSILON_SYMBOL_@aba", TOK, TYPE);
	HfstTransducer r2tmp("abababa", "ababx@_EPSILON_SYMBOL_@a", TOK, TYPE);
	HfstTransducer r3tmp("abababa", "abx@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@a", TOK, TYPE);
	result1.disjunct(r1tmp).disjunct(r2tmp).minimize().disjunct(r3tmp).minimize();


	Rule rule(mappingPair, ContextVector, REPL_UP);

	// Unconditional  optional replace
	HfstTransducer replaceTr(TYPE);
	replaceTr = replace(rule, true);

	HfstTransducer tmp2(TYPE);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("abababa optional: \n");
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


// a @-> x
void test1b()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, TYPE);
	HfstTransducer rightMapping("x", TOK, TYPE);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("aaana", TOK, TYPE);

//	HfstTransducer result1("aaana", TOK, TYPE);


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

	HfstTransducer result1(bt, TYPE);
	HfstTransducer result2("aaana", "xxxnx", TOK, TYPE);


	Rule rule(mappingPair, ContextVector, REPL_UP);

	// Unconditional  optional replace
	HfstTransducer replaceTr(TYPE);
	replaceTr = replace(rule, true);

	HfstTransducer tmp2(TYPE);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("aaana optional: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));


	// non optional
	replaceTr = replace(rule, false);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("left most: \n");
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


// a+ -> x || a _ a
// a+ -> x // a _ a
// a+ -> x \\ a _ a
// a+ -> x \/ a _ a
void test2a()
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
	HfstTransducer leftMapping("a", TOK, TYPE);
	leftMapping.repeat_plus().minimize();
	HfstTransducer rightMapping("x", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context(HfstTransducer("a", TOK, TYPE), HfstTransducer("a", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);


	HfstTransducer input1("aaaa", TOK, TYPE);
	HfstTransducer input2("aaaaabaaaa", TOK, TYPE);
	HfstTransducer input3("aaaaabaaaacaaaa", TOK, TYPE);

	// results:

	HfstTransducer result1("aaaa", TOK, TYPE);
	HfstTransducer r1tmp("aaaa","ax@_EPSILON_SYMBOL_@a",TOK, TYPE);
	HfstTransducer r2tmp("aaaa","axaa",TOK, TYPE);
	HfstTransducer r3tmp("aaaa","aaxa",TOK, TYPE);
	HfstTransducer r4tmp("aaaa","axxa",TOK, TYPE);

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


	HfstTransducer result4("aaaaabaaaa","ax@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@abax@_EPSILON_SYMBOL_@a",TOK, TYPE);
	HfstTransducer result5("aaaaabaaaa","axxxabaxxa",TOK, TYPE);

	HfstTransducer result6("aaaaabaaaacaaaa","ax@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@abax@_EPSILON_SYMBOL_@acax@_EPSILON_SYMBOL_@a",TOK, TYPE);
	HfstTransducer result7("aaaaabaaaacaaaa","axxxabaxxacaxxa",TOK, TYPE);





	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);





	HfstTransducer replaceTrUp(TYPE);
	HfstTransducer replaceTrLeft(TYPE);
	HfstTransducer replaceTrRight(TYPE);
	HfstTransducer replaceTrDown(TYPE);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);


	HfstTransducer tmp2(TYPE);
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

	HfstTransducer a(TYPE);

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

	HfstTransducer replaceTr(TYPE);
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

void test2b()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer aPlus("a", TOK, TYPE);
	aPlus.repeat_plus().minimize();
	HfstTransducer bPlus("b", TOK, TYPE);
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
	HfstTransducer rightMapping("x", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));
	//HfstTransducerPair Context(HfstTransducer("a", TOK, TYPE), HfstTransducer("a", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("aabbaa", TOK, TYPE);


	HfstTransducer result1("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@aa",TOK, TYPE);
	HfstTransducer result2("aabbaa", "aax@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@",TOK, TYPE);
	HfstTransducer result3("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@a",TOK, TYPE);
	HfstTransducer result4("aabbaa", "ax@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@",TOK, TYPE);




	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


	HfstTransducer replaceTr(TYPE);
	HfstTransducer tmp2(TYPE);

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

}



// test multiple contexts
// a -> b ||  x _ x ;
void test3a()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);


	// Mapping
	HfstTransducer leftMapping("a", TOK, TYPE);
	HfstTransducer rightMapping("b", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context( HfstTransducer("x",TOK, TYPE),  HfstTransducer("x",TOK, TYPE));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("xaxax", TOK, TYPE);

	HfstTransducer result1("xaxax", TOK, TYPE);
	HfstTransducer r1tmp("xaxax", "xbxax", TOK, TYPE);
	HfstTransducer r2tmp("xaxax", "xaxbx", TOK, TYPE);
	HfstTransducer r3tmp("xaxax", "xbxbx", TOK, TYPE);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();

	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(TYPE);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(TYPE);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}

// test multiple contexts
// a b -> b ||  x_y, y_z
void test3b()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, TYPE);
	leftMapping.repeat_plus().minimize();
	HfstTransducer rightMapping("b", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context( HfstTransducer("x",TOK, TYPE),  HfstTransducer("y",TOK, TYPE));
	HfstTransducerPair Context2( HfstTransducer("y",TOK, TYPE),  HfstTransducer("z", TOK, TYPE));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);
	ContextVector.push_back(Context2);

	HfstTransducer input1("axayaz", TOK, TYPE);

	HfstTransducer result1("axayaz", TOK, TYPE);
	HfstTransducer r1tmp("axayaz", "axbybz", TOK, TYPE);
	HfstTransducer r2tmp("axayaz", "axbyaz", TOK, TYPE);
	HfstTransducer r3tmp("axayaz", "axaybz", TOK, TYPE);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();


	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(TYPE);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(TYPE);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();

	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));
}

// test multiple contexts
// a+ -> x  || x x _ y y, y _ x
void test3c()

{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, TYPE);
	leftMapping.repeat_plus().minimize();
	HfstTransducer rightMapping("x", TOK, TYPE);


	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context( HfstTransducer("xx",TOK, TYPE),  HfstTransducer("yy",TOK, TYPE));
	HfstTransducerPair Context2( HfstTransducer("y",TOK, TYPE),  HfstTransducer("x", TOK, TYPE));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);
	ContextVector.push_back(Context2);

	HfstTransducer input1("axxayyax", TOK, TYPE);

	HfstTransducer result1("axxayyax", TOK, TYPE);
	HfstTransducer r1tmp("axxayyax", "axxayyxx", TOK, TYPE);
	HfstTransducer r2tmp("axxayyax", "axxxyyax", TOK, TYPE);
	HfstTransducer r3tmp("axxayyax", "axxxyyxx", TOK, TYPE);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();

	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(TYPE);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(TYPE);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();

	//printf("Unconditional optional replace: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}
// test multiple contexts
// a -> b ;
void test3d()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, TYPE);
	HfstTransducer rightMapping("b", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context( HfstTransducer("@_EPSILON_SYMBOL_@",TOK, TYPE),
								HfstTransducer("@_EPSILON_SYMBOL_@",TOK, TYPE));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("xaxax", TOK, TYPE);

	HfstTransducer result1("xaxax", TOK, TYPE);
	HfstTransducer r1tmp("xaxax", "xbxax", TOK, TYPE);
	HfstTransducer r2tmp("xaxax", "xaxbx", TOK, TYPE);
	HfstTransducer r3tmp("xaxax", "xbxbx", TOK, TYPE);
	result1.disjunct(r1tmp).disjunct(r2tmp).disjunct(r3tmp).minimize();


	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


	// Unconditional  optional replace
	HfstTransducer replaceTr(TYPE);
	replaceTr = replace(ruleUp, true);

	HfstTransducer tmp2(TYPE);
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
void test4a()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("b", TOK, TYPE);
	HfstTransducer rightMapping("a", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("a", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("bbba", TOK, TYPE);


	// results:
	HfstTransducer result1("bbba", "bbaa", TOK, TYPE);
	HfstTransducer result2("bbba", "aaaa", TOK, TYPE);
	HfstTransducer r1Tmp("bbba", "baaa", TOK, TYPE);
	HfstTransducer result3(input1);
	result3.disjunct(result1).minimize();


	HfstTransducer result4(result3);
	result4.disjunct(result2).minimize().disjunct(r1Tmp).minimize();




	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);





	HfstTransducer replaceTrUp(TYPE);
	HfstTransducer replaceTrLeft(TYPE);
	HfstTransducer replaceTrRight(TYPE);
	HfstTransducer replaceTrDown(TYPE);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);



	// Unconditional optional replace
	HfstTransducer tmp2(TYPE);
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
void test4b()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("b", TOK, TYPE);
	HfstTransducer rightMapping("a", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context(HfstTransducer("a", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE) );

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);




	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);


	HfstTransducer replaceTrUp(TYPE);
	HfstTransducer replaceTrLeft(TYPE);
	HfstTransducer replaceTrRight(TYPE);
	HfstTransducer replaceTrDown(TYPE);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);

	HfstTransducer input1("abbb", TOK, TYPE);

	// results:
	HfstTransducer result1("abbb", "aabb", TOK, TYPE);
	HfstTransducer result2("abbb", "aaaa", TOK, TYPE);
	HfstTransducer r1Tmp("abbb", "aaab", TOK, TYPE);
	HfstTransducer result3(input1);
	result3.disjunct(result1).minimize();

	HfstTransducer result4(result3);
	result4.disjunct(r1Tmp).minimize().disjunct(result2).minimize();


	// Unconditional optional replace
	HfstTransducer tmp2(TYPE);

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

void test4c()
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	// Mapping
	HfstTransducer leftMapping("ab", TOK, TYPE);
	HfstTransducer rightMapping("x", TOK, TYPE);

	HfstTransducerPair mappingPair(leftMapping, rightMapping);

	// Context
	HfstTransducerPair Context( HfstTransducer("ab",TOK, TYPE),  HfstTransducer("a",TOK, TYPE));


	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("abababa", TOK, TYPE);

	HfstTransducer result1("abababa", "abababa", TOK, TYPE);
	HfstTransducer r2tmp("abababa", "ababx@_EPSILON_SYMBOL_@a", TOK, TYPE);
	HfstTransducer r3tmp("abababa", "abx@_EPSILON_SYMBOL_@aba", TOK, TYPE);
	HfstTransducer r4tmp("abababa", "abx@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@a", TOK, TYPE);

	HfstTransducer result2(result1);
	result2.disjunct(r2tmp).disjunct(r3tmp).minimize();

	HfstTransducer result3(result2);
	result3.disjunct(r4tmp).minimize();

	HfstTransducer result4(r2tmp);
	result4.disjunct(r3tmp).minimize();



	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);


	HfstTransducer replaceTrUp(TYPE);
	HfstTransducer replaceTrLeft(TYPE);
	HfstTransducer replaceTrRight(TYPE);
	HfstTransducer replaceTrDown(TYPE);

	// Unconditional optional replace
	replaceTrUp 	= replace(ruleUp, true);
	replaceTrLeft 	= replace(ruleLeft, true);
	replaceTrRight	= replace(ruleRight, true);
	replaceTrDown	= replace(ruleDown, true);

	HfstTransducer tmp2(TYPE);

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
void test5()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer lmtmp("b", TOK, TYPE);
	HfstTransducer leftMapping("a", TOK, TYPE);
	leftMapping.disjunct(lmtmp).minimize();

	StringPair marks("[","]");


	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));

//	HfstTransducerPair Context(HfstTransducer("m", TOK, TYPE), HfstTransducer("k", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mba", TOK, TYPE);

	HfstTransducer result1("m@_EPSILON_SYMBOL_@b@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@a@_EPSILON_SYMBOL_@", "m[b][a]",TOK, TYPE);


	HfstTransducer replaceTr(TYPE);
	HfstTransducer tmp2(TYPE);

	Rule ruleUp(leftMapping, ContextVector, REPL_UP);

	replaceTr = mark_up_replace(ruleUp, marks, false);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}


// epenthesis rules
void test6a()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping

	HfstTransducer leftMapping("@_EPSILON_SYMBOL_@", TOK, TYPE);

	HfstTransducer rightMapping("p", TOK, TYPE);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);


	// Context
	HfstTransducerPair Context(HfstTransducer("m", TOK, TYPE), HfstTransducer("k", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mk", TOK, TYPE);

	HfstTransducer result1("m@_EPSILON_SYMBOL_@k", "mpk",TOK, TYPE);
	HfstTransducer result2("mk", "mk",TOK, TYPE);
	result2.disjunct(result1).minimize();



	Rule ruleUp(mappingPair, ContextVector, REPL_UP);

	HfstTransducer replaceTr(TYPE);
	HfstTransducer tmp2(TYPE);

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

void test6b()
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, TYPE);
	leftMapping.repeat_star().minimize();

//	printf("leftMapping: \n");
//	leftMapping.write_in_att_format(stdout, 1);

	HfstTransducer rightMapping("p", TOK, TYPE);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);


	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE),
							   HfstTransducer("@_EPSILON_SYMBOL_@", TOK, TYPE));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mak", TOK, TYPE);

	HfstTransducer result1("@_EPSILON_SYMBOL_@m@_EPSILON_SYMBOL_@a@_EPSILON_SYMBOL_@k@_EPSILON_SYMBOL_@",
							"pmpppkp", TOK, TYPE);


	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


	HfstTransducer replaceTr(TYPE);
	HfstTransducer tmp2(TYPE);

	// epsilon
	replaceTr = replace_epenthesis(ruleUp, false);


	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}
int main()
{

	// ab -> x  ab_a
	// also @-> and @>

	printf("--test1---\n\n");
	test1();
	test1b();

	// a+ -> x  a_a
	// also @-> and @>
	printf("--test2---\n\n");
	test2a();
	// >@ ->@
	test2b();

	// testing unconditional replace with and without contexts

	printf("--test3--- \n\n");
	test3a();

	test3b();
	test3c();
	test3d();

	// left - right - down tests
	// b -> a  || _a (r: bbaa)
	// b -> a  \\ _a (r:aaaa)
	// input: bbba
	printf("--test4---\n\n");
	test4a();
	test4b();
	test4c();

	// mark up rule
	printf("--test5---\n\n");
	test5();

	// epenthesis rules
	printf("--test6---\n\n");
	test6a();
	test6b();

	return 0;
}
