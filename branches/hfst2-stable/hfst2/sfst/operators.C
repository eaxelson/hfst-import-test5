

/*MA****************************************************************/
/*                                                                 */
/*  FILE     operators.C                                           */
/*  MODULE   operators                                             */
/*  PROGRAM  SFST                                                  */
/*  AUTHOR   Helmut Schmid, IMS, University of Stuttgart           */
/*                                                                 */
/*ME****************************************************************/


#include "fst.h"

static void compose_nodes( Node*, Node*, Node*, Transducer*, PairMapping& );


/*FA****************************************************************/
/*                                                                 */
/*  check_cyclicity                                                */
/*                                                                 */
/*FE****************************************************************/

static bool check_cyclicity( Node *node, NodeHashSet &visited, 
			     const Alphabet &alphabet,
			     bool lower)
{ 
 
  if (!visited.insert(node).second)
    return true; // node was visited before

  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    if ((lower and arc->label().lower_is_epsilon()) 
	or ((not lower) and arc->label().upper_is_epsilon())) {
      if (check_cyclicity(arc->target_node(), visited, alphabet, upper)) {
	return true;
      }
    }
  }
  visited.erase(node);
  return false;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::infinitely_ambiguous_node                          */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::infinitely_ambiguous_node( Node *node, bool lower )

{
  if (!node->was_visited( vmark )) {
    NodeHashSet visited;
    if (check_cyclicity(node, visited, alphabet, lower))
      return true;

    // iterate over all outgoing arcs
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      if (infinitely_ambiguous_node( arc->target_node(),lower ))
	return true;
    }
  }
  return false;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::is_infinitely_ambiguous                            */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::is_infinitely_ambiguous(bool lower)

{
  incr_vmark();
  return infinitely_ambiguous_node(root_node(),lower);
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::is_cyclic_node                                     */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::is_cyclic_node( Node *node, NodeHashSet &previous )

{
  if (!node->was_visited( vmark )) {
    NodeHashSet visited;
    
    NodeHashSet::iterator it=previous.insert(node).first;

    // iterate over all outgoing arcs
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      if (previous.find(arc->target_node()) != previous.end() || 
	  is_cyclic_node( arc->target_node(), previous ))
	return true;
    }

    previous.erase(it);
  }
  return false;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::is_cyclic                                          */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::is_cyclic()

{
  incr_vmark();
  NodeHashSet previous;
  acyclic = !(is_cyclic_node(root_node(), previous));
  return !acyclic;
}


// *********************
// added by Erik Axelson
// *********************

bool Transducer::is_minimised() {
  return minimised;
};


// *********************
// added by Erik Axelson
// *********************

bool Transducer::is_deterministic(bool omit_root_node) {

  if (deterministic) // it is known for sure that transducer is deterministic
    return true;

  // for each state, check if there are epsilon transitions or
  // transitions whose label pairs are equal

  NodeNumbering index(*this);
  size_t states = index.number_of_nodes();

  for (size_t n = (omit_root_node)? 1 : 0; n<states; n++) {
    Node *node = index.get_node(n);
    Alphabet::LabelSet transitions;  // transitions leaving from node

    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Label l=arc->label();
      if (l.is_epsilon())
	return false;
      if (transitions.find(l) != transitions.end())
	return false;
      else
	transitions.insert(l);
    }
  }
  // no indeterminism found
  deterministic=true;
  return true;
    
};


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::is_automaton_node                                  */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::is_automaton_node( Node *node )

{
  if (!node->was_visited( vmark )) {
    // iterate over all outgoing arcs
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Label l=arc->label();
      if (l.upper_char() != l.lower_char())
	return false;
      if (!is_automaton_node( arc->target_node()))
	return false;
    }
  }
  return true;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::is_automaton                                       */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::is_automaton()

{
  incr_vmark();
  return is_automaton_node(root_node());
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::is_empty                                           */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::is_empty()

{
  if (!minimised) {
    Transducer *tmp=&minimise();
    bool result=tmp->is_empty();
    delete tmp;
    return result;
  }
  if (root_node()->is_final())
    return false;
  return root_node()->arcs()->is_empty();
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::generates_empty_string                             */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::generates_empty_string()

{
  if (!minimised) {
    Transducer *tmp=&minimise();
    bool result=tmp->root_node()->is_final();
    delete tmp;
    return result;
  }
  return root_node()->is_final();
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::reverse_node                                       */
/*                                                                 */
/*FE****************************************************************/

void Transducer::reverse_node( Node *node, Transducer *na )

{
  if (!node->was_visited( vmark )) {

    // create a new node
    node->set_forward( na->new_node() );

    if (node->is_final())
      // add epsilon transition from new root to this node
      na->root_node()->add_arc( Label(), node->forward(), na );
    
    // iterate over all outgoing arcs
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      
      // reverse the subgraph headed by the target node
      reverse_node( arc->target_node(), na );
      Node *n = arc->target_node()->forward();

      // create the reverse arc
      n->add_arc( arc->label(), node->forward(), na );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::reverse                                            */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::reverse()

{
  Transducer *na = new Transducer();
  na->alphabet.copy(alphabet);

  incr_vmark();
  reverse_node(root_node(), na);
  root_node()->forward()->set_final(1);
  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::recode_label                                       */
/*                                                                 */
/*FE****************************************************************/

Label Transducer::recode_label( Label l, bool lswitch, bool recode, 
				Alphabet &al )
{
  if (lswitch)
    l = Label(l.upper_char(), l.lower_char());

  if (recode) {
    // ADDED
    Character lc=Label::epsilon;
    Character uc=Label::epsilon;
    // ADDED
    if (l.lower_char() != Label::epsilon)
      lc = al.add_symbol(alphabet.code2symbol(l.lower_char()));
    // ADDED
    if (l.upper_char() != Label::epsilon)
      uc = al.add_symbol(alphabet.code2symbol(l.upper_char()));
    l = Label(lc, uc);
    al.insert(l);
  }

  return l;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::copy_nodes                                         */
/*                                                                 */
/*FE****************************************************************/

Node *Transducer::copy_nodes( Node *node, Transducer *a, 
			     bool lswitch, bool recode )
{
  if (!node->was_visited(vmark)) {

    node->set_forward(a->new_node());

    // define final nodes
    if (node->is_final())
      node->forward()->set_final(1);

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Node *tn = copy_nodes( arc->target_node(), a, lswitch, recode );

      // Add a link to the new node
      Label l=recode_label(arc->label(), lswitch, recode, a->alphabet);
      node->forward()->add_arc( l, tn, a );
    }
  }

  return node->forward();
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::copy                                               */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::copy( bool lswitch, const Alphabet *al )

{
  bool recode = false;
  Transducer *na = new Transducer();
  if (al == NULL)
    al = &alphabet;
  else
    recode = true;

  na->alphabet.utf8 = al->utf8;
  if (lswitch) {
    na->alphabet.insert_symbols(*al);
    for( Alphabet::iterator it=al->begin(); it!=al->end(); it++ ) {
      Character lc=it->lower_char();
      Character uc=it->upper_char();
      na->alphabet.insert(Label(uc,lc));
    }
  }
  else
    na->alphabet.copy(*al);
  
  na->deterministic = deterministic;
  na->minimised = minimised;
  na->root_node()->set_final(root_node()->is_final());
  incr_vmark();

  root_node()->set_forward(na->root_node());
  root_node()->was_visited(vmark);

  for( ArcsIter p(root_node()->arcs()); p; p++ ) {
    Arc *arc=p;
    Node *target_node=copy_nodes(arc->target_node(), na, lswitch, recode);
    Label l = recode_label(arc->label(), lswitch, recode, na->alphabet);
    na->root_node()->add_arc( l, target_node, na);
  }

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator |                                         */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::operator|( Transducer &a )

{
  Transducer *na = new Transducer();
  na->alphabet.copy(alphabet);
  na->alphabet.copy(a.alphabet);

  incr_vmark();
  na->root_node()->add_arc( Label(), copy_nodes(root_node(), na), na);
  a.incr_vmark();
  na->root_node()->add_arc( Label(), a.copy_nodes(a.root_node(), na), na);

  na->acyclic = acyclic && a.acyclic;
  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::rec_cat_nodes                                      */
/*                                                                 */
/*FE****************************************************************/

void Transducer::rec_cat_nodes( Node *node, Node *node2, bool preserve_final_states )

{
  if (!node->was_visited( vmark )) {

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      rec_cat_nodes( arc->target_node(), node2, preserve_final_states );
    }
    
    if (node->is_final()) {
      // link this node to node2
      if (!preserve_final_states)
	node->set_final(0);
      node->add_arc( Label(), node2, this );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator+                                          */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::operator+( Transducer &a )

{
  Transducer *na = new Transducer();
  na->alphabet.copy(alphabet);
  na->alphabet.copy(a.alphabet);

  // copy Transducer1 to the new Transducer
  incr_vmark();
  Node *node=copy_nodes(root_node(), na);
  na->root_node()->add_arc( Label(), node, na);

  // copy Transducer2 to the new Transducer
  a.incr_vmark();
  node=a.copy_nodes(a.root_node(), na);

  // catenate the two automata
  na->incr_vmark();
  na->rec_cat_nodes(na->root_node(), node);

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::kleene_star (now works for cyclic transducers)     */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::kleene_star()

{
  Transducer *na = &copy();
  na->alphabet.copy(alphabet);

  // link back to the start node
  na->incr_vmark();
  na->rec_cat_nodes(na->root_node(), na->root_node(), true);
  
  //na->root_node()->set_final(1);

  Transducer eps;
  eps.root_node()->set_final(1);

  Transducer *result = &(eps | *na);
  delete na;

  result->deterministic = result->minimised = false;

  return *result;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::negate_nodes                                       */
/*                                                                 */
/*FE****************************************************************/

void Transducer::negate_nodes( Node *node, Node *accept )

{
  if (!node->was_visited(vmark)) {
    node->set_final( !node->is_final() );

    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      negate_nodes( arc->target_node(), accept );
    }

    for( Alphabet::iterator it=alphabet.begin(); it!=alphabet.end(); it++)
      if (!node->target_node(*it))
	node->add_arc( *it, accept, this );
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::productive_node                                    */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::productive_node( Node *node )

{
  if (node->was_visited(vmark))
    return (node->forward() != NULL);

  bool productive;
  if (node->is_final()) {
    productive = true;
    node->set_forward( node );
  }
  else {
    productive = false;
    node->set_forward( NULL );
  }

  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    if (productive_node( arc->target_node() ))
      productive = true;
  }

  if (productive)
    // use forwardp to indicate whether the node is productive
    node->set_forward(node);
  return productive;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::prune_nodes                                        */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::prune_nodes( Node *node )

{
  if (!node->was_visited(vmark)) {
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      if (prune_nodes( arc->target_node() ))
	node->arcs()->remove_arc(arc);
    }
    if (!node->arcs()->is_empty())
      node->set_forward(node);
  }
  return (node->forward() == NULL);
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::prune                                              */
/*                                                                 */
/*FE****************************************************************/

void Transducer::prune()

{
  incr_vmark();
  productive_node( root_node() );
  incr_vmark();
  prune_nodes( root_node() );
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator!                                          */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::operator!()

{
  Transducer *na;

  if (alphabet.size() == 0)
    throw "Negation of Transducer with undefined alphabet attempted!";

  if (minimised)
    na = &copy();
  else
    na = &minimise();
  na->alphabet.clear_char_pairs();  // added
  na->alphabet.copy(alphabet);

  Node *accept_node=na->new_node();
  accept_node->set_final(1);
  for( Alphabet::iterator it=alphabet.begin(); it!=alphabet.end(); it++)
    accept_node->add_arc( *it, accept_node, na );

  na->incr_vmark();
  na->negate_nodes( na->root_node(), accept_node );
  //na->prune();
  na->minimised = na->deterministic = false;
  // cyclicity?

  return *na;
}

// for computing subtraction
// operator! causes error if the first transducer is empty
// because empty LabelSet is interpreted as undefined alphabet
Transducer &Transducer::subtraction_negation()

{
  Transducer *na;

  //if (alphabet.size() == 0)
  //  throw "Negation of Transducer with undefined alphabet attempted!";

  if (minimised)
    na = &copy();
  else
    na = &minimise();
  na->alphabet.copy(alphabet);

  Node *accept_node=na->new_node();
  accept_node->set_final(1);
  for( Alphabet::iterator it=alphabet.begin(); it!=alphabet.end(); it++)
    accept_node->add_arc( *it, accept_node, na );

  na->incr_vmark();
  na->negate_nodes( na->root_node(), accept_node );
  //na->prune();
  na->minimised = na->deterministic = false;
  // cyclicity?

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  conjoin_nodes                                                  */
/*                                                                 */
/*FE****************************************************************/

static void conjoin_nodes( Node *n1, Node *n2, Node *node, 
			   Transducer *a, PairMapping &map )
  
{
  // if both input nodes are final, so is the new one
  if (n1->is_final() && n2->is_final())
    node->set_final(1);

  // iterate over all outgoing arcs of the first node
  for( ArcsIter i(n1->arcs()); i; i++ ) {
    Arc *arc=i;
    Label l=arc->label();
    Node *t1 = arc->target_node();
    Node *t2 = n2->target_node(l);

    // Does the second node have an outgoing arc with the same label?
    if (t2) {
      // Check whether this node pair has been encountered before
      PairMapping::iterator it=map.find(t1, t2);
      
      if (it == map.end()) {
	// new node pair
	// create a new node in the conjunction Transducer
	Node *target_node = a->new_node();
	// map the target node pair to the new node
	map[pair<Node*,Node*>(t1,t2)] = target_node;
	// add an arc to the new node
	node->add_arc( l, target_node, a );
	// recursion
	conjoin_nodes( t1, t2, target_node, a, map );
      }
      else {
	// add an arc to the already existing target node 
	node->add_arc( l, it->second, a );
      }
    }
  }
}

static void conjoin_filter( Node *n1, Node *n2, Node *node, 
			   Transducer *a, PairMapping &map )
  
{
  // if both input nodes are final, so is the new one
  if (n1->is_final() && n2->is_final())
    node->set_final(1);

  // iterate over all outgoing arcs of the first node
  for( ArcsIter i(n1->arcs()); i; i++ ) {
    Arc *arc=i;
    Label l=arc->label();
    Node *t1 = arc->target_node();
    Node *t2 = n2->target_node_filter(l);

    // Does the second node have an outgoing arc with the same label?
    if (t2) {
      // Check whether this node pair has been encountered before
      PairMapping::iterator it=map.find(t1, t2);
      
      if (it == map.end()) {
	// new node pair
	// create a new node in the conjunction Transducer
	Node *target_node = a->new_node();
	// map the target node pair to the new node
	map[pair<Node*,Node*>(t1,t2)] = target_node;
	// add an arc to the new node
	node->add_arc( l, target_node, a );
	// recursion
	conjoin_filter( t1, t2, target_node, a, map );
      }
      else {
	// add an arc to the already existing target node 
	node->add_arc( l, it->second, a );
      }
    }
  }
}

/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator &                                         */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::operator&( Transducer &a )

{
  Transducer *tmp1=NULL;
  Transducer *tmp2=NULL;
  Node *r1, *r2;

  //if (deterministic)  FIX THESE
  //  r1 = root_node();
  //else {
  tmp1 = &determinise();
  r1 = tmp1->root_node();
  //}
  
  //if (a.deterministic)
  //r2 = a.root_node();
  //else {
  tmp2 = &a.determinise();
  r2 = tmp2->root_node();
  //}

  PairMapping map;

  Transducer *na = new Transducer();
  na->alphabet.copy(alphabet);
  na->alphabet.copy(a.alphabet);

  // map the two root nodes to the new root node
  map[pair<Node*,Node*>(r1, r2)] = na->root_node();

  // recursively conjoin the two automata
  conjoin_nodes( r1, r2, na->root_node(), na, map);

  na->deterministic = 1;
  na->acyclic = acyclic && a.acyclic;
  delete tmp1;
  delete tmp2;

  return *na;
}

// HFST code
Transducer &Transducer::filter_epsilon_paths( Transducer &a )

{
  Transducer *tmp1=NULL;
  Transducer *tmp2=NULL;
  Node *r1, *r2;

  tmp1 = &determinise();
  r1 = tmp1->root_node();

  tmp2 = &a.determinise();
  r2 = tmp2->root_node();

  PairMapping map;

  Transducer *na = new Transducer();
  na->alphabet.copy(alphabet);
  na->alphabet.copy(a.alphabet);

  // map the two root nodes to the new root node
  map[pair<Node*,Node*>(r1, r2)] = na->root_node();

  // recursively conjoin the two automata
  conjoin_filter( r1, r2, na->root_node(), na, map );

  na->deterministic = 1;
  na->acyclic = acyclic && a.acyclic;
  delete tmp1;
  delete tmp2;

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  shuffle_nodes                                                  */
/*                                                                 */
/*FE****************************************************************/

static void shuffle_nodes( Node *n1, Node *n2, Node *node, 
			   Transducer *a, PairMapping &map )
{
  // if both input nodes are final, so is the new one
  if (n1->is_final() && n2->is_final())
    node->set_final(1);

  // iterate over all outgoing arcs of the first node
  for( ArcsIter i(n1->arcs()); i; i++ ) {
    Arc *arc=i;
    Label l=arc->label();
    Node *t1 = arc->target_node();
    Node *t2 = n2;

    // Does the second node have an outgoing arc with the same label?
    if (1) {
      // Check whether this node pair has been encountered before
      PairMapping::iterator it=map.find(t1, t2);
      
      if (it == map.end()) {
	// new node pair
	// create a new node in the conjunction Transducer
	Node *target_node = a->new_node();
	// map the target node pair to the new node
	map[pair<Node*,Node*>(t1,t2)] = target_node;
	// add an arc to the new node
	node->add_arc( l, target_node, a );
	// recursion
	shuffle_nodes( t1, t2, target_node, a, map );
      }
      else {
	// add an arc to the already existing target node 
	node->add_arc( l, it->second, a );
      }
    }
  }

  for( ArcsIter i(n2->arcs()); i; i++ ) {
    Arc *arc=i;
    Label l=arc->label();
    Node *t2 = arc->target_node();
    Node *t1 = n1;

    // Does the second node have an outgoing arc with the same label?
    if (1) {
      // Check whether this node pair has been encountered before
      PairMapping::iterator it=map.find(t1, t2);
      
      if (it == map.end()) {
	// new node pair
	// create a new node in the conjunction Transducer
	Node *target_node = a->new_node();
	// map the target node pair to the new node
	map[pair<Node*,Node*>(t1,t2)] = target_node;
	// add an arc to the new node
	node->add_arc( l, target_node, a );
	// recursion
	shuffle_nodes( t1, t2, target_node, a, map );
      }
      else {
	// add an arc to the already existing target node 
	node->add_arc( l, it->second, a );
      }
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator &&                                        */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::operator&&( Transducer &a )
{
  Transducer *tmp1=NULL;
  Transducer *tmp2=NULL;
  Node *r1, *r2;

  if (deterministic)
    r1 = root_node();
  else {
    tmp1 = &determinise();
    r1 = tmp1->root_node();
  }

  if (a.deterministic)
    r2 = a.root_node();
  else {
    tmp2 = &a.determinise();
    r2 = tmp2->root_node();
  }

  PairMapping map;

  Transducer *na = new Transducer();
  na->alphabet.copy(alphabet);
  na->alphabet.copy(a.alphabet);

  // recursively conjoin the two automata
  shuffle_nodes( r1, r2, na->root_node(), na, map);

  na->deterministic = 1;
  delete tmp1;
  delete tmp2;

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  add_composed_node                                              */
/*                                                                 */
/*FE****************************************************************/

static void add_composed_node( Label l, Node *n1, Node *n2, Node *node, 
			       Transducer *a, PairMapping &map )
  
{
  // Check whether this node pair has been encountered before
  PairMapping::iterator it=map.find(n1, n2);
  
  if (it != map.end()) {
    // add an arc to the already existing target node 
    node->add_arc( l, it->second, a );
    //fprintf(stderr, "added composed node %hu:%hu from node %p to node %p\n", l.lower_char(), l.upper_char(), node, it->second);
    return;
  }

  // create a new node in the composed Transducer
  Node *target_node = a->new_node();
  
  // map the target node pair to the new node
  map[pair<Node*,Node*>(n1,n2)] = target_node;
  
  // add an arc to the new node
  node->add_arc( l, target_node, a );
  //fprintf(stderr, "added composed node %hu:%hu from node %p to node %p\n", l.lower_char(), l.upper_char(), node, target_node);
  
  // recursion
  compose_nodes( n1, n2, target_node, a, map );
}


/*FA****************************************************************/
/*                                                                 */
/*  compose_nodes                                                  */
/*                                                                 */
/*FE****************************************************************/

static void compose_nodes( Node *n1, Node *n2, Node *node, 
			   Transducer *a, PairMapping &map )
{

  // if both input nodes are final, so is the new one
  if (n1->is_final() && n2->is_final())
    node->set_final(1);

  // iterate over all outgoing arcs of the first node
  for( ArcsIter i(n1->arcs()); i; i++ ) {
    Arc *arc1=i;
    Node *t1 = arc1->target_node();
    Label l1=arc1->label();
    Character uc1=l1.upper_char();
    Character lc1=l1.lower_char();

    if (uc1 == Label::epsilon) {
      add_composed_node( l1, t1, n2, node, a, map );
    }

    else {
      for( ArcsIter k(n2->arcs()); k; k++ ) {
	Arc *arc2=k;
	Node *t2 = arc2->target_node();
	Label l2=arc2->label();
	Character lc2=l2.lower_char();
	Character uc2=l2.upper_char();
	
	if (uc1 == lc2)
	  add_composed_node( Label(lc1,uc2), t1, t2, node, a, map);
      }
    }
  }

  // epsilon input characters of the second Transducer

    for( ArcsIter i(n2->arcs()); i; i++ ) {
      Arc *arc=i;
      Node *t = arc->target_node();
      Label l=arc->label();
      Character lc=l.lower_char();

      if (lc == Label::epsilon)
	  add_composed_node( l, n1, t, node, a, map);
    }

}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator ||                                        */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::operator||( Transducer &a )

{
  PairMapping map;

  Transducer *na = new Transducer();
  na->alphabet.compose(alphabet, a.alphabet);

  // map the two root nodes to the new root node
  map[pair<Node*,Node*>(root_node(), a.root_node())] = na->root_node();

  // recursively compose the two automata
  compose_nodes( root_node(), a.root_node(), na->root_node(), na, map);

  na->acyclic = acyclic && a.acyclic;
  return *na;
}



/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator /                                         */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::operator/( Transducer &a )

{
  complete_alphabet();
  a.alphabet.copy(alphabet);
  //Transducer *a1 = &(!a);
  Transducer *a1 = &(a.subtraction_negation());
  Transducer *a2 = &(*this & *a1);
  delete a1;
  return *a2;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::compare_nodes                                      */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::compare_nodes( Node *node, Node *node2, Transducer &a2 )

{
  if (node->was_visited( vmark )) {
    if (node2->was_visited( a2.vmark ))
      return (node->forward() == node2 && node2->forward() == node);
    else
      return false;
  }
  else if (node2->was_visited( a2.vmark ))
    return false;

  node->set_forward( node2 );
  node2->set_forward( node );
    
  if (node->is_final() != node2->is_final())
    return false;

  // iterate over all outgoing arcs
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    Node *t2=node2->target_node(arc->label());

    if (t2 == NULL)
      return false;
    else if (!compare_nodes(arc->target_node(), t2, a2))
      return false;
  }

  return true;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::operator ==                                        */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::operator==( Transducer &a )

{

  Transducer *p1 = (minimised)? this: &minimise();
  Transducer *p2 = (a.minimised)? &a: &a.minimise();

  p1->incr_vmark();
  p2->incr_vmark();
  bool result = p1->compare_nodes(p1->root_node(), p2->root_node(), *p2 );

  if (p1 != this)  delete p1;
  if (p2 != &a)    delete p2;

  return result;
}



/*FA****************************************************************/
/*                                                                 */
/*  Transducer::map_nodes                                          */
/*                                                                 */
/*FE****************************************************************/

void Transducer::map_nodes( Node *node, Node *node2, Transducer *a, Level level)

{
  if (!node->was_visited(vmark)) {

    node->set_forward(node2);

    // define final nodes
    if (node->is_final())
      node2->set_final(1);

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Label l(arc->label().get_char(level));
      Node *t2=NULL, *t=arc->target_node();

      if (t->check_visited(vmark))
	t2 = t->forward();
      else
	t2 = a->new_node(); // create a new node
      
      node2->add_arc(l, t2, a); // add a link to the node

      map_nodes( t, t2, a, level );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::level                                              */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::level( Level level )

{
  Transducer *na = new Transducer();

  for( Alphabet::iterator it=alphabet.begin(); it!=alphabet.end(); it++ ) {
    Character c = it->get_char(level);
    if (alphabet.code2symbol(c) != NULL)
      na->alphabet.add_symbol( alphabet.code2symbol(c), c );
    na->alphabet.insert(Label(c));
  }

  incr_vmark();
  map_nodes(root_node(), na->root_node(), na, level );

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::freely_insert_at_node                              */
/*                                                                 */
/*FE****************************************************************/

void Transducer::freely_insert_at_node( Node *node, Label l )

{
  if (!node->was_visited(vmark)) {
    node->add_arc(l, node, this); // add a recursive link labelled with l

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      freely_insert_at_node(arc->target_node(), l );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::freely_insert                                      */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::freely_insert( Label l )

{
  Transducer *na = &copy();

  na->incr_vmark();
  na->freely_insert_at_node(na->root_node(), l );

  deterministic = minimised = acyclic = false; // added by Erik Axelson

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::splice_arc                                         */
/*                                                                 */
/*FE****************************************************************/

void Transducer::splice_arc( Node *node, Node *node2, Node *next_node,
			    Transducer *a )
{
  if (node->is_final()) {
    // link final node to the next node
    node2->add_arc( Label(), next_node, a );
    return;
  }

  // iterate over the outgoing arcs
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    Node *tn=a->new_node();

    node2->add_arc( arc->label(), tn, a );
    splice_arc( arc->target_node(), tn, next_node, a );
  }
}

/*FA****************************************************************/
/*                                                                 */
/*  Transducer::duplicate_node (by Miikka Silfverberg)             */
/*                                                                 */
/*  description: Duplicate the transitions of the node             */
/*               node_in_this_transducer into the node             */
/*               node_in_another_transducer. If the target node of */
/*               a transition has alreayd been duplicated, the     */
/*               node in another_transducer corresponding to       */
/*               the target node is listed in node_mapping.        */
/*               Otherwise the corresponding node has to be        */
/*               created and the transitions of the target node    */
/*               have to be duplicated into that node.             */
/*                                                                 */
/*               If node_in_this_transducer is final, an epsilon   */
/*               transition into the target state of the transducer*/
/*               valued transition is added (see function          */
/*               Transducer::add_as_transition)                    */
/*                                                                 */
/*  parameters:                                                    */
/*         node_in_this_transducer The node, whose transitions are */
/*                                 duplicated.                     */
/*                                                                 */
/*         node_in_another_transducer The node in                  */
/*                                    another_transducer which is  */
/*                                    going to be the copy of      */
/*                                    node_in_this_transducer      */
/*                                                                 */
/*         another_transducer The other transducer, where          */
/*                            node_in_this_transducer is being     */
/*                            copied into.                         */
/*                                                                 */
/*         node_mapping A mapping associating nodes in this        */
/*                      transducer with nodes in                   */
/*                      another_transducer.                        */
/*                                                                 */ 
/*                                                                 */
/*  preconditions:                                                 */
/*         node_in_This_transducer and node_in_another_transducer  */
/*         are nodes in two different transducers.                 */
/*                                                                 */
/*         node_in_another_transducer and                          */
/*         target_node_of_transducer_transition are both nodes in  */
/*         the transducer another_transducer.                      */
/*                                                                 */
/*FE****************************************************************/

void Transducer::duplicate_node(Node * node_in_this_transducer,
				Node * node_in_another_transducer,
				Node * target_node_of_transducer_transition,
				Transducer * another_transducer,
				map<Node*,Node*> &node_mapping) 
{
  for ( ArcsIter a_it(node_in_this_transducer->arcs());
	a_it;
	a_it++ ) {
    Arc a = *a_it;
    
    Node * target_of_transition = a.target_node();
    
    Node * corresponding_target_in_another_transducer;
    
    // If the target node of the transition a hasn't been duplicated yet,
    // duplicate it and add the corresponding target node in another_transducer,
    // to node_mapping.
    if ( not target_of_transition->was_visited(vmark) ) {
      
      corresponding_target_in_another_transducer = another_transducer->new_node();
      node_mapping[ target_of_transition ] = 
	corresponding_target_in_another_transducer;
      
      duplicate_node(target_of_transition, 
		     corresponding_target_in_another_transducer,
		     target_node_of_transducer_transition,
		     another_transducer,
		     node_mapping);
      
      // Since were building a transducer transition, finality of the node 
      // node_in_this_transducer is signaled by building an epsilon transition 
      // to the target state of the teansducer transition. 
      if ( target_of_transition->is_final() )
	corresponding_target_in_another_transducer->add_arc(Label(0),
							    target_node_of_transducer_transition,     
							    another_transducer);
      
    }
    else {
      corresponding_target_in_another_transducer = node_mapping[ target_of_transition ];
    }

    
  // Clone the transition a into the transducer transition we're building.
    node_in_another_transducer->add_arc(a.label(),
					corresponding_target_in_another_transducer,
					another_transducer);
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::add_as_transition (by Miikka Silfverberg)          */
/*                                                                 */
/*  description: Add this transducer T as a transition into        */
/*               the node node_in_another_transducer in the        */
/*               transducer another_transducer ending in the node  */
/*               target_node_in_another_transducer. Basically, we  */
/*               copy the nodes and transition of this transducer  */
/*               into another transducer, add an epsilon transition*/
/*               from node_in_another_transducer to the start state*/
/*               of the copy and add an epsilon transition to the  */
/*               node target_node_in_another_transducer from       */
/*               every node in the copy corresponding to a final   */
/*               node in this transducer. Note that the copies of  */
/*               final nodes won't actually be final nodes in      */
/*               another_transducer.                               */
/*                                                                 */
/*  parameters: node_in_another_transducer The node, in            */
/*              another_transducer, where this transducer will be  */
/*              added as a transition.                             */
/*                                                                 */
/*              target_node_in_another_transducer The target of    */
/*              the transducer transition, which is being          */
/*              constructed.                                       */
/*                                                                 */
/*              another_transducer The transducer where the nodes  */
/*              node_in_another_transducer and                     */
/*              target_node_in_another_transducer are located.     */
/*                                                                 */      
/*FE****************************************************************/

void Transducer::add_as_transition(Node * node_in_another_transducer, 
				   Node * target_node_in_another_transducer,
				   Transducer * another_transducer )
{
  incr_vmark();

  Node * start_of_transducer_transition = another_transducer->new_node();
  node_in_another_transducer->add_arc(Label(0),
				      start_of_transducer_transition,
				      another_transducer);
  
  map<Node*,Node*> corresponding_nodes_in_the_transducers;
  corresponding_nodes_in_the_transducers[ root_node() ] = start_of_transducer_transition;

  if ( root_node()->is_final() )
    start_of_transducer_transition->add_arc(Label(0),
					    target_node_in_another_transducer,
					    another_transducer);

  root_node()->was_visited(vmark);

  duplicate_node(root_node(),
		 start_of_transducer_transition,
		 target_node_in_another_transducer,
		 another_transducer,
		 corresponding_nodes_in_the_transducers);
}

/*FA****************************************************************/
/*                                                                 */
/*  Transducer::splice_nodes                                       */
/*                                                                 */
/*FE****************************************************************/

void Transducer::splice_nodes(Node *node, Node *node2, Label sl, 
			     Transducer *sa, Transducer *a)
{
  if (!node->was_visited(vmark)) {

    node->set_forward(node2);

    // define final nodes
    if (node->is_final())
      node2->set_final(1);

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Node *t2=NULL, *t=arc->target_node();

      if (t->check_visited(vmark))
	t2 = t->forward();
      else
	t2 = a->new_node(); // create a new node

      if (arc->label() == sl) {
	// insert the transducer
	//Miikka Silfverberg commented the following line 
	//splice_arc(sa->root_node(), node2, t2, a);
	//Miikka Silfverberg added the following line.
	sa->add_as_transition(node2,t2,a);
      }
      else {
	// add a link to the node
	node2->add_arc(arc->label(), t2, a);
      }
      splice_nodes( t, t2, sl, sa, a );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::splice                                             */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::splice( Label sl, Transducer *sa )

{
  Alphabet::iterator it;

  Transducer *na = new Transducer();

  for( it=alphabet.begin(); it!=alphabet.end(); it++ ) {
    Label l = *it;
    if (l != sl)
      na->alphabet.insert(l);
  }
  for( it=sa->alphabet.begin(); it!=sa->alphabet.end(); it++ )
    na->alphabet.insert(*it);

  incr_vmark();
  splice_nodes(root_node(), na->root_node(), sl, sa, na );

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::replace_char                                       */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::replace_char( Character c, Character nc, bool ignore_epsilon_pairs )

{
  Alphabet::iterator it;

  Transducer *na = new Transducer();

  for( it=alphabet.begin(); it!=alphabet.end(); it++ ) {
    Label l = *it;
    if (l.lower_char() == c)
      l = Label(nc, l.upper_char());
    if (l.upper_char() == c)
      l = Label(nc, l.lower_char());
    na->alphabet.insert(l.replace_char(c,nc,ignore_epsilon_pairs));
  }

  incr_vmark();
  replace_char2(root_node(), na->root_node(), c, nc, na, ignore_epsilon_pairs );

  return *na;
}

Transducer &Transducer::replace_char( KeySet *ks, Character nc )

{
  Alphabet::iterator it;
  KeySet::iterator it2;

  Transducer *na = new Transducer();

  for( it=alphabet.begin(); it!=alphabet.end(); it++ ) {
    Label l = *it;
    for ( it2=ks->begin(); it2!=ks->end(); it++ ) {
      Key c = *it2;
      if (l.lower_char() == c)
	l = Label(nc, l.upper_char());
      if (l.upper_char() == c)
	l = Label(nc, l.lower_char());
      na->alphabet.insert(l.replace_char(c,nc));
    }
  }

  incr_vmark();
  replace_char2(root_node(), na->root_node(), ks, nc, na );

  return *na;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::replace_char2                                      */
/*                                                                 */
/*FE****************************************************************/

void Transducer::replace_char2(Node *node, Node *node2, Character c, 
			       Character nc, Transducer *a, bool ignore_epsilon_pairs)
{
  if (!node->was_visited(vmark)) {

    node->set_forward(node2);

    // define final nodes
    if (node->is_final())
      node2->set_final(1);

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Node *t2=NULL, *t=arc->target_node();

      if (t->check_visited(vmark))
	t2 = t->forward();
      else
	t2 = a->new_node(); // create a new node

      node2->add_arc(arc->label().replace_char(c, nc, ignore_epsilon_pairs), t2, a);
      replace_char2( t, t2, c, nc, a, ignore_epsilon_pairs );
    }
  }
}

void Transducer::replace_char2(Node *node, Node *node2, KeySet *ks, 
			       Character nc, Transducer *a)
{
  if (!node->was_visited(vmark)) {

    node->set_forward(node2);

    // define final nodes
    if (node->is_final())
      node2->set_final(1);

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Node *t2=NULL, *t=arc->target_node();

      if (t->check_visited(vmark))
	t2 = t->forward();
      else
	t2 = a->new_node(); // create a new node

      node2->add_arc(arc->label().replace_char(ks, nc), t2, a);

      replace_char2( t, t2, ks, nc, a );
    }
  }
}
