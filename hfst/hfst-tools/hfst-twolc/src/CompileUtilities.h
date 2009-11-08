#ifndef _COMPILE_UTILITIES_H_
#define _COMPILE_UTILITIES_H_
#include "../config.h"
#include <hfst2/hfst.h>
#include <cassert>

typedef map<HFST::Key,HFST::KeyPairSet*> KeyImageMap;
typedef set<HFST::KeyPairSet*> GarbageSet;

class TwolCAlphabet
{
 private:
  HFST::KeyTable * kt;
  HFST::KeyPairSet * alphabet_set;
  KeyImageMap key_images;
  GarbageSet garbage_set;
  HFST::Key marker;
  HFST::Symbol define_symbol(const char * symbol);
  void associate(HFST::Key k, HFST::Symbol s);
  HFST::Key associate_next(HFST::Symbol s);
  void define_epsilon(const char * symbol);
  void define_marker(const char * symbol);
 public:
  TwolCAlphabet(void):
    kt(HFST::create_key_table()),
    alphabet_set(HFST::create_empty_keypair_set())
    {
      define_epsilon("@0@");
      define_marker("__HFST_TWOLC_CENTER_MARKER");
    }
  ~TwolCAlphabet(void)
    {
      delete kt;
      destroy(alphabet_set);
      for (KeyImageMap::iterator it = key_images.begin();
	   it != key_images.end();
	   ++it)
	{
	  destroy(it->second);
	}
      for (GarbageSet::iterator it = garbage_set.begin();
	   it != garbage_set.end();
	   ++it)
	{
	  delete *it;
	}
    }
  static HFST::KeyPairSet * join(HFST::KeyPairSet * s1,
				 HFST::KeyPairSet * s2);
  static void destroy(HFST::KeyPairSet * set);
  HFST::Key get_marker(void);
  HFST::KeyTable * get_key_table(void);
  HFST::KeyPairSet * get_alphabet_set(void);
  HFST::KeyPairSet * get_input_matching_pairs(HFST::Key input);
  HFST::KeyPairSet * get_input_matching_pairs(HFST::KeyPairSet * key_pair_set);
  HFST::Key get_key(char * symbol);
  const char * get_name(HFST::Key k);
  void define_pair(char * input,
		   char * output);
  HFST::KeyPair * get_pair(char * input,
			   char * output);
};

class Weighted 
{
 protected:
  HWFST::TransducerHandle t;
 public:
  Weighted(void): t(HWFST::create_epsilon_transducer()) {};
  Weighted(HWFST::Key input, HWFST::Key output) 
  {
    HWFST::KeyPair * kp = HWFST::define_keypair(input,output);
    t = HWFST::define_transducer(kp);
    delete kp;
  };
  Weighted(char * input, char * output, TwolCAlphabet &alphabet)
  {
    HWFST::KeyPair * kp = alphabet.get_pair(input,output);
    t = HWFST::define_transducer(kp);
    delete kp;
  }
  Weighted(HWFST::TransducerHandle tr): t(tr) {};
  HWFST::TransducerHandle operator() (void); 
  void display(TwolCAlphabet &alphabet);
  void display(void);
  void destroy_fst(void);
  void store(ostream &out,KeyTable * kt);
  Weighted insert(HWFST::KeyPairSet * key_pair_set);
  static Weighted universal(TwolCAlphabet &alphabet);
  static Weighted any_pair(TwolCAlphabet &alphabet);
  static Weighted empty_language(void);
  static Weighted to_transducer(HWFST::KeyPairSet * set);
  static Weighted input_image(HWFST::KeyPairSet * set, 
			      TwolCAlphabet &alphabet);
  Weighted copy(void);
  Weighted containment(TwolCAlphabet &alphabet);
  Weighted containment_once(TwolCAlphabet &alphabet);
  Weighted lose(HWFST::Key marker);
  Weighted repeat_star(void);
  Weighted repeat_plus(void);
  Weighted optionalize(void);
  Weighted concatenate(Weighted another); 
  Weighted disjunct(Weighted another); 
  Weighted intersect(Weighted another);
  Weighted subtract(Weighted another);
  bool is_less_than(Weighted another);
  bool is_intersecting(Weighted another);
};

class Unweighted 
{
 protected:
  HFST::TransducerHandle t;
 public:
  Unweighted(void): t(HFST::create_epsilon_transducer()) {};
  Unweighted(HFST::Key input, HFST::Key output) 
  {
    HFST::KeyPair * kp = HFST::define_keypair(input,output);
    t = HFST::define_transducer(kp);
    delete kp;
  };
  Unweighted(char * input, char * output, TwolCAlphabet &alphabet)
  {
    HFST::KeyPair * kp = alphabet.get_pair(input,output);
    t = HFST::define_transducer(kp);
    delete kp;
  }
  Unweighted(HFST::TransducerHandle tr): t(tr) {};
  HFST::TransducerHandle operator() (void); 
  void display(TwolCAlphabet &alphabet);
  void display(void);
  void destroy_fst(void);
  void store(ostream &out,KeyTable * kt);
  Unweighted insert(HFST::KeyPairSet * key_pair_set);
  Unweighted copy(void);
  Unweighted lose(HFST::Key marker);
  Unweighted containment(TwolCAlphabet &alphabet);
  Unweighted containment_once(TwolCAlphabet &alphabet);
  Unweighted repeat_star(void);
  Unweighted repeat_plus(void);
  Unweighted optionalize(void);
  static Unweighted universal(TwolCAlphabet &alphabet);
  static Unweighted any_pair(TwolCAlphabet &alphabet);
  static Unweighted empty_language(void);
  static Unweighted to_transducer(HFST::KeyPairSet * set);
  static Unweighted input_image(HFST::KeyPairSet * set, 
				TwolCAlphabet &alphabet);
  Unweighted concatenate(Unweighted another); 
  Unweighted disjunct(Unweighted another); 
  Unweighted intersect(Unweighted another);
  Unweighted subtract(Unweighted another);
  bool is_less_than(Unweighted another);
  bool is_intersecting(Unweighted another);
};

template<class T> class ContextCondition
{
 private:
  T left;
  T center;
  T right;
  HFST::Key marker;
  T left_universal;
  T right_universal;
  T marker_language(void)
  {
    return T(marker,marker);
  }
 public:
  ContextCondition(T l, T c, T r, TwolCAlphabet &alphabet):
   left(l), center(c), right(r), marker(alphabet.get_marker()), 
   left_universal(T::universal(alphabet)), 
   right_universal(T::universal(alphabet)) {};
  T operator() (void)
  {
    T marker_language1 = marker_language();
    T marker_language2 = marker_language();
    left_universal = left_universal.concatenate(left);
    left_universal = left_universal.concatenate(marker_language1);
    left_universal = left_universal.concatenate(center);
    left_universal = left_universal.concatenate(marker_language2);
    left_universal = left_universal.concatenate(right);
    left_universal = left_universal.concatenate(right_universal);
    return left_universal;
  }
};

template<class T> class RightArrowRuleCenterCondition : 
public ContextCondition<T>
{
 public:
  RightArrowRuleCenterCondition(T c, TwolCAlphabet &alphabet): 
    ContextCondition<T>(T(),c,T(),alphabet) {};
  RightArrowRuleCenterCondition(HFST::KeyPairSet * key_pair_set, 
				TwolCAlphabet &alphabet):
    ContextCondition<T>
      (T(),
       T::to_transducer(key_pair_set),
       T(), alphabet) {};

};

template<class T> class LeftArrowRuleCenterCondition :
public ContextCondition<T>
{
 public:
  LeftArrowRuleCenterCondition(HFST::Key input, 
			       HFST::Key output,
			       TwolCAlphabet &alphabet):
  ContextCondition<T>
    (T(),
     T::to_transducer
     (alphabet.get_input_matching_pairs(input)).subtract(T(input,
							   output)),
     T(),alphabet) {};
  LeftArrowRuleCenterCondition(HFST::KeyPairSet * key_pair_set, 
			       TwolCAlphabet &alphabet):
  ContextCondition<T>
    (T(),
     T::to_transducer
     (alphabet.get_input_matching_pairs(key_pair_set)).
     subtract(T::to_transducer(key_pair_set)),
     T(), alphabet) {};
};

template<class T> class RestrictionArrowRuleCenterCondition :
public RightArrowRuleCenterCondition<T>
{
 public:
 RestrictionArrowRuleCenterCondition(HFST::Key input, 
				     HFST::Key output,
				     TwolCAlphabet &alphabet):
    RightArrowRuleCenterCondition<T>(T(input,output),alphabet) {};
 RestrictionArrowRuleCenterCondition(HFST::KeyPairSet * key_pair_set, 
				     TwolCAlphabet &alphabet):
    RightArrowRuleCenterCondition<T>(key_pair_set,alphabet) {};
};

template<class T> class LeftArrowEpenthesisRuleCenterCondition :
public ContextCondition<T>
{
 public:
  LeftArrowEpenthesisRuleCenterCondition(HFST::Key output,
					 TwolCAlphabet &alphabet):
  ContextCondition<T>
    (T(),
     (T::to_transducer
     (alphabet.get_input_matching_pairs(HFST::Epsilon)).
     subtract(T(HFST::Epsilon,
		output))).optionalize(),
     T(),alphabet) {};
};

template<class T> class RuleContextCondition : public ContextCondition<T>
{
 public:
  RuleContextCondition(T l, T r, TwolCAlphabet &alphabet): 
  ContextCondition<T>(l,T::universal(alphabet),r,alphabet) {};
};

#endif
