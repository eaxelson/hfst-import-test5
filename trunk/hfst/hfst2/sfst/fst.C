#include "fst.h"
#include "escape.h"

const int BUFFER_SIZE=100000;


/*FA****************************************************************/
/*                                                                 */
/*  Arcs::size                                                     */
/*                                                                 */
/*FE****************************************************************/

int Arcs::size() const

{
  int n=0;
  for( Arc *p=first_arcp; p; p=p->next ) n++;
  for( Arc *p=first_epsilon_arcp; p; p=p->next ) n++;
  return n;
}


/*FA****************************************************************/
/*                                                                 */
/*  Arcs::target_node                                              */
/*                                                                 */
/*FE****************************************************************/

Node *Arcs::target_node( Label l )

{
  Arc *arc;

  for( arc=first_arcp; arc; arc=arc->next)
    if (arc->label() == l)
      return arc->target_node();
  
  return NULL;
}

// HFST test function for filtering multiple epsilon paths
// in composition
Node *Arcs::target_node_filter( Label l )

{
  Arc *arc;

  for( arc=first_arcp; arc; arc=arc->next) {
    Label label = arc->label();
    if (label.lower_char() == Label::epsilon &&
	l.lower_char() == Label::epsilon )
      return arc->target_node();
    if (label.upper_char() == Label::epsilon &&
	l.upper_char() == Label::epsilon )
      return arc->target_node();
    if (label.lower_char() == ANYKEY &&
	label.upper_char() == ANYKEY &&
	l.lower_char() != Label::epsilon &&
	l.upper_char() != Label::epsilon )
      return arc->target_node();
  }
  
  return NULL;
}

const Node *Arcs::target_node( Label l ) const

{
  const Arc *arc;

  for( arc=first_arcp; arc; arc=arc->next)
    if (arc->label() == l)
      return arc->target_node();
  
  return NULL;
}


/*FA****************************************************************/
/*                                                                 */
/*  Arcs::add_arc                                                  */
/*                                                                 */
/*FE****************************************************************/

void Arcs::add_arc( Label l, Node *node, Transducer *a )

{
  Arc *arc=a->new_arc( l, node );

  if (l.is_epsilon()) {
    arc->next = first_epsilon_arcp;
    first_epsilon_arcp = arc;
  }
  else {
    arc->next = first_arcp;
    first_arcp = arc;
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Arcs::remove_arc                                               */
/*                                                                 */
/*FE****************************************************************/

int Arcs::remove_arc( Arc *arc )

{
  Arc **p = (arc->label().is_epsilon()) ? &first_epsilon_arcp : &first_arcp;
  for( ; *p; p=&(*p)->next )
    if (*p == arc) {
      *p = arc->next;
      return 1;
    }
  return 0;
}


/*FA****************************************************************/
/*                                                                 */
/*  Node::init                                                     */
/*                                                                 */
/*FE****************************************************************/

void Node::init()

{
  final = false;
  visited = 0;
  arcsp.init();
  forwardp = NULL;
  //marked = false;  // ADDED by Erik Axelson
}


/*FA****************************************************************/
/*                                                                 */
/*  NodeNumbering::number_node                                     */
/*                                                                 */
/*FE****************************************************************/

void NodeNumbering::number_node( Node *node, Transducer &a )

{
  if (!node->was_visited( a.vmark )) {
    nummap[node] = nodes.size();
    nodes.push_back(node);
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      number_node( arc->target_node(), a );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  NodeNumbering::NodeNumbering                                   */
/*                                                                 */
/*FE****************************************************************/

NodeNumbering::NodeNumbering( Transducer &a )

{
  a.incr_vmark();
  number_node( a.root_node(), a );
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::new_node                                           */
/*                                                                 */
/*FE****************************************************************/

Node *Transducer::new_node()

{
  Node *node=(Node*)mem.alloc( sizeof(Node) );

  node->init();
  return node;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::new_arc                                            */
/*                                                                 */
/*FE****************************************************************/

Arc *Transducer::new_arc( Label l, Node *target )

{
  Arc *arc=(Arc*)mem.alloc( sizeof(Arc) );

  arc->init( l, target);
  return arc;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::add_string                                         */
/*                                                                 */
/*FE****************************************************************/

void Transducer::add_string( char *s, bool extended )

{
  Node *node=root_node();
  
  Label l;
  while (!(l = alphabet.next_label(s, extended)).is_epsilon()) {
    alphabet.insert(l);
    Arcs *arcs=node->arcs();
    node = arcs->target_node( l );
    if (node == NULL) {
      node = new_node();
      arcs->add_arc( l, node, this );
    }
  }
  node->set_final(1);
}


// added by Erik Axelson
void Transducer::add_path( vector<Label> path ) 

{
  Node *node=root_node(); 

  for (unsigned int i=0; i<path.size(); i++) {
    Label l=path[i];
    Arcs *arcs=node->arcs();
    node = arcs->target_node( l );
    if (node == NULL) {
      node = new_node();
      arcs->add_arc( l, node, this );
    }
  }
  node->set_final(1);
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::Transducer                                         */
/*                                                                 */
/*FE****************************************************************/

Transducer::Transducer( vector<Label> &path )
  : root(), mem()
{

  Node *node=root_node();

  vmark = 0;
  deterministic = minimised = true;
  acyclic = true;
  for( size_t i=0; i<path.size(); i++ ) {
    Arcs *arcs=node->arcs();
    node = new_node();
    arcs->add_arc( path[i], node, this );
    if (path[i].is_epsilon())
      deterministic = minimised = false;
  }
  node->set_final(1);
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::Transducer                                         */
/*                                                                 */
/*FE****************************************************************/

Transducer::Transducer( istream &is, const Alphabet *a, bool verbose  )
  : root(), mem()
{
  bool extended=false;
  int n=0;
  char buffer[10000];

  vmark = 0;
  deterministic = true;
  minimised = false;
  if (a) {
    alphabet.copy(*a);
    extended = true;
  }
  while (is.getline(buffer, 10000)) {
    if (verbose && ++n % 10000 == 0) {
      if (n == 10000)
	cerr << "\n";
      cerr << "\r" << n << " words";
    }
    // delete final whitespace characters
    int l;
    for( l=strlen(buffer)-1; l>=0; l-- )
      if ((buffer[l] != ' ' && buffer[l] != '\t' && buffer[l] != '\r') ||
	  (l > 0 && buffer[l-1] == '\\'))
	break;
    buffer[l+1] = 0;

    add_string(buffer,extended);
  }
  if (verbose && n >= 10000)
    cerr << "\n";
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::Transducer                                         */
/*                                                                 */
/*FE****************************************************************/

Transducer::Transducer( char *s, const Alphabet *a, bool extended )
  : root(), mem()
{
  vmark = 0;
  deterministic = minimised = true;
  if (a)
    alphabet.copy(*a);
  add_string(s, extended);
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::clear                                              */
/*                                                                 */
/*FE****************************************************************/

void Transducer::clear()

{
  vmark = 0;
  deterministic = minimised = false;
  root.init();
  mem.clear();
  alphabet.clear();
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::store_symbols                                      */
/*                                                                 */
/*FE****************************************************************/

void Transducer::store_symbols(Node *node, SymbolMap &symbol, LabelSet &labels)

{
  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      Label l=arc->label();

      labels.insert(l);

      Character c = l.upper_char();
      if (symbol.find(c) == symbol.end()) {
	const char *s = alphabet.code2symbol(c);
	if (s)
	  symbol[c] = strdup(s);
      }

      c = l.lower_char();
      if (symbol.find(c) == symbol.end()) {
	const char *s = alphabet.code2symbol(c);
	if (s)
	  symbol[c] = strdup(s);
      }

      store_symbols( arc->target_node(), symbol, labels );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::minimise_alphabet                                  */
/*                                                                 */
/*FE****************************************************************/

void Transducer::minimise_alphabet()

{
  SymbolMap symbols;
  LabelSet labels;
  incr_vmark();
  store_symbols(root_node(), symbols, labels);
  alphabet.clear();
  for( SymbolMap::iterator it=symbols.begin(); it!=symbols.end(); it++ ) {
    alphabet.add_symbol( it->second, it->first );
    free(it->second);
  }
  for( LabelSet::iterator it=labels.begin(); it!=labels.end(); it++ )
    alphabet.insert(*it);
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::minimise                                           */
/*                                                                 */
/*FE****************************************************************/

Transducer &Transducer::minimise( bool verbose )

{
  if (minimised)
    return copy();

  Transducer *a1, *a2;

  a1 = &reverse();
  a2 = &a1->determinise();
  delete a1;

  a1 = &a2->reverse();
  delete a2;

  a2 = &a1->determinise();
  delete a1;

  a2->minimised = a2->deterministic = true; // deterministic added
  a2->minimise_alphabet();

  return *a2;
}



// *************************************************** //
//                                                     //
//  EPSILON REMOVAL ALGORITHM written by Erik Axelson  //
//                                                     //
// *************************************************** //


/* Find the corresponding node in 'copy_tr' for 'node'. If needed, create a new node to 'copy_tr'
   and update 'mapper' accordingly. */

Node *node_in_copy_tr( Node *node, Transducer *copy_tr, NodeNumbering &nn, map<int, Node*> &mapper ) {
  int node_number = nn[node];  // node number in original transducer
  map<int,Node*>::iterator it = mapper.find(node_number); // iterator to associated node in copy_tr
  if (it == mapper.end()) {
    Node *associated_node = copy_tr->new_node(); // create new node in copy_tr
    if (node->is_final())
      associated_node->set_final(true);
    mapper[node_number] = associated_node; // and associate it with node_number
    return associated_node;
  }
  else
    return it->second;
}; 


/* Recursive epsilon removal algorithm. Copies arcs and their
   target nodes starting from search_node to node copy_tr_start_node
   in transducer copy_tr. nn and mapper are used to associate nodes
   with nodes in copy_tr. */

void Transducer::copy_nodes( Node *search_node, Transducer *copy_tr,
			     Node *copy_tr_start_node,
			     NodeNumbering &nn, map<int, Node*> &mapper ) {

  // go through all arcs leaving from search node
  // (the iterator lists the epsilon arcs first)
  for( ArcsIter it(search_node->arcs()); it; it++ ) {
    Arc arc=*it;

    if (arc.label().is_epsilon()) {
      // 'forward', which is originally NULL, is used as a flag
      // for detecting epsilon transition loops
      if (search_node->forward() != copy_tr_start_node) { 
	search_node->set_forward(copy_tr_start_node);  // set epsilon flag
	if (arc.target_node()->is_final())
	  copy_tr_start_node->set_final(true);
	copy_nodes(arc.target_node(), copy_tr, copy_tr_start_node, nn, mapper);
	search_node->set_forward(NULL);  // remove epsilon flag
      }
    }

    else {
      // target node in copy_tr
      Node *copy_tr_end_node = node_in_copy_tr(arc.target_node(), copy_tr, nn, mapper);
      // add arc to copy_tr
      copy_tr_start_node->add_arc( Label(arc.label().lower_char(),
					 arc.label().upper_char()),
				   copy_tr_end_node,
				   copy_tr );
      // if the target node is not visited, copy nodes recursively
      if ( !(arc.target_node()->was_visited(vmark)) )
	copy_nodes(arc.target_node(), copy_tr, copy_tr_end_node, nn, mapper);
    }

  }
};


Transducer &Transducer::remove_epsilons() {

  if ( deterministic || minimised )
    return this->copy();

  NodeNumbering nn(*this);
  incr_vmark();
  Transducer *copy_tr = new Transducer();
  copy_tr->alphabet.copy(alphabet);
  map<int, Node*> mapper;
  // mark root node as visited
  root_node()->was_visited(vmark);
  // set copy_tr root node final, if needed
  if (root_node()->is_final())
    copy_tr->root_node()->set_final(true);
  // associate the root_nodes in this and copy_tr 
  // (node numbering for root_node is zero)
  mapper[0] = copy_tr->root_node();

  copy_nodes(root_node(), copy_tr, copy_tr->root_node(), nn, mapper);
  incr_vmark();	

  return *copy_tr;
};



/*FA****************************************************************/
/*                                                                 */
/*  Transducer::enumerate_paths_node                               */
/*                                                                 */
/*FE****************************************************************/

void Transducer::enumerate_paths_node( Node *node, vector<Label> &path, 
				       NodeHashSet &previous,
				       vector<Transducer*> &result )
{
  if (node->is_final())
    result.push_back(new Transducer(path));

  for( ArcsIter it(node->arcs()); it; it++ ) {
    Arc *arc=it;

    NodeHashSet::iterator it=previous.insert(node).first;
    path.push_back(arc->label());
    enumerate_paths_node( arc->target_node(), path, previous, result );
    path.pop_back();
    previous.erase(it);
  }
}

void Transducer::enumerate_paths_node( Node *node, vector<Label> &path, 
				       NodeHashSet &previous,
				       vector<vector<Label> > &result )
{
  if (node->is_final())
    result.push_back(path);

  for( ArcsIter it(node->arcs()); it; it++ ) {
    Arc *arc=it;

    NodeHashSet::iterator it=previous.insert(node).first;
    path.push_back(arc->label());
    enumerate_paths_node( arc->target_node(), path, previous, result );
    path.pop_back();
    previous.erase(it);
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::enumerate_paths                                    */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::enumerate_paths( vector<Transducer*> &result )

{
  if (is_infinitely_ambiguous())
    return true;
  for( size_t i=0; i<result.size(); i++ )
    delete result[i];
  result.clear();

  vector<Label> path;
  NodeHashSet previous;
  enumerate_paths_node( root_node(), path, previous, result );
  return false;
}

// added by Erik Axelson
bool Transducer::enumerate_paths( vector<vector<Label> > &result )

{
  if (is_infinitely_ambiguous())
    return true;
  //for( size_t i=0; i<result.size(); i++ )
  //  delete result[i];
  //result.clear();

  vector<Label> path;
  NodeHashSet previous;
  enumerate_paths_node( root_node(), path, previous, result );
  return false;
}




/*FA****************************************************************/
/*                                                                 */
/*  Transducer::print_strings_node                                 */
/*                                                                 */
/*FE****************************************************************/

int Transducer::print_strings_node(Node *node, char *buffer, int pos,
				  FILE *file, bool with_brackets )
{
  int result = 0;

  if (node->was_visited( vmark )) {
    if (node->forward() != NULL) { // cycle detected
      cerr << "Warning: cyclic analyses (cycle aborted)\n";
      return 0;
    }
    node->set_forward(node);  // used like a flag for loop detection
  }
  if (pos == BUFFER_SIZE)
    throw "Output string in function print_strings_node is too long";
  if (node->is_final()) {
    buffer[pos] = '\0';
    fprintf(file,"%s\n", buffer);
    result = 1;
  }
  for( ArcsIter i(node->arcs()); i; i++ ) {
    int p=pos;
    Arc *arc=i;
    Label l=arc->label();
    alphabet.write_label(l, buffer, &p, with_brackets);
    result |= print_strings_node(arc->target_node(), buffer, p, 
				 file, with_brackets );
  }
  node->set_forward(NULL);

  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::print_strings                                      */
/*                                                                 */
/*FE****************************************************************/

int Transducer::print_strings( FILE *file, bool with_brackets )

{
  char buffer[BUFFER_SIZE];
  incr_vmark();
  return print_strings_node( root_node(), buffer, 0, file, with_brackets );
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::analyze_string                                     */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::analyze_string( char *string, FILE *file, bool with_brackets )

{
  Transducer a1(string, &alphabet, false);
  Transducer *a2=&(*this || a1);
  Transducer *a3=&(a2->lower_level());
  delete a2;
  a2 = &a3->minimise();
  delete a3;

  a2->alphabet.copy(alphabet);
  bool result = a2->print_strings( file, with_brackets );
  delete a2;
  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::generate_string                                    */
/*                                                                 */
/*FE****************************************************************/

bool Transducer::generate_string( char *string, FILE *file, bool with_brackets)

{
  Transducer a1(string, &alphabet, false);
  Transducer *a2=&(a1 || *this);
  Transducer *a3=&(a2->upper_level());
  delete a2;
  a2 = &a3->minimise();
  delete a3;

  a2->alphabet.copy(alphabet);
  bool result = a2->print_strings( file, with_brackets );
  delete a2;
  return result;
}

// ADDED by Erik Axelson

static void keys_to_alphabet_( Node *node, Alphabet &alphabet, int vmark) {
  if (node->was_visited( vmark ))
    return;
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    if (!arc->label().is_epsilon()) {
      Character c_upper = arc->label().upper_char();
      Character c_lower = arc->label().lower_char();
      char name_upper [33];
      char name_lower [33];
      sprintf(name_upper, "%d", (int)c_upper);
      sprintf(name_lower, "%d", (int)c_lower);
      alphabet.add_symbol(name_upper, c_upper);
      alphabet.add_symbol(name_lower, c_lower);
      //alphabet.add_symbol(" ", c_upper);
      //alphabet.add_symbol(" ", c_lower);
    }
    keys_to_alphabet_(arc->target_node(), alphabet, vmark);
  }  
}

void Transducer::keys_to_alphabet() {
  incr_vmark();
  keys_to_alphabet_(root_node(), alphabet, vmark);
}

/*FA****************************************************************/
/*                                                                 */
/*  complete                                                       */
/*                                                                 */
/*FE****************************************************************/

static void complete( Node *node, Alphabet &alphabet, int vmark)

{
  if (node->was_visited( vmark ))
    return;
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    if (!arc->label().is_epsilon())
      alphabet.insert(arc->label());
    complete(arc->target_node(), alphabet, vmark);
  }  
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::complete_alphabet                                  */
/*                                                                 */
/*FE****************************************************************/

void Transducer::complete_alphabet()

{
  incr_vmark();
  complete(root_node(), alphabet, vmark);
}



// *********************
// Added by Erik Axelson
// *********************

static void define_key_set_( Node *node, set<Key> &keys, int vmark) {
  if (node->was_visited( vmark ))
    return;
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    //if (!arc->label().is_epsilon()) {
      keys.insert( arc->label().lower_char() );
      keys.insert( arc->label().upper_char() );
      //}
    define_key_set_(arc->target_node(), keys, vmark);
  }  
}


set<Key> Transducer::define_key_set() {
  set<Key> keys;
  incr_vmark();
  define_key_set_(root_node(), keys, vmark);
  return keys;
}


static void define_key_set_pointer_( Node *node, set<Key> *keys, int vmark) {
  if (node->was_visited( vmark ))
    return;
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    //if (!arc->label().is_epsilon()) {
      keys->insert( arc->label().lower_char() );
      keys->insert( arc->label().upper_char() );
      //}
    define_key_set_pointer_(arc->target_node(), keys, vmark);
  }  
}

set<Key> *Transducer::define_key_set_pointer() {
  set<Key> *keys = new set<Key>();
  incr_vmark();
  define_key_set_pointer_(root_node(), keys, vmark);
  return keys;
}




/*FA****************************************************************/
/*                                                                 */
/*  print_node                                                     */
/*                                                                 */
/*FE****************************************************************/

static void print_node( ostream &s, Node *node, NodeNumbering &index, 
			long vmark, Alphabet &abc )

{
  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    if (node->is_final())
      s << "final\t" << index[node] << "\n";
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      s << index[node] << "\t";
      s << abc.write_label(arc->label()) << "\t";
      s << index[arc->target_node()] << "\n";
    }
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      print_node( s, arc->target_node(), index, vmark, abc );
    }
  }
}

// ADDED Print transducer in AT&T format

static void print_node_att( ostream &s, Node *node, NodeNumbering &index, 
			    long vmark, Alphabet &abc )

{
  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    if (node->is_final())
      s << index[node] << "\n";
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      s << index[node] << "\t";
      s << index[arc->target_node()] << "\t";
      //s << abc.write_label_att(arc->label()) << "\n";
      Key input_key = arc->label().lower_char();
      Key output_key = arc->label().upper_char();
      const char *input = abc.code2symbol(input_key);
      const char *output = abc.code2symbol(output_key);
      //fprintf(stderr, "print_node_att: %s:%s\n", input, output);
      //s << input << "\t" << output << "\n";
      // ASSUMPTION: '\n' and '\t' are not used in multicharacter symbols
      //if (strcmp(input, "\n") == 0) input = "\\n";
      //if (strcmp(input, "\t") == 0) input = "\\t";
      //if (strcmp(output, "\n") == 0) output = "\\n";
      //if (strcmp(output, "\t") == 0) output = "\\t";
      //s << input << "\t";
      //s << output << "\n";
      if (input)
	COMMON::escape_and_print(input, s, true, false, false);
      else
	s << "\\" << input_key;
      s << "\t";
      if (output)
	COMMON::escape_and_print(output, s, true, false, false);
      else
	s << "\\" << output_key;
      s << "\n";
      //delete input; delete output; ?
    }
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      print_node_att( s, arc->target_node(), index, vmark, abc );
    }
  }
}

void Transducer::harmonize(KeyTable *old_table, KeyTable *new_table, Alphabet& alpha) {
  incr_vmark();
  harmonize_node(root_node(), vmark, old_table, new_table, alpha);
}


void Transducer::harmonize_node(Node *node, long vmark, KeyTable *old_table, KeyTable *new_table, Alphabet& alpha) 
{
  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    if (arcs == NULL)
      fprintf(stderr, "ERROR: arcs is NULL\n");

    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      Label l = arc->label();      
      Key ikey = KeyTable::harmonize_key(l.lower_char(), old_table, new_table, alpha);
      Key okey = KeyTable::harmonize_key(l.upper_char(), old_table, new_table, alpha);
      //fprintf(stderr, "harmonize_node: harmonized %hu:%hu to %hu:%hu", l.lower_char(), l.upper_char(), ikey, okey);
      //arc->print();
      arc->set_label(Label(ikey,okey));
      //arc->print();
      harmonize_node(arc->target_node(), vmark, old_table, new_table, alpha);
    }
  }
}


static void print_node_number( ostream &s, Node *node, NodeNumbering &index, 
			       long vmark ) 
{
  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    if (arcs == NULL)
      fprintf(stderr, "ERROR: arcs is NULL\n");
    if (node->is_final())
      s << index[node] << "\n";
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      s << index[node] << "\t";
      s << index[arc->target_node()] << "\t";
      s << arc->label().lower_char() << "\t";
      s << arc->label().upper_char() << "\n";
    }
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      print_node_number( s, arc->target_node(), index, vmark );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  operator<<                                                     */
/*                                                                 */
/*FE****************************************************************/

ostream &operator<<( ostream &s, Transducer &a )

{
  NodeNumbering index(a);
  a.incr_vmark();
  print_node( s, a.root_node(), index, a.vmark,  a.alphabet );
  return s;
}

// ADDED

void print_operator( ostream &s, Transducer &a, Alphabet &alpha )

{
  NodeNumbering index(a);
  a.incr_vmark();
  print_node_att( s, a.root_node(), index, a.vmark, alpha );
  //return s;
}



void to_string( std::string& path, Node *node, NodeNumbering& index,
		long vmark, Alphabet& alpha, bool spaces) {
  if (!node->was_visited( vmark )) { 
    Arcs *arcs=node->arcs();
    for( ArcsIter p(arcs); p; p++ ) {
      if (path.length() > 0 && spaces) {
	path.append(1, ' ');
      }
      Arc *arc=p;
      const char *isymbol = alpha.code2symbol(arc->label().lower_char());
      const char *osymbol = alpha.code2symbol(arc->label().upper_char());
      for (int i=0; isymbol[i] != '\0'; i++) {
	if (isymbol[i] == ':' || isymbol[i] == '\\' || 
	    (spaces && isymbol[i] == ' ' ) )
	  path.append(1, '\\');
	path.append(1, isymbol[i]);
      }
      if (strcmp(isymbol, osymbol) != 0) {
	path.append(1, ':');
	for (int i=0; osymbol[i] != '\0'; i++) {
	  if (osymbol[i] == ':' || osymbol[i] == '\\' || 
	      (spaces && isymbol[i] == ' ' ) )
	    path.append(1, '\\');
	  path.append(1, osymbol[i]);
	}
      }
    }
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      to_string( path, arc->target_node(), index, vmark, alpha, spaces );
    }
  }
}

char * to_string( Transducer &a, Alphabet &alpha, bool spaces ) {
  if (a.is_empty()) {
    //std::string result="\\empty_transducer";
    //return strdup(result.c_str());
    return NULL;
  }
  NodeNumbering index(a);
  a.incr_vmark();
  std::string result="";
  to_string( result, a.root_node(), index, a.vmark, alpha, spaces );
  /*if (strcmp(result.c_str(), "") == 0) {
    const char *name = alpha.code2symbol(Label::epsilon);
    if (name == NULL)
      result.append("\\epsilon");
    else
      return strdup(name);
      }*/
  return strdup(result.c_str());
}


Transducer * to_transducer( const char *str, Alphabet &alpha ) {  // copy alphabet?

  //if (strcmp(str, "\\empty_transducer") == 0 )  // empty transducer
  //  return new Transducer();
  if (strcmp(str, "") == 0 ) {                  // epsilon transducer
    Transducer *t = new Transducer();
    t->root_node()->set_final(1);
    return t;
  }

  vector<KeyPair> pairs = alpha.string_to_keypair_vector(str);
  Transducer *result = new Transducer(pairs);
  return result;
};


void print_operator_number( ostream &s, Transducer &a )

{
  NodeNumbering index(a);
  a.incr_vmark();
  print_node_number( s, a.root_node(), index, a.vmark );
}




/*FA****************************************************************/
/*                                                                 */
/*  store_node_info                                                */
/*                                                                 */
/*FE****************************************************************/

static void store_node_info( FILE *file, Node *node )

{
  // write final flag
  char c=node->is_final();
  fwrite(&c,sizeof(c),1,file);
  
  // write the number of arcs
  int nn = node->arcs()->size();
  if (nn > 65535)
    throw "Error: in function store_node\n";
  unsigned short n=(unsigned short)nn;
  fwrite(&n,sizeof(n),1,file);
}

//Added by Miikka Silfverberg
static void store_node_info( ostream &out, Node *node )

{
  // write final flag
  char c=node->is_final();
  out.write(&c,sizeof(c));
  
  // write the number of arcs
  int nn = node->arcs()->size();
  if (nn > 65535)
    throw "Error: in function store_node\n";
  unsigned short n=(unsigned short)nn;
  out.write((char*)(&n),sizeof(n));
}

/*FA****************************************************************/
/*                                                                 */
/*  store_arc_label                                                */
/*                                                                 */
/*FE****************************************************************/

static void store_arc_label( FILE *file, Arc *arc )

{
  Label l=arc->label();
  Character lc=l.lower_char();
  Character uc=l.upper_char();
  fwrite(&lc,sizeof(lc),1,file);
  fwrite(&uc,sizeof(uc),1,file);
}

//Added by Miikka Silfverberg
static void store_arc_label( ostream &out, Arc *arc )

{
  Label l=arc->label();
  Character lc=l.lower_char();
  Character uc=l.upper_char();
  out.write((char*)(&lc),sizeof(lc));
  out.write((char*)(&uc),sizeof(uc));
}


/*FA****************************************************************/
/*                                                                 */
/*  store_node                                                     */
/*                                                                 */
/*FE****************************************************************/

static void store_node( FILE *file, Node *node, NodeNumbering &index, 
			long vmark )
{
  if (!node->was_visited( vmark )) {

    store_node_info( file, node );
  
    // write the arcs
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      store_arc_label( file, arc );
      unsigned int t=index[arc->target_node()];
      fwrite(&t,sizeof(t),1,file);
      store_node(file, arc->target_node(), index, vmark );
    }
  }
}

//Added by Miikka Silfverberg
static void store_node( ostream &out, Node *node, NodeNumbering &index, 
			long vmark )
{
  if (!node->was_visited( vmark )) {

    store_node_info( out, node );
  
    // write the arcs
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      store_arc_label( out, arc );
      unsigned int t=index[arc->target_node()];
      out.write((char*)(&t),sizeof(t));
      store_node(out, arc->target_node(), index, vmark );
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  store_lowmem_node                                              */
/*                                                                 */
/*FE****************************************************************/

static void store_lowmem_node( FILE *file, Node *node, NodeNumbering &index,
			       vector<unsigned int> &startpos)
{
  store_node_info( file, node );
  
  // write the arcs
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    store_arc_label( file, arc );
    unsigned int t=startpos[index[arc->target_node()]];
    fwrite(&t,sizeof(t),1,file);
  }
}

//Added by Miikka Silfverberg
static void store_lowmem_node( ostream &out, Node *node, NodeNumbering &index,
			       vector<unsigned int> &startpos)
{
  store_node_info( out, node );
  
  // write the arcs
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    store_arc_label( out, arc );
    unsigned int t=startpos[index[arc->target_node()]];
    out.write((char*)(&t),sizeof(t));
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::store_lowmem                                       */
/*                                                                 */
/*FE****************************************************************/

void Transducer::store_lowmem( FILE *file )

{
  fputc('l',file);
  alphabet.store(file);

  // storing size of index table
  NodeNumbering index(*this);

  // compute the start position of the first node
  unsigned int pos=(unsigned int)ftell(file);
  vector<unsigned int> startpos;
  for( size_t i=0; i<index.number_of_nodes(); i++ ) {
    startpos.push_back(pos);
    Node *node=index.get_node(i);
    Arcs *arcs=node->arcs();
    pos += sizeof(char) // size of final flag
      + sizeof(unsigned short) // size of number of arcs
      + arcs->size() * (sizeof(Character) * 2 + sizeof(unsigned int)); // size of n arcs
  }

  // storing nodes
  for( size_t i=0; i<index.number_of_nodes(); i++ )
    store_lowmem_node( file, index.get_node(i), index, startpos );
}

//Added by Miikka Silfverberg
void Transducer::store_lowmem( ostream &out )

{
  out.put('l');
  alphabet.store(out);

  // storing size of index table
  NodeNumbering index(*this);

  // compute the start position of the first node
  unsigned int pos=(unsigned int)out.tellp();
  vector<unsigned int> startpos;
  for( size_t i=0; i<index.number_of_nodes(); i++ ) {
    startpos.push_back(pos);
    Node *node=index.get_node(i);
    Arcs *arcs=node->arcs();
    pos += sizeof(char) // size of final flag
      + sizeof(unsigned short) // size of number of arcs
      + arcs->size() * (sizeof(Character) * 2 + sizeof(unsigned int)); // size of n arcs
  }

  // storing nodes
  for( size_t i=0; i<index.number_of_nodes(); i++ )
    store_lowmem_node( out, index.get_node(i), index, startpos );
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::store                                              */
/*                                                                 */
/*FE****************************************************************/

void Transducer::store( FILE *file, bool alpha )

{
  if (!alpha)
    fputc('A',file);
  fputc('a',file);

  NodeNumbering index(*this);
  incr_vmark();
  unsigned int n=index.number_of_nodes();
  fwrite(&n,sizeof(n),1,file);
  store_node( file, root_node(), index, vmark );

  if (alpha)
    alphabet.store(file);
}

//Added by Miikka Silfverberg
/*void Transducer::store( ostream &out, bool alpha ) {
  
  if (!alpha)
    out.put('A');

  //Write identifier.
  out.put('a');
  NodeNumbering index(*this);
  incr_vmark();
  unsigned int n=index.number_of_nodes();
  out.write((char*)(&n),sizeof(n));
  store_node( out, root_node(), index, vmark );

  //fprintf(stderr, "\nTransducer::store: storing the alphabet...\n");
  if (alpha)
    alphabet.store(out);
}
*/

/*FA****************************************************************/
/*                                                                 */
/*  read_node                                                      */
/*                                                                 */
/*FE****************************************************************/

static void read_node( FILE *file, Node *node, Node **p, Transducer *a )
{
  char c;
  size_t foo;
  foo=fread(&c,sizeof(c),1,file);
  node->set_final(c);

  unsigned short n;
  foo=fread( &n, sizeof(n), 1, file);

  for( int i=0; i<n; i++ ) {
    Character lc,uc;
    unsigned int t;
    foo=fread(&lc,sizeof(lc),1,file);
    foo=fread(&uc,sizeof(uc),1,file);
    foo=fread(&t,sizeof(t),1,file);
    if (ferror(file))
      throw "Error encountered while reading transducer from file";
    if (p[t])
      node->add_arc( Label(lc,uc), p[t], a );
    else {
      p[t] = a->new_node();
      node->add_arc( Label(lc,uc), p[t], a );
      read_node(file, p[t], p, a );
    }
  }
}

// Added by Miikka Silfverberg
static void read_node( Node *node, Node **p, Transducer *a, BinaryReader &reader)
{
  char is_final;
  reader.read(&is_final,sizeof(is_final));

  node->set_final(is_final);

  unsigned short number_of_arcs;
  reader.read(&number_of_arcs,sizeof(number_of_arcs));

  for( int i=0; i<number_of_arcs; i++ ) {
    Character lower_char,upper_char;
    unsigned int target_node_index = 0;

    reader.read(&lower_char,sizeof(lower_char));
    reader.read(&upper_char,sizeof(upper_char));
    reader.read(&target_node_index,sizeof(target_node_index));

    if (p[target_node_index]){
      node->add_arc( Label(lower_char,upper_char), p[target_node_index], a );
    }
    else {
      p[target_node_index] = a->new_node();
      node->add_arc( Label(lower_char,upper_char), p[target_node_index], a );
      read_node(p[target_node_index], p, a, reader);
    }
  }
}



// Added by Miikka Silfverberg
void Transducer::read_transducer_binary( istream& in, bool alpha )

{  

  // reading the header, added by Erik Axelson
  bool has_header=false;
  properties props;
  props.init();
  int next=in.peek();
  if (next == 'P') {
    has_header=true;
    in.get(); // remove 'P' from stream
    unsigned short header_length;
    in >> header_length; // should be 2
    props.read(in);
  }

  BinaryReader reader(in);
  char identifier;
  reader.read(&identifier,sizeof(identifier)); 

  if (identifier != 'a') {
    fprintf(stderr, "identifier is '%c'", identifier);
    throw "Error: wrong file format (not a standard transducer)\n";
  }

  vmark = deterministic = minimised = 0;  // minimised added

  unsigned int n;
  reader.read(&n,sizeof(n));

  Node **p=new Node*[n];  // maps indices to nodes
  p[0] = root_node();

  for( unsigned int i=1; i<n; i++)
    p[i] = NULL;

  read_node( root_node(), p, this, reader);
  delete[] p;
  if (alpha)
    alphabet.read(reader);
  
  vmark = 1;

  // reading the header, added by Erik Axelson
  if (has_header) {
    set_properties(props);
  }

  
  //deterministic = minimised = 1;  // not true in HFST
}



/*FA****************************************************************/
/*                                                                 */
/*  Transducer::read_transducer_binary                             */
/*                                                                 */
/*FE****************************************************************/

void Transducer::read_transducer_binary( FILE *file, bool alpha )

{
  char identifier = fgetc(file);
  if (identifier != 'a') {
    throw "Error: wrong file format (not a standard transducer)\n";
  }

  vmark = deterministic = 0;
  unsigned int n;
  size_t foo;
  foo=fread(&n,sizeof(n),1,file); // number of nodes
  if (ferror(file))
    throw "Error encountered while reading transducer from file";

  Node **p=new Node*[n];  // maps indices to nodes
  p[0] = root_node();
  for( unsigned int i=1; i<n; i++)
    p[i] = NULL;
  read_node( file, root_node(), p, this );
  delete[] p;

  if (alpha)
    alphabet.read(file);
  
  vmark = 1;
  //deterministic = minimised = 1;  // not true in HFST
}


/*FA****************************************************************/
/*                                                                 */
/*  error_message                                                  */
/*                                                                 */
/*FE****************************************************************/

static void error_message( size_t line )

{
  static char message[1000];
  sprintf(message, "Error: in line %u of text transducer file", 
	  (unsigned int)line);
  throw message;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::create_node                                        */
/*                                                                 */
/*FE****************************************************************/

Node *Transducer::create_node( vector<Node*> &node, char *s, size_t line )

{
  char *p;
  long n = strtol(s, &p, 10);

  if (s == p || n < 0)
    error_message( line );
  if ((long)node.size() <= n)
    node.resize(n+1, NULL);
  if (node[n] == NULL)
    node[n] = new Node;

  return node[n];
}


  char *next_string_att( char* &s, size_t line ) {

    if (s == NULL)
      return NULL;
    
    // scan the input up to the next tab or newline character
    // and unquote symbols preceded by a backslash
    char *p = s;
    char *q = s;
    while (*q!=0 && *q!='\t' && *q!='\n' && *q!='\r') {
      //if (*q == '\\')
      //  q++;
      *(p++) = *(q++);
    }
    if (p == s)
      error_message(line); // no string found

    char *result=s;
    // skip over following whitespace
    while (*q == ' ' || *q == '\t' || *q == '\n' || *q == '\r')
      q++;
    
    if (*q == 0)
      s = NULL; // end of string was reached
    else
      s = q;  // move the string pointer s
    
    *p = 0; // mark the end of the result string
    
    return result;
  }

// *** Read a transducer in AT&T text format ***
//
//  NOTE (by Erik Axelson):
//
//  Epsilon name must be associated to the alphabet of the transducer
//  with number zero. Other names will be automatically associated to
//  numbers in the alphabet.

void Transducer::read_transducer_text_new( istream& is, bool numbers, KeyTable *T,  Alphabet& alpha ) {

  vector<Node*> nodes;
  nodes.push_back(root_node());
  
  vmark = deterministic = 0;
  char buffer[10000];
  for( size_t line=0; is.getline(buffer, 10000); line++ ) {
    char *p;    
    p = buffer;
    if ( strncmp(p, "--", (size_t)2) == 0  ||  strcmp(p, "") == 0 ) {
      break;
    }
    if ( buffer[0] != '%' ) {  // not a comment line

      char *s1 = strtok(p, "\t\n");  // originating state or final state
      char *s2 = strtok(NULL, "\t\n");  // destination state or optional final weight
      char *s3 = strtok(NULL, "\t");  // input symbol or NULL
      
      char *s4; //char *s5;

      if (s2 == NULL || s3 == NULL) {  // final state with or without weight
	create_node( nodes, s1, line )->set_final(true);
      }
      else {   // originating state, destination state, input symbol, output symbol and optional transition weight
	s4 = strtok(NULL, "\t\n");  // output symbol 
	// s5 = strtok(NULL, "\n");    // optional transition weight or empty string or NULL

	//fprintf(stderr, " ..read line: %s\t%s\t%s\t%s\n", s1, s2, s3, s4);

	Node *node = create_node( nodes, s1, line );
	//s = next_string_sfst(p, line);
	//Label l = alphabet.next_label( s, 2 );
	
	Label l; 
	if (numbers) {
	  l = Label((unsigned int)atoi(s3), (unsigned int)atoi(s4));
	}
        else {
	  // unescape the symbols (e.g. backslash plus t becomes tabulator)
	  std::string s3_unescaped;
	  COMMON::unescape_and_add( s3, s3_unescaped);
	  const char *s3_unesc = strdup(s3_unescaped.c_str());
	  std::string s4_unescaped;
	  COMMON::unescape_and_add( s4, s4_unescaped);
	  const char *s4_unesc = strdup(s4_unescaped.c_str());
	  Key input_number;
	  Key output_number;

	  if (T) {
	    input_number = T->add_symbol(alpha.add_symbol(s3_unesc));
	    output_number = T->add_symbol(alpha.add_symbol(s4_unesc));
	  }
	  else {
	    input_number = alpha.add_symbol(s3_unesc);
	    output_number = alpha.add_symbol(s4_unesc);
	  }
	  delete s3_unesc; delete s4_unesc;
	  l = Label(input_number, output_number);
        }
	//if (*s != 0 || l == Label::epsilon)
	//  error_message( line );
	//s = next_string_sfst(p, line);
	Node *target = create_node( nodes, s2, line );
	node->add_arc( l, target, this );
	//fprintf(stderr, " ..added arc %hu:%hu\n", l.lower_char(), l.upper_char());
      }
    }
  }
  
  vmark = 1;
  //deterministic = minimised = 1;  // not true in HFST
 }


/*FA****************************************************************/
/*                                                                 */
/*  next_string                                                    */
/*                                                                 */
/*FE****************************************************************/

char *next_string_sfst( char* &s, size_t line )

{
  // scan the input up to the next tab or newline character
  // and unquote symbols preceded by a backslash
  char *p = s;
  char *q = s;
  while (*q!=0 && *q!='\t' && *q!='\n' && *q!='\r') {
    if (*q == '\\')
      q++;
    *(p++) = *(q++);
  }
  if (p == s)
    error_message(line); // no string found

  char *result=s;
  // skip over following whitespace
  while (*q == ' ' || *q == '\t' || *q == '\n' || *q == '\r')
    q++;
  
  if (*q == 0)
    s = NULL; // end of string was reached
  else
    s = q;  // move the string pointer s

  *p = 0; // mark the end of the result string
  
  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  Transducer::read_transducer_text                               */
/*                                                                 */
/*FE****************************************************************/

void Transducer::read_transducer_text_old( istream& is, KeyTable *T ) {

  vector<Node*> nodes;
  nodes.push_back(root_node());

  vmark = deterministic = 0;
  char buffer[10000];
  for( size_t line=0; is.getline(buffer, 10000); line++ ) {
    char *p;    
    //if (att)
    //  p = att2sfst(buffer, line);  does not work for special characters
    //else
    p = buffer;

    char *s = next_string_sfst(p, line);
    if (strcmp(s, "final") == 0) {
      s = next_string_sfst(p, line);
      create_node( nodes, s, line )->set_final(true);
    }
    else {
      Node *node = create_node( nodes, s, line );
      s = next_string_sfst(p, line);
      Label l = alphabet.next_label( s, 2 );
      if (*s != 0 || l == Label::epsilon)
	error_message( line );
      s = next_string_sfst(p, line);
      Node *target = create_node( nodes, s, line );
      node->add_arc( l, target, this );
    }
  }

  vmark = 1;
  //deterministic = minimised = 1;  // not true in HFST
}




/*FA****************************************************************/
/*                                                                 */
/*  Transducer::Transducer                                         */
/*                                                                 */
/*FE****************************************************************/

// The alphabet of the result can be defined by parameter alpha
// If not defined (NULL) the transducer constructs its own alphabet
Transducer::Transducer( istream& is, bool binary , bool att, bool alpha, bool numbers, KeyTable *T )
  
{  
  if (binary)
    read_transducer_binary( is, alpha );
  //else if (att)
  //  read_transducer_text_new( is, numbers, T );
  else
    read_transducer_text_old( is, T);
}

void Transducer::read_stream( istream& in, bool alpha ) {
  read_transducer_binary( in, alpha );
} 




/*****************************************/
/*                                       */
/*  Properties functions (Erik Axelson)  */
/*                                       */
/*****************************************/

/* Go through all nodes accessible from node node and set them visited
   by marking them with current vmark.
   If reverse is false, marker transitions are ignored.
   If reverse is true, only marker transitions are taken into account. */
void Transducer::traverse(Node *node, Character marker, bool reverse) {

  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      if ( (!reverse && arc->label().upper_char() != marker) ||
	   (reverse && arc->label().upper_char() == marker) )
	traverse(arc->target_node(), marker, reverse);
    }
  }
};

/* Whether the root node is accessible through node 'node'. */
bool Transducer::is_initial_cyclic_node(Node *node) {

  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      if ( arc->target_node() == root_node() )
	return true;
      if (is_initial_cyclic_node(arc->target_node()))
	return true;
    }
  }
  return false;
};

/* Calculate properties of the transducer. Modifies the transducer. */
properties Transducer::calculate_properties() {

  Character marker = 10000; // FIX THIS

  properties props;
  props.init();
  props.acceptor=true;
  props.deterministic=true;
  props.input_deterministic=true;
  props.output_deterministic=true;

  props.acyclic = !is_cyclic();
  incr_vmark();
  props.initial_cyclic = is_initial_cyclic_node(root_node());

  NodeNumbering index(*this);
  props.states = index.number_of_nodes();
  
  for (size_t n=0; n<props.states; n++) {
    Node *node = index.get_node(n);
    if (node->is_final())
      props.final_states++;

    Alphabet::LabelSet transitions;
    set<Key> ikeys;
    set<Key> okeys;

    for( ArcsIter p(node->arcs()); p; p++ ) {

      Arc *arc=p;
      Character ikey = arc->label().lower_char();
      Character okey = arc->label().upper_char();

      if (ikey != marker && okey != marker) {
	
      props.arcs++;

      if (props.deterministic && !deterministic) {  // not known if transducer is not deterministic
	if (transitions.find(arc->label()) != transitions.end())
	  props.deterministic = deterministic = false;
	else
	  transitions.insert(arc->label());
      }

      if (props.input_deterministic) {  // not known if transducer is not input deterministic
	if (ikeys.find(arc->label().lower_char()) != ikeys.end())
	  props.input_deterministic = false;
	else
	  ikeys.insert(arc->label().lower_char());
      }

      if (props.output_deterministic) {  // not known if transducer is not output deterministic
	if (okeys.find(arc->label().upper_char()) != okeys.end())
	  props.output_deterministic = false;
	else
	  okeys.insert(arc->label().upper_char());
      }

      if (ikey != okey)
	props.acceptor = false;
      if (ikey == Label::epsilon)
	props.iepsilons++;
      if (okey == Label::epsilon)
	props.oepsilons++;
      if (okey == Label::epsilon && ikey == Label::epsilon)
	props.ioepsilons++;

      if (props.ioepsilons > 0)
	props.deterministic = deterministic = false;

      // add a marker arc so transducer can be traversed
      // in reverse order
      Label l(marker, marker);
      arc->target_node()->add_arc( l, node, this );
      }
    }
  }

  deterministic = props.deterministic;
  props.minimised = minimised;

  props.has_iepsilons = (props.iepsilons > 0);
  props.has_oepsilons = (props.oepsilons > 0);
  props.has_ioepsilons = (props.ioepsilons > 0);

  incr_vmark();
  traverse(root_node(), marker); 

  bool accessible_states[(int)props.states];
  vector<int> final_states;
  for (size_t n=0; n<props.states; n++) {
    Node *node = index.get_node((int)n);
    accessible_states[(int)n] = node->was_visited(vmark);
    if (node->is_final())
      final_states.push_back((int)n);
  }

  incr_vmark();
  for(size_t n=0; n<final_states.size(); n++)
    traverse(index.get_node(final_states[n]), marker, true); 

  bool coaccessible_states[(int)props.states];
  for (size_t n=0; n<props.states; n++) {
    Node *node = index.get_node((int)n);
    coaccessible_states[(int)n] = node->was_visited(vmark);
  }

  for (unsigned int n=0; n<props.states; n++) {
    if (accessible_states[n])
      props.accessible_states++;
    if (coaccessible_states[n]) {
      props.coaccessible_states++;
      if (accessible_states[n])
	props.connected_states++;
    }
  }

  props.accessible = (props.accessible_states == props.states);
  props.coaccessible = (props.coaccessible_states == props.states);

  return props;

};


properties Transducer::get_properties() {
  properties props;
  props.init();
  props.minimised = minimised;
  props.deterministic = deterministic;
  props.acyclic = acyclic;
  return props;
};


void Transducer::set_properties(properties props) {
  minimised = props.minimised;
  deterministic = props.deterministic;
  acyclic = props.acyclic;
};


/*####################################################*/
/* TALLENNETAAN STANDARDIMUOTOA */
/* fst.C ja fst.h */
/*####################################################*/
//Added by Miikka Silfverberg
/*static void store_arc_label( ostream &out, Arc *arc )

{
  Label l=arc->label();
  Character lc=l.lower_char();
  Character uc=l.upper_char();
  out.write((char*)(&lc),sizeof(lc));
  out.write((char*)(&uc),sizeof(uc));
}

//Added by Miikka Silfverberg
static void store_node_info( ostream &out, Node *node )

{
  // write final flag
  char c=node->is_final();
  out.write(&c,sizeof(c));
  
  // write the number of arcs
  int nn = node->arcs()->size();
  if (nn > 65535)
    throw "Error: in function store_node\n";
  unsigned short n=(unsigned short)nn;
  out.write((char*)(&n),sizeof(n));
}

//Added by Miikka Silfverberg

static void store_node( ostream &out, Node *node, NodeNumbering &index, 
			long vmark )
{
  if (!node->was_visited( vmark )) {

    store_node_info( out, node );
  
    // write the arcs
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      store_arc_label( out, arc );
      unsigned int t=index[arc->target_node()];
      out.write((char*)(&t),sizeof(t));
      store_node(out, arc->target_node(), index, vmark );
    }
  }
}

*/

//Added by Miikka Silfverberg

void Transducer::store( ostream &out, bool alpha, bool prop ) {
  
  if (!alpha)
    out.put('A');
  if (prop) {
    out.put('P');
    out << (unsigned short)2; // length of the properties header in bytes
    properties props = get_properties();
    //fprintf(stderr, "storing following properties: \n");
    //props.print(cerr);
    props.write(out);
  }

  //Write identifier.
  out.put('a');
  NodeNumbering index(*this);
  incr_vmark();
  unsigned int n=index.number_of_nodes();
  out.write((char*)(&n),sizeof(n));
  store_node( out, root_node(), index, vmark );

  if (alpha)
    alphabet.store(out);
}


/*###################################################*/
/* TALLENNETAAN LOWMEM-MUOTO*/
/* fst.C ja fst.h*/
/*###################################################*/
//Added by Miikka Silfverberg
/*static void store_lowmem_node( ostream &out, Node *node, NodeNumbering &index,
			       vector<unsigned int> &startpos)
{
  store_node_info( out, node );
  
  // write the arcs
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *arc=p;
    store_arc_label( out, arc );
    unsigned int t=startpos[index[arc->target_node()]];
    out.write((char*)(&t),sizeof(t));
  }
  }

//Added by Miikka Silfverberg

void Transducer::store_lowmem( ostream &out )

{
  out.put('l');
  alphabet.store(out);

  // storing size of index table
  NodeNumbering index(*this);

  // compute the start position of the first node
  unsigned int pos=(unsigned int)out.tellp();
  vector<unsigned int> startpos;
  for( size_t i=0; i<index.number_of_nodes(); i++ ) {
    startpos.push_back(pos);
    Node *node=index.get_node(i);
    Arcs *arcs=node->arcs();
    pos += sizeof(char) // size of final flag
      + sizeof(unsigned short) // size of number of arcs
      + arcs->size() * (sizeof(Character) * 2 + sizeof(unsigned int)); // size of n arcs
  }

  // storing nodes
  for( size_t i=0; i<index.number_of_nodes(); i++ )
    store_lowmem_node( out, index.get_node(i), index, startpos );
}

*/


/* Change a line in AT&T text format to SFST text format. */

/*char *att2sfst(char *att, size_t line) {
  
  //fprintf(stderr, "att2sfst starts, input argument is '%s'.\n", att);

  char *p = att;

  char *s1 = next_string_null(p, line);  // originating state or final state
  char *s2 = next_string_null(p, line);  // destination state or optional final weight
  char *s3 = next_string_null(p, line);  // input symbol or NULL

  if (s1 == NULL) {
    fprintf(stderr, "ERROR: transducer text file has an empty line %i.\n", line);
    exit(1);
  }
  if (s2 == NULL || s3 == NULL) {
    //fprintf(stderr, "final state %s.", s1);
    string fin("final\t");
    fin.append(s1);
    char *result = new char[fin.length()+1];
    strcpy(result, fin.c_str());
    return result;
  }

  char *s4 = next_string_null(p, line);  // output symbol 
  //char *s5 = next_string_null(p, line);  // optional transition weight or NULL
  //fprintf(stderr, "from state %s to state %s with input '%s' and output '%s'.\n", s1, s2, s3, s4);
  string res("");
  res.append(s1);
  res.append("\t");
  res.append(s3);
  res.append(":");
  res.append(s4);
  res.append("\t");
  res.append(s2);
  char *result = new char[res.length()+1];
  strcpy(result, res.c_str());
  return result;
  }*/
