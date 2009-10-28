#include "fst_intersecting_compose.h"
size_t fst::RulesInfo::number_of_rules;
vector<fst::Transducer*> fst::RulesInfo::Rules;
fst::StdVectorFst * fst::Tree::composition;

int fst::Arcs::pair_compare( fst::LabelPair p1, fst::LabelPair p2 ) {
  
  if ( p1.input == p2.input ) {
    if ( p1.output == p2.output )
      return 0;
    if ( p1.output < p2.output )
      return -1;
    return 1;
  }
  if ( p1.input < p2.input )
    return -1;
  return 1;

};

bool fst::Arcs::find_first ( void ) {

  Label input_label = Value().ilabel;
  
  while( current > 0 ) {
    if ( Peek( current -1 ).ilabel == input_label )
      Seek( current - 1 );
    else
      return true;
  }
  return true;
};

bool fst::Arcs::find_input_label( Label input_label ) {
  size_t low = current; 
  size_t high = number_of_arcs; 

  while ( low < high ) {
    size_t mid = ( high + low ) / 2; 
    Seek( mid );
    
    Label value = Value().ilabel;
    
    if ( value == input_label ) {
      return find_first();
    }
    if ( value < input_label )
      low = mid + 1;
    else
      high = mid;
  };
  return false;
};

bool fst::Arcs::find_pair( LabelPair pair ) {
  size_t high = number_of_arcs;
  size_t low = current;
  
  while ( low < high ) {
    
    size_t mid = ( low + high ) / 2;
    Seek( mid );
    LabelPair p;
    p.input = Value().ilabel;
    p.output = Value().olabel;
    int comparison = pair_compare( pair, p);

    if ( comparison == 0 )
      return true;
    if ( comparison < 0 )
      high = mid;
    else
      low = mid +1;
  }
  return false;
};

fst::RulesInfo::RulesInfo( void ) :
  next_states(number_of_rules,0)
{

  /* All states are initial. */
  for ( unsigned int i = 0; i < number_of_rules; ++i ) {
    states.push_back( 0 );
    Rule_arcs.push_back( new Arcs( *Rules.at(i), 0 ) ); 
  }

  set_pair();

};

/* Given states s_1, ..., s_n. */
fst::RulesInfo::RulesInfo( vector<StateId> &s ) :
  states(number_of_rules,-1),
  next_states(number_of_rules,-1)
{
  /* Set states and arcs. */
  for ( unsigned int i = 0; i < number_of_rules; ++i ) {
    states.at(i) = s.at(i);
    Rule_arcs.push_back( new Arcs( *Rules.at(i), s.at(i) ) );
  }
  set_pair();
  set_next_states();
};

void fst::RulesInfo::set_pair( void )
{

  if ( Rule_arcs.at(0)->Done() )
    no_more_transitions = true;
  else {
    const Arc &first_arc = Rule_arcs.at(0)->Value();
    pair.input = first_arc.ilabel;
    pair.output = first_arc.olabel;
  }
  set_next_states();
};

bool fst::RulesInfo::set_states( LabelPair pair ) {

  for ( size_t i = 1; i < number_of_rules; ++i ) {
    if ( not Rule_arcs.at(i)->find_pair( pair ) )
      return false;
  }
  return true;
};

void fst::RulesInfo::set_next_states( void ) {

  if ( Done() )
    return;

  for ( unsigned int i = 0; i < number_of_rules; ++i ){
    next_states.at(i) = Rule_arcs.at(i)->Value().nextstate;
  }
};

bool fst::RulesInfo::first_in( Label input ) {

  if ( not first( input ))
    return false;

  LabelPair first_rule_pair;
  first_rule_pair.input = Rule_arcs.at(0)->Value().ilabel;
  first_rule_pair.output = Rule_arcs.at(0)->Value().olabel;


  while ( first_rule_pair.input == input ) {
    bool found = true;
    for ( size_t i = 1; i < number_of_rules; ++i ) {
      if ( not Rule_arcs.at(i)->find_pair( first_rule_pair ) ){
	found = false;
	break;
      }
    }
    if ( found ) {
      set_pair();
      return true;
    }

    Rule_arcs.at(0)->Next();
    first_rule_pair.input = Rule_arcs.at(0)->Value().ilabel;
    first_rule_pair.output = Rule_arcs.at(0)->Value().olabel;

  }

  return false;

};

bool fst::RulesInfo::next( Label input ) {

  if ( Rule_arcs.at(0)->Value().ilabel != input ) {

    if ( not first( input ))
      return false;
  }
  else {
    Rule_arcs.at(0)->Next();
  }

  if ( Done() )
    return false;

  LabelPair first_rule_pair;
  first_rule_pair.input = Rule_arcs.at(0)->Value().ilabel;
  first_rule_pair.output = Rule_arcs.at(0)->Value().olabel;


  while ( first_rule_pair.input == input ) {
    bool found = true;
    for ( size_t i = 1; i < number_of_rules; ++i ) {
      if ( not Rule_arcs.at(i)->find_pair( first_rule_pair ) ){
	found = false;
	break;
      }
    }
    if ( found ) {
      set_pair();
      return true;
    }

    Rule_arcs.at(0)->Next();
    first_rule_pair.input = Rule_arcs.at(0)->Value().ilabel;
    first_rule_pair.output = Rule_arcs.at(0)->Value().olabel;

  }

  return false;

};

fst::StateId fst::LexiconInfo::find(StateId lexicon_state, vector<StateId> &rule_states, bool &new_state) {
  
  if ( states.at(lexicon_state) == NULL ) {
    StateId composition_target;
    states.at(lexicon_state) = new Tree(rule_states.begin(), rule_states.end(), composition_target);
    new_state = true;
    return composition_target;
  }
  
  return  states.at(lexicon_state)->find(rule_states.begin(), 
					rule_states.end(), 
					new_state);
  
};

fst::LabelPair fst::LexiconInfo::compose_arcs( const fst::Arc &arc1, const fst::LabelPair rules_pair ) {

  LabelPair pair;
  pair.input = arc1.ilabel;
  pair.output = rules_pair.output;

  return pair;
};

// Prepare to apply the composition operation for a new configuration of states and 
// apply it to that configuration.
void fst::Composer::more( StateId lexicon_target, RulesInfo &Rules, StateId composition_target, bool epsilon = false) {

  StateId old_lexicon_state = lexicon.current_lexicon_state;
  lexicon.current_lexicon_state = lexicon_target;
  
  StateId old_composition_state = composition_state;
  composition_state = composition_target;
  
  if ( not epsilon ) {
    RulesInfo NextRules( Rules.next_states );
    compose( NextRules );
  }
  else {
    RulesInfo NextRules( Rules.states );
    compose( NextRules );
  }
  
  lexicon.current_lexicon_state = old_lexicon_state;
  composition_state = old_composition_state;
  
};

float fst::RulesInfo::get_weight(void)
{
  float w = 0;
  for (vector<Arcs*>::iterator it = Rule_arcs.begin();
       it != Rule_arcs.end();
       ++it)
    {
      w += (*it)->Value().weight.Value();
    }
  return w;
}

float fst::Composer::multiply_weights(Arcs &L, RulesInfo &R)
{
  return L.Value().weight.Value() + R.get_weight();
}

// Compose a single transition x:t in the lexicon with a single transition
// t:z in the rules.
void fst::Composer::single_compose( Arcs &lexicon_arcs, RulesInfo &Rules ) {

  bool new_state;

  StateId lexicon_target;
  LabelPair pair;

  if (Rules.pair.input == 0) {
    lexicon_target = lexicon.current_lexicon_state;
    pair = Rules.pair;
  }
  else {
    lexicon_target = lexicon_arcs.Value().nextstate;
    pair = lexicon.compose_arcs( lexicon_arcs.Value(), Rules.pair );
  }

  StateId composition_target = lexicon.compose( lexicon_target, Rules, new_state );

  // If the resulting state in the composition-transducer was a new one, i.e
  // the lexicon-state/rule-states configuration was new, apply the composition-
  // operation to the state.
  if ( new_state ) {
    more( lexicon_target, Rules, composition_target );
  }

  if (Rules.pair.input == 0)
    {
      composition->AddArc(composition_state, 
			  Arc(pair.input, 
			      pair.output, 
			      Rules.get_weight(), 
			      composition_target));      
    }
  else
    {
      composition->AddArc(composition_state, 
			  Arc(pair.input, 
			      pair.output, 
			      multiply_weights(lexicon_arcs,Rules), 
			      composition_target));
    }
};

// Compose a single transition x:0 with transitions in the rules.
void fst::Composer::single_compose_epsilon( Arcs &lexicon_arcs, RulesInfo &Rules ) {

  bool new_state;

  StateId lexicon_target;

  lexicon_target = lexicon_arcs.Value().nextstate;

  StateId composition_target = lexicon.compose_epsilon( lexicon_target, Rules, new_state);

  // If the resulting state in the composition-transducer was a new one, i.e
  // the lexicon-state/rule-states configuration was new, apply the composition-
  // operation to the state.
  if ( new_state ) {
    more( lexicon_target, Rules, composition_target, true);
  }

  LabelPair pair;
  pair.input = lexicon_arcs.Value().ilabel;
  pair.output = lexicon_arcs.Value().olabel;

  composition->AddArc(composition_state, 
		      Arc(pair.input, 
			  pair.output, 
			  lexicon_arcs.Value().weight.Value(),  
			  composition_target));
  
};

// Main function: We're currently at some states in the rules and at some state in the lexicon. 
// Loop through all transitions x:t in the lexicon and compose them with transitions t:y in
// the rules.
void fst::Composer::compose( RulesInfo &Rules ) {
  Arcs lexicon_arcs(lexicon.lex,lexicon.current_lexicon_state);

  // Handle x:0 transitions in the lexicon.
  while ( (not lexicon_arcs.Done()) and lexicon_arcs.Value().olabel == 0 ) {
    single_compose_epsilon( lexicon_arcs, Rules );  
    lexicon_arcs.Next();
  }

  // Handle 0:y transitions in the rules.
  if ( Rules.first_in(0) )
    do {
      single_compose(lexicon_arcs, Rules);
    } while ( (not Rules.Done()) and Rules.next( 0 ) );
  
	      
  Label lexicon_out;

  // Normal transitions x:a in lexicon and a:z in rules.
  while ( not lexicon_arcs.Done() and not Rules.Done()) {

    // This is the label a in the lexicon
    lexicon_out = lexicon_arcs.Value().olabel;
    StateId first_occurence_of_out = lexicon_arcs.get_current_state();

    // Iterate over all transitions a:z in the rules forming each
    // transition x:z for all transitions x:a in the lexicon.
    if ( Rules.first_in( lexicon_out ) ) {

      while ( true ) {
	do {
	  single_compose( lexicon_arcs, Rules);
	  lexicon_arcs.Next();

	} while ( (not lexicon_arcs.Done()) and 
		  (lexicon_arcs.Value().olabel == lexicon_out) );

	if (Rules.Done() or not Rules.next( lexicon_out )) // If there are no more a:z transitions in the rules,
	  break;                                           // move on.
	else
	  lexicon_arcs.Seek( first_occurence_of_out ); // Return to the first transition x:a
	                                               // in the lexicon.
      }
    }
    else {
      while ( (not lexicon_arcs.Done()) and lexicon_arcs.Value().olabel == lexicon_out )
	lexicon_arcs.Next();
    }
  }
};

/*int main ( int argc, char * argv[] ) {
  
  Transducer * lex = Transducer::Read("./kompositio_testi/lex.ofst");
  Transducer * rule = Transducer::Read("./kompositio_testi/rule.ofst");

  vector<Transducer*> v;

  v.push_back(rule);

  Composer c(*lex,v);
  printf("Hmmm....\n");
    Transducer * t = c.comp();

  t->Write("./kompositio_testi/res");
  
  delete lex;
  delete rule;
  //  delete t;
  char * n = "./kompositio_testi/Rule";

  char * w = new char[100];
  vector<fst::StdVectorFst*> Rules;

  Transducer * t;

  for ( int i = 0; i < 22; ++i ) {

    sprintf(w,"%s%i.ofst",n,i);
    printf("Reading the rules %s\n",w);

    Rules.push_back( StdVectorFst::Read(w) );
    //    if ( i == 20 ) 
    //  t = StdVectorFst::Read(w);
  }

  printf("Reading the lexicon ./kompositio_testi/astevaihtelu.ofst\n");

  StdVectorFst * lexicon = StdVectorFst::Read("./kompositio_testi/astevaihtelu.ofst");
  StdVectorFst l = *lexicon;
  
  printf("Composing\n");

  
  StdVectorFst * res =   fst::Composer(*lexicon, Rules)();
  printf("Done\n");
  

  res->Write("res");
};

*/
