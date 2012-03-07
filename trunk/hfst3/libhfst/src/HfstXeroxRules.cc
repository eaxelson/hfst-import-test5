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

#include "HfstXeroxRules.h"

#ifndef MAIN_TEST


namespace hfst
{
  namespace xeroxRules
  {
  	  Rule::Rule ( const HfstTransducer &a_mapping )
	  {
		HfstTokenizer TOK;
		TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


		ImplementationType type = a_mapping.get_type();


		HfstTransducerPair contextPair(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type),
									   HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));
		HfstTransducerPairVector epsilonContext;
		epsilonContext.push_back(contextPair);


		mapping = a_mapping;
		context = epsilonContext;
		replType = REPL_UP;
	  }
	  Rule::Rule ( const HfstTransducer &a_mapping,
			  	   const HfstTransducerPairVector &a_context,
			  	   ReplaceType a_replType )
	  {

		for ( unsigned int i = 0; i < a_context.size(); i++ )
		 {
			if ( a_mapping.get_type() != a_context[i].first.get_type()
				|| 	a_mapping.get_type() != a_context[i].second.get_type()
				|| 	a_context[i].first.get_type() != a_context[i].second.get_type())
			{
				HFST_THROW_MESSAGE(TransducerTypeMismatchException, "Rule");
			}
		 }

		mapping = a_mapping;
		context = a_context;
		replType = a_replType;
	  }

	  Rule::Rule ( const HfstTransducerPair &mappingPair )
	  {
		HfstTokenizer TOK;
		TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

		if ( mappingPair.first.get_type() != mappingPair.second.get_type())
		{
			HFST_THROW_MESSAGE(TransducerTypeMismatchException, "Rule");
		}

		HfstTransducer tmpMapping(mappingPair.first);
		tmpMapping.cross_product(mappingPair.second).minimize();

		ImplementationType type = tmpMapping.get_type();

		HfstTransducerPair contextPair(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type),
									   HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));
		HfstTransducerPairVector epsilonContext;
		epsilonContext.push_back(contextPair);


		mapping = tmpMapping;
		context = epsilonContext;
		replType = REPL_UP;

	  }
	  Rule::Rule ( const HfstTransducerPair &mappingPair,
			  	   const HfstTransducerPairVector &a_context,
			  	   ReplaceType a_replType )
	  {

		if ( mappingPair.first.get_type() != mappingPair.second.get_type())
		{
		  HFST_THROW_MESSAGE(TransducerTypeMismatchException, "Rule");
		}


		for ( unsigned int i = 0; i < a_context.size(); i++ )
		{
			if ( mappingPair.first.get_type() != a_context[i].first.get_type()
				|| 	mappingPair.first.get_type() != a_context[i].second.get_type()
				|| 	a_context[i].first.get_type() != a_context[i].second.get_type())
			{
				HFST_THROW_MESSAGE(TransducerTypeMismatchException, "Rule");
			}
		}


		HfstTransducer tmpMapping(mappingPair.first);
		tmpMapping.cross_product(mappingPair.second).minimize();

		mapping = tmpMapping;
		context = a_context;
		replType = a_replType;
	  }

	  HfstTransducer Rule::get_mapping() const
	  {
		return mapping;
	  }

	  HfstTransducerPairVector Rule::get_context() const
	  {
		return context;
	  }
	  ReplaceType Rule::get_replType() const
	  {
		return replType;
	  }


	  ///////


	  MarkUpRule::MarkUpRule ( const HfstTransducer &a_mapping,
			  	  	  	  	   StringPair a_marks ):
	  		Rule(a_mapping), marks(a_marks)
	  {
	  	marks = a_marks;
	  }
	  MarkUpRule::MarkUpRule ( const HfstTransducer &a_mapping,
			  	  	  	  	   const HfstTransducerPairVector &a_contextVector,
			  	  	  	  	   ReplaceType a_replType,
			  	  	  	  	   StringPair a_marks ):
	  		Rule(a_mapping, a_contextVector, a_replType), marks(a_marks)
	  {
	  	marks = a_marks;
	  }
	  MarkUpRule::MarkUpRule ( const HfstTransducerPair &a_mapping,
			  	  	  	  	   StringPair a_marks ):
	  		Rule(a_mapping), marks(a_marks)
	  {
	  	marks = a_marks;
	  }
	  MarkUpRule::MarkUpRule ( const HfstTransducerPair &a_mapping,
			  	  	  	  	   const HfstTransducerPairVector &a_contextVector,
			  	  	  	  	   ReplaceType a_replType,
			  	  	  	  	   StringPair a_marks  ):
	  		Rule(a_mapping, a_contextVector, a_replType), marks(a_marks)
	  {
	  	marks = a_marks;
	  }

	  StringPair MarkUpRule::get_marks() const
	  {
	  	return marks;
	  }


	  //////////////////////////////////////



	  HfstTransducer disjunctVectorMembers( const HfstTransducerVector &trVector )
	  {
		  HfstTransducer retval( trVector[0] );
		  for ( unsigned int i = 1; i < trVector.size(); i++ )
		  {
			  retval.disjunct(trVector[i]).minimize();
		  }
		  return retval;
	  }








	  //////////////////////////////////////







	  HfstTransducer removeMarkers( const HfstTransducer &tr )
	  {
		String LeftMarker("@_LM_@");
		String RightMarker("@_RM_@");

		HfstTransducer retval(tr);

		retval.substitute(StringPair(LeftMarker, LeftMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();
		retval.substitute(StringPair(RightMarker, RightMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();


		retval.remove_from_alphabet(LeftMarker);
		retval.remove_from_alphabet(RightMarker);

		retval.minimize();

		//printf("tr without markers: \n");
		//tr.write_in_att_format(stdout, 1);
		return retval;
	  }

	  // tmp = t.1 .o. Constr .o. t.1
	  // (t.1 - tmp.2) .o. t
	  HfstTransducer constraintComposition( const HfstTransducer &t, const HfstTransducer &Constraint )
	  {
	  	HfstTransducer retval(t);
	  	retval.input_project().minimize();

	  	//printf("retval: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	HfstTransducer tmp(retval);

	  	tmp.compose(Constraint).minimize();
	  	//printf("first composition \n");
	  	//tmp.write_in_att_format(stdout, 1);
	  	tmp.compose(retval).minimize();
	  	//printf("tmp: \n");
	  	//tmp.write_in_att_format(stdout, 1);

	  	tmp.output_project().minimize();
	  	retval.subtract(tmp).minimize();

	  	retval.compose(t).minimize();

	  	return retval;
	  }

	  void insertFreelyAllTheBrackets( HfstTransducer &t, bool optional )
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

	  	ImplementationType type = t.get_type();

	  	HfstTransducer leftBracket(leftMarker, TOK, type);
	  	HfstTransducer rightBracket(rightMarker, TOK, type);


	  	t.insert_freely(leftBracket).minimize();
	  	t.insert_freely(rightBracket).minimize();

	  	if ( !optional )
	  	{
	  		HfstTransducer leftBracket2(leftMarker2, TOK, type);
	  		HfstTransducer rightBracket2(rightMarker2, TOK, type);

	  		t.insert_freely(leftBracket2).minimize();
	  		t.insert_freely(rightBracket2).minimize();
	  	}
	  	//return retval;

	  }



	  HfstTransducer expandContextsWithMapping (const HfstTransducerPairVector &ContextVector,
	  								const HfstTransducer &mappingWithBracketsAndTmpBoundary,
	  								const HfstTransducer &identityExpanded,
	  								ReplaceType replType,
	  								bool optional)
	  {

		ImplementationType type = identityExpanded.get_type();

	  	HfstTransducer unionContextReplace(type);

	  	for ( unsigned int i = 0; i < ContextVector.size(); i++ )
	  	{
			// Expand context with mapping
			// Cr' = (Rc .*) << Markers (<,>,|) .o. [I:I | <a:b>]*
			// Cr = Cr|Cr'
			// (same for left context)

			// Lc = (*. Lc) << {<,>}
			HfstTransducer firstContext(identityExpanded);
			firstContext.concatenate(ContextVector[i].first).minimize();

			insertFreelyAllTheBrackets( firstContext, optional );

			// Rc =  (Rc .*) << {<,>}
			HfstTransducer secondContext(ContextVector[i].second);
			secondContext.concatenate(identityExpanded).minimize();
			insertFreelyAllTheBrackets( secondContext, optional);

			/* RULE:	LC:		RC:
			 * up		up		up
			 * left		up		down
			 * right	down	up
			 * down		down	down
			 */

			HfstTransducer leftContextExpanded(type);
			HfstTransducer rightContextExpanded(type);

			// both contexts are in upper language
			if ( replType == REPL_UP)
			{

				// compose them with [I:I | <a:b>]*
				leftContextExpanded = firstContext;
				rightContextExpanded = secondContext;

				leftContextExpanded.compose(identityExpanded).minimize();
				rightContextExpanded.compose(identityExpanded).minimize();
			}
			// left context is in lower language, right in upper ( // )
			if ( replType == REPL_RIGHT )
			{
					// compose them with [I:I | <a:b>]*

				// left compose opposite way
				leftContextExpanded = identityExpanded;
				rightContextExpanded = secondContext;

				leftContextExpanded.compose(firstContext).minimize();
				rightContextExpanded.compose(identityExpanded).minimize();
			}
			// right context is in lower language, left in upper ( \\ )
			if ( replType == REPL_LEFT )
			{
				// compose them with [I:I | <a:b>]*
				leftContextExpanded = firstContext;
				rightContextExpanded = identityExpanded;

				leftContextExpanded.compose(identityExpanded).minimize();
				rightContextExpanded.compose(secondContext).minimize();
			}
			if ( replType == REPL_DOWN )
			{
				// compose them with [I:I | <a:b>]*
				leftContextExpanded = identityExpanded;
				rightContextExpanded = identityExpanded;

				leftContextExpanded.compose(firstContext).minimize();
				rightContextExpanded.compose(secondContext).minimize();
			}

			firstContext.disjunct(leftContextExpanded).minimize();
			secondContext.disjunct(rightContextExpanded).minimize();


	  		// put mapping between (expanded) contexts
	  		HfstTransducer oneContextReplace(firstContext);
	  		oneContextReplace.concatenate(mappingWithBracketsAndTmpBoundary).
	  					concatenate(secondContext);

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

	  HfstTransducer bracketedReplace( const Rule &rule, bool optional)
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


	  	HfstTransducer mapping( rule.get_mapping() );
	  	HfstTransducerPairVector ContextVector( rule.get_context() );
	  	ReplaceType replType( rule.get_replType() );

	  	ImplementationType type = mapping.get_type();





	  	HfstTransducer leftBracket(leftMarker, TOK, type);
	  	HfstTransducer rightBracket(rightMarker, TOK, type);
	  	HfstTransducer leftBracket2(leftMarker2, TOK, type);
	  	HfstTransducer rightBracket2(rightMarker2, TOK, type);
	  	HfstTransducer tmpBracket(tmpMarker, TOK, type);



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
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );

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
	  		HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
	  		if ( ContextVector[0].first.compare(epsilon) && ContextVector[0].second.compare(epsilon) )
	  		{
	  			//printf("context 1.1 je 0\n");
	  			return identityExpanded;
	  		}


	  	}




	  	// Surround mapping with tmp boudaries
	  	HfstTransducer mappingWithBracketsAndTmpBoundary(tmpBracket);
	  	mappingWithBracketsAndTmpBoundary.concatenate(mappingWithBrackets).concatenate(tmpBracket).minimize();


	  	//printf("mappingWithBracketsAndTmpBoundary: \n");
	  	//mappingWithBracketsAndTmpBoundary.write_in_att_format(stdout, 1);



	  	// .* |<a:b>| :*
	  	HfstTransducer bracketedReplace(identityExpanded);
	  	bracketedReplace.concatenate(mappingWithBracketsAndTmpBoundary).concatenate(identityExpanded).minimize();

	  	//printf("bracketedReplace: \n");
	  	//bracketedReplace.write_in_att_format(stdout, 1);









	  	// Expand all contexts with mapping taking in consideration replace type
	  	// result is their union
	  	HfstTransducer unionContextReplace(type);
	  	unionContextReplace = expandContextsWithMapping (ContextVector,
	  									 mappingWithBracketsAndTmpBoundary,
	  									 identityExpanded,
	  									 replType,
	  									 optional);



	  	//printf("unionContextReplace: \n");
	  	//unionContextReplace.write_in_att_format(stdout, 1);







	  	// subtract all mappings in contexts from replace without contexts
	  	HfstTransducer replaceWithoutContexts(bracketedReplace);
	  	replaceWithoutContexts.subtract(unionContextReplace).minimize();






	  	// remove tmpMaprker
	  	replaceWithoutContexts.substitute(StringPair(tmpMarker, tmpMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();
	  	replaceWithoutContexts.remove_from_alphabet(tmpMarker);
	  	replaceWithoutContexts.minimize();

	  	identityExpanded.remove_from_alphabet(tmpMarker);

	  	// final negation
	  	HfstTransducer uncondidtionalTr(identityExpanded);
	  	uncondidtionalTr.subtract(replaceWithoutContexts).minimize();

	  //printf("uncondidtionalTr: \n");
	  //uncondidtionalTr.write_in_att_format(stdout, 1);

	  	return uncondidtionalTr;

	  }



	  // bracketed replace for parallel rules
	  HfstTransducer parallelBracketedReplace( const vector<Rule> &ruleVector, bool optional)
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

	  	ImplementationType type = ruleVector[0].get_mapping().get_type();

	  	HfstTransducer leftBracket(leftMarker, TOK, type);
	  	HfstTransducer rightBracket(rightMarker, TOK, type);
	  	HfstTransducer leftBracket2(leftMarker2, TOK, type);
	  	HfstTransducer rightBracket2(rightMarker2, TOK, type);
	  	HfstTransducer tmpBracket(tmpMarker, TOK, type);


	  	// Identity pair
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );

	  	HfstTransducer identityExpanded(identityPair);
	  	identityExpanded.insert_to_alphabet(leftMarker);
	  	identityExpanded.insert_to_alphabet(rightMarker);
	  	identityExpanded.insert_to_alphabet(leftMarker2);
	  	identityExpanded.insert_to_alphabet(rightMarker2);
	  	identityExpanded.insert_to_alphabet(tmpMarker);
	  	// will be expanded with mappings


	  	HfstTransducerVector mappingWithBracketsVector;
	  	bool noContexts  = true;
	  	// go through vector and do everything for each rule
	  	for ( unsigned int i = 0; i < ruleVector.size(); i++ )
	  	{

	  		HfstTransducer mapping = ruleVector[i].get_mapping();
	  		HfstTransducerPairVector contextVector = ruleVector[i].get_context();
	  		//ReplaceType replaceType = ruleVector[i].get_replType();

	  		// when there aren't any contexts, result is identityExpanded
	  		if ( contextVector.size() == 1 )
	  		{
	  			HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
	  			if ( !(contextVector[0].first.compare(epsilon) && contextVector[0].second.compare(epsilon)) )
	  			{
	  				//printf("context 1.1 je 0\n");
	  				noContexts = false;
	  			}
	  		}

	  		// Surround mapping with brackets
	  		HfstTransducer mappingWithBrackets(leftBracket);
	  		mappingWithBrackets.concatenate(mapping).concatenate(rightBracket).minimize();


	  		// non - optional
	  		// mapping = T<a:b>T u T<2a:a>2T


	  		// non - optional
	  		// mapping = <a:b> u <2a:a>2
	  		if ( optional != true )
	  		{
	  			HfstTransducer mappingProject(mapping);
	  			mappingProject.input_project().minimize();

	  			HfstTransducer mappingWithBracketsNonOptional(leftBracket2);
	  			mappingWithBracketsNonOptional.concatenate(mappingProject).
	  											concatenate(rightBracket2).
	  											minimize();
	  			// mappingWithBrackets...... expanded
	  			mappingWithBrackets.disjunct(mappingWithBracketsNonOptional).minimize();
	  		}

	  		identityExpanded.disjunct(mappingWithBrackets).minimize();
	  		mappingWithBracketsVector.push_back(mappingWithBrackets);

	  	}

	  		identityExpanded.repeat_star().minimize();

	  		// if none of the rules have contexts, return identityExpanded
	  		if ( noContexts )
	  		{
	  			return identityExpanded;
	  		}



	  		// if they have contexts, process them

	  		HfstTransducer bracketedReplace(type);
	  		HfstTransducer unionContextReplace(type);


	  		if ( ruleVector.size() != mappingWithBracketsVector.size() )
	  		{
	  			//TODO: throw exception
	  			printf("\nparralelBracketedReplace -> error\n");
	  		}

	  		for ( unsigned int i = 0; i < ruleVector.size(); i++ )
	  		{

	  			// Surround mapping with brackets with tmp boudaries
	  			HfstTransducer mappingWithBracketsAndTmpBoundary(tmpBracket);
	  			mappingWithBracketsAndTmpBoundary.concatenate(mappingWithBracketsVector[i])
	  											.concatenate(tmpBracket)
	  											.minimize();

	  			// .* |<a:b>| :*
	  			HfstTransducer bracketedReplaceTmp(identityExpanded);
	  			bracketedReplaceTmp.concatenate(mappingWithBracketsAndTmpBoundary)
	  								.concatenate(identityExpanded)
	  								.minimize();

	  			bracketedReplace.disjunct(bracketedReplaceTmp).minimize();



	  			//Create context part
	  			HfstTransducer unionContextReplaceTmp(type);
	  			unionContextReplaceTmp = expandContextsWithMapping ( ruleVector[i].get_context(),
	  												 mappingWithBracketsAndTmpBoundary,
	  												 identityExpanded,
	  												 ruleVector[i].get_replType(),
	  												 optional);
	  			unionContextReplace.disjunct(unionContextReplaceTmp).minimize();


	  		}
	  	/////////////////////}






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



	  //---------------------------------
	  //	CONSTRAINTS
	  //---------------------------------

	  // (help function)
	  // returns: [ B:0 | 0:B | ?-B ]*
	  // which is used in some constraints
	  HfstTransducer constraintsRightPart( ImplementationType type )
	  {
	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	  	String LeftMarker("@_LM_@");
	  	String RightMarker("@_RM_@");
	  	TOK.add_multichar_symbol(LeftMarker);
	  	TOK.add_multichar_symbol(RightMarker);

	  	// Identity pair (normal)
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );


	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);

	  	// Create Right Part
	  	HfstTransducer B(leftBracket);
	  	B.disjunct(rightBracket).minimize();

	  	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", LeftMarker, TOK, type);
	  	HfstTransducer LeftMarkToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, type);

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
	  HfstTransducer leftMostConstraint( const HfstTransducer &uncondidtionalTr )
	  {
	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	  	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");

	  	String LeftMarker("@_LM_@");
	  	String RightMarker("@_RM_@");
	  	TOK.add_multichar_symbol(LeftMarker);
	  	TOK.add_multichar_symbol(RightMarker);

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);


	  	// Identity (normal)
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity (identityPair);
	  	identity.repeat_star().minimize();


	  	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	  	HfstTransducer rightPart(type);
	  	rightPart = constraintsRightPart(type);


	  	// epsilon
	  	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
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


	  	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, type);

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

	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);

	  //printf("Constraint: \n");
	  //Constraint.write_in_att_format(stdout, 1);
	  	return retval;

	  }

	  // [ B:0 | 0:B | ?-B ]* [I-B]+  >:0 [ ?-B ]*
	  HfstTransducer rightMostConstraint( const HfstTransducer &uncondidtionalTr )
	  {
	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	  	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");

	  	String LeftMarker("@_LM_@");
	  	String RightMarker("@_RM_@");
	  	TOK.add_multichar_symbol(LeftMarker);
	  	TOK.add_multichar_symbol(RightMarker);

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);


	  	// Identity (normal)
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity (identityPair);
	  	identity.repeat_star().minimize();


	  	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	  	HfstTransducer rightPart(type);
	  	rightPart = constraintsRightPart(type);


	  	// epsilon
	  	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
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


	  	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, type);

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

	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);


	  //printf("Constraint: \n");
	  //Constraint.write_in_att_format(stdout, 1);
	  	return retval;

	  }


	  // Longest match
	  // it should be composed to left most transducer........
	  // ?* < [?-B]+ 0:> [ ? | 0:< | <:0 | 0:> | B ] [ B:0 | 0:B | ?-B ]*
	  HfstTransducer longestMatchLeftMostConstraint( const HfstTransducer &uncondidtionalTr )
	  {

	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	  	String LeftMarker("@_LM_@");
	  	String RightMarker("@_RM_@");
	  	TOK.add_multichar_symbol(LeftMarker);
	  	TOK.add_multichar_symbol(RightMarker);

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);

	  	// Identity
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity(identityPair);
	  	identity.repeat_star().minimize();

	  	// epsilon
	  	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
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
	  	HfstTransducer rightPart(type);
	  	rightPart = constraintsRightPart(type);

	  	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", RightMarker, TOK, type);
	  	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", LeftMarker, TOK, type);

	  	//[ ? | 0:< | <:0 | 0:> | B ]
	 // 	HfstTransducer nonClosingBracketInsertion(identityPair);
	  	HfstTransducer nonClosingBracketInsertion(epsilonToLeftBracket);
	  	nonClosingBracketInsertion.
	  			//disjunct(epsilonToLeftBracket).
	  			disjunct(LeftBracketToEpsilon).
	  			disjunct(epsilonToRightBracket).
	  			disjunct(B).
	  			minimize();
	  	//	printf("nonClosingBracketInsertion: \n");
	  	//	nonClosingBracketInsertion.write_in_att_format(stdout, 1);


	  	nonClosingBracketInsertion.concatenate(identityPairMinusBracketsPlus).minimize();

	  	HfstTransducer middlePart(identityPairMinusBrackets);
	  	middlePart.disjunct(nonClosingBracketInsertion).minimize();


	  	// ?* < [?-B]+ 0:> [ ? | 0:< | <:0 | 0:> | B ] [?-B]+ [ B:0 | 0:B | ?-B ]*
	  	HfstTransducer Constraint(identity);
	  	Constraint.concatenate(leftBracket).
	  			concatenate(identityPairMinusBracketsPlus).
	  			concatenate(epsilonToRightBracket).
	  		//	concatenate(nonClosingBracketInsertion).
	  		//	concatenate(identityPairMinusBracketsPlus).
	  			concatenate(middlePart).
	  			concatenate(rightPart).
	  			minimize();
//printf("Constraint Longest Match: \n");
//Constraint.write_in_att_format(stdout, 1);


	  	//uncondidtionalTr should be left most for the left most longest match
	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);


	  	return retval;

	  }

	  // Longest match RIGHT most
	  HfstTransducer longestMatchRightMostConstraint(const HfstTransducer &uncondidtionalTr )
	  {
	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	  	String LeftMarker("@_LM_@");
	  	String RightMarker("@_RM_@");
	  	TOK.add_multichar_symbol(LeftMarker);
	  	TOK.add_multichar_symbol(RightMarker);

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);

	  	// Identity
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity(identityPair);
	  	identity.repeat_star().minimize();

	  	// epsilon
	  	HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
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
	  	HfstTransducer rightPart(type);
	  	rightPart = constraintsRightPart(type);



	  	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, type);

	  	HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", RightMarker, TOK, type);
	  	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", LeftMarker, TOK, type);


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
	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);


	  	return retval;
	  }

	  // Shortest match
	  // it should be composed to left most transducer........
	  // ?* < [?-B]+ >:0
	  // [?-B] or [ ? | 0:< | <:0 | >:0 | B ][?-B]+
	  // [ B:0 | 0:B | ?-B ]*
	  HfstTransducer shortestMatchLeftMostConstraint( const HfstTransducer &uncondidtionalTr )
	  {

	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	  	String LeftMarker("@_LM_@");
	  	String RightMarker("@_RM_@");
	  	TOK.add_multichar_symbol(LeftMarker);
	  	TOK.add_multichar_symbol(RightMarker);

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);

	  	// Identity
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity(identityPair);
	  	identity.repeat_star().minimize();


	  	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	  	HfstTransducer rightPart(type);
	  	rightPart = constraintsRightPart(type);

	  	// [?-B] and [?-B]+
	  	HfstTransducer B(leftBracket);
	  	B.disjunct(rightBracket).minimize();
	  	HfstTransducer identityPairMinusBrackets(identityPair);
	  	identityPairMinusBrackets.subtract(B).minimize();
	  	HfstTransducer identityPairMinusBracketsPlus(identityPairMinusBrackets);
	  	identityPairMinusBracketsPlus.repeat_plus().minimize();


	  	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", RightMarker, TOK, type);
	  	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", LeftMarker, TOK, type);


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
	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);


	  	return retval;

	  }

	  // Shortest match
	  // it should be composed to left most transducer........
	  //[ B:0 | 0:B | ?-B ]*
	  // [?-B] or [?-B]+  [ ? | 0:> | >:0 | <:0 | B ]
	  // <:0 [?-B]+   > ?*
	  HfstTransducer shortestMatchRightMostConstraint( const HfstTransducer &uncondidtionalTr )
	  {

	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	  	String LeftMarker("@_LM_@");
	  	String RightMarker("@_RM_@");
	  	TOK.add_multichar_symbol(LeftMarker);
	  	TOK.add_multichar_symbol(RightMarker);

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);

	  	// Identity
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity(identityPair);
	  	identity.repeat_star().minimize();


	  	// Create Right Part:  [ B:0 | 0:B | ?-B ]*
	  	HfstTransducer rightPart(type);
	  	rightPart = constraintsRightPart(type);

	  	// [?-B] and [?-B]+
	  	HfstTransducer B(leftBracket);
	  	B.disjunct(rightBracket).minimize();
	  	HfstTransducer identityPairMinusBrackets(identityPair);
	  	identityPairMinusBrackets.subtract(B).minimize();
	  	HfstTransducer identityPairMinusBracketsPlus(identityPairMinusBrackets);
	  	identityPairMinusBracketsPlus.repeat_plus().minimize();



	  	HfstTransducer RightBracketToEpsilon(RightMarker, "@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", RightMarker, TOK, type);
	  	HfstTransducer LeftBracketToEpsilon(LeftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
	  	HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", LeftMarker, TOK, type);


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
	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);


	  	return retval;

	  }


	  // ?* [ BL:0 (?-B)+ BR:0 ?* ]+
	  HfstTransducer mostBracketsPlusConstraint( const HfstTransducer &uncondidtionalTr )
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

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(leftMarker, TOK, type);
	  	HfstTransducer rightBracket(rightMarker, TOK, type);
	  	HfstTransducer leftBracket2(leftMarker2, TOK, type);
	  	HfstTransducer rightBracket2(rightMarker2, TOK, type);


	  	// Identity (normal)
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity (identityPair);
	  	identity.repeat_star().minimize();

	  	HfstTransducer identityPlus (identityPair);
	  	identityPlus.repeat_plus().minimize();


	  	HfstTransducer identityStar (identityPair);
	  	identityStar.repeat_star().minimize();


	  	// epsilon
	  	String epsilon("@_EPSILON_SYMBOL_@");

	  	// BL:0 ( <1 : 0, <2 : 0)
	  	HfstTransducer leftBracketToEpsilon(leftMarker, epsilon, TOK, type );
	  	HfstTransducer leftBracket2ToEpsilon(leftMarker2, epsilon, TOK, type );
	  	HfstTransducer allLeftBracketsToEpsilon(leftBracketToEpsilon);
	  	allLeftBracketsToEpsilon.disjunct(leftBracket2ToEpsilon).minimize();


	  //	printf("allLeftBracketsToEpsilon: \n");
	  //	allLeftBracketsToEpsilon.write_in_att_format(stdout, 1);

	  	// BR:0 ( >1 : 0, >2 : 0)
	  	HfstTransducer rightBracketToEpsilon(rightMarker, epsilon, TOK, type );
	  	HfstTransducer rightBracket2ToEpsilon(rightMarker2, epsilon, TOK, type );
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

	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);


	  //printf("After composition: \n");
	  //retval.write_in_att_format(stdout, 1);

	  	return retval;

	  }

	  // ?* [ BL:0 (?-B)* BR:0 ?* ]+
	  HfstTransducer mostBracketsStarConstraint( const HfstTransducer &uncondidtionalTr )
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

	  	ImplementationType type = uncondidtionalTr.get_type();

	  	HfstTransducer leftBracket(leftMarker, TOK, type);
	  	HfstTransducer rightBracket(rightMarker, TOK, type);

	  	HfstTransducer leftBracket2(leftMarker2, TOK, type);
	  	HfstTransducer rightBracket2(rightMarker2, TOK, type);


	  	// Identity (normal)
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
	  	HfstTransducer identity (identityPair);
	  	identity.repeat_star().minimize();

	  	HfstTransducer identityPlus (identityPair);
	  	identityPlus.repeat_plus().minimize();


	  	HfstTransducer identityStar (identityPair);
	  	identityStar.repeat_star().minimize();


	  	// epsilon
	  	String epsilon("@_EPSILON_SYMBOL_@");

	  	// BL:0 ( <1 : 0, <2 : 0)
	  	HfstTransducer leftBracketToEpsilon(leftMarker, epsilon, TOK, type );
	  	HfstTransducer leftBracket2ToEpsilon(leftMarker2, epsilon, TOK, type );
	  	HfstTransducer allLeftBracketsToEpsilon(leftBracketToEpsilon);
	  	allLeftBracketsToEpsilon.disjunct(leftBracket2ToEpsilon).minimize();


	  	//	printf("allLeftBracketsToEpsilon: \n");
	  	//	allLeftBracketsToEpsilon.write_in_att_format(stdout, 1);

	  	// BR:0 ( >1 : 0, >2 : 0)
	  	HfstTransducer rightBracketToEpsilon(rightMarker, epsilon, TOK, type );
	  	HfstTransducer rightBracket2ToEpsilon(rightMarker2, epsilon, TOK, type );
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

	  	HfstTransducer retval(type);
	  	retval = constraintComposition(uncondidtionalTr, Constraint);


	  	//printf("After composition: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	return retval;

	  }
	  // ?* B2 ?*
	  HfstTransducer removeB2Constraint( const HfstTransducer &t )
	  {
	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	  	String leftMarker2("@_LM2_@");
	  	String rightMarker2("@_RM2_@");

	  	TOK.add_multichar_symbol(leftMarker2);
	  	TOK.add_multichar_symbol(rightMarker2);

	  	ImplementationType type = t.get_type();

	  	HfstTransducer leftBracket2(leftMarker2, TOK, type);
	  	HfstTransducer rightBracket2(rightMarker2, TOK, type);

	  	// Identity (normal)
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );
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

	  	HfstTransducer retval(type);
	  	retval = constraintComposition(t, Constraint);


	  	retval.remove_from_alphabet(leftMarker2);
	  	retval.remove_from_alphabet(rightMarker2);

	  	//printf("Remove B2 After composition: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	return retval;

	  }
	  // to avoid repetition in empty replace rule
	  HfstTransducer noRepetitionConstraint( const HfstTransducer &t )
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

	  	ImplementationType type = t.get_type();

	  	HfstTransducer leftBracket(LeftMarker, TOK, type);
	  	HfstTransducer rightBracket(RightMarker, TOK, type);

	  	HfstTransducer leftBracket2(leftMarker2, TOK, type);
	  	HfstTransducer rightBracket2(rightMarker2, TOK, type);


	  	HfstTransducer leftBrackets(leftBracket);
	  	leftBrackets.disjunct(leftBracket2).minimize();

	  	HfstTransducer rightBrackets(rightBracket);
	  	rightBrackets.disjunct(rightBracket2).minimize();

	  	// Identity (normal)
	  	HfstTransducer identityPair = HfstTransducer::identity_pair( type );

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

	  	HfstTransducer retval(type);
	  	retval = constraintComposition(t, Constraint);


	  	return retval;

	  }
		//---------------------------------
		//	INTERFACE HELPING FUNCTIONS
		//---------------------------------

	  HfstTransducer create_mapping_for_mark_up_replace( const HfstTransducer &leftMapping,
														  const StringPair &marks )
	  {
		HfstTokenizer TOK;
		TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

		ImplementationType type = leftMapping.get_type();

		HfstTransducer leftMark(marks.first, TOK, type);
		HfstTransducer rightMark(marks.second, TOK, type);

		HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", TOK, type);
		epsilonToLeftMark.cross_product(leftMark).minimize();

		//printf("epsilonToLeftMark: \n");
		//epsilonToLeftMark.write_in_att_format(stdout, 1);


		HfstTransducer epsilonToRightMark("@_EPSILON_SYMBOL_@", TOK, type);
		epsilonToRightMark.cross_product(rightMark).minimize();

		//printf("epsilonToRightMark: \n");
		//epsilonToRightMark.write_in_att_format(stdout, 1);

		// Mapping
		HfstTransducer retval(epsilonToLeftMark);
		retval.concatenate(leftMapping).
				minimize().
				concatenate(epsilonToRightMark).
				minimize();

		return retval;

	  }
	  HfstTransducer create_mapping_for_mark_up_replace( const HfstTransducer &leftMapping,
															  const HfstTransducerPair &marks )
	  {
		HfstTokenizer TOK;
		TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

		ImplementationType type = leftMapping.get_type();

		HfstTransducer leftMark(marks.first);
		HfstTransducer rightMark(marks.second);

		HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", TOK, type);
		epsilonToLeftMark.cross_product(leftMark).minimize();

		//printf("epsilonToLeftMark: \n");
		//epsilonToLeftMark.write_in_att_format(stdout, 1);


		HfstTransducer epsilonToRightMark("@_EPSILON_SYMBOL_@", TOK, type);
		epsilonToRightMark.cross_product(rightMark).minimize();

		//printf("epsilonToRightMark: \n");
		//epsilonToRightMark.write_in_att_format(stdout, 1);

		// Mapping
		HfstTransducer retval(epsilonToLeftMark);
		retval.concatenate(leftMapping).
				minimize().
				concatenate(epsilonToRightMark).
				minimize();
		return retval;

	  }

	  //---------------------------------
	  //	REPLACE FUNCTIONS - INTERFACE
	  //---------------------------------




	  // replace up, left, right, down
	  HfstTransducer replace( const Rule &rule, bool optional)
	  {

	  	HfstTransducer retval( bracketedReplace(rule, optional) );

	  //	retval = bracketedReplace(rule, optional);

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

	  // for parallel rules
	  HfstTransducer replace( const vector<Rule> &ruleVector, bool optional)
	  {
		  HfstTransducer retval;
		  // If there is only one rule in the vector, it is not parallel
			if ( ruleVector.size() == 1 )
			{
				retval = bracketedReplace( ruleVector[0], optional) ;
			}
			else
			{
				retval = parallelBracketedReplace(ruleVector, optional);

			}
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


	  // replace left
	  HfstTransducer replace_left( const Rule &rule, bool optional)
	  {
		  HfstTransducer newMapping = rule.get_mapping();
		  newMapping.invert().minimize();
		  Rule newRule ( newMapping, rule.get_context(), rule.get_replType());
		  return replace( newRule, optional);
	  }
	  // replace left parallel
	  HfstTransducer replace_left( const vector<Rule> &ruleVector, bool optional)
	  {
		  return replace( ruleVector, optional).invert();
	  }

	  // left to right
	  HfstTransducer replace_leftmost_longest_match( const Rule &rule )
	  {

	  	HfstTransducer uncondidtionalTr( bracketedReplace(rule, true) );
	  	//uncondidtionalTr = bracketedReplace(rule, true);

	  	//printf("uncondidtionalTr: \n");
	  	//uncondidtionalTr.write_in_att_format(stdout, 1);



	  	HfstTransducer retval (leftMostConstraint(uncondidtionalTr));
	  	//retval = leftMostConstraint(uncondidtionalTr);

	  	//printf("leftMostConstraint: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = longestMatchLeftMostConstraint( retval );

	  	//printf("longestMatchLeftMostConstraint: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );

	  	return retval;
	  }
	  // left to right
	  HfstTransducer replace_leftmost_longest_match( const vector<Rule> &ruleVector )
	  {

		HfstTransducer uncondidtionalTr;
		if ( ruleVector.size() == 1 )
		{
			uncondidtionalTr = bracketedReplace( ruleVector[0], true) ;
		}
		else
		{
			uncondidtionalTr = parallelBracketedReplace(ruleVector, true);
		}



	  	HfstTransducer retval (leftMostConstraint(uncondidtionalTr));
	  	//retval = leftMostConstraint(uncondidtionalTr);

		//printf("leftMostConstraint: \n");
		//retval.write_in_att_format(stdout, 1);

	  	retval = longestMatchLeftMostConstraint( retval );

	  	//printf("longestMatchLeftMostConstraint: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );

	  	return retval;
	  }


	  // right to left
	  HfstTransducer replace_rightmost_longest_match( const Rule &rule )
	  {

	  	HfstTransducer uncondidtionalTr(bracketedReplace(rule, true));
	  	//uncondidtionalTr = bracketedReplace(rule, true);

	  	HfstTransducer retval (rightMostConstraint(uncondidtionalTr));
	  	//retval = rightMostConstraint(uncondidtionalTr);

	  	//printf("rightMostConstraint: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = longestMatchRightMostConstraint( retval );

	  	//printf("longestMatchLeftMostConstraint: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );

	  	return retval;
	  }


	  // right to left
	  HfstTransducer replace_rightmost_longest_match( const vector<Rule> &ruleVector )
	  {
		HfstTransducer uncondidtionalTr;
		if ( ruleVector.size() == 1 )
		{
			uncondidtionalTr = bracketedReplace( ruleVector[0], true) ;
		}
		else
		{
			uncondidtionalTr = parallelBracketedReplace(ruleVector, true);
		}


	  	HfstTransducer retval (rightMostConstraint(uncondidtionalTr));
	  	//retval = rightMostConstraint(uncondidtionalTr);

	  	//printf("rightMostConstraint: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = longestMatchRightMostConstraint( retval );

	  	//printf("longestMatchLeftMostConstraint: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );

	  	return retval;
	  }

	  HfstTransducer replace_leftmost_shortest_match( const Rule &rule)
	  {

	  	HfstTransducer uncondidtionalTr(bracketedReplace(rule, true));
	  //	uncondidtionalTr = bracketedReplace(rule, true);

	  	HfstTransducer retval (leftMostConstraint(uncondidtionalTr));
	  	//retval = leftMostConstraint(uncondidtionalTr);
	  	retval = shortestMatchLeftMostConstraint( retval );

	  	//printf("sh tr: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );


	  	return retval;
	  }


	  HfstTransducer replace_leftmost_shortest_match(const vector<Rule> &ruleVector )
	  {
		HfstTransducer uncondidtionalTr;
		if ( ruleVector.size() == 1 )
		{
			uncondidtionalTr = bracketedReplace( ruleVector[0], true) ;
		}
		else
		{
			uncondidtionalTr = parallelBracketedReplace(ruleVector, true);
		}


	  	HfstTransducer retval (leftMostConstraint(uncondidtionalTr));
	  	//retval = leftMostConstraint(uncondidtionalTr);
	  	retval = shortestMatchLeftMostConstraint( retval );

	  	//printf("sh tr: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );


	  	return retval;
	  }

	  HfstTransducer replace_rightmost_shortest_match( const Rule &rule )
	  {

	  	HfstTransducer uncondidtionalTr(bracketedReplace( rule, true));
	  	//uncondidtionalTr = bracketedReplace( rule, true);

	  	HfstTransducer retval (rightMostConstraint(uncondidtionalTr));
	  	//retval = rightMostConstraint(uncondidtionalTr);
	  	retval = shortestMatchRightMostConstraint( retval );

	  	//printf("sh tr: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );


	  	return retval;
	  }

	  HfstTransducer replace_rightmost_shortest_match( const vector<Rule> &ruleVector )
	  {


		HfstTransducer uncondidtionalTr;
		if ( ruleVector.size() == 1 )
		{
			uncondidtionalTr = bracketedReplace( ruleVector[0], true) ;
		}
		else
		{
			uncondidtionalTr = parallelBracketedReplace(ruleVector, true);
		}


	  	HfstTransducer retval (rightMostConstraint(uncondidtionalTr));
	  	//retval = rightMostConstraint(uncondidtionalTr);
	  	retval = shortestMatchRightMostConstraint( retval );

	  	//printf("sh tr: \n");
	  	//retval.write_in_att_format(stdout, 1);

	  	retval = removeMarkers( retval );


	  	return retval;
	  }



	  HfstTransducer mark_up_replace(	const Rule &rule,
	  						const StringPair &marks,
	  						bool optional)
	  {

		HfstTransducer mapping = create_mapping_for_mark_up_replace(rule.get_mapping(), marks);
	  	Rule newRule(mapping, rule.get_context(), rule.get_replType());
	  	//printf("epsilonToRightMark: \n");
	  	//epsilonToRightMark.write_in_att_format(stdout, 1);

	  	HfstTransducer retval(replace(newRule, optional));


	  	return retval;
	  }
	  HfstTransducer mark_up_replace(	const Rule &rule,
	 	  						const HfstTransducerPair &marks,
	 	  						bool optional)
	 {
		  HfstTransducer mapping = create_mapping_for_mark_up_replace(rule.get_mapping(), marks);

	 	  	Rule newRule(mapping, rule.get_context(), rule.get_replType());
	 	  	//printf("epsilonToRightMark: \n");
	 	  	//epsilonToRightMark.write_in_att_format(stdout, 1);

	 	  	HfstTransducer retval(replace(newRule, optional));


	 	  	return retval;
	 	  }


	  // TODO:
	  HfstTransducer mark_up_replace(	const vector<MarkUpRule> &markUpRuleVector,
	  						bool optional)
	  {
	  	HfstTokenizer TOK;
	  	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	  	ImplementationType type = markUpRuleVector[0].get_mapping().get_type();
	  	vector<Rule> ruleVector;

	  	for ( unsigned int i = 0; i < markUpRuleVector.size(); i++ )
	  	{
	  		HfstTransducer leftMark(markUpRuleVector[i].get_marks().first, TOK, type);
	  		HfstTransducer rightMark(markUpRuleVector[i].get_marks().second, TOK, type);

	  		//printf("leftMark: \n");
	  		//leftMark.write_in_att_format(stdout, 1);

	  		//printf("rightMark: \n");
	  		//rightMark.write_in_att_format(stdout, 1);



	  		HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", TOK, type);
	  		epsilonToLeftMark.cross_product(leftMark).minimize();

	  		HfstTransducer epsilonToRightMark("@_EPSILON_SYMBOL_@", TOK, type);
	  		epsilonToRightMark.cross_product(rightMark).minimize();

	  		// Create mapping
	  		HfstTransducer mapping(epsilonToLeftMark);
	  		mapping.concatenate(markUpRuleVector[i].get_mapping()).
	  				minimize().
	  				concatenate(epsilonToRightMark).
	  				minimize();

	  		//Rule newRule(mapping, markUpRuleVector[i].get_context(), markUpRuleVector[i].get_replType());
	  		Rule newRule(mapping);

	  		// create parallel rule vector and call replace f w it
	  		// without context
	  		ruleVector.push_back(newRule);
	  	}
	  	HfstTransducer retval(replace(ruleVector, optional));


	  	return retval;

	  }


	  // replace up, left, right, down
	  HfstTransducer replace_epenthesis(	const Rule &rule, bool optional)
	  {
	  	HfstTransducer retval(bracketedReplace(rule, optional));

	  	//retval = bracketedReplace(rule, optional);

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

	  // replace up, left, right, down
	  HfstTransducer replace_epenthesis(	const vector<Rule> &ruleVector, bool optional)
	  {
	  	HfstTransducer retval(parallelBracketedReplace(ruleVector, optional));


	  	//retval = parallelBracketedReplace(ruleVector, optional);

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
























  }
}
#else

using namespace hfst;
using namespace implementations;
using namespace hfst::xeroxRules;





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

	HfstTransducer tmp2(type);
	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("abba optional: \n");
	//tmp2.write_in_att_format(stdout, 1);
	assert(tmp2.compare(result1));

}

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


	Rule rule(mappingPair, ContextVector, REPL_UP);

	// Unconditional  optional replace
	HfstTransducer replaceTr(type);
	replaceTr = replace(rule, true);

	HfstTransducer tmp2(type);
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



// a -> x
void test1b( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer leftMapping("a", TOK, type);
	HfstTransducer rightMapping("x", TOK, type);
	HfstTransducerPair mappingPair(leftMapping, rightMapping);

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


	Rule rule(mappingPair, ContextVector, REPL_UP);

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





	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);





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

	HfstTransducer input1("aabbaa", TOK, type);

	HfstTransducer result1("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@aa",TOK, type);
	HfstTransducer result2("aabbaa", "aax@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@",TOK, type);
	HfstTransducer result3("aabbaa", "x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@a",TOK, type);
	HfstTransducer result4("aabbaa", "ax@_EPSILON_SYMBOL_@x@_EPSILON_SYMBOL_@@_EPSILON_SYMBOL_@",TOK, type);




	Rule ruleUp(mappingPair);


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

	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);

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

	// Context
	HfstTransducerPair Context(HfstTransducer("c", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);


	HfstTransducer input1("caav", TOK, type);

	// results:

	HfstTransducer result1("caav", "cx@_EPSILON_SYMBOL_@v", TOK, type);


	Rule ruleUp(mappingPair, ContextVector, REPL_UP);

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

	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


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


	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


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

	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


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


	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


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




	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);





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

	// Context
	HfstTransducerPair Context(HfstTransducer("a", TOK, type), HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type) );

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);




	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);


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



	Rule ruleUp(mappingPair, ContextVector, REPL_UP);
	Rule ruleLeft(mappingPair, ContextVector, REPL_LEFT);
	Rule ruleRight(mappingPair, ContextVector, REPL_RIGHT);
	Rule ruleDown(mappingPair, ContextVector, REPL_DOWN);


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

	String LeftMarker("@_LM_@");
	String RightMarker("@_RM_@");
	TOK.add_multichar_symbol(LeftMarker);
	TOK.add_multichar_symbol(RightMarker);

	// Mapping
	HfstTransducer lmtmp("b", TOK, type);
	HfstTransducer leftMapping("a", TOK, type);
	leftMapping.disjunct(lmtmp).minimize();

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

	Rule ruleUp(leftMapping, ContextVector, REPL_UP);

	replaceTr = mark_up_replace(ruleUp, marks, false);

	tmp2 = input1;
	tmp2.compose(replaceTr).minimize();
	//printf("Replace leftmost tr2: \n");
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


	// Context
	HfstTransducerPair Context(HfstTransducer("m", TOK, type), HfstTransducer("k", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mk", TOK, type);

	HfstTransducer result1("m@_EPSILON_SYMBOL_@k", "mpk",TOK, type);
	HfstTransducer result2("mk", "mk",TOK, type);
	result2.disjunct(result1).minimize();



	Rule ruleUp(mappingPair, ContextVector, REPL_UP);

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


	// Context
	HfstTransducerPair Context(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type),
							   HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));

	HfstTransducerPairVector ContextVector;
	ContextVector.push_back(Context);

	HfstTransducer input1("mak", TOK, type);

	HfstTransducer result1("@_EPSILON_SYMBOL_@m@_EPSILON_SYMBOL_@a@_EPSILON_SYMBOL_@k@_EPSILON_SYMBOL_@",
							"pmpppkp", TOK, type);


	Rule ruleUp(mappingPair, ContextVector, REPL_UP);


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


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);



	// without context
	Rule rule1(mapping1);
	Rule rule2(mapping2);

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


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);


	// without context
	Rule rule1(mapping1);
	Rule rule2(mapping2);

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


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);

	//printf("mapping1: \n");
	//mapping1.write_in_att_format(stdout, 1);

	//printf("mapping2: \n");
	//mapping2.write_in_att_format(stdout, 1);

	// without context
	Rule rule1(mapping1);
	Rule rule2(mapping2);

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
	Rule rule2aUp(mapping1, ContextVector1, REPL_UP);
	Rule rule2bUp(mapping2, ContextVector2, REPL_UP);

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

	Rule rule2aDown(mapping1, ContextVector1, REPL_DOWN);
	Rule rule2bDown(mapping2, ContextVector2, REPL_DOWN);

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
	 *    0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ] ;
	 */

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
	Rule rule1(mapping1, ContextVector1, REPL_DOWN);
	Rule rule2(mapping2, ContextVector2, REPL_DOWN);

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


void test7e( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");


	// Mapping

	HfstTransducer leftMapping1("a", TOK, type);


	HfstTransducer leftMapping2("b", TOK, type);


	StringPair marks1("[","]");
	StringPair marks2("|","|");

	// without context
	Rule rule1(leftMapping1);
	Rule rule2(leftMapping2);


	MarkUpRule markUpRule1(leftMapping1, marks1 );
	MarkUpRule markUpRule2(leftMapping2, marks2 );

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


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);



	// without context
	Rule rule1(mapping1);
	Rule rule2(mapping2);

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


	HfstTransducer mapping1(mappingPair1.first);
	mapping1.cross_product(mappingPair1.second);

	HfstTransducer mapping2(mappingPair2.first);
	mapping2.cross_product(mappingPair2.second);



	// without context
	Rule rule1(mapping1);
	Rule rule2(mapping2);

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

int main(int argc, char * argv[])
	  {
	      std::cout << "Unit tests for " __FILE__ ":" << std::endl;


	      ImplementationType types[] = {SFST_TYPE, TROPICAL_OPENFST_TYPE, FOMA_TYPE};
	      unsigned int NUMBER_OF_TYPES=3;

	      for (unsigned int i=0; i < NUMBER_OF_TYPES; i++)
	      {
	        if (! HfstTransducer::is_implementation_type_available(types[i]))
	  	continue;

			test1( types[i] );
		//	test1b( types[i]);

			// a+ -> x  a_a
			// also @-> and @>
			test2a( types[i] );
			// >@ ->@
			test2b( types[i] );
			test2c( types[i] );

			// testing unconditional replace with and without contexts


			test3a( types[i] );
			test3b( types[i] );
			test3c( types[i] );
			test3d( types[i] );

			// left - right - down tests
			// b -> a  || _a (r: bbaa)
			// b -> a  \\ _a (r:aaaa)
			// input: bbba
			test4a( types[i] );
			test4b( types[i] );
			test4c( types[i] );

			// mark up rule
			test5( types[i] );

			// epenthesis rules
			test6a( types[i] );
			test6b( types[i] );

			//parralel rules
			// a -> b , b -> c
			test7a( types[i] );
			// [. .] -> b , a -> c ;
			test7b( types[i] );
			// a @-> x , b @-> y
			// a @> x , b @> y
			// a -> x \/ m _ ,, b -> y || x _
			test7c( types[i] );
			// 0 .o. [ [. 0 .] -> a \/ _ b a , a b _ ,, [. 0 .] -> b \/ a _ a ]
			test7d( types[i] );
			test7e( types[i] );
			// a -> b, b -> a
			test7f( types[i] );

			// a -> b b , a -> b
			test7g( types[i] );


			test8( types[i] );


	      }



	  /*
	   *
	   */
	      std::cout << "ok" << std::endl;
	      return 0;
	  }
#endif // MAIN_TEST
