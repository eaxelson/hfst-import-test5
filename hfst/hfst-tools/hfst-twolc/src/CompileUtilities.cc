#include "CompileUtilities.h"

void TwolCAlphabet::associate(HFST::Key k, HFST::Symbol s)
{
  return HFST::associate_key(k,kt,s);
}

HFST::Key TwolCAlphabet::associate_next(HFST::Symbol s)
{
  HFST::Key k = HFST::get_unused_key(kt);
  HFST::associate_key(k,kt,s);
  return k;
}

HFST::KeyPairSet * TwolCAlphabet::join(HFST::KeyPairSet * s1,
				       HFST::KeyPairSet * s2)
{
  for (HFST::KeyPairSet::iterator it = s2->begin();
       it != s2->end();
       ++it)
    {
      if (s1->find(*it) == s1->end())
	{
	  s1->insert(*it);
	}
      else
	{
	  delete *it;
	}
    }
  delete s2;
  return s1;
}


HFST::Symbol TwolCAlphabet::define_symbol(const char * symbol)
{
  return HFST::define_symbol(symbol);
}

void TwolCAlphabet::destroy(HFST::KeyPairSet * set)
{
  for (HFST::KeyPairSet::iterator it = set->begin();
       it != set->end();
       ++it)
    {
      delete *it;
    }
  delete set;
}

HFST::Key TwolCAlphabet::get_marker(void)
{
  return marker;
}

const char * TwolCAlphabet::get_name(HFST::Key k)
{
  try
    {
      return HFST::get_symbol_name(HFST::get_key_symbol(k,kt));
    }
  catch (const char * err)
    {
      std::cerr << err << " This is a bug. Please report it at"
	" hfst-bugs@helsinki.fi" << std::endl;
      assert(false);
    }
  return NULL;
}

HFST::KeyTable * TwolCAlphabet::get_key_table(void)
{
  return kt;
}

HFST::KeyPairSet * TwolCAlphabet::get_alphabet_set(void)
{
  return alphabet_set;
}

HFST::KeyPairSet * TwolCAlphabet::get_input_matching_pairs(HFST::Key input)
{
  if (key_images.find(input) != key_images.end())
    {
      return key_images[input];
    }
  HFST::KeyPairSet * input_matches = HFST::create_empty_keypair_set();
  for (HFST::KeyPairSet::iterator it = alphabet_set->begin();
       it != alphabet_set->end();
       ++it)
    {
      if (HFST::get_input_key(*it) == input)
	{
	  input_matches->insert(HFST::define_keypair
				(HFST::get_input_key(*it),
				 HFST::get_output_key(*it)));
	}
    }
  key_images[input] = input_matches;
  return input_matches;
}

typedef std::set<HFST::Key> InputKeys;

HFST::KeyPairSet * 
TwolCAlphabet::get_input_matching_pairs(HFST::KeyPairSet * key_pair_set)
{
  InputKeys k;
  for (KeyPairSet::iterator it = key_pair_set->begin();
       it != key_pair_set->end();
       ++it)
    {
      HFST::Key input = HFST::get_input_key(*it);
      k.insert(input);
    }
  HFST::KeyPairSet * result_key_pair_set = new HFST::KeyPairSet;;
  for (InputKeys::iterator it = k.begin();
       it != k.end();
       ++it)
    {
      result_key_pair_set->insert(get_input_matching_pairs(*it)->begin(),
				  get_input_matching_pairs(*it)->end());
    }
  garbage_set.insert(result_key_pair_set);
  return result_key_pair_set;
}

HFST::Key TwolCAlphabet::get_key(char * symbol)
{
  HFST::Symbol s = define_symbol(symbol);
  free(symbol);
  if (not HFST::is_symbol(s,kt))
    {
      associate_next(s);
    }
  return HFST::get_key(s,kt);
}

void TwolCAlphabet::define_epsilon(const char * symbol)
{
  HFST::Symbol epsilon_symbol = define_symbol(symbol);
  associate(HFST::Epsilon,epsilon_symbol);
}

void TwolCAlphabet::define_marker(const char * symbol)
{
  HFST::Symbol marker_symbol = define_symbol(symbol);
  marker = associate_next(marker_symbol);
}

void TwolCAlphabet::define_pair(char * input,
				char * output)
{
  HFST::Key input_key = get_key(input);
  HFST::Key output_key = get_key(output);
  HFST::KeyPair * kp = HFST::define_keypair(input_key,output_key);
  if (alphabet_set->find(kp) == alphabet_set->end())
    {
      alphabet_set->insert(kp);
    }
  else
    {
      delete kp;
    }
}

HFST::KeyPair * TwolCAlphabet::get_pair(char * input,
					char * output)
{
  HFST::Key input_key = get_key(input);
  HFST::Key output_key = get_key(output);
  HFST::KeyPair * kp = HFST::define_keypair(input_key,output_key);
  assert(HFST::has_keypair(kp,alphabet_set));
  return kp;
}

HWFST::TransducerHandle Weighted::operator() (void)
{
  return t;
}

void Weighted::display(TwolCAlphabet &alphabet)
{
  HWFST::print_transducer(t,alphabet.get_key_table());
}

void Weighted::display(void)
{
  std::cerr << "DEBUG PRINT!!!" << std::endl;
  HWFST::print_transducer_number(t);
  std::cerr << "DEBUG PRINT!!!" << std::endl;
}

void Weighted::destroy_fst(void)
{
  HWFST::delete_transducer(t);
}

void Weighted::store(ostream &out,HWFST::KeyTable * kt)
{
  HWFST::write_transducer(t,kt,out);
}

Weighted Weighted::universal(TwolCAlphabet &alphabet)
{
  return 
    Weighted(HWFST::define_transducer(alphabet.get_alphabet_set())).
    repeat_star();
}

Weighted Weighted::any_pair(TwolCAlphabet &alphabet)
{
  return 
    Weighted(HWFST::define_transducer(alphabet.get_alphabet_set()));
}

Weighted Weighted::containment(TwolCAlphabet &alphabet)
{
  return 
    universal(alphabet).concatenate
    (concatenate(Weighted(t)).concatenate
     (concatenate(universal(alphabet))));
}

Weighted Weighted::containment_once(TwolCAlphabet &alphabet)
{
  Weighted containment_twice =
    (copy().concatenate(universal(alphabet)).concatenate(copy())).
    containment(alphabet);
  return
    containment(alphabet).subtract(containment_twice);
}

Weighted Weighted::empty_language(void)
{
  return Weighted(HWFST::create_empty_transducer());
}

Weighted Weighted::to_transducer(HWFST::KeyPairSet * set)
{
  return Weighted(HWFST::define_transducer(set));
}

Weighted Weighted::input_image(HWFST::KeyPairSet * set, 
			       TwolCAlphabet &alphabet)
{
  Weighted set_transducer = to_transducer(set);
  set_transducer.t = 
    HWFST::minimize(HWFST::extract_input_language(set_transducer.t));
  Weighted alphabet_transducer = universal(alphabet);
  set_transducer.t = HWFST::compose(set_transducer.t,
				    alphabet_transducer.t);
  return set_transducer;
}


Weighted Weighted::copy(void)
{
  return Weighted(HWFST::copy(t));
}

Weighted Weighted::lose(HWFST::Key marker)
{
  return 
    Weighted(HWFST::minimize(HWFST::substitute_key(t,marker,HWFST::Epsilon)));
}

Weighted Weighted::insert(HWFST::KeyPairSet * key_pair_set)
{
  for (HWFST::KeyPairSet::iterator it = key_pair_set->begin();
       it != key_pair_set->end();
       ++it)
    {
      t = HWFST::minimize(HWFST::insert_freely(t,*it));
    }
  TwolCAlphabet::destroy(key_pair_set);
  return Weighted(t);
}

Weighted Weighted::repeat_star(void)
{
  return Weighted(HWFST::repeat_star(t));
}

Weighted Weighted::optionalize(void)
{
  return Weighted(HWFST::optionalize(t));
}

Weighted Weighted::repeat_plus(void)
{
  return Weighted(HWFST::repeat_plus(t));
}

Weighted Weighted::concatenate(Weighted another)
{
  return Weighted(HWFST::minimize(HWFST::concatenate(t,another.t)));
}

Weighted Weighted::disjunct(Weighted another)
{
  return Weighted(HWFST::minimize(HWFST::disjunct(t,another.t)));
}

Weighted Weighted::intersect(Weighted another)
{
  return Weighted(HWFST::minimize(HWFST::intersect(t,another.t)));
}

Weighted Weighted::subtract(Weighted another)
{
  return Weighted(HWFST::minimize
		  (HWFST::subtract(t,another.t)));
}

bool Weighted::is_less_than(Weighted another)
{
  return HWFST::is_subset(t,another.t);
}

bool Weighted::is_intersecting(Weighted another)
{
  return not HWFST::are_disjoint(t,another.t);
}


HFST::TransducerHandle Unweighted::operator() (void)
{
  return t;
}

void Unweighted::display(TwolCAlphabet &alphabet)
{
  HFST::print_transducer(t,alphabet.get_key_table());
}

void Unweighted::display(void)
{
  std::cerr << "DEBUG PRINT!!!" << std::endl;
  HFST::print_transducer_number(t);
  std::cerr << "DEBUG PRINT!!!" << std::endl;
}

void Unweighted::destroy_fst(void)
{
  HFST::delete_transducer(t);
}

void Unweighted::store(ostream &out,HFST::KeyTable * kt)
{
  HFST::write_transducer(t,kt,out);
}

Unweighted Unweighted::universal(TwolCAlphabet &alphabet)
{
  return 
    Unweighted(HFST::define_transducer(alphabet.get_alphabet_set())).
    repeat_star();
}

Unweighted Unweighted::containment(TwolCAlphabet &alphabet)
{
  return 
    universal(alphabet).concatenate
    (concatenate(Unweighted(t)).concatenate
     (concatenate(universal(alphabet))));
}

Unweighted Unweighted::containment_once(TwolCAlphabet &alphabet)
{
  Unweighted containment_twice =
    (copy().concatenate(universal(alphabet)).concatenate(copy())).
    containment(alphabet);
  return
    containment(alphabet).subtract(containment_twice);
}

Unweighted Unweighted::any_pair(TwolCAlphabet &alphabet)
{
  return 
    Unweighted(HFST::define_transducer(alphabet.get_alphabet_set()));
}


Unweighted Unweighted::empty_language(void)
{
  return Unweighted(HFST::create_empty_transducer());
}

Unweighted Unweighted::to_transducer(HFST::KeyPairSet * set)
{
   Unweighted u(HFST::define_transducer(set));
   return u;
}

Unweighted Unweighted::input_image(HFST::KeyPairSet * set, 
				   TwolCAlphabet &alphabet)
{
  Unweighted set_transducer = to_transducer(set);
  set_transducer.t = 
    HFST::minimize(HFST::extract_input_language(set_transducer.t));
  Unweighted alphabet_transducer = universal(alphabet);
  set_transducer.t = HFST::compose(set_transducer.t,
				   alphabet_transducer.t);
  return set_transducer;
}

Unweighted Unweighted::copy(void)
{
  return Unweighted(HFST::copy(t));
}

Unweighted Unweighted::lose(HFST::Key marker)
{
  return 
    Unweighted(HFST::minimize(HFST::substitute_key(t,marker,HFST::Epsilon)));
}

Unweighted Unweighted::insert(HFST::KeyPairSet * key_pair_set)
{
  for (HFST::KeyPairSet::iterator it = key_pair_set->begin();
       it != key_pair_set->end();
       ++it)
    {
      t = HFST::minimize(HFST::insert_freely(t,*it));
    }
  TwolCAlphabet::destroy(key_pair_set);
  return Unweighted(t);
}

Unweighted Unweighted::repeat_star(void)
{
  return Unweighted(HFST::repeat_star(t));
}

Unweighted Unweighted::repeat_plus(void)
{
  return Unweighted(HFST::repeat_plus(t));
}

Unweighted Unweighted::optionalize(void)
{
  return Unweighted(HFST::optionalize(t));
}

Unweighted Unweighted::concatenate(Unweighted another)
{
  return Unweighted(HFST::minimize(HFST::concatenate(t,another.t)));
}

Unweighted Unweighted::disjunct(Unweighted another)
{
  return Unweighted(HFST::minimize(HFST::disjunct(t,another.t)));
}

Unweighted Unweighted::intersect(Unweighted another)
{
  return Unweighted(HFST::minimize(HFST::intersect(t,another.t)));
}

Unweighted Unweighted::subtract(Unweighted another)
{
  try {
    return Unweighted(HFST::minimize(HFST::subtract(t,another.t)));
  }
  catch (const char * err)
    {
      std::cerr << err << " This is a bug. Please report it at"
	" hfst-bugs@helsinki.fi" << std::endl;
      assert(false);
    }
}

bool Unweighted::is_less_than(Unweighted another)
{
  return HFST::is_subset(t,another.t);
}

bool Unweighted::is_intersecting(Unweighted another)
{
  return not HFST::are_disjoint(t,another.t);
}


