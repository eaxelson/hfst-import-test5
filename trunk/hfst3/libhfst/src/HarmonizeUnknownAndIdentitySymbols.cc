#include "HarmonizeUnknownAndIdentitySymbols.h"

namespace hfst
{

const char * HarmonizeUnknownAndIdentitySymbols::identity = 
"@_IDENTITY_SYMBOL_@";
const char * HarmonizeUnknownAndIdentitySymbols::unknown = 
"@_UNKNOWN_SYMBOL_@";

size_t max(size_t t1,size_t t2)
{ return t1 < t2 ? t2 : t1; }

HarmonizeUnknownAndIdentitySymbols::HarmonizeUnknownAndIdentitySymbols
(HfstBasicTransducer &t1,HfstBasicTransducer &t2) :
  t1(t1),
  t2(t2)
{
  populate_symbol_set(t1,t1_symbol_set);
  populate_symbol_set(t2,t2_symbol_set);
  
  std::vector<std::string> diff_vector
    (max(t1_symbol_set.size(),t2_symbol_set.size()),"");
  
  if (debug_harmonize)
    { debug_harmonize_print("Computing t1 symbols - t2 symbols."); }

  std::vector<std::string>::iterator diff_end =
  std::set_difference(t1_symbol_set.begin(),t1_symbol_set.end(),
		      t2_symbol_set.begin(),t2_symbol_set.end(),
		      diff_vector.begin());

  StringSet t1_symbols_minus_t2_symbols(diff_vector.begin(),diff_end);

  t1_symbols_minus_t2_symbols.erase(identity);
  t1_symbols_minus_t2_symbols.erase(unknown);

  if (debug_harmonize)
    {
      debug_harmonize_print("Symbols:");
      debug_harmonize_print(t1_symbols_minus_t2_symbols);
    }

  if (debug_harmonize)
    { debug_harmonize_print("Computing t2 symbols - t1 symbols."); }

  diff_end = std::set_difference(t2_symbol_set.begin(),t2_symbol_set.end(),
				 t1_symbol_set.begin(),t1_symbol_set.end(),
				 diff_vector.begin());

  StringSet t2_symbols_minus_t1_symbols(diff_vector.begin(),diff_end);

  t1_symbols_minus_t2_symbols.erase(unknown);
  t1_symbols_minus_t2_symbols.erase(identity);

  if (debug_harmonize)
    {
      debug_harmonize_print("Symbols:");
      debug_harmonize_print(t2_symbols_minus_t1_symbols);
    }
  
  if (debug_harmonize)
    { debug_harmonize_print("Harmonizing identity symbols."); }

  harmonize_identity_symbols(t1,t2_symbols_minus_t1_symbols);
  harmonize_identity_symbols(t2,t1_symbols_minus_t2_symbols);

  if (debug_harmonize)
    { debug_harmonize_print("Harmonizing unknown symbols."); }

  harmonize_unknown_symbols(t1,t2_symbols_minus_t1_symbols);
  harmonize_unknown_symbols(t2,t1_symbols_minus_t2_symbols);

  if (debug_harmonize)
    {
      if (t2_symbols_minus_t1_symbols.empty())
	{ debug_harmonize_print("t1 includes no symbols not found in t2."); }
      else
	{ 
	  HfstTransducer fst1(t1,TROPICAL_OPENFST_TYPE);
	  std::cerr << fst1 << std::endl; 
	}

      if (t1_symbols_minus_t2_symbols.empty())
	{ debug_harmonize_print("t2 includes no symbols not found in t1."); }
      else
	{ 
	  HfstTransducer fst2(t2,TROPICAL_OPENFST_TYPE);
	  std::cerr << fst2 << std::endl; 
	}
    }
}

void HarmonizeUnknownAndIdentitySymbols::populate_symbol_set
(const HfstBasicTransducer &t,StringSet &s)
{
  for (HfstBasicTransducer::const_iterator it = t.begin(); it != t.end(); ++it)
    {
      for (HfstBasicTransducer::HfstTransitions::const_iterator jt = 
	     it->second.begin();
	   jt != it->second.end();
	   ++jt)
	{
	  s.insert(jt->get_input_symbol());
	  s.insert(jt->get_output_symbol());
	}
    }
  if (debug_harmonize)
    {
      debug_harmonize_print("Symbols:");
      debug_harmonize_print(s);
    }
}

void HarmonizeUnknownAndIdentitySymbols::harmonize_identity_symbols
(HfstBasicTransducer &t,const StringSet &missing_symbols)
{
  if (missing_symbols.empty())
    { return; }

  for (HfstBasicTransducer::iterator it = t.begin(); it != t.end(); ++it)
    {

      HfstBasicTransducer::HfstTransitions added_transitions;

      for (HfstBasicTransducer::HfstTransitions::const_iterator jt = 
	     it->second.begin();
	   jt != it->second.end();
	   ++jt)
	{
	  if (jt->get_input_symbol() == identity)
	    {
	      assert(jt->get_output_symbol() == identity);
	      for (StringSet::const_iterator kt = missing_symbols.begin();
		   kt != missing_symbols.end();
		   ++kt)
		{ added_transitions.push_back
		    (HfstBasicTransition(jt->get_target_state(),
					 *kt,*kt,
					 jt->get_weight())); }
	    }
	}
      it->second.insert(it->second.end(),
			added_transitions.begin(),added_transitions.end());
    }  
}

void HarmonizeUnknownAndIdentitySymbols::harmonize_unknown_symbols
(HfstBasicTransducer &t,const StringSet &missing_symbols)
{
  if (missing_symbols.empty())
    { return; }

  for (HfstBasicTransducer::iterator it = t.begin(); it != t.end(); ++it)
    {
      HfstBasicTransducer::HfstTransitions added_transitions;

      for (HfstBasicTransducer::HfstTransitions::const_iterator jt = 
	     it->second.begin();
	   jt != it->second.end();
	   ++jt)
	{
	  if (jt->get_input_symbol() == unknown)
	    {
	      assert(jt->get_output_symbol() != identity);

	      for (StringSet::const_iterator kt = missing_symbols.begin();
		   kt != missing_symbols.end();
		   ++kt)
		{ 
		  added_transitions.push_back
		    (HfstBasicTransition(jt->get_target_state(),
					 *kt,jt->get_output_symbol(),
					 jt->get_weight())); 
		}
	    }
	  if (jt->get_output_symbol() == unknown)
	    {
	      assert(jt->get_input_symbol() != identity);

	      for (StringSet::const_iterator kt = missing_symbols.begin();
		   kt != missing_symbols.end();
		   ++kt)
		{ added_transitions.push_back
		    (HfstBasicTransition(jt->get_target_state(),
					 jt->get_input_symbol(),*kt,
					 jt->get_weight())); }

	    }
	  if (jt->get_input_symbol() == unknown and 
	      jt->get_output_symbol() == unknown)
	    {
	      for (StringSet::const_iterator kt = missing_symbols.begin();
		   kt != missing_symbols.end();
		   ++kt)
		{ 
		  for (StringSet::const_iterator lt = missing_symbols.begin();
		       lt != missing_symbols.end();
		       ++lt)
		    { 
		      if (kt == lt)
			{ continue; }

		      added_transitions.push_back
			(HfstBasicTransition(jt->get_target_state(),
					     jt->get_input_symbol(),*kt,
					     jt->get_weight())); 
		    }
		}
	    }
	}
      it->second.insert(it->second.end(),
			added_transitions.begin(),added_transitions.end());
    }
}

void debug_harmonize_print(const StringSet &s)
{
  for (StringSet::const_iterator it = s.begin();
       it != s.end();
       ++it)
    { std::cerr << *it << std::endl; }
}

void debug_harmonize_print(const std::string &s)
{ std::cerr << s << std::endl; }

}

#ifdef TEST_HARMONIZE_UNKNOWN_AND_IDENTITY_SYMBOLS
using hfst::HfstInputStream;
using hfst::HfstTransducer;
using hfst::HfstOutputStream;

int main(void)
{
  HfstInputStream in;
  
  std::cerr << "Reading first transducer" << std::endl;
  HfstTransducer t1(in);

  std::cerr << "Converting first transducer to basic transducer" << std::endl;
  HfstBasicTransducer b1(t1);

  std::cerr << "Reading second transducer" << std::endl;
  HfstTransducer t2(in);

  std::cerr << "Converting second transducer to basic transducer" << std::endl;
  HfstBasicTransducer b2(t2);
  
  std::cerr << "Harmonizing" << std::endl;
  HarmonizeUnknownAndIdentitySymbols(b1,b2);

  t1 = HfstTransducer(b1,TROPICAL_OPENFST_TYPE);
  t2 = HfstTransducer(b2,TROPICAL_OPENFST_TYPE);

  HfstOutputStream out(TROPICAL_OPENFST_TYPE);

  std::cerr << "composing" << std::endl;
  out << t1.compose(t2);
}
#endif // TEST_HARMONIZE_UNKNOWN_AND_IDENTITY_SYMBOLS
