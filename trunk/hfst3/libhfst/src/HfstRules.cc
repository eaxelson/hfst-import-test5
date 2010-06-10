#include "HfstTransducer.h"

namespace hfst
{
  namespace rules
  {

    /* identical to  ![ .* l [a:. & !a:b] r .* ]  */
    HfstTransducer two_level_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet) 
    {
      if (context.first.get_type() != context.second.get_type())
	throw hfst::exceptions::TransducerTypeMismatchException();
      ImplementationType type = context.first.get_type();

      assert(context.first.get_type() != UNSPECIFIED_TYPE);
      assert(context.second.get_type() != ERROR_TYPE);
      assert(context.first.get_type() != UNSPECIFIED_TYPE);
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
      HfstTransducer neg_mappings(alphabet, type);
      neg_mappings.repeat_star();
      HfstTransducer mappings_tr(mappings, type);
      neg_mappings.subtract(mappings_tr);

      // center == [ a:. & !a:b ]
      center.intersect(neg_mappings);

      // left context == [ .* l ]
      HfstTransducer left_context(alphabet, type);
      left_context.repeat_star();
      left_context.concatenate(context.first);

      // right_context == [ r .* ]
      HfstTransducer right_context(context.second);
      HfstTransducer universal(alphabet, type);
      universal.repeat_star();
      right_context.concatenate(universal);

      HfstTransducer inside(left_context.concatenate(center).concatenate(right_context));

      HfstTransducer retval(universal.subtract(inside));
      return retval;
    }

    // equivalent to !(!(.* l) a:b .* j .* a:b !(r .*))
    HfstTransducer two_level_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet) 
    { 
      if (context.first.get_type() != context.second.get_type())
	throw hfst::exceptions::TransducerTypeMismatchException();
      ImplementationType type = context.first.get_type();

      assert(context.first.get_type() != UNSPECIFIED_TYPE);
      assert(context.second.get_type() != ERROR_TYPE);
      assert(context.first.get_type() != UNSPECIFIED_TYPE);
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
      HfstTransducer neg_mappings(alphabet, type);
      neg_mappings.repeat_star();
      HfstTransducer mappings_tr(mappings, type);
      neg_mappings.subtract(mappings_tr);

      // center == [ a:. & !a:b ]
      center.intersect(neg_mappings);

      // left context == [ .* l ]
      HfstTransducer left_context(alphabet, type);
      left_context.repeat_star();
      left_context.concatenate(context.first);

      // right_context == [ r .* ]
      HfstTransducer right_context(context.second);
      HfstTransducer universal(alphabet, type);
      universal.repeat_star();
      right_context.concatenate(universal);

      HfstTransducer inside(left_context.concatenate(center).concatenate(right_context));

      HfstTransducer retval(universal.subtract(inside));
      return retval;
    }



    HfstTransducer two_level_if_and_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 

    HfstTransducer replace_up(HfstTransducerPairSet &contexts, HfstTransducer &mapping, bool optional, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer replace_down(HfstTransducerPairSet &contexts, HfstTransducer &mapping, bool optional, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer replace_right(HfstTransducerPairSet &contexts, HfstTransducer &mapping, bool optional, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer replace_left(HfstTransducerPairSet &contexts, HfstTransducer &mapping, bool optional, StringPairSet &alphabet)
    { throw hfst::exceptions::FunctionNotImplementedException(); } 

    HfstTransducer restriction(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer restriction_and_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer surface_restriction(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer surface_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer surface_restriction_and_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer deep_restriction(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer deep_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 
    HfstTransducer deep_restriction_and_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet) 
    { throw hfst::exceptions::FunctionNotImplementedException(); } 

  }

}
