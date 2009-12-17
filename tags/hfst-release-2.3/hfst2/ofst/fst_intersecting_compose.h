#pragma once 

#include "fst/lib/fstlib.h"
#include <vector>
#include <set>

typedef std::set<unsigned short> KeySet;

using std::vector;


namespace fst {


using fst::StdArc;
using fst::StdVectorFst;
using fst::ArcIterator;
using fst::ILabelCompare;
using fst::OLabelCompare;
using fst::ArcSort;
using fst::kArcSortProperties;
using fst::kILabelSorted;

typedef StdArc Arc;
typedef StdVectorFst Transducer;
typedef Transducer::StateId StateId;
typedef Arc::Label Label;

class Tree {

 private:

  static Transducer * composition;

  static StateId new_state( void ) {
    return composition->AddState();

  };

  class STree {
 
   private:
    
    STree * right;
    StateId target;
    
   public:

    StateId compare;
    STree * left;
    
    // a b c => [compare a right [ compare b right [ compare c right [ compare -1 target new_state ]]]] t = target
    STree( vector<StateId>::iterator it, vector<StateId>::iterator end, StateId &t) :
      right(NULL),
      target(-1),
      compare(-1),
      left(NULL)
    {
      
      if ( it == end ) {
	t = target = new_state();
      }
      else {
	compare = *it;
	right = new STree( it+1, end, t );
      }
      
    };
    
    ~STree( void ) {
      delete left;
      delete right;
    };
    
    StateId find(vector<StateId>::iterator it, vector<StateId>::iterator end, bool &new_target) {
      if ( it == end )
	return target;
      if ( *it == compare ) {
	if ( (it + 1) != end )
	  if ( *(it + 1) > right->compare ) {
	    StateId t = -1;
	    new_target = true;
	    STree * new_right = new STree( it+1,end, t);
	    new_right->left = right;
	    right = new_right;
	    return t;
	  }
	return right->find( it+1, end, new_target );
      }
      if ( left == NULL ) {
	StateId t = -1;
	new_target = true;
	left = new STree( it, end, t );
	return t;
      }
      if ( *it > left->compare ) {
	STree * tmp = left;
	StateId t = -1;
	new_target = true;
	left = new STree( it, end, t );
	left->left = tmp;
	return t;
      }
      return left->find( it, end, new_target );
    };
    
  };
  
  STree * Root;
  
 public:

  static void init( Transducer * t ) {
    composition = t;
  }

  Tree( vector<StateId>::iterator it, vector<StateId>::iterator end, StateId &t) {

    if ( not (it < end) )
      throw "Logical Error!\n\n";
    Root = new STree( it, end, t);

  };

  ~Tree( void ) { delete Root; };

  StateId find( vector<StateId>::iterator it, vector<StateId>::iterator end, bool &new_target) {
    if ( not (it < end) )
      throw "Logical Error!\n\n";

    new_target = false;

    if ( *it < Root->compare ) {
      STree * tmp = Root;

      StateId t = -1;
      new_target = true;
      Root = new STree( it, end, t);
      
      Root->left = tmp;
      return t;
    }

    return Root->find(it, end, new_target);

  };

};

//Transducer * Tree::composition;


struct LabelPair {

  Label input;
  Label output;

};

class input_compare {
  
 public:
  bool operator() ( const StdArc &arc1, const StdArc &arc2 ) const {
    if ( arc1.ilabel == arc2.ilabel )
      return arc1.olabel < arc2.olabel;
    return arc1.ilabel < arc2.ilabel;
  };

  uint64 Properties(uint64 props) const {
    return props & kArcSortProperties;
  };
};

class output_compare {

 public:
  bool operator() ( const Arc &arc1, const Arc &arc2 ) const {
    if( arc1.olabel == arc2.olabel )
      return arc1.ilabel < arc2.ilabel;
    return arc1.olabel < arc2.olabel;
  };
  uint64 Properties(uint64 props) const {
    return props & kArcSortProperties;
  };
};

/**********************************************************************************/
/*                                                                                */
/* Arcs stores a vector of rule-arcs. It supports efficient look-up of            */
/* transitions.                                                                   */
/*                                                                                */
/**********************************************************************************/
class Arcs {

 private:
  ArcIterator<Transducer> arcs;
  StateId current;
  size_t number_of_arcs;

 public:
  Arcs( Transducer &t, StateId s) :
    arcs(t,s),
    current(0),
    number_of_arcs(t.NumArcs( s )) 
  {};

  void Next( void ) {
    if ( Done() )
      return;
    ++current;
    arcs.Next();
  };
  
  void Seek( StateId i ) {
    current = i;
    arcs.Seek(i);
  };

  const Arc &Peek( StateId i ) {
    StateId old_current = current;
    Seek(i);
    const Arc &a = Value();
    Seek(old_current);
    return a;
  };

  void Reset( void ) {
    current = 0;
    arcs.Reset();
  };

  const Arc &Value( void ) {
    return arcs.Value();
  }
  
  bool Done( void ) {
    return arcs.Done();
  }

  size_t get_current_state( void ) { return current; };

  static int pair_compare( LabelPair p1, LabelPair p2 );
  
  bool find_first( void );

  bool find_input_label( Label input_label );

  bool find_pair( LabelPair pair );

  void next_output_label( void ) {

    Label output = arcs.Value().olabel;
    while ( (not Done()) and (output == arcs.Value().olabel) )
      Next();
  }
};

/*******************************************************************************/
/*                                                                             */
/* RulesInfo is a class, that stores all the information about a configuration */
/* of the rules. It contains the public variables:                             */
/*                                                                             */
/*    - LabelPair pair  The current pair, of the transitions of the rules.     */
/*    - vector<stateId> next_states  The targets of the current transitions    */
/*                                   of the rules.                             */
/*                                                                             */
/* and the public methods:                                                     */
/*                                                                             */
/*    - bool next( Label input )  Set the rules into the next transition with  */
/*                                input-label input and return true. If there  */
/*                                are no more transitions with input-label     */
/*                                label, return false.                         */
/*                                                                             */
/*    - bool Done( void )  Have the transitions of the first rule been         */
/*                         exhausted?                                          */
/*                                                                             */
/* An instatioations may be constructed for any valid configuration of rule-   */
/* states.                                                                     */
/*                                                                             */
/* Before any instatioations are created, the class-initialistion-method       */
/* static void init( vector<Transducer*> &R ) needs to be run in order to set  */
/* the rule-transducers.                                                       */
/*                                                                             */
/*******************************************************************************/
class RulesInfo {
  
 private:
  
  /* The rule-transducers given as parameter to the operation R_1, ..., R_n */
  static vector<Transducer*> Rules;
  
  /* Number of rules n. */
  static size_t number_of_rules;


  /* The current arcs s_1 -x:y-> t_1, ..., s_n -x:y-> t_n. */
  vector<Arcs*> Rule_arcs;

  /* Set to true, if the first rule has no more transitions left. */
  bool no_more_transitions;
  
  /* 
     Set the variable pair, if the first rule has transitions, otherwise, set the
     variable no_more_transitions to true.
  */
  void set_pair( void );

  /* Given some transition of the first rule with pair pair, set the Rule_arcs and 
     return true, if there are corresponding transitions for rest of the rules. If
     there aren't, move to the next transitions of the first rule and return false.
  */
  bool set_states( LabelPair pair );
  
  void set_next_states( void );

  bool first( Label input ) { 
    return Rule_arcs.at(0)->find_input_label( input ); 
  };

 public:

  /* 
     The pair of the current transition. If the arcs have been exhausted, this could
     be anything.
  */
  LabelPair pair;

  /* The current states of the rule-transducers s_1, ..., s_n. */
  vector<StateId> states;
  
  /*
    The target-states of the rules. If the arcs have been exhausted, this could be anything.
  */
  vector<StateId> next_states;

  /* This class-initialisation-method is to be used before any instantiations are made. */
  static void init( vector<Transducer*> &R ) {
    Rules.assign( R.begin(), R.end() );
    number_of_rules = R.size();

    for ( unsigned int i = 0; i < number_of_rules; ++i )
      ArcSort(Rules.at(i),input_compare());
  };
  
  /* This constructor sets all rules to their start-states. */
  RulesInfo( void );
  
  /* This constructor sets the rules to the states given by the vector<StateId> &states. */
  RulesInfo( vector<StateId> &states );
  
  ~RulesInfo( void ) {
    for ( unsigned int i = 0; i < number_of_rules; ++i )
      delete Rule_arcs.at(i);
  };

  /* 
     Try to find the next input:x transitions, that are common for all of the rules.
     Change pair to input:x and next_states to the targets of the transitions. If there
     are no more transitions input:x, return false, else return true.
  */
  bool next( Label input ); 

  bool first_in( Label input );

  /* Return true, if the arcs of the first rule have been exhausted. */
  bool Done( void ) { return Rule_arcs.at(0)->Done(); };

  bool Final( void ) {
    for ( unsigned int i = 0; i < number_of_rules; ++i ) 
      if (Rules.at(i)->Final(states.at(i)) == TropicalWeight::Zero().Value())
	return false;
    return true;
  };

  bool NextFinal( void ) {
    for ( unsigned int i = 0; i < number_of_rules; ++i ) 
      if (Rules.at(i)->Final(next_states.at(i)) == TropicalWeight::Zero().Value())
	return false;
    return true;
  };
  float get_weight(void);
  float get_final_weight(void) {
    float w = 0;
    for ( unsigned int i = 0; i < number_of_rules; ++i ) 
      w += Rules.at(i)->Final(states.at(i)).Value();
    return w;
  };
  float get_next_final_weight(void) {
    float w = 0;
    for ( unsigned int i = 0; i < number_of_rules; ++i ) 
      w += Rules.at(i)->Final(next_states.at(i)).Value();
    return w;
  }
};

class LexiconInfo {
  
 private:
  
  vector<Tree*> states;

  size_t number_of_nodes;

  Transducer * composition_result;

  StateId find(StateId lexicon_state, vector<StateId> &rule_states, bool &new_state) ;

 public:

  Transducer &lex;
  
  StateId current_lexicon_state;

  LexiconInfo( Transducer &lexicon, Transducer * composition) :
    number_of_nodes( lexicon.NumStates()),
    composition_result(composition),
    lex(lexicon),
      current_lexicon_state(lexicon.Start())
    {
      ArcSort(&lex,output_compare());
      Tree::init(composition);
      for ( size_t id = 0; id < number_of_nodes; ++id )
	states.push_back(NULL);
    };

  ~LexiconInfo( void ) {
    for ( unsigned int id = 0; id < number_of_nodes; ++id )
      delete states.at(id);
  };

  static LabelPair compose_arcs( const Arc &lexicon_arc, const LabelPair rules_pair );

  StateId compose(StateId lexicon_state, RulesInfo &Rules, bool &new_state) {
    StateId s = find(lexicon_state, Rules.next_states, new_state);
    if ((lex.Final(lexicon_state) != TropicalWeight::Zero().Value()) and Rules.NextFinal()) {
    composition_result->SetFinal(s,
				 lex.Final(lexicon_state).Value() + 
				 Rules.get_next_final_weight());
    }

    return s;
  };
  StateId compose_epsilon(StateId lexicon_state, RulesInfo &Rules, bool &new_state) {
    StateId s = find(lexicon_state,Rules.states,new_state);
    if ( (lex.Final(lexicon_state) == TropicalWeight::Zero().Value())  and Rules.Final()) {
      composition_result->SetFinal(s,
				   lex.Final(lexicon_state).Value() + 
				   Rules.get_final_weight());
    }
    return s;
  };

};

class Composer {

 private:
  
  Transducer * composition;
  LexiconInfo lexicon;
  StateId composition_state;

  float multiply_weights(Arcs &L, RulesInfo &R);
  KeySet &skip_symbols;

 public:

 Composer( Transducer &lex , vector<Transducer*> &Rules,KeySet &skip_syms) :
    composition(new Transducer()),
      lexicon( lex, composition ),
      composition_state(0),
      skip_symbols(skip_syms)
    { composition->AddState();
      composition->SetStart(0);
      RulesInfo::init(Rules); };

  void more( StateId lexicon_target, 
	     RulesInfo &Rules, 
	     StateId composition_target, 
	     bool epsilon );
  void single_compose( Arcs &lexicon_arcs, RulesInfo &Rules );
  void single_compose_epsilon( Arcs &lexicon_arcs, RulesInfo &Rules );
  void compose( RulesInfo &Rules );

  Transducer * operator() ( void ) {
    RulesInfo R;
    compose(R);
    composition->SetStart(0);
    return composition;
  }
};

};
