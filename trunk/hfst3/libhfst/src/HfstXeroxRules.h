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


//#include "HfstDataTypes.h"
//#include "HfstSymbolDefs.h"
#include "HfstTransducer.h"

namespace hfst
{
	namespace xeroxRules
	{
		enum ReplaceType {REPL_UP, REPL_DOWN, REPL_RIGHT, REPL_LEFT};

		enum ReplaceArrow {E_REPLACE_RIGHT,
						   E_REPLACE_RIGHT_MARKUP,
						   E_OPTIONAL_REPLACE_RIGHT,
						   E_RTL_LONGEST_MATCH,
						   E_RTL_SHORTEST_MATCH,
						   E_LTR_LONGEST_MATCH,
						   E_LTR_SHORTEST_MATCH};

		//ImplementationType TYPE = TROPICAL_OPENFST_TYPE;
		//ImplementationType TYPE = SFST_TYPE;
		//ImplementationType TYPE = FOMA_TYPE;



		class Rule
		{
			HfstTransducer mapping;
			HfstTransducerPairVector context;
			ReplaceType replType;

		  public:
			Rule ( const HfstTransducer& ); // mapping
			Rule ( const HfstTransducer&, const HfstTransducerPairVector&, ReplaceType); // mapping, context
			Rule ( const HfstTransducerPair& );
			Rule ( const HfstTransducerPair&, const HfstTransducerPairVector&, ReplaceType );

			HfstTransducer get_mapping() const;
			HfstTransducerPairVector get_context() const;
			ReplaceType get_replType() const;
		};

		class MarkUpRule : public Rule
		{
			StringPair marks;

		  public:
			// for mark up replace

			MarkUpRule ( const HfstTransducer&, StringPair ); // mapping
			MarkUpRule ( const HfstTransducer&,  const HfstTransducerPairVector&, ReplaceType, StringPair); // mapping, context
			MarkUpRule ( const HfstTransducerPair&, StringPair );
			MarkUpRule ( const HfstTransducerPair&, const HfstTransducerPairVector&, ReplaceType, StringPair );
			StringPair get_marks() const;
		};












		HfstTransducer removeMarkers( const HfstTransducer &tr );

		HfstTransducer constraintComposition( const HfstTransducer &t, const HfstTransducer &Constraint );

		void insertFreelyAllTheBrackets( HfstTransducer &t, bool optional );

		HfstTransducer expandContextsWithMapping (const HfstTransducerPairVector &ContextVector,
										const HfstTransducer &mappingWithBracketsAndTmpBoundary,
										const HfstTransducer &identityExpanded,
										ReplaceType replType,
										bool optional);


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
		HfstTransducer bracketedReplace( const Rule &rule, bool optional);

		// bracketed replace for parallel rules
		HfstTransducer parallelBracketedReplace( const vector<Rule> &ruleVector, bool optional);




		//---------------------------------
		//	CONSTRAINTS
		//---------------------------------

		// (help function)
		// returns: [ B:0 | 0:B | ?-B ]*
		// which is used in some constraints
		HfstTransducer constraintsRightPart( ImplementationType type );

		// ?* <:0 [B:0]* [I-B] [ B:0 | 0:B | ?-B ]*
		HfstTransducer leftMostConstraint( const HfstTransducer &uncondidtionalTr );

		// [ B:0 | 0:B | ?-B ]* [I-B]+  >:0 [ ?-B ]*
		HfstTransducer rightMostConstraint( const HfstTransducer &uncondidtionalTr );

		// Longest match
		// it should be composed to left most transducer........
		// ?* < [?-B]+ 0:> [ ? | 0:< | <:0 | 0:> | B ] [ B:0 | 0:B | ?-B ]*
		HfstTransducer longestMatchLeftMostConstraint( const HfstTransducer &uncondidtionalTr );


		// Longest match RIGHT most
		HfstTransducer longestMatchRightMostConstraint(const HfstTransducer &uncondidtionalTr );


		// Shortest match
		// it should be composed to left most transducer........
		// ?* < [?-B]+ >:0
		// [?-B] or [ ? | 0:< | <:0 | >:0 | B ][?-B]+
		// [ B:0 | 0:B | ?-B ]*
		HfstTransducer shortestMatchLeftMostConstraint( const HfstTransducer &uncondidtionalTr );


		// Shortest match
		// it should be composed to left most transducer........
		//[ B:0 | 0:B | ?-B ]*
		// [?-B] or [?-B]+  [ ? | 0:> | >:0 | <:0 | B ]
		// <:0 [?-B]+   > ?*
		HfstTransducer shortestMatchRightMostConstraint( const HfstTransducer &uncondidtionalTr );


		// ?* [ BL:0 (?-B)+ BR:0 ?* ]+
		HfstTransducer mostBracketsPlusConstraint( const HfstTransducer &uncondidtionalTr );


		// ?* [ BL:0 (?-B)* BR:0 ?* ]+
		HfstTransducer mostBracketsStarConstraint( const HfstTransducer &uncondidtionalTr );

		// ?* B2 ?*
		HfstTransducer removeB2Constraint( const HfstTransducer &t );

		// to avoid repetition in empty replace rule
		HfstTransducer noRepetitionConstraint( const HfstTransducer &t );


		//---------------------------------
		//	REPLACE FUNCTIONS - INTERFACE
		//---------------------------------

		// replace up, left, right, down
		HfstTransducer replace(	const Rule &rule, bool optional);
		// for parallel rules
		HfstTransducer replace(	const vector<Rule> &ruleVector, bool optional);
		// left to right
		HfstTransducer replace_leftmost_longest_match( const Rule &rule );
		// left to right
		HfstTransducer replace_leftmost_longest_match( const vector<Rule> &ruleVector );
		// right to left
		HfstTransducer replace_rightmost_longest_match( const Rule &rule );

		// right to left
		HfstTransducer replace_rightmost_longest_match( const vector<Rule> &ruleVector );

		HfstTransducer replace_leftmost_shortest_match( const Rule &rule);

		HfstTransducer replace_leftmost_shortest_match(const vector<Rule> &ruleVector );
		HfstTransducer replace_rightmost_shortest_match( const Rule &rule );
		HfstTransducer replace_rightmost_shortest_match( const vector<Rule> &ruleVector );
		HfstTransducer mark_up_replace(	const Rule &rule,
								const StringPair &marks,
								bool optional);
		 HfstTransducer mark_up_replace(const Rule &rule,
			 	  						const HfstTransducerPair &marks,
			 	  						bool optional);

		HfstTransducer mark_up_replace(	const vector<MarkUpRule> &markUpRuleVector,
								bool optional);
		// replace up, left, right, down
		HfstTransducer replace_epenthesis(	const Rule &rule, bool optional);
		// replace up, left, right, down
		HfstTransducer replace_epenthesis(	const vector<Rule> &ruleVector, bool optional);









		//---------------------------------
		//	UNIT TESTS
		//---------------------------------


		// ab->x  ab_a
		//void test1();



	}
}
