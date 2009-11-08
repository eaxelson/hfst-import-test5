#include "fst-lex.h"
#include "parse_flag_diacritics.h"
#include <string.h>

/********************************************************************************************************/
/*                                                                                                      */
/*                                      class CompInfo                                                  */
/*                                                                                                      */
/********************************************************************************************************/

CompInfo::CompInfo(NodeNumbering * num,  vector<StateInfo*> * lex, Transducer * res,  Alphabet * lex_alpha,  
		   Alphabet * rules_alpha,   FlagDiacriticSet * flag_diacritics) {
  lex_numbering = num;
  lexicon = lex;
  result = res;
  known_flag_diacritics = flag_diacritics;
  if (known_flag_diacritics == NULL)
    {
      there_are_no_flag_diacritics = true;
    }
  else
    {
      there_are_no_flag_diacritics = false;
    }
  current_comp_node = res->root_node();
  lexicon_alphabet = lex_alpha;
  rules_alphabet = rules_alpha;
  composition_alphabet = &res->alphabet;
  final = false;  
  number_of_nodes = 0;
  failing_rule_index = -1;
}

CompInfo::~CompInfo( void ) {}

Label CompInfo::compose_arcs( Arc * lex_arc,  Arc * rules_arc) {
  if (! lex_arc  ) { return rules_arc->label(); }
  if (! rules_arc) { return lex_arc->label();  }

  return Label(lex_arc->label().lower_char(),rules_arc->label().upper_char());

  //const char * lexicon_char = lexicon_alphabet->code2symbol(lex_arc->label().lower_char());
  //const char * rules_char = rules_alphabet->code2symbol(rules_arc->label().upper_char());
  //return Label(composition_alphabet->symbol2code(lexicon_char),composition_alphabet->symbol2code(rules_char));
}

Node * CompInfo::target(Node * lex_node,  vector<Node*> * rule_nodes, bool &existed_already) {

  unsigned int index = lex_numbering->operator[](lex_node);
  StateInfo * lex_state = lexicon->at(index);
  return lex_state->has_comp_state(rule_nodes,existed_already,result);
}

void CompInfo::compose( LexInfo * Lex,  RulesInfo * Rules,  Arc * lex_arc,  Arc * rule_arc) {

  Node * lex_target;
  vector<Node*> * rule_targets;
  Node * comp_target;

  bool lex_final = false;
  bool rules_final = false;
 

  if ( ! rule_arc ) { rule_targets = new vector<Node*>(*(Rules->nodes)); rules_final = Rules->finals;}
  else              { rule_targets = Rules->targets( rule_arc, rules_final, failing_rule_index );}
  
  if ( ! rule_targets ) { return; }

  if ( ! lex_arc ) { lex_target = Lex->node; lex_final = Lex->node->is_final(); }
  else             { 
    lex_target = Lex->node->target_node( lex_arc->label() ); 
    lex_final = Lex->node->target_node( lex_arc->label() )->is_final(); 
  }
  
  Label new_label = compose_arcs(lex_arc, rule_arc);
  bool existed_already = false;
  comp_target = target(lex_target,rule_targets,existed_already);
  current_comp_node->add_arc(new_label, comp_target, result);

  if( ! existed_already ) {
    ++number_of_nodes;
    comp_target->set_final( lex_final && rules_final );

    LexInfo * NewLex = new LexInfo( lex_target, lex_final );
    RulesInfo * NewRules = new RulesInfo(rule_targets, rules_final);

    Node * old_current = current_comp_node;
    bool old_final = final;

    current_comp_node = comp_target;
    final = lex_final && rules_final;
    isect_compose(this, NewLex, NewRules);

    delete NewLex; NewLex = NULL;
    delete NewRules; NewRules = NULL;

    current_comp_node = old_current;
    final = old_final;
  }
  delete rule_targets; rule_targets = NULL;
}

/********************************************************************************************************/
/*                                                                                                      */
/*                                      class RulesInfo                                                 */
/*                                                                                                      */
/********************************************************************************************************/
RulesInfo::RulesInfo(  vector<Node*> * rule_nodes, bool rules_final ) {
  nodes = rule_nodes;
  first_rule_arcs = rule_nodes->at(0)->arcs();
  finals = rules_final;
}

vector<Node*> * RulesInfo::targets( Arc * arc, bool &finals, int &failing_rule_index) {
  vector<Node*> * targets = new vector<Node*>;
  Label l = arc->label();
  finals = true;
  Node * t;

  bool has_failed = true;
  if ( failing_rule_index == -1 ) { 
    failing_rule_index = 0; 
    has_failed = false;
  }

  for(vector<Node*>::const_iterator it = nodes->begin(); it != nodes->end(); ++it ) {
    t = (*it)->target_node(l);
    if ( ! t ) {
      delete targets;
      targets = NULL;
      return NULL;
    }
    targets->push_back(t);
    finals = finals && t->is_final();
    if ( not has_failed ) { ++failing_rule_index; }
  }
  if ( not has_failed ) { failing_rule_index = -1; }
  return targets;
}

/********************************************************************************************************/
/*                                                                                                      */
/*                                        class LexInfo                                                 */
/*                                                                                                      */
/********************************************************************************************************/
LexInfo::LexInfo(  Node * n, bool n_final ) {
  node = n;
  arcs = n->arcs();
  final = n_final;
}
LexInfo::LexInfo(  Node * n ){
  node = n;
  arcs = n->arcs();
  final = n->is_final();
}

/********************************************************************************************************/
/*                                                                                                      */
/*                                      class StateInfo                                                 */
/*                                                                                                      */
/********************************************************************************************************/
StateInfo::StateInfo( void ) {
  rules = new vector<vector<Node*>*>;
  comp_nodes = new vector<Node*>;
}

StateInfo::~StateInfo( void ) {
  for ( vector<vector<Node*>*>::iterator it = rules->begin(); it != rules->end(); ++it ){ delete (*it); (*it) = NULL; }
  delete rules; rules = NULL;
  delete comp_nodes; comp_nodes = NULL;
}

/* We'll search for rule_nodes in nodes. If it doesn't exist, we'll add it and return 
   the corresponding composition node.                                                 */
Node * StateInfo::has_comp_state( vector<Node*> * rule_nodes, bool &existed_already,  Transducer * result) {

  int index  = binsearch(rules,rule_nodes,existed_already);

  if ( not existed_already ) {

    vector<vector<Node*>*>::iterator it = rules->begin();
    vector<Node*>::iterator          jt = comp_nodes->begin();

    it += index;
    jt += index;

    rules->insert( it, new vector<Node*>(*rule_nodes) );
    Node * new_comp_node = result->new_node();
    comp_nodes->insert( jt, new_comp_node );
    return new_comp_node;
  }

  comp_nodes->at(index);
  return comp_nodes->at(index);
}

/********************************************************************************************************

   Function: isect_compose

   Takes: CompInfo * Comp    A struct that contains the global information regarding the operation.
          LexInfo * Lex      A struct that contains the information regarding the current node 
                             in the lexicon.
          RulesInfo * Rules  A struct containing the information regarding the current rule nodes.

   Returns: void. 

   Given the node l ( Lex->node ) in the lexicon and the rule nodes

        <r1, r2, ..., rn >  ( Rules->nodes ) 

   isect_compose will create new transitions in the transducer t ( Comp->result ) in the following manner:

    -  For every set of transitions 

        r1 -0:z-> rt1, r2 -0:z-> rt2, ..., rn -0:z-> rtn
 
       ( here rt1 is rule_arc->target_node(), where rule_arc is the transition corresponding to the pair z:0 
       in the first of the rule nodes r1 ) a transition to the state s in the transducer t corresponding to the 
       state l in the lexicon and the states 

       <rt1, rt2, ..., rtn > 

       in the rules will be created with label 0:z.

    -  For every transition l -x:0-> m in the lexicon ( here m is arc->target_node(), where arc corresponds to 
       the transition x:0 in the lexicon) a transition to the state s in the transducer t corresponding to the
       state m int he lexicon and the original rule nodes <r1, r2, ..., rn > will be created with label x:0.

    - For every transition l -x:y-> m in the lexicon and transitions

        r1 -y:z-> rt1, r2 -y:z-> rt2, ..., rn -y:z-> rtn

      in the rules a transition to the state s in the transducer t corresponding to the state m in the lexicon 
      and the states 

        <rt1, rt2, ..., rtn >

      in the rules will be created corresponding to the label x:z

    Unless the state s in the transducer t exists, it will be created. If the target of the transition in the 
    lexicon is a final state and the targets of the transitions in the rules are final states, the node s will
    be a final state. Otherwise it won't. 

    Doesn't determinise or minimise the result.
******************************************************************************************************************/

void isect_compose( CompInfo * Comp, LexInfo * Lex, RulesInfo * Rules) {

  // Transitions -0:z-> in the rules.
  for(ArcsIter it(Rules->first_rule_arcs); it ; it++) {
    Arc * rule_arc = it; 
    if ( not rule_arc->label().lower_char() == 0 ) { continue; }
    Comp->compose(Lex, Rules, NULL, rule_arc);
  }
 
  for(ArcsIter it(Lex->arcs); it ; it++) {
    Arc * lex_arc = it;

    // Transitions -x:0-> in the lexicon. 
    if ( lex_arc->label().upper_char() == 0 ){
      Comp->compose(Lex, Rules, lex_arc, NULL);
      continue;
    }

    if ( Comp->is_flag_diacritic(lex_arc->label().upper_char()))
      {
	Comp->compose(Lex, Rules, lex_arc, NULL);
	continue;
      }
    // Transitions -x:y-> in the lexicon (x not equal to 0). 
    for(ArcsIter jt(Rules->first_rule_arcs); jt ; jt++) {
      Arc * rule_arc = jt;

      Character lexicon_upper = lex_arc->label().upper_char();
      Character rules_lower = rule_arc->label().lower_char();

      // Transitions -x:y-> in the lexicon and -y:z-> in the first rule.
      if( lexicon_upper != rules_lower ) { continue; }
      Comp->compose(Lex, Rules,lex_arc,rule_arc);

      //const char * lexicon_upper = Comp->lexicon_alphabet->code2symbol(lex_arc->label().upper_char());
      //const char * rules_lower = Comp->rules_alphabet->code2symbol(rule_arc->label().lower_char());

    }
  }
}

Alphabet * make_alphabet( const Alphabet &sigma1, const Alphabet &sigma2) {

  Alphabet * rho = new Alphabet();
  rho->copy( sigma1 );

  for ( Alphabet::const_iterator it = sigma2.begin(); it != sigma2.end(); ++it ) {
    const char * symu = sigma2.code2symbol( (*it).upper_char() );
    const char * symd = sigma2.code2symbol( (*it).lower_char() );

    Character charu = rho->add_symbol(symu); 
    Character chard = rho->add_symbol(symd);
    rho->insert( Label(chard,charu) );    

  }
  return rho;
}

Transducer * intersecting_composition(  Transducer &lexicon, 
					vector<Transducer*> * rules,
					FlagDiacriticSet * known_flag_diacritics, 
					bool minimise, 
					vector<char*> * rule_names, 
					bool verbose){
  
  if ( not (known_flag_diacritics == NULL))
    {
      //fprintf(stderr,"\nknown flag diacritics:\n");
      for (FlagDiacriticSet::iterator it = known_flag_diacritics->begin();
	   it != known_flag_diacritics->end();
	   ++it)
	{
	  fprintf(stderr,"%u\n",*it);
	}
    }
  NodeNumbering * numbering = new NodeNumbering( lexicon );
  vector<StateInfo*> * states = new vector<StateInfo*>; 
  for (unsigned int i = 0; i < numbering->number_of_nodes(); i++) {
    states->push_back( new StateInfo );
  }
  Transducer * res = new Transducer;  
  Alphabet * rules_alphabet = &( rules->at(0)->alphabet );
  Alphabet * lex_alphabet = &( lexicon.alphabet );

  //Alphabet * sigma = make_alphabet( *lex_alphabet, *rules_alphabet );
  //res->alphabet.copy(*sigma);
  //delete sigma; sigma = NULL;
  res->alphabet.copy( lexicon.alphabet );
  res->alphabet.copy( rules->at(0)->alphabet );

  CompInfo * Comp = new CompInfo( numbering, states, res, lex_alphabet, rules_alphabet, known_flag_diacritics );
  LexInfo * Lex = new LexInfo( lexicon.root_node(), lexicon.root_node()->is_final() ); 
  
  vector<Node*> * rules_start_states = new vector<Node*>;
  bool finals = true;
  for ( vector<Transducer*>::iterator it = rules->begin(); it != rules->end(); ++it ){
    finals = finals and (*it)->root_node()->is_final();
    rules_start_states->push_back( (*it)->root_node() );
  }
  RulesInfo * Rules = new RulesInfo( rules_start_states, finals );
  if ( verbose ) { cout << "Computing the intersecting composition... "; cout.flush(); }

  states->at( numbering->operator[]( lexicon.root_node() ))->rules->push_back( new vector<Node*>(*rules_start_states));
  states->at( numbering->operator[]( lexicon.root_node() ))->comp_nodes->push_back( res->root_node() );

  isect_compose( Comp, Lex, Rules );

  int number_of_nodes = Comp->number_of_nodes;
  if ( verbose ) { cout << "Done.\n"; }

  /* Collecting garbage, storing and such... */
  if ( verbose ) { cout << "Collecting garbage... "; cout.flush(); }
  delete numbering; numbering = NULL;
  for( vector<StateInfo*>::iterator it = states->begin(); it != states->end(); ++it) { delete (*it); *it = NULL; }
  delete rules_start_states; rules_start_states = NULL;
  delete states; states = NULL;
  delete Comp; Comp = NULL;
  delete Lex; Lex = NULL;
  delete Rules; Rules = NULL;
  if ( verbose ) { cout << "Done.\n"; }

  if ( verbose ) { cout << "Determinising... "; cout.flush(); }
 
  Transducer * temp = res;
  res = &(res->determinise());
  delete temp; temp = NULL;
 
  if ( verbose ) { cout << "Done.\n"; }

  if ( (not minimise) && verbose ) { cout << "There are " << number_of_nodes << " nodes in the transducer.\n"; }
  if ( minimise ) {
    if ( verbose ) {
      cout << "Before minimisation, there are " << number_of_nodes << " nodes in the transducer.\n";
      cout << "Minimising... "; cout.flush(); 
    }
    temp = res;
    res = &(res->minimise());
    delete temp; temp = NULL;

    if ( verbose ) {
      NodeNumbering * n = new NodeNumbering( *res );
      cout << "Done.\n"; 
      cout << "After minimisation, there are " << n->number_of_nodes()  << " in the transducer.\n";
      delete n;
    }
  }

  return res;

}
