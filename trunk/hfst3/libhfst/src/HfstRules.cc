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
#include "HfstTransducer.h"

namespace hfst
{
  namespace rules
  {

    HfstTransducer replace( HfstTransducer &t, ReplaceType repl_type, bool optional, StringPairSet &alphabet ) 
    {

      bool DEBUG=false;

      if (DEBUG) printf("replcae..\n");

      ImplementationType type = t.get_type();

      HfstTransducer t_proj(t);
      if (repl_type == REPL_UP)
	t_proj.input_project();
      else if (repl_type == REPL_DOWN)
	t_proj.output_project();
      else
	throw hfst::exceptions::ImpossibleReplaceTypeException();

      HfstTransducer pi_star(alphabet, type, true);

      // tc = ( .* t_proj .* )
      HfstTransducer tc(pi_star);
      tc.concatenate(t_proj);
      tc.concatenate(pi_star);
		     
      // tc_neg = ! ( .* t_proj .* )
      HfstTransducer tc_neg(pi_star);
      tc_neg.subtract(tc);

      // retval = ( tc_neg t )* tc_neg
      HfstTransducer retval(tc_neg);
      retval.concatenate(t);
      retval.repeat_star();
      retval.concatenate(tc_neg);

      if (optional)
	retval.disjunct(pi_star);

      if (DEBUG) printf("..replcae\n");

      return retval;
    }

    HfstTransducer replace_transducer(HfstTransducer &t, std::string lm, std::string rm, ReplaceType repl_type, StringPairSet &alphabet)
    {
      bool DEBUG=false;

      t.minimize();
      if (DEBUG) t.write_in_att_format("compiler_t.att",true);

      if (DEBUG) printf("replcae_transducer..\n");

      ImplementationType type = t.get_type();

      // tm = ( L (L >> (R >> t)) R )
      HfstTransducer tc(t);
      tc.insert_freely(StringPair(rm,rm));
      tc.insert_freely(StringPair(lm,lm));
      HfstTransducer tm(lm, type);
      HfstTransducer rmtr(rm,type);
      tm.concatenate(tc);
      tm.concatenate(rmtr);

      tm.minimize();
      HfstTransducer retval = replace(tm, repl_type, false, alphabet);

      if (DEBUG) printf("..replcae_transducer\n");

      retval.minimize();

      return retval;
    }



    HfstTransducer replace_context(HfstTransducer &t, std::string m1, std::string m2, StringPairSet &alphabet)
    {
      // ct = .* ( m1 >> ( m2 >> t ))  ||  !(.* m1)

      bool DEBUG=false;

      if (DEBUG) printf("    replace_context..\n");

      // m1 >> ( m2 >> t )
      HfstTransducer t_copy(t);
      t_copy.insert_freely(StringPair(m1,m1));
      t_copy.insert_freely(StringPair(m2,m2));

      if (DEBUG) printf("    (1)\n");

      HfstTransducer pi_star(alphabet, t.get_type(), true);
 
      // arg1 = .* ( m1 >> ( m2 >> t ))
      HfstTransducer arg1(pi_star);

       if (DEBUG) printf("    (2)\n");

       arg1.concatenate(t_copy);

      if (DEBUG) printf("    (3)\n");

      // arg2 = !(.* m1)
      HfstTransducer m1_tr(m1, t.get_type());
      HfstTransducer tmp(pi_star);
      tmp.concatenate(m1_tr);
      HfstTransducer arg2(pi_star);
      arg2.subtract(tmp);

      // ct = .* ( m1 >> ( m2 >> t ))  ||  !(.* m1)
      HfstTransducer ct = arg1.compose(arg2);

      if (DEBUG) printf("    (4)\n");

      // mt = m2* m1 .*
      HfstTransducer mt(m2, t.get_type());
      mt.repeat_star();
      mt.concatenate(m1_tr);
      mt.concatenate(pi_star);


      // !( (!ct mt) | (ct !mt) )

      if (DEBUG) printf("    (5)\n");

      // ct !mt
      HfstTransducer tmp2(pi_star);
      tmp2.subtract(mt);
      HfstTransducer ct_neg_mt(ct);
      ct_neg_mt.concatenate(tmp2);


      if (DEBUG) printf("    (6)\n");

      // !ct mt
      HfstTransducer neg_ct_mt(pi_star);
      neg_ct_mt.subtract(ct);
      neg_ct_mt.concatenate(mt);


      if (DEBUG) printf("    (7)\n");

      // disjunction
      HfstTransducer disj = neg_ct_mt.disjunct(ct_neg_mt);

      if (DEBUG) printf("    (8)\n");

      // negation
      HfstTransducer retval(pi_star);
      retval.subtract(disj);

      if (DEBUG) printf("    (9)\n");

      retval.minimize();

      return retval;
    }



    /* identical to  ![ .* l [a:. & !a:b] r .* ]  */
    HfstTransducer two_level_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet) 
    {
      if (context.first.get_type() != context.second.get_type())
	throw hfst::exceptions::TransducerTypeMismatchException();
      ImplementationType type = context.first.get_type();

      //assert(context.first.get_type() != UNSPECIFIED_TYPE);
      assert(context.second.get_type() != ERROR_TYPE);
      //assert(context.first.get_type() != UNSPECIFIED_TYPE);
      assert(context.second.get_type() != ERROR_TYPE);

      // calculate [ a:. ]
      StringPairSet input_to_any;
      for (StringPairSet::iterator it = mappings.begin(); it != mappings.end(); it++)
	{
	  for (StringPairSet::iterator alpha_it = alphabet.begin(); alpha_it != alphabet.end(); alpha_it++)
	    {
	      if (alpha_it->first == it->first)
		{
		  input_to_any.insert(StringPair(alpha_it->first, alpha_it->second));
		}
	    }
	}

      // center == [ a:. ]
      HfstTransducer center(input_to_any, type);

      // calculate [ .* - a:b ]
      HfstTransducer neg_mappings(alphabet, type, true);
      //neg_mappings.repeat_star();

      HfstTransducer mappings_tr(mappings, type);
      neg_mappings.subtract(mappings_tr);

      // center == [ a:. & !a:b ]
      center.intersect(neg_mappings);

      // left context == [ .* l ]
      HfstTransducer left_context(alphabet, type, true);
      left_context.concatenate(context.first);

      // right_context == [ r .* ]
      HfstTransducer right_context(context.second);
      HfstTransducer universal(alphabet, type, true);
      right_context.concatenate(universal);

      HfstTransducer inside(left_context.concatenate(center).concatenate(right_context));

      HfstTransducer retval(universal.subtract(inside));
      return retval;
    }


    // equivalent to !(!(.* l) a:b .* | .* a:b !(r .*))
    HfstTransducer two_level_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet) 
    { 
      if (context.first.get_type() != context.second.get_type())
	throw hfst::exceptions::TransducerTypeMismatchException();
      ImplementationType type = context.first.get_type();

      //assert(context.first.get_type() != UNSPECIFIED_TYPE);
      assert(context.second.get_type() != ERROR_TYPE);
      //assert(context.first.get_type() != UNSPECIFIED_TYPE);
      assert(context.second.get_type() != ERROR_TYPE);

      // center = a:b
      HfstTransducer center(mappings, type);

      // left_neg = !(.* l)
      HfstTransducer left(alphabet, type, true);
      left.concatenate(context.first);
      HfstTransducer left_neg(alphabet, type, true);
      left_neg.subtract(left);

      // right_neg = !(r .*)
      HfstTransducer universal(alphabet, type, true);
      HfstTransducer right(context.second);
      right.concatenate(universal);
      HfstTransducer right_neg(alphabet, type, true);
      right_neg.subtract(right);

      // left_neg + center + universal  |  universal + center + right_neg
      HfstTransducer rule(left_neg);
      rule.concatenate(center);
      rule.concatenate(universal);
      HfstTransducer rule_right(universal);
      rule_right.concatenate(center);
      rule_right.concatenate(right_neg);
      rule.disjunct(rule_right);

      HfstTransducer rule_neg(alphabet, type, true);
      rule_neg.subtract(rule);

      return rule_neg;
    }

    HfstTransducer two_level_if_and_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet) 
    {
      HfstTransducer if_rule = two_level_if(context, mappings, alphabet);
      HfstTransducer only_if_rule = two_level_only_if(context, mappings, alphabet);
      return if_rule.intersect(only_if_rule);
    }

    HfstTransducer replace_in_context(HfstTransducerPair &context, ReplaceType repl_type, HfstTransducer &t, bool optional, StringPairSet &alphabet)
    {

      bool DEBUG=false;

      if (DEBUG) printf("replace_in_context...\n");

      // test that all transducers have the same type
      if (context.first.get_type() != context.second.get_type() || 
	  context.first.get_type() != t.get_type() )
	throw hfst::exceptions::TransducerTypeMismatchException();
      ImplementationType type = t.get_type();      

      if (DEBUG) printf("  ..transducers have the same type\n");


      // test that both context transducers are automata
      // this could be done more efficiently...
      /*HfstTransducer t1_proj(context.first);
      t1_proj.input_project();
      HfstTransducer t2_proj(context.second);
      t2_proj.input_project();

      if (DEBUG) printf("  testing if context transducers are automata..\n");

      if ( not HfstTransducer::are_equivalent(t1_proj, context.first) ||
	   not HfstTransducer::are_equivalent(t2_proj, context.second) )
	throw hfst::exceptions::ContextTransducersAreNotAutomataException();

	if (DEBUG) printf("  ..context transducers are automata\n");*/
      
      std::string leftm("@_LEFT_MARKER_@");
      std::string rightm("@_RIGHT_MARKER_@");
      std::string epsilon("@_EPSILON_SYMBOL_@");

      // HfstTransducer pi(alphabet, type);

      // Create the insert boundary transducer (.|<>:<L>|<>:<R>)*    
      StringPairSet pi1 = alphabet;
      pi1.insert(StringPair("@_EPSILON_SYMBOL_@", leftm));
      pi1.insert(StringPair("@_EPSILON_SYMBOL_@", rightm));
      HfstTransducer ibt(pi1, type, true);

      if (DEBUG) printf("  ..ibt created\n");

      // Create the remove boundary transducer (.|<L>:<>|<R>:<>)*    
      StringPairSet pi2 = alphabet;
      pi2.insert(StringPair(leftm, "@_EPSILON_SYMBOL_@"));
      pi2.insert(StringPair(rightm, "@_EPSILON_SYMBOL_@"));
      HfstTransducer rbt(pi2, type, true);

      if (DEBUG) printf("  ..rbt created\n");

      // Add the markers to the alphabet
      alphabet.insert(StringPair(leftm,leftm));
      alphabet.insert(StringPair(rightm,rightm));

      HfstTransducer pi_star(alphabet,type,true);

      // Create the constrain boundary transducer !(.*<L><R>.*)
      HfstTransducer leftm_to_leftm(leftm, leftm, type);
      HfstTransducer rightm_to_rightm(rightm, rightm, type);
      HfstTransducer tmp(pi_star);
      tmp.concatenate(leftm_to_leftm);
      tmp.concatenate(rightm_to_rightm);
      tmp.concatenate(pi_star);
      HfstTransducer cbt(pi_star);
      cbt.subtract(tmp);
      cbt.minimize();
      if (DEBUG) cbt.write_in_att_format("compiler_cbt.att",false);
      if (DEBUG) printf("  ..cbt created\n");

      // left context transducer .* (<R> >> (<L> >> LEFT_CONTEXT)) || !(.*<L>)    
      HfstTransducer lct = replace_context(context.first, leftm, rightm, alphabet); 

      lct.minimize();
      if (DEBUG) lct.write_in_att_format("compiler_lct.att",false);
      if (DEBUG) printf("  ..lct created\n");

      // right context transducer:  reversion( (<R> >> (<L> >> reversion(RIGHT_CONTEXT))) .* || !(<R>.*) )
      HfstTransducer right_rev(context.second);
      if (DEBUG) printf("(1)\n");
      right_rev.reverse();
      if (DEBUG) printf("(2)\n");
      right_rev.minimize();
      if (DEBUG) right_rev.write_in_att_format("right_rev.att",true);

      HfstTransducer rct = replace_context(right_rev, rightm, leftm, alphabet);
      if (DEBUG) printf("(3)\n");
      rct.reverse();
      if (DEBUG) printf("(4)\n");
      rct.minimize();
      if (DEBUG) printf("(5)\n");
      if (DEBUG) rct.write_in_att_format("compiler_rct.att",false);
      if (DEBUG) printf("  ..rct created\n");

      // unconditional replace transducer      
      HfstTransducer rt(type);
      if (repl_type == REPL_UP || repl_type == REPL_RIGHT || repl_type == REPL_LEFT)
	rt = replace_transducer( t, leftm, rightm, REPL_UP, alphabet );
      else
	rt = replace_transducer( t, leftm, rightm, REPL_DOWN, alphabet );
      if (DEBUG) printf("  minimizing rt\n");
      rt.minimize();
      if (DEBUG) printf("  ..rt createD\n");


      // build the conditional replacement transducer
      HfstTransducer result(ibt);
      if (DEBUG) printf("#0\n");
      result.compose(cbt);
      result.minimize(); // added
      
      if (DEBUG) result.write_in_att_format("result0.att", true);

      if (DEBUG) printf("#1\n");

      if (repl_type == REPL_UP || repl_type == REPL_RIGHT)
	result.compose(rct);

      if (repl_type == REPL_UP || repl_type == REPL_LEFT)
	result.compose(lct);
      
      if (DEBUG) printf("#2\n");

      result.minimize();  // ADDED

      if (DEBUG) result.write_in_att_format("result2.att", true);

      result.compose(rt);
      
      if (DEBUG) printf("#3\n");

      if (repl_type == REPL_DOWN || repl_type == REPL_RIGHT)
	result.compose(lct);

      if (repl_type == REPL_DOWN || repl_type == REPL_LEFT)
	result.compose(rct);
      
      if (DEBUG) printf("#4\n");

      result.minimize();  // ADDED

      if (DEBUG) result.write_in_att_format("result4.att", true);

      result.compose(rbt);

      if (DEBUG) printf("#5\n");

      // Remove the markers from the alphabet
      alphabet.erase(StringPair(leftm,leftm));
      alphabet.erase(StringPair(rightm,rightm));
      
      if (optional) {
	HfstTransducer pi_star_(alphabet, type, true);
	result.disjunct(pi_star_);
      }

      result.minimize();
      if (DEBUG) result.write_in_att_format("result.att",true);

      if (DEBUG) printf("...replace_in_context\n");

      return result;
    }


    HfstTransducer replace_up(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet) 
    { 
      return replace_in_context(context, REPL_UP, mapping, optional, alphabet);
    }

    HfstTransducer replace_down(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet) 
    { 
      return replace_in_context(context, REPL_DOWN, mapping, optional, alphabet);
    }

    HfstTransducer replace_right(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet) 
    { 
      return replace_in_context(context, REPL_RIGHT, mapping, optional, alphabet);
    }

    HfstTransducer replace_left(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet)
    { 
      return replace_in_context(context, REPL_LEFT, mapping, optional, alphabet);
    }

    HfstTransducer replace_up(HfstTransducer &mapping, bool optional, StringPairSet &alphabet)
    {
      return replace(mapping, REPL_UP, optional, alphabet);
    }

    HfstTransducer replace_down(HfstTransducer &mapping, bool optional, StringPairSet &alphabet)
    {
      return replace(mapping, REPL_DOWN, optional, alphabet);
    }


    HfstTransducer restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet,
			       TwolType twol_type, int direction ) 
    { //(void)contexts; (void)mapping; (void)alphabet;
      //throw hfst::exceptions::FunctionNotImplementedException(); } 

      // Make sure that contexts contains at least one transducer pair and that all
      // transducers in the set have the same type.
      ImplementationType type;
      bool type_defined=false;
      for (HfstTransducerPairVector::const_iterator it = contexts.begin(); it != contexts.end(); it++)
	{
	  if (not type_defined) {
	    type = it->first.get_type();
	    type_defined=true;
	  } 
	  else { 
	    if (type != it->first.get_type())
	      throw hfst::exceptions::TransducerTypeMismatchException();
	  }
	  if (type != it->second.get_type())
	    throw hfst::exceptions::TransducerTypeMismatchException();
 	}
      if (not type_defined)
	throw hfst::exceptions::EmptySetOfContextsException();

      std::string marker("@_MARKER_@");
      HfstTransducer mt(marker, type);
      HfstTransducer pi_star(alphabet, type, true);
      
      // center transducer
      HfstTransducer l1("@_EPSILON_SYMBOL_@", type);
      l1.concatenate(pi_star);
      l1.concatenate(mt);
      l1.concatenate(mapping);
      l1.concatenate(mt);
      l1.concatenate(pi_star);

      HfstTransducer tmp(type);
      if (direction == 0)
	tmp = pi_star;
      else if (direction == 1)
	tmp = mapping.input_project().compose(pi_star);
      else {
	tmp = pi_star;
	tmp.compose(mapping.output_project());
      }

      // context transducer pi_star + left[i] + mt + tmp + mt + + right[i] + pi_star
      HfstTransducer l2(type);
      for (HfstTransducerPairVector::const_iterator it = contexts.begin(); it != contexts.end(); it++)
	{
	  HfstTransducer ct("@_EPSILON_SYMBOL_@", type);
	  ct.concatenate(pi_star);
	  ct.concatenate(it->first);
	  ct.concatenate(mt);	  
	  ct.concatenate(tmp);
	  ct.concatenate(mt);
	  ct.concatenate(it->second);
	  ct.concatenate(pi_star);
	  l2.disjunct(ct);
	}

      HfstTransducer result(type);
      
      if (twol_type == twol_right) {
	// TheAlphabet - ( l1 - l2 ).substitute(marker,epsilon)
	HfstTransducer retval(alphabet, type, true);
	HfstTransducer tmp1(l1);
	tmp1.subtract(l2);
	tmp1.substitute(marker,"@_EPSILON_SYMBOL_@");
	retval.subtract(tmp1);
	return retval;
      }
      else if (twol_type == twol_left) {
	// TheAlphabet - ( l2 - l1 ).substitute(marker,epsilon)
	HfstTransducer retval(alphabet, type, true);
	HfstTransducer tmp1(l2);
	tmp1.subtract(l1);
	tmp1.substitute(marker,"@_EPSILON_SYMBOL_@");
	retval.subtract(tmp1);
	return retval;
      }
      else if (twol_type == twol_both) {
	// TheAlphabet - ( l1 - l2 ).substitute(marker,epsilon)
	// TheAlphabet - ( l2 - l1 ).substitute(marker,epsilon)
	// intersect
	HfstTransducer retval1(alphabet, type, true);
	HfstTransducer tmp1(l1);
	tmp1.subtract(l2);
	tmp1.substitute(marker,"@_EPSILON_SYMBOL_@");
	retval1.subtract(tmp1);

	HfstTransducer retval2(alphabet, type, true);
	HfstTransducer tmp2(l2);
	tmp2.subtract(l1);
	tmp2.substitute(marker,"@_EPSILON_SYMBOL_@");
	retval2.subtract(tmp2);

	return retval1.intersect(retval2);
      }
      else
	assert(false);

    }

      //Transducer *result=result_transducer( l1, l2, type, marker );
      /*
  Transducer *Interface::result_transducer( Transducer *l1, Transducer *l2,
					    Twol_Type type, Character marker )
  {
    Transducer *result=NULL;
    if (type == twol_right)
      result = restriction_transducer( l1, l2, marker );
    else if (type == twol_left)
      result = restriction_transducer( l2, l1, marker );
    else if (type == twol_both) {
      Transducer *t1 = restriction_transducer( l1, l2, marker );
      Transducer *t2 = restriction_transducer( l2, l1, marker );
      result = &(*t1 & *t2);
      delete t1;
      delete t2;
    }

    return result;
    }

  // TheAlphabet - ( l1 - l2 ).substitute(marker,epsilon)
  Transducer *Interface::restriction_transducer( Transducer *l1, Transducer *l2,
						 Character marker )
  {
    l1->alphabet.copy(TheAlphabet);
    Transducer *t1 = &(*l1 / *l2);

    Transducer *t2 = &t1->replace_char(marker, Label::epsilon);
    delete t1;

    t2->alphabet.copy(TheAlphabet);
    t1 = &(!*t2);
    delete t2;

    return t1;
  }
*/


    HfstTransducer restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) {
      return restriction(contexts, mapping, alphabet, twol_right, 0); 
    }

    HfstTransducer coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) { 
      return restriction(contexts, mapping, alphabet, twol_left, 0); 
    }

    HfstTransducer restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) {
      return restriction(contexts, mapping, alphabet, twol_both, 0); 
    }


    HfstTransducer surface_restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) { 
      return restriction(contexts, mapping, alphabet, twol_right, 1); 
    }

    HfstTransducer surface_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) { 
      return restriction(contexts, mapping, alphabet, twol_left, 1); 
    }

    HfstTransducer surface_restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) {  
      return restriction(contexts, mapping, alphabet, twol_both, 1); 
    }


    HfstTransducer deep_restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) {  
      return restriction(contexts, mapping, alphabet, twol_right, -1); 
    }

    HfstTransducer deep_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) {  
      return restriction(contexts, mapping, alphabet, twol_left, -1); 
    }

    HfstTransducer deep_restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet) {  
      return restriction(contexts, mapping, alphabet, twol_both, -1); 
    }

  }

}
