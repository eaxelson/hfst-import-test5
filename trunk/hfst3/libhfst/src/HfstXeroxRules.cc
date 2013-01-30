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
      Rule::Rule ( const HfstTransducerPairVector &mappingPairVector )
      {
        HfstTokenizer TOK;
        TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

        for ( unsigned int i = 0; i < mappingPairVector.size(); i++ )
        {
            if ( mappingPairVector[i].first.get_type() != mappingPairVector[i].second.get_type())
            {
                HFST_THROW_MESSAGE(TransducerTypeMismatchException, "Rule");
            }
        }
        ImplementationType type = mappingPairVector[0].first.get_type();

        HfstTransducerPair contextPair(HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type),
                                       HfstTransducer("@_EPSILON_SYMBOL_@", TOK, type));
        HfstTransducerPairVector epsilonContext;
        epsilonContext.push_back(contextPair);


        mapping = mappingPairVector;
        context = epsilonContext;
        replType = REPL_UP;

      }
      Rule::Rule ( const HfstTransducerPairVector &mappingPairVector,
                     const HfstTransducerPairVector &a_context,
                     ReplaceType a_replType )
      {
        for ( unsigned int i = 0; i < mappingPairVector.size(); i++ )
        {
            if ( mappingPairVector[i].first.get_type() != mappingPairVector[i].second.get_type())
            {
              HFST_THROW_MESSAGE(TransducerTypeMismatchException, "Rule");
            }
        }
/* TODO
        for ( unsigned int i = 0; i < a_context.size(); i++ )
        {
            if ( mappingPair.first.get_type() != a_context[i].first.get_type()
                ||     mappingPair.first.get_type() != a_context[i].second.get_type()
                ||     a_context[i].first.get_type() != a_context[i].second.get_type())
            {
                HFST_THROW_MESSAGE(TransducerTypeMismatchException, "Rule");
            }
        }
*/

    //    HfstTransducer tmpMapping(mappingPair.first);
    //    tmpMapping.cross_product(mappingPair.second).minimize();

        mapping = mappingPairVector;
        context = a_context;
        replType = a_replType;
      }

      HfstTransducerPairVector Rule::get_mapping() const
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

      MarkUpRule::MarkUpRule ( const HfstTransducerPairVector &a_mapping,
                                       StringPair a_marks ):
              Rule(a_mapping), marks(a_marks)
      {
          marks = a_marks;
      }
      MarkUpRule::MarkUpRule ( const HfstTransducerPairVector &a_mapping,
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
        String leftMarker("@_LM_@");
        String rightMarker("@_RM_@");

        HfstTransducer retval(tr);

        retval.substitute(StringPair(leftMarker, leftMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();
        retval.substitute(StringPair(rightMarker, rightMarker), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();


        retval.remove_from_alphabet(leftMarker);
        retval.remove_from_alphabet(rightMarker);

        retval.minimize();

        //printf("tr without markers: \n");
        //tr.write_in_att_format(stdout, 1);
        return retval;
      }

      /*
       * Generalized Lenient Composition, described in Anssi Yli-Jyrä 2008b
       */
      // tmp = t.1 .o. Constr .o. t.1
      // (t.1 - tmp.2) .o. t
      HfstTransducer constraintComposition( const HfstTransducer &t, const HfstTransducer &Constraint )
      {
          HfstTransducer retval(t);
          retval.input_project().minimize();

//printf("input projectt: \n");
//retval.write_in_att_format(stdout, 1);

          HfstTransducer tmp(retval);

          tmp.compose(Constraint).minimize();
//printf("first composition \n");
//tmp.write_in_att_format(stdout, 1);
          tmp.compose(retval).minimize();


          tmp.output_project().minimize();
//printf("tmp: \n");
//tmp.write_in_att_format(stdout, 1);
          retval.subtract(tmp).minimize();
//printf("after subtract \n");
//retval.write_in_att_format(stdout, 1);

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

            /* RULE:    LC:        RC:
             * up        up        up
             * left        up        down
             * right    down    up
             * down        down    down
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
       *                         where .* = [I:I (+ {tmpMarker (T), <,>} in alphabet) | <a:b>]*
       * then: remove tmpMarker from transducer and alphabet, and do negation:
       *         .* - result from upper operations
       *
       * Also note that context is expanded in this way:
       *         Cr' = (Rc .*) << Markers (<,>,|) .o. [I:I | <a:b>]*
       *         Cr = Cr | Cr'
       *         (same for left context, (.* Cl))
      */

      HfstTransducer bracketedReplace( const Rule &rule, bool optional)
      {
        //printf("bracketedReplace function..... \n");


          HfstTokenizer TOK;
          TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
          TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");
          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          String tmpMarker("@_TMPM_@");
          String leftMarker2("@_LM2_@");
          String rightMarker2("@_RM2_@");
          String markupMarker("@_MMM_@");


          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);
          TOK.add_multichar_symbol(leftMarker2);
          TOK.add_multichar_symbol(rightMarker2);
          TOK.add_multichar_symbol(tmpMarker);
          TOK.add_multichar_symbol(markupMarker);


          HfstTransducerPairVector mappingPairVector( rule.get_mapping() );

          HfstTransducerPairVector ContextVector( rule.get_context() );
          ReplaceType replType( rule.get_replType() );

          ImplementationType type = mappingPairVector[0].first.get_type();
          //printf("mapping 1 \n");
          //mappingPairVector[0].first.write_in_att_format(stdout, 1);


          //printf("mapping 2 \n");
          //mappingPairVector[0].second.write_in_att_format(stdout, 1);


            HfstTransducer mapping(type);
            for ( unsigned int i = 0; i < mappingPairVector.size(); i++ )
            {
                //mappingPairVector[0].first.substitute(StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@"), StringPair(markupMarker, markupMarker) ).minimize();

                HfstTransducer oneMappingPair(mappingPairVector[i].first);
            //oneMappingPair.insert_to_alphabet("x");

                // if it is markup rule, substitute epsilon in left mapping with marker
                if ( mappingPairVector[i].second.get_property("isMarkup") == "yes" )
                {
                    oneMappingPair.substitute(StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@"), StringPair(markupMarker, markupMarker) ).minimize();
                }

                oneMappingPair.cross_product(mappingPairVector[i].second);

                // if it is mark up rule
                if ( mappingPairVector[i].second.get_property("isMarkup") == "yes" )
                {
                    HfstTransducer identityPairTmp = HfstTransducer::identity_pair( type );
                    identityPairTmp.insert_to_alphabet(markupMarker);

                    // remove relations from the cross product
                    HfstTransducer tmpForIntersect(identityPairTmp);
                    HfstTransducer markupMarkToUnknown(markupMarker, "@_UNKNOWN_SYMBOL_@", TOK, type);
                    tmpForIntersect.disjunct(markupMarkToUnknown);
                    tmpForIntersect.repeat_star().minimize();
                    oneMappingPair.intersect(tmpForIntersect).minimize();

                    // replace temporary mark-up marker back to epsilons
                    HfstTransducer tmpForCompose("@_EPSILON_SYMBOL_@", markupMarker, TOK, type);
                    tmpForCompose.disjunct(identityPairTmp).repeat_star().minimize();
                    tmpForCompose.compose(oneMappingPair);
                    oneMappingPair = tmpForCompose;
                    oneMappingPair.remove_from_alphabet(markupMarker);
                }
                if ( i == 0 )
                {
                  mapping = oneMappingPair;
                }
                else
                {
                  mapping.disjunct(oneMappingPair).minimize();
                }
            }


            //printf("mapping all after cross product \n");
            //mapping.write_in_att_format(stdout, 1);
            // needed in case of ? -> x replacement
            mapping.insert_to_alphabet(leftMarker);
            mapping.insert_to_alphabet(rightMarker);
            mapping.insert_to_alphabet(tmpMarker);

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

            // Identity pair
            HfstTransducer identityPair = HfstTransducer::identity_pair( type );
            // for non-optional replacements
            if ( optional != true )
            {
                // non - optional
                // mapping = <a:b> u <2a:a>2

                HfstTransducer mappingWithBrackets2(leftBracket2);
                HfstTransducer leftMappingUnion(type);
                leftMappingUnion = mappingPairVector[0].first;
                for ( unsigned int i = 1; i < mappingPairVector.size(); i++ )
                {
                    leftMappingUnion.disjunct(mappingPairVector[i].first).minimize();
                }
                // needed in case of ? -> x replacement
                leftMappingUnion.insert_to_alphabet(leftMarker2);
                leftMappingUnion.insert_to_alphabet(rightMarker2);
                leftMappingUnion.insert_to_alphabet(leftMarker);
                leftMappingUnion.insert_to_alphabet(rightMarker);
                leftMappingUnion.insert_to_alphabet(tmpMarker);

                //printf("leftMappingUnion: \n");
                //leftMappingUnion.minimize().write_in_att_format(stdout, 1);


                mappingWithBrackets2.concatenate(leftMappingUnion).concatenate(rightBracket2).minimize();

                //printf("mappingWithBrackets2: \n");
                //mappingWithBrackets2.minimize().write_in_att_format(stdout, 1);

                  // mappingWithBrackets...... expanded
                mappingWithBrackets.insert_to_alphabet(leftMarker2);
                mappingWithBrackets.insert_to_alphabet(rightMarker2);
                //mappingWithBrackets.insert_to_alphabet(leftMarker);
                //mappingWithBrackets.insert_to_alphabet(rightMarker);
                mappingWithBrackets.disjunct(mappingWithBrackets2).minimize();
          }

          //printf("mappingWithBrackets: \n");
          //mappingWithBrackets.minimize().write_in_att_format(stdout, 1);


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

          // when there aren't any contexts, result is identityExpanded
          if ( ContextVector.size() == 1 )
          {
              HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
              if ( ContextVector[0].first.compare(epsilon) && ContextVector[0].second.compare(epsilon) )
              {
                  identityExpanded.remove_from_alphabet(tmpMarker);
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

          return uncondidtionalTr;

      }



      // bracketed replace for parallel rules
    HfstTransducer parallelBracketedReplace( const std::vector<Rule> &ruleVector, bool optional)
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

          ImplementationType type = ruleVector[0].get_mapping()[0].first.get_type();

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


              HfstTransducerPairVector mappingPairVector = ruleVector[i].get_mapping();


            HfstTransducer mapping(type);
              for ( unsigned int j = 0; j < mappingPairVector.size(); j++ )
              {
                  HfstTransducer oneMappingPair(mappingPairVector[j].first);
                  oneMappingPair.cross_product(mappingPairVector[j].second);
                  if ( j == 0 )
                  {
                      mapping = oneMappingPair;
                  }
                  else
                  {
                      mapping.disjunct(oneMappingPair).minimize();
                  }
              }

      //        HfstTransducer mapping = ruleVector[i].get_mapping();
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


              // needed in case of ? -> x replacement
              mapping.insert_to_alphabet(leftMarker);
              mapping.insert_to_alphabet(rightMarker);
              mapping.insert_to_alphabet(tmpMarker);


              // Surround mapping with brackets
              HfstTransducer mappingWithBrackets(leftBracket);
              mappingWithBrackets.concatenate(mapping).concatenate(rightBracket).minimize();


              // non - optional
              // mapping = T<a:b>T u T<2a:a>2T


              // non - optional
              // mapping = <a:b> u <2a:a>2
              if ( optional != true )
              {
                  // needed in case of ? -> x replacement
                mapping.insert_to_alphabet(leftMarker2);
                mapping.insert_to_alphabet(rightMarker2);
                mappingWithBrackets.insert_to_alphabet(leftMarker2);
                mappingWithBrackets.insert_to_alphabet(rightMarker2);


                  HfstTransducer mappingProject(mapping);
                  mappingProject.input_project().minimize();

                  HfstTransducer mappingWithBracketsNonOptional(leftBracket2);
                  // needed in case of ? -> x replacement
              //    mappingWithBracketsNonOptional.insert_to_alphabet(leftMarker2);
              //    mappingWithBracketsNonOptional.insert_to_alphabet(rightMarker2);

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
                  identityExpanded.remove_from_alphabet(tmpMarker);
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



          identityExpanded.remove_from_alphabet(tmpMarker);



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
      //    CONSTRAINTS
      //---------------------------------

      // (help function)
      // returns: [ B:0 | 0:B | ?-B ]*
      // which is used in some constraints
      HfstTransducer constraintsRightPart( ImplementationType type )
      {
          HfstTokenizer TOK;
          TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          // Identity pair (normal)
          HfstTransducer identityPair = HfstTransducer::identity_pair( type );


          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);

          // Create Right Part
          HfstTransducer B(leftBracket);
          B.disjunct(rightBracket).minimize();

          HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", leftMarker, TOK, type);
          HfstTransducer LeftMarkToEpsilon(leftMarker, "@_EPSILON_SYMBOL_@", TOK, type);

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

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          ImplementationType type = uncondidtionalTr.get_type();

          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);


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


          HfstTransducer LeftBracketToEpsilon(leftMarker, "@_EPSILON_SYMBOL_@", TOK, type);

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

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          ImplementationType type = uncondidtionalTr.get_type();

          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);


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


          HfstTransducer RightBracketToEpsilon(rightMarker, "@_EPSILON_SYMBOL_@", TOK, type);

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

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          ImplementationType type = uncondidtionalTr.get_type();

          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);

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

          HfstTransducer RightBracketToEpsilon(rightMarker, "@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", rightMarker, TOK, type);
          HfstTransducer LeftBracketToEpsilon(leftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", leftMarker, TOK, type);

          //[ ? | 0:< | <:0 | 0:> | B ]
     //     HfstTransducer nonClosingBracketInsertion(identityPair);
          HfstTransducer nonClosingBracketInsertion(epsilonToLeftBracket);
          nonClosingBracketInsertion.
                  //disjunct(epsilonToLeftBracket).
                  disjunct(LeftBracketToEpsilon).
                  disjunct(epsilonToRightBracket).
                  disjunct(B).
                  minimize();
          //    printf("nonClosingBracketInsertion: \n");
          //    nonClosingBracketInsertion.write_in_att_format(stdout, 1);


          nonClosingBracketInsertion.concatenate(identityPairMinusBracketsPlus).minimize();

          HfstTransducer middlePart(identityPairMinusBrackets);
          middlePart.disjunct(nonClosingBracketInsertion).minimize();


          // ?* < [?-B]+ 0:> [ ? | 0:< | <:0 | 0:> | B ] [?-B]+ [ B:0 | 0:B | ?-B ]*
          HfstTransducer Constraint(identity);
          Constraint.concatenate(leftBracket).
                  concatenate(identityPairMinusBracketsPlus).
                  concatenate(epsilonToRightBracket).
              //    concatenate(nonClosingBracketInsertion).
              //    concatenate(identityPairMinusBracketsPlus).
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

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          ImplementationType type = uncondidtionalTr.get_type();

          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);

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



          HfstTransducer RightBracketToEpsilon(rightMarker, "@_EPSILON_SYMBOL_@", TOK, type);

          HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", rightMarker, TOK, type);
          HfstTransducer LeftBracketToEpsilon(leftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", leftMarker, TOK, type);


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

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          ImplementationType type = uncondidtionalTr.get_type();

          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);

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


          HfstTransducer RightBracketToEpsilon(rightMarker, "@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", rightMarker, TOK, type);
          HfstTransducer LeftBracketToEpsilon(leftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", leftMarker, TOK, type);


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

      //    printf("nonClosingBracketInsertion: \n");
      //    nonClosingBracketInsertion.write_in_att_format(stdout, 1);

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

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          ImplementationType type = uncondidtionalTr.get_type();

          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);

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



          HfstTransducer RightBracketToEpsilon(rightMarker, "@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToRightBracket("@_EPSILON_SYMBOL_@", rightMarker, TOK, type);
          HfstTransducer LeftBracketToEpsilon(leftMarker, "@_EPSILON_SYMBOL_@", TOK, type);
          HfstTransducer epsilonToLeftBracket("@_EPSILON_SYMBOL_@", leftMarker, TOK, type);


          // [?-B]+ [ 0:> | >:0 | <:0 | B ]
          HfstTransducer nonClosingBracketInsertionTmp(epsilonToRightBracket);
          nonClosingBracketInsertionTmp.
                  disjunct(RightBracketToEpsilon).
                  disjunct(LeftBracketToEpsilon).
                  disjunct(B).minimize();
          HfstTransducer nonClosingBracketInsertion(identityPairMinusBracketsPlus);
          nonClosingBracketInsertion.concatenate(nonClosingBracketInsertionTmp).minimize();

      //    printf("nonClosingBracketInsertion: \n");
      //    nonClosingBracketInsertion.write_in_att_format(stdout, 1);

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


      //    printf("allLeftBracketsToEpsilon: \n");
      //    allLeftBracketsToEpsilon.write_in_att_format(stdout, 1);

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


          //    printf("allLeftBracketsToEpsilon: \n");
          //    allLeftBracketsToEpsilon.write_in_att_format(stdout, 1);

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
          //repeatingPart.write_in_att_format(stdout, 1);



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

          String leftMarker("@_LM_@");
          String rightMarker("@_RM_@");
          TOK.add_multichar_symbol(leftMarker);
          TOK.add_multichar_symbol(rightMarker);

          String leftMarker2("@_LM2_@");
          String rightMarker2("@_RM2_@");

          TOK.add_multichar_symbol(leftMarker2);
          TOK.add_multichar_symbol(rightMarker2);

          ImplementationType type = t.get_type();

          HfstTransducer leftBracket(leftMarker, TOK, type);
          HfstTransducer rightBracket(rightMarker, TOK, type);

          HfstTransducer leftBracket2(leftMarker2, TOK, type);
          HfstTransducer rightBracket2(rightMarker2, TOK, type);


          HfstTransducer leftBrackets(leftBracket);
          leftBrackets.disjunct(leftBracket2).minimize();

          HfstTransducer rightBrackets(rightBracket);
          rightBrackets.disjunct(rightBracket2).minimize();

          // Identity (normal)
          HfstTransducer identityPair = HfstTransducer::identity_pair( type );
          /*
          identityPair.insert_to_alphabet(leftMarker);
          identityPair.insert_to_alphabet(rightMarker);
          identityPair.insert_to_alphabet(leftMarker);
          identityPair.insert_to_alphabet(rightMarker2);
           */

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

          //printf("...Constraint: \n");
          //Constraint.write_in_att_format(stdout, 1);


          HfstTransducer retval(type);
          retval = constraintComposition(t, Constraint);

      //    retval = removeB2Constraint(retval);

          return retval;

      }



        // to apply boundary marker (.#.)
      /*
       * [0:.#. | ? - .#.]*
       *         .o.
       *     tr., ie. a -> b || .#. _ ;
       *         .o.
       *     .#. (? - .#.)* .#.
       *         .o.
       * [.#.:0 | ? - .#.]*
       */
        HfstTransducer applyBoundaryMark( const HfstTransducer &t )
        {
            HfstTokenizer TOK;
            TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
            TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");
            TOK.add_multichar_symbol("@_TMP_UNKNOWN_@");
            ImplementationType type = t.get_type();

            String boundaryMarker(".#.");
            TOK.add_multichar_symbol(boundaryMarker);
            HfstTransducer boundary(boundaryMarker, TOK, type);

            HfstTransducer identityPair = HfstTransducer::identity_pair( type );
            identityPair.insert_to_alphabet(boundaryMarker);
            // ? - .#.
            HfstTransducer identityMinusBoundary(identityPair);
            identityMinusBoundary.subtract(boundary).minimize();

            // (? - .#.)*
            HfstTransducer identityMinusBoundaryStar(identityMinusBoundary);
            identityMinusBoundaryStar.repeat_star().minimize();

            // .#. (? - .#.)* .#.
            HfstTransducer boundaryAnythingBoundary(boundary);
            boundaryAnythingBoundary.concatenate(identityMinusBoundaryStar)
                                    .concatenate(boundary)
                                    .minimize();



            // [0:.#. | ? - .#.]*
            HfstTransducer zeroToBoundary("@_EPSILON_SYMBOL_@", boundaryMarker, TOK, type);
            HfstTransducer retval(zeroToBoundary);
            retval.disjunct(identityMinusBoundary)
                  .minimize()
                  .repeat_star()
                  .minimize();

            //printf("retval .o. t: \n");
            //retval.write_in_att_format(stdout, 1);
            // [.#.:0 | ? - .#.]*
            HfstTransducer boundaryToZero(boundaryMarker, "@_EPSILON_SYMBOL_@", TOK, type);
            HfstTransducer removeBoundary(boundaryToZero);
            removeBoundary.disjunct(identityMinusBoundary)
               .minimize()
               .repeat_star()
               .minimize();

            // apply boundary to the transducer
            // compose [0:.#. | ? - .#.]* .o. t
            HfstTransducer tr(t);
            // substitutute unknowns with tmp symbol
            // this is necessary because of first composition
            tr.substitute("@_UNKNOWN_SYMBOL_@", "@_TMP_UNKNOWN_@");

            retval.compose(tr).minimize();
            //printf("tr: \n");
            //tr.write_in_att_format(stdout, 1);
            //printf("after compose 1: \n");
            //retval.write_in_att_format(stdout, 1);


            // compose with .#. (? - .#.)* .#.
            retval.compose(boundaryAnythingBoundary)
                  .minimize();

            // compose with [.#.:0 | ? - .#.]*
            retval.compose(removeBoundary).minimize();

            //printf("after compose 3: \n");
            //retval.write_in_att_format(stdout, 1);

            // bring back unknown symbols
            retval.substitute("@_TMP_UNKNOWN_@", "@_UNKNOWN_SYMBOL_@");
            retval.remove_from_alphabet("@_TMP_UNKNOWN_@");

            // remove boundary from alphabet
            retval.remove_from_alphabet(boundaryMarker);
            return retval;
        }


        //---------------------------------
        //    INTERFACE HELPING FUNCTIONS
        //---------------------------------

        // used by hfst-regexp parser
        HfstTransducerPair create_mapping_for_mark_up_replace( const HfstTransducerPair &mappingPair,
                                                          const HfstTransducerPair &marks )
        {
            HfstTokenizer TOK;
            TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

            ImplementationType type = mappingPair.first.get_type();

            HfstTransducer leftMark(marks.first);
            HfstTransducer rightMark(marks.second);

            HfstTransducer epsilonToLeftMark("@_EPSILON_SYMBOL_@", TOK, type);
            epsilonToLeftMark.cross_product(leftMark).minimize();

            HfstTransducer epsilonToRightMark("@_EPSILON_SYMBOL_@", TOK, type);
            epsilonToRightMark.cross_product(rightMark).minimize();

            //printf("epsilonToRightMark: \n");
            //epsilonToRightMark.write_in_att_format(stdout, 1);

            //Go through left part of every mapping pair
            // and concatenate: epsilonToLeftMark.leftMapping.epsilonToRightMark
            //then put it into right part of the new transducerPairVector
            HfstTransducer mappingCrossProduct(epsilonToLeftMark);
            mappingCrossProduct.concatenate(mappingPair.first).
                    minimize().
                    concatenate(epsilonToRightMark).
                    minimize();

            //printf("mappingCrossProduct: \n");
            //mappingCrossProduct.write_in_att_format(stdout, 1);

            HfstTransducer in(mappingCrossProduct);
            in.input_project();
            //printf("in: \n");
            //in.write_in_att_format(stdout, 1);

            //printf("out: \n");

            HfstTransducer out(mappingCrossProduct);
            out.output_project();
            //printf("out: \n");
            //out.write_in_att_format(stdout, 1);

            out.set_property("isMarkup", "yes");
            HfstTransducerPair retval(in, out);

            return retval;
        }


        HfstTransducerPairVector create_mapping_for_mark_up_replace( const HfstTransducerPairVector &mappingPairVector,
                                                          const StringPair &marks )
        {
            HfstTokenizer TOK;
            TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

            ImplementationType type = mappingPairVector[0].first.get_type();

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

            //Go through left part of every mapping pair
            // and concatenate: epsilonToLeftMark.leftMapping.epsilonToRightMark
            //then put it into right part of the new transducerPairVector
            HfstTransducerPairVector retval;
            //HfstTransducer mapping(type);
            for ( unsigned int i = 0; i < mappingPairVector.size(); i++ )
            {

                //printf("mapping left: \n");
                //mappingPairVector[i].first.write_in_att_format(stdout, 1);

                HfstTransducer mappingCrossProduct(epsilonToLeftMark);
                mappingCrossProduct.concatenate(mappingPairVector[i].first).
                        minimize().
                        concatenate(epsilonToRightMark).
                        minimize();

                //printf("mappingCrossProduct: \n");
                //mappingCrossProduct.write_in_att_format(stdout, 1);



                HfstTransducer in(mappingCrossProduct);
                in.input_project();
                //printf("in: \n");
                //in.write_in_att_format(stdout, 1);

                //printf("out: \n");

                HfstTransducer out(mappingCrossProduct);
                out.output_project();
                //printf("out: \n");
                //out.write_in_att_format(stdout, 1);

                out.set_property("isMarkup", "yes");
                retval.push_back(HfstTransducerPair(in, out));
            }
            return retval;
        }
        HfstTransducerPairVector create_mapping_for_mark_up_replace( const HfstTransducerPairVector &mappingPairVector,
                                                              const HfstTransducerPair &marks )
        {
            HfstTokenizer TOK;
            TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

            ImplementationType type = mappingPairVector[0].first.get_type();

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


            //Go through left part of every mapping pair
            // and concatenate: epsilonToLeftMark.leftMapping.epsilonToRightMark
            //then put it into right part of the new transducerPairVector
            HfstTransducerPairVector retval;
            //HfstTransducer mapping(type);
            for ( unsigned int i = 0; i < mappingPairVector.size(); i++ )
            {

                //printf("mapping left: \n");
                //mappingPairVector[i].first.write_in_att_format(stdout, 1);

                HfstTransducer mappingCrossProduct(epsilonToLeftMark);
                mappingCrossProduct.concatenate(mappingPairVector[i].first).
                        minimize().
                        concatenate(epsilonToRightMark).
                        minimize();

                //printf("mappingCrossProduct: \n");
                //mappingCrossProduct.write_in_att_format(stdout, 1);



                HfstTransducer in(mappingCrossProduct);
                in.input_project();
                //printf("in: \n");
                //in.write_in_att_format(stdout, 1);

                //printf("out: \n");

                HfstTransducer out(mappingCrossProduct);
                out.output_project();
                //printf("out: \n");
                //out.write_in_att_format(stdout, 1);

                out.set_property("isMarkup", "yes");
                retval.push_back(HfstTransducerPair(in, out));
            }
            return retval;
        }













      //---------------------------------
      //    REPLACE FUNCTIONS - INTERFACE
      //---------------------------------


      // replace up, left, right, down
      HfstTransducer replace( const Rule &rule, bool optional)
      {

          HfstTransducer retval( bracketedReplace(rule, optional) );

//printf("---bracketed replace done---: \n");
//retval.minimize().write_in_att_format(stdout, 1);

          // for epenthesis rules
          // it can't have more than one epsilon repetition in a row

          retval = noRepetitionConstraint( retval );

//printf("-----noRepetitionConstraint-----: \n");
//retval.write_in_att_format(stdout, 1);

          // deals with boundary symbol, must be before mostBracketsStarConstraint
          // TODO: this constraint doesn't work for ? -> x
              // because first composition composes 0:.#. .o. U:x
         retval = applyBoundaryMark( retval );

//printf("----after applyBoundaryMark: ----\n");
//retval.write_in_att_format(stdout, 1);
          if ( !optional )
          {

              //printf(" ----------  mostBracketsStarConstraint --------------\n");
              // Epenthesis rules behave differently if used mostBracketsPlusConstraint
              //retval = mostBracketsPlusConstraint(retval);
              retval = mostBracketsStarConstraint(retval);
//printf("after non optional: \n");
//retval.write_in_att_format(stdout, 1);
          }
          retval = removeB2Constraint(retval);
          retval = removeMarkers( retval );
//printf("after removeMarkers: \n");
//retval.write_in_att_format(stdout, 1);
          return retval;
      }

      // for parallel rules
    HfstTransducer replace( const std::vector<Rule> &ruleVector, bool optional)
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

            // for epenthesis rules
            // it can't have more than one epsilon repetition in a row
            retval = noRepetitionConstraint( retval );


            // deals with boundary symbol
            retval = applyBoundaryMark( retval );

            if ( !optional )
            {
                // Epenthesis rules behave differently if used mostBracketsPlusConstraint
               // retval = mostBracketsPlusConstraint(retval);
                retval = mostBracketsStarConstraint(retval);
            }
            retval = removeB2Constraint(retval);
            retval = removeMarkers( retval );
            return retval;

      }
      // replace left
      HfstTransducer replace_left( const Rule &rule, bool optional)
      {
          HfstTransducerPairVector mappingPairVector = rule.get_mapping();
          //HfstTransducer newMapping = rule.get_mapping();
          //newMapping.invert().minimize();

          HfstTransducerPairVector newMappingPairVector;
          for ( unsigned int i = 0; i < mappingPairVector.size(); i++ )
          {
              // in every mapping pair invert first and second
              //HfstTransducer newMapping = rule.get_mapping();
            HfstTransducer first = mappingPairVector[i].first;
            HfstTransducer second = mappingPairVector[i].second;
            newMappingPairVector.push_back(HfstTransducerPair(second, first));

          }

          Rule newRule ( newMappingPairVector, rule.get_context(), rule.get_replType());
          HfstTransducer retval (replace( newRule, optional));

          retval.invert().minimize();
          return retval;
      }
      // replace left parallel
      HfstTransducer replace_left( const std::vector<Rule> &ruleVector, bool optional)
      {
          std::vector<Rule> leftRuleVector;

          for ( unsigned int i = 0; i < ruleVector.size(); i++ )
          {

              HfstTransducerPairVector mappingPairVector = ruleVector[i].get_mapping();
              //HfstTransducer newMapping = rule.get_mapping();
              //newMapping.invert().minimize();

              HfstTransducerPairVector newMappingPairVector;
              for ( unsigned int j = 0; j < mappingPairVector.size(); j++ )
              {
                  // in every mapping pair invert first and second
                  //HfstTransducer newMapping = rule.get_mapping();
                HfstTransducer first = mappingPairVector[j].first;
                HfstTransducer second = mappingPairVector[j].second;
                newMappingPairVector.push_back(HfstTransducerPair(second, first));

              }

                Rule newRule(newMappingPairVector, ruleVector[i].get_context(), ruleVector[i].get_replType());

                leftRuleVector.push_back(newRule);
          }

          HfstTransducer retval(replace(leftRuleVector, optional));
          retval.invert().minimize();

          return retval;
      }

      // left to right
      HfstTransducer replace_leftmost_longest_match( const Rule &rule )
      {

          HfstTransducer uncondidtionalTr( bracketedReplace(rule, true) );
          //uncondidtionalTr = bracketedReplace(rule, true);

          //printf("LM uncondidtionalTr: \n");
          //uncondidtionalTr.write_in_att_format(stdout, 1);



          HfstTransducer retval (leftMostConstraint(uncondidtionalTr));
          //retval = leftMostConstraint(uncondidtionalTr);

          //printf("leftMostConstraint: \n");
          //retval.write_in_att_format(stdout, 1);

          retval = longestMatchLeftMostConstraint( retval );

          //printf("longestMatchLeftMostConstraint: \n");
          //retval.write_in_att_format(stdout, 1);


          // for epenthesis rules
        // it can't have more than one epsilon repetition in a row
        retval = noRepetitionConstraint( retval );



          retval = removeB2Constraint(retval);

          retval = removeMarkers( retval );

          // deals with boundary symbol
          retval = applyBoundaryMark( retval );

          return retval;
      }
      // left to right
    HfstTransducer replace_leftmost_longest_match( const std::vector<Rule> &ruleVector )
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


          // for epenthesis rules
          // it can't have more than one epsilon repetition in a row
          retval = noRepetitionConstraint( retval );

          // remove LM2, RM2
          retval = removeB2Constraint(retval);

          retval = removeMarkers( retval );


          // deals with boundary symbol
          retval = applyBoundaryMark( retval );


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


          // for epenthesis rules
          // it can't have more than one epsilon repetition in a row
          retval = noRepetitionConstraint( retval );
          // remove LM2, RM2
          retval = removeB2Constraint(retval);

          retval = removeMarkers( retval );


          // deals with boundary symbol
          retval = applyBoundaryMark( retval );


          return retval;
      }


      // right to left
    HfstTransducer replace_rightmost_longest_match( const std::vector<Rule> &ruleVector )
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


          // for epenthesis rules
          // it can't have more than one epsilon repetition in a row
          retval = noRepetitionConstraint( retval );
          // remove LM2, RM2
          retval = removeB2Constraint(retval);

          retval = removeMarkers( retval );

          // deals with boundary symbol
          retval = applyBoundaryMark( retval );

          return retval;
      }

      HfstTransducer replace_leftmost_shortest_match( const Rule &rule)
      {

          HfstTransducer uncondidtionalTr(bracketedReplace(rule, true));
      //    uncondidtionalTr = bracketedReplace(rule, true);

          HfstTransducer retval (leftMostConstraint(uncondidtionalTr));
          //retval = leftMostConstraint(uncondidtionalTr);
          retval = shortestMatchLeftMostConstraint( retval );

          //printf("sh tr: \n");
          //retval.write_in_att_format(stdout, 1);


          // for epenthesis rules
          // it can't have more than one epsilon repetition in a row
          retval = noRepetitionConstraint( retval );
          // remove LM2, RM2
          retval = removeB2Constraint(retval);

          retval = removeMarkers( retval );

          // deals with boundary symbol
          retval = applyBoundaryMark( retval );

          return retval;
      }


    HfstTransducer replace_leftmost_shortest_match(const std::vector<Rule> &ruleVector )
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


        // for epenthesis rules
        // it can't have more than one epsilon repetition in a row
        retval = noRepetitionConstraint( retval );
        // remove LM2, RM2
        retval = removeB2Constraint(retval);

        retval = removeMarkers( retval );

        // deals with boundary symbol
        retval = applyBoundaryMark( retval );

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


        // for epenthesis rules
        // it can't have more than one epsilon repetition in a row
        retval = noRepetitionConstraint( retval );
        // remove LM2, RM2
        retval = removeB2Constraint(retval);

        retval = removeMarkers( retval );

        // deals with boundary symbol
        retval = applyBoundaryMark( retval );

        return retval;
    }

    HfstTransducer replace_rightmost_shortest_match( const std::vector<Rule> &ruleVector )
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


        // for epenthesis rules
        // it can't have more than one epsilon repetition in a row
        retval = noRepetitionConstraint( retval );
        // remove LM2, RM2
        retval = removeB2Constraint(retval);

        retval = removeMarkers( retval );

        // deals with boundary symbol
        retval = applyBoundaryMark( retval );

        return retval;
    }



      HfstTransducer mark_up_replace( const Rule &rule,
                              const StringPair &marks,
                              bool optional)
      {

        HfstTransducerPairVector new_mapping = create_mapping_for_mark_up_replace(rule.get_mapping(), marks);
          Rule newRule(new_mapping, rule.get_context(), rule.get_replType());

          HfstTransducer retval(replace(newRule, optional));

          //printf("after replace: \n");
          //retval.write_in_att_format(stdout, 1);

/*
HfstTokenizer TOK;
String epsilon("@_EPSILON_SYMBOL_@");
String mark_up_mark("@_MUP_MARK_@");

TOK.add_multichar_symbol(epsilon);
TOK.add_multichar_symbol(mark_up_mark);



retval.substitute(StringPair(mark_up_mark, marks.first), StringPair("@_EPSILON_SYMBOL_@", marks.first)).minimize();
retval.substitute(StringPair(mark_up_mark, marks.first), StringPair("@_EPSILON_SYMBOL_@", marks.second)).minimize();
retval.substitute(StringPair(mark_up_mark, mark_up_mark), StringPair("@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@")).minimize();


retval.remove_from_alphabet(mark_up_mark);


retval.minimize();

printf("after substitute: \n");
retval.write_in_att_format(stdout, 1);
*/
/*

          HfstTokenizer TOK;
          String epsilon("@_EPSILON_SYMBOL_@");
        String mark_up_mark("@_MUP_MARK_@");

        TOK.add_multichar_symbol(epsilon);
        TOK.add_multichar_symbol(mark_up_mark);

        ImplementationType type = retval.get_type();

        HfstTransducer identityPair = HfstTransducer::identity_pair( type );
        HfstTransducer identity (identityPair);
        //identity.repeat_star().minimize();



        HfstTransducer markToEpsilon(mark_up_mark, epsilon, TOK, type);
        markToEpsilon.disjunct(identity);
        retval.compose(markToEpsilon).minimize();

printf("after compose: \n");
retval.write_in_att_format(stdout, 1);
*/

          return retval;
      }

      HfstTransducer mark_up_replace( const Rule &rule,
                                   const HfstTransducerPair &marks,
                                   bool optional)
     {
          HfstTransducerPairVector new_mapping = create_mapping_for_mark_up_replace(rule.get_mapping(), marks);

               Rule newRule(new_mapping, rule.get_context(), rule.get_replType());
               //printf("epsilonToRightMark: \n");
               //epsilonToRightMark.write_in_att_format(stdout, 1);

               HfstTransducer retval(replace(newRule, optional));


               return retval;
     }

/*
      // TODO:
    HfstTransducer mark_up_replace(    const std::vector<MarkUpRule> &markUpRuleVector,
                              bool optional)
      {
          HfstTokenizer TOK;
          TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

          ImplementationType type = markUpRuleVector[0].get_mapping().get_type();
        std::vector<Rule> ruleVector;

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
*/

      // replace up, left, right, down
      HfstTransducer replace_epenthesis(    const Rule &rule, bool optional)
      {
          return replace(rule, optional);
      }

      // replace up, left, right, down
    HfstTransducer replace_epenthesis(    const std::vector<Rule> &ruleVector, bool optional)
      {
          return replace(ruleVector, optional);
      }


    //---------------------------------
      //    RESTRICTION FUNCTIONS
      //---------------------------------


    /*
        define U [ ? - %<D%> ] ;

        define CENTER [ x y | x x y y ];

        define L1 [ a ] ;
        define R1 [ b ] ;

        define L2 [ x ] ;
        define R2 [ y ] ;

        define RES1 [ U* L1 %<D%> U* %<D%> R1 U* ] ;
        define RES2 [ U* L2 %<D%> U* %<D%> R2 U* ] ;

        define CEN1 [ U* %<D%> CENTER %<D%> U* ] ;

        define NODU [ U | 0:%<D%> ]* ;
        define NODL [ U | %<D%>:0 ]* ;

        regex U* - [ NODU .o. [ CEN1 - [ RES1 | RES2 ] ] .o. NODL ] ;
      */
    HfstTransducer restriction( const HfstTransducer &_center, const HfstTransducerPairVector &context)
    {
        // TODO: check if the center is automata
        ImplementationType type = _center.get_type();
        String restrictionMark("@_D_@");

        HfstTokenizer TOK;
        TOK.add_multichar_symbol(restrictionMark);
        TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

        HfstTransducer mark(restrictionMark, TOK, type);
        HfstTransducer epsilon("@_EPSILON_SYMBOL_@", TOK, type);

        // Identity
        HfstTransducer identityPair = HfstTransducer::identity_pair( type );
        HfstTransducer identity (identityPair);
        identity.repeat_star().minimize();

        HfstTransducer universalWithoutD(identity);
        universalWithoutD.insert_to_alphabet(restrictionMark);
        HfstTransducer universalWithoutDStar(universalWithoutD);
        universalWithoutDStar.repeat_star().minimize();

        // NODU
        HfstTransducer noDUpper("@_EPSILON_SYMBOL_@", restrictionMark, TOK, type );
        noDUpper.disjunct(universalWithoutD).repeat_star().minimize();

        // NODL
        HfstTransducer noDLower(restrictionMark, "@_EPSILON_SYMBOL_@", TOK, type );
        noDLower.disjunct(universalWithoutD).repeat_star().minimize();

        // 1. Surround center with marks
        // [ U* %<D%> CENTER %<D%> U* ]
        HfstTransducer center(_center);
        center.insert_to_alphabet(restrictionMark);

        HfstTransducer centerMarked(universalWithoutDStar);
        centerMarked.concatenate(mark)
                    .concatenate(center)
                    .concatenate(mark)
                    .concatenate(universalWithoutDStar)
                    .minimize();

        // 2. Put mark in context
        // [ U* L1 %<D%> U* %<D%> R1 U* ]
        HfstTransducer contextMarked;
        for ( unsigned int i = 0; i < context.size(); i++ )
        {
            HfstTransducer lefContext(context[i].first);
            lefContext.insert_to_alphabet(restrictionMark);

            HfstTransducer rightContext(context[i].second);
            rightContext.insert_to_alphabet(restrictionMark);

            HfstTransducer RES(universalWithoutDStar);
            RES.concatenate(lefContext)
                .concatenate(mark)
                .concatenate(universalWithoutDStar)
                .concatenate(mark)
                .concatenate(rightContext)
                .concatenate(universalWithoutDStar)
                .minimize();

            if ( i == 0 )
            {
                contextMarked = RES;
            }
            else
            {
                contextMarked.disjunct(RES).minimize();
            }
        }
        HfstTransducer centerMinusCtx(centerMarked);
        centerMinusCtx.subtract(contextMarked).minimize();

        HfstTransducer tmp(noDUpper);
        tmp.compose(centerMinusCtx)
            .compose(noDLower)
            .minimize();

        HfstTransducer retval(universalWithoutDStar);
        retval.subtract(tmp).minimize();

        retval.remove_from_alphabet(restrictionMark);

        // deals with boundary symbol
        retval = applyBoundaryMark( retval );

        return retval;
    }


    // a < b
    HfstTransducer before( const HfstTransducer &left, const HfstTransducer &right)
    {
        // TODO: check if the center is automata
       ImplementationType type = left.get_type();

       // Identity
       HfstTransducer identityPair = HfstTransducer::identity_pair( type );
       HfstTransducer identity (identityPair);
       identity.repeat_star().minimize();

       HfstTransducer tmp(identity);
       tmp.concatenate(right)
           .concatenate(identity)
           .concatenate(left)
           .concatenate(identity)
           .minimize();

       HfstTransducer retval(identity);
       retval.subtract(tmp).minimize();

       return retval;
    }
    // a > b
    HfstTransducer after( const HfstTransducer &left, const HfstTransducer &right)
        {
            // TODO: check if the center is automata
           ImplementationType type = left.get_type();

           // Identity
           HfstTransducer identityPair = HfstTransducer::identity_pair( type );
           HfstTransducer identity (identityPair);
           identity.repeat_star().minimize();

           HfstTransducer tmp(identity);
           tmp.concatenate(left)
               .concatenate(identity)
               .concatenate(right)
               .concatenate(identity)
               .minimize();

           HfstTransducer retval(identity);
           retval.subtract(tmp).minimize();

           return retval;
        }





  }
}
#else

#include "HfstXeroxRulesTest.cc"

int main(int argc, char * argv[])
      {
          std::cout << "Unit tests for " __FILE__ ":" << std::endl;


          ImplementationType types[] = {SFST_TYPE, TROPICAL_OPENFST_TYPE, FOMA_TYPE};
          unsigned int NUMBER_OF_TYPES=3;


          for (unsigned int i=0; i < NUMBER_OF_TYPES; i++)
          {
            if (! HfstTransducer::is_implementation_type_available(types[i]))
          continue;

          // std::cout << "----- Type --------- " << i << std::endl;



            test1( types[i] );

            test1b( types[i]);

            // ? -> a it doesn't work because of apply boundary thing
            test1c( types[i]);

            // a -> b || .#. _ ;
            test1d( types[i]);

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
            // [. 0 .] -> b || _ a a ;
            test6c( types[i] );

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
            // ? -> x , a -> b
            test7e( types[i] );

            // TODO
            // markup parallel rules
            //test7e( types[i] );

            // a -> b, b -> a
            test7f( types[i] );

            // a -> b b , a -> b
            test7g( types[i] );
        // TODO
        //    test8( types[i] );

            test9a( types[i] );
            test9b( types[i] );

            // rules with empty language
            test10a( types[i] );
            // empty -> non-empty
            // TODO
            //test10b( types[i] );

            // restriction functions =>

            restriction_test1( types[i] );
            restriction_test1a( types[i] );
            restriction_test1b( types[i] );
            restriction_test2( types[i] );
            restriction_test3( types[i] );
            restriction_test3a( types[i] );
            restriction_test3b( types[i] );
            restriction_test3c( types[i] );
            restriction_test4( types[i] );
            restriction_test5( types[i] );
            restriction_test5a( types[i] );
            restriction_test6( types[i] );
            restriction_test7( types[i] );
            restriction_test8( types[i] );

            before_test1( types[i] );

          }

          std::cout << "ok" << std::endl;
          return 0;
      }
#endif // MAIN_TEST
