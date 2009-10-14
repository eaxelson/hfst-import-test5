#ifndef _FST_H_
#define _FST_H_

#include "alphabet.h"

/*******************************************************************/
/* include commands                                                */
/*******************************************************************/

using std::cin;
using std::cout;
using std::cerr;

#include <string>
using std::string;

#include <vector>
using std::vector;

// added by Erik Axelson
#include <map>
using std::map;

#include <bitset>
using std::bitset;

using std::pair;
using std::sort;

#include "mem.h"

typedef unsigned short VType;

extern int Quiet;

// HFST code
const Character ANYKEY=1;

class Node;
class Arc;
class Arcs;
class Transducer;

struct hashf {
  size_t operator()(const Node *n) const { return (size_t) n; }
};
struct equalf {
  int operator()(const Node *n1, const Node *n2) const { return n1==n2; }
};
typedef hash_set<Node*, hashf, equalf> NodeHashSet;

// ADDED
void print_operator(ostream &s, Transducer &a, Alphabet &alpha);
void print_operator_number(ostream &s, Transducer &a );
//char *att2sfst(char *att, size_t line);

char * to_string( Transducer&, Alphabet&, bool spaces=false );
// added by Erik Axelson: turns a string to a transducer
// the string is of type "a:b c:delta"
Transducer * to_transducer( const char *str, Alphabet &alpha );

struct properties {

  size_t states;
  size_t arcs;
  size_t final_states;
  bool acceptor;
  size_t ioepsilons;
  size_t iepsilons;
  size_t oepsilons;

  bool has_ioepsilons;
  bool has_iepsilons;
  bool has_oepsilons;

  size_t accessible_states;
  size_t coaccessible_states;
  size_t connected_states;
  bool acyclic;
  bool initial_cyclic;

  bool accessible;
  bool coaccessible;
  
  bool minimised;
  bool deterministic;
  bool input_deterministic;
  bool output_deterministic;

  void init() {
    states=0;
    arcs=0;
    final_states=0;
    acceptor=false;
    ioepsilons=0;
    iepsilons=0;
    oepsilons=0;

    has_ioepsilons=false;
    has_iepsilons=false;
    has_oepsilons=false;

    accessible_states=0;
    coaccessible_states=0;
    connected_states=0;
    acyclic=false;
    initial_cyclic=false;

    accessible=false;
    coaccessible=false;

    deterministic=false;
    minimised=false;
    input_deterministic=false;
    output_deterministic=false;
  }

  void print(ostream &os) {
    os << "# of states                     " << (int)states << "\n";
    os << "# of arcs                       " << (int)arcs << "\n";
    os << "# of final states               " << (int)final_states << "\n";
    os << "# of input/output epsilons      " << (int)ioepsilons << "\n";
    os << "# of input epsilons             " << (int)iepsilons << "\n";
    os << "# of output epsilons            " << (int)oepsilons << "\n";

    os << "# of accessible states          " << (int)accessible_states << "\n";
    os << "# of coaccessible states        " << (int)coaccessible_states << "\n";
    os << "# of connected states           " << (int)connected_states << "\n";

    os << "acceptor                        ";
    if (acceptor) os << "y\n";
    else os << "n\n";
    os << "input deterministic             ";
    if (input_deterministic) os << "y\n";
    else os << "n\n";  
    os << "output deterministic            ";
    if (output_deterministic) os << "y\n";
    else os << "n\n";  

    os << "input/output epsilons           ";
    if (has_ioepsilons) os << "y\n";
    else os << "n\n";
    os << "input epsilons                  ";
    if (has_iepsilons) os << "y\n";
    else os << "n\n";
    os << "output epsilons                 ";
    if (has_oepsilons) os << "y\n";
    else os << "n\n";

    os << "cyclic                          ";
    if (acyclic) os << "n\n";
    else os << "y\n";  
    os << "cyclic at initial state         ";
    if (initial_cyclic) os << "y\n";
    else os << "n\n";

    os << "accessible                      ";
    if (accessible) os << "y\n";
    else os << "n\n";
    os << "coaccessible                    ";
    if (coaccessible) os << "y\n";
    else os << "n\n";

    os << "deterministic                   ";
    if (deterministic) os << "y\n";
    else os << "n\n";  
    os << "minimised                       ";
    if (minimised) os << "y\n";
    else os << "n\n";  
  }

  void write(ostream &os) {
    unsigned int byte1=0;
    unsigned int byte2=0;

    if (acceptor) byte1 =+ 128;
    if (input_deterministic) byte1 =+ 64;
    if (output_deterministic) byte1 =+ 32;
    if (has_ioepsilons) byte1 =+ 16;
    if (has_iepsilons) byte1 =+ 8;
    if (has_oepsilons) byte1 =+ 4;
    if (acyclic) byte1 =+ 2;
    if (initial_cyclic) byte1 =+ 1;

    if (accessible) byte2 =+ 128;
    if (coaccessible) byte2 =+ 64;
    if (deterministic) byte2 =+ 32;
    if (minimised) byte2 =+ 16; 

    os << (char)byte1 << (char)byte2;
    /*bitset<8> byte1(string("00000000"));
    bitset<8> byte2(string("00000000"));

    if (acceptor) byte1[7] = 1;
    if (input_deterministic) byte1[6] = 1;
    if (output_deterministic) byte1[5] = 1;
    if (has_ioepsilons) byte1[4] = 1;
    if (has_iepsilons) byte1[3] = 1;
    if (has_oepsilons) byte1[2] = 1;
    if (acyclic) byte1[1] = 1;
    if (initial_cyclic) byte1[0] = 1;

    if (accessible) byte2[7] = 1;
    if (coaccessible) byte2[6] = 1;
    if (deterministic) byte2[5] = 1;
    if (minimised) byte2[4] = 1;

    os << byte1;
    os << byte2;*/
  }

  void read(istream &is) {

    unsigned int byte1=(unsigned int)is.get(); 
    unsigned int byte2=(unsigned int)is.get();

    if (byte1%2 == 1) initial_cyclic=true;
    byte1 = byte1 >> 1;
    if (byte1%2 == 1) acyclic=true;
    byte1 = byte1 >> 1;
    if (byte1%2 == 1) has_oepsilons=true;
    byte1 = byte1 >> 1;
    if (byte1%2 == 1) has_iepsilons=true;
    byte1 = byte1 >> 1;
    if (byte1%2 == 1) has_ioepsilons=true;
    byte1 = byte1 >> 1;
    if (byte1%2 == 1) output_deterministic=true;
    byte1 = byte1 >> 1;
    if (byte1%2 == 1) input_deterministic=true;
    byte1 = byte1 >> 1;
    if (byte1%2 == 1) acceptor=true;

    byte2 = byte2 >> 4;
    if (byte2%2 == 1) minimised=true; 
    byte2 = byte2 >> 1;
    if (byte2%2 == 1) deterministic=true;
    byte2 = byte2 >> 1;
    if (byte2%2 == 1) coaccessible=true;
    byte2 = byte2 >> 1;
    if (byte2%2 == 1) accessible=true;

    /*bitset<8> byte1;
    bitset<8> byte2;
    
    is >> byte1;
    is >> byte2;

    if (byte1[0]) initial_cyclic=true;
    if (byte1[1]) acyclic=true;
    if (byte1[2]) has_oepsilons=true;
    if (byte1[3]) has_iepsilons=true;
    if (byte1[4]) has_ioepsilons=true;
    if (byte1[5]) output_deterministic=true;
    if (byte1[6]) input_deterministic=true;
    if (byte1[7]) acceptor=true;

    if (byte2[4]) minimised=true;
    if (byte2[5]) deterministic=true;
    if (byte2[6]) coaccessible=true;
    if (byte2[7]) accessible=true;*/
  }

};



/*****************  class Arc  *************************************/

class Arc {

 private:
  Label l;
  Node *target;
  Arc *next;

 public:
  void init( Label ll, Node *node ) { l=ll; target=node; };
  Label label( void ) const { return l; };
  Node *target_node( void ) { return target; };
  const Node *target_node( void ) const { return target; };
  void set_label( Label ll ) { l=ll; }; // ADDED
  void print() { fprintf(stderr, "Label is %hu:%hu\n", l.lower_char(), l.upper_char()); } ;

  friend class Arcs;
  friend class ArcsIter;
};


/*****************  class Arcs  ************************************/

class Arcs {

 private:
  Arc *first_arcp;
  Arc *first_epsilon_arcp;

 public:
  void init() { first_arcp = first_epsilon_arcp = NULL; };
  Arcs() { init(); };
  Node *target_node( Label l );
  Node *target_node_filter( Label l );  // HFST code
  const Node *target_node( Label l ) const;
  void add_arc( Label, Node*, Transducer* );
  int remove_arc( Arc* );
  bool is_empty() const { return !(first_arcp || first_epsilon_arcp); };
  bool epsilon_transition_exists() { return first_epsilon_arcp != NULL; };
  bool non_epsilon_transition_exists() { return first_arcp != NULL; };
  int size() const;

  friend class ArcsIter;
};


/*****************  class ArcsIter  ********************************/

class ArcsIter {

  // ArcsIter iterates over the arcs starting with epsilon arcs

 private:
  Arc *current_arcp;
  Arc *more_arcs;

 public:
  typedef enum {all,non_eps,eps}  IterType;

  ArcsIter( Arcs *arcs, IterType type=all ) {
    more_arcs = NULL;
    if (type == all) {
      if (arcs->first_epsilon_arcp) {
	current_arcp = arcs->first_epsilon_arcp;
	more_arcs = arcs->first_arcp;
      }
      else
	current_arcp = arcs->first_arcp;
    }
    else if (type == non_eps)
      current_arcp = arcs->first_arcp;
    else
      current_arcp = arcs->first_epsilon_arcp;
  };

  ArcsIter( const Arcs *arcs, IterType type=all ) {
    more_arcs = NULL;
    if (type == all) {
      if (arcs->first_epsilon_arcp) {
	current_arcp = arcs->first_epsilon_arcp;
	more_arcs = arcs->first_arcp;
      }
      else
	current_arcp = arcs->first_arcp;
    }
    else if (type == non_eps)
      current_arcp = arcs->first_arcp;
    else
      current_arcp = arcs->first_epsilon_arcp;
  };
  
  void operator++( int ) {
    if (current_arcp) {
      current_arcp = current_arcp->next;
      if (!current_arcp && more_arcs) {
	current_arcp = more_arcs;
	more_arcs = NULL;
      }
    }
  };
  operator Arc*() { return current_arcp; };
  
};


/*****************  class Node  ************************************/

class Node {

 private:
  bool final;
  VType visited;
  Arcs arcsp;
  Node *forwardp;
  //bool marked;

 public:
  Node( void ) { init(); };
  void init( void );
  bool is_final( void ) const { return final; };
  void set_final( bool flag ) { final = flag; };
  void set_forward( Node *node ) { forwardp = node; };
  const Node *target_node( Label l ) const { return arcs()->target_node(l); };
  Node *target_node( Label l ) { return arcs()->target_node(l); };
  Node *target_node_filter( Label l ) { return arcs()->target_node_filter(l); };
  void add_arc( Label l, Node *n, Transducer *a ) { arcs()->add_arc(l, n, a); };
  Arcs *arcs( void ) { return &arcsp; };
  const Arcs *arcs( void ) const { return &arcsp; };
  Node *forward( void ) { return forwardp; };
  bool was_visited( VType vmark ) {
    if (visited == vmark)
      return true;
    visited = vmark;
    return false;
  };
  bool check_visited( VType vm ) // leaves the visited flag unchanged
    { return (visited==vm); };
  void set_marked() {} //{ marked=true; };
  void unset_marked() {} // { marked=false; };
  bool is_marked() {return false;} // { return marked; };
};


/*****************  class Node2Int *********************************/

class Node2Int {

  struct hashf {
    size_t operator()(const Node *node) const { 
      return (size_t)node;
    }
  };
  struct equalf {
    int operator()(const Node *n1, const Node *n2) const {
      return (n1 == n2);
    }
  };
  typedef hash_map<Node*, int, hashf, equalf> NL;

 private:
  int current_number;
  NL number;

public:
  int &operator[]( Node *node ) {
    NL::iterator it=number.find(node);
    if (it == number.end())
      return number.insert(NL::value_type(node, 0)).first->second;
    return it->second;
  };
};




/*****************  class NodeNumbering ****************************/

class NodeNumbering {

 private:
  vector<Node*> nodes;
  Node2Int nummap;
  void number_node( Node*, Transducer& );

 public:
  NodeNumbering( Transducer& );
  int operator[]( Node *node ) { return nummap[node]; };
  size_t number_of_nodes() { return nodes.size(); };
  Node *get_node( size_t n ) { return nodes[n]; };
};


/*****************  class PairMapping  ****************************/

class PairMapping {
  // This class is used to map a node pair from two transducers
  // to a single node in another transducer

  typedef pair<Node*, Node*> NodePair;

 private:
  struct hashf {
    size_t operator()(const NodePair p) const { 
      return (size_t)p.first ^ (size_t)p.second;
    }
  };
  struct equalf {
    int operator()(const NodePair p1, const NodePair p2) const {
      return (p1.first==p2.first && p1.second == p2.second);
    }
  };
  typedef hash_map<NodePair, Node*, hashf, equalf> PairMap;
  PairMap pm;
  
 public:
  typedef PairMap::iterator iterator;
  iterator begin() { return pm.begin(); };
  iterator end() { return pm.end(); };
  iterator find( Node *n1, Node *n2 )
    { return pm.find( NodePair(n1,n2) ); };
  Node* &operator[]( NodePair p ) { return pm.operator[](p); };
  
};


//class Node2Node;

/*****************  class Transducer  *******************************/

class Transducer {

 private:
  bool deterministic;
  bool minimised;
  bool acyclic;
  //VType vmark;
  Node root;
  Mem mem;

  typedef set<Label, Label::label_cmp> LabelSet;
  typedef hash_map<Character, char*> SymbolMap;

  /*void incr_vmark() {
    if (++vmark == 0)
      throw "Overflow of generation counter!";
      };*/

  void reverse_node( Node *old_node, Transducer *new_node );
  Label recode_label( Label, bool lswitch, bool recode, Alphabet& );
  Node *copy_nodes( Node *n, Transducer *a, 
		    bool lswitch=false, bool recode=false );
  void rec_cat_nodes( Node*, Node*, bool preserve_final_states=false );
  bool productive_node( Node* );
  bool prune_nodes( Node* );
  void negate_nodes( Node*, Node* );
  bool compare_nodes( Node *node, Node *node2, Transducer &a2 );
  void map_nodes( Node *node, Node *node2, Transducer *a, Level level );
  void freely_insert_at_node( Node *node, Label l );
  int print_strings_node(Node *node, char *buffer, int pos, FILE *file, bool);
  bool infinitely_ambiguous_node( Node*, bool lower=false );
  bool is_cyclic_node( Node*, NodeHashSet &visited );
  bool is_automaton_node( Node* );
  bool generate1( Node*, Node2Int&, char*, int, char*, int, FILE* );
  void store_symbols( Node*, SymbolMap&, LabelSet& );

  void splice_nodes(Node*, Node*, Label sl, Transducer*, Transducer*);
  void splice_arc( Node*, Node*, Node*, Transducer* );
  void enumerate_paths_node( Node*, vector<Label>&, NodeHashSet&, 
			     vector<Transducer*>& );
  // added by Erik Axelson
  void enumerate_paths_node( Node*, vector<Label>&, NodeHashSet&, 
			     vector<vector<Label> >& );
  void replace_char2( Node*, Node*, Character, Character, Transducer*, bool ignore_epsilon_pairs=false );
  void replace_char2( Node*, Node*, KeySet*, Character, Transducer* );
  
  Node *create_node( vector<Node*>&, char*, size_t line );

  void read_transducer_binary( FILE*, bool alpha=true );
  void read_transducer_binary( istream&, bool alpha=true ); // Added by Miikka Silfverberg
  //void read_transducer_text( istream& is, bool att, bool numbers=false );      // bool parameter ADDED

  void read_transducer_text_old( istream& is, KeyTable *T );
  void att_to_transducer( FILE *file );


  void copy_nodes( Node *search_node, Transducer *copy_tr,
		   Node *start_node,
		   NodeNumbering &nn, map<int, Node*> &mapper );
  //void copy_epsilon_nodes(Node*, Transducer *rm, Node2Node&, NodeNumbering&);
  //void copy_epsilon_nodes(Node*, Node*, Transducer *rm, Node2Node&, NodeNumbering&);
  void duplicate_node(Node * node_in_this_transducer, 
		      Node * node_in_another_transducer,
		      Node * target_node_of_transducer_transition, 
		      Transducer * another_transducer,
		      map<Node*,Node*> &node_mapping);
 public:

  // CHANGED
  VType vmark;
  void incr_vmark() {
    if (++vmark == 0)
      throw "Overflow of generation counter!";
  };

  Alphabet alphabet; // The set of all labels, i.e. character pairs

  Transducer( void ) : root(), mem()
    { vmark = 0; deterministic = minimised = acyclic = false; };
  // convertion of a string to an transducer
  Transducer( char *s, const Alphabet *a=NULL, bool extended=false );
  // reads a word list from a file and stores it in the transducer
  Transducer( istream&, const Alphabet *a=NULL, bool verbose=false );
  // reads a transducer from a binary or text file
  Transducer( istream& is, bool binary=true, bool att=false, bool alpha=true, bool numbers=false, KeyTable *T=NULL );  // CONFLICT ???
  // turns a sequence of labels into a transducer
  Transducer( vector<Label>& );
  
  void read_transducer_text_new( istream& is, bool numbers, KeyTable *T, Alphabet& alpha );      // bool parameter ADDED

  void read_stream( istream& in, bool alpha=true ); // Added by Miikka Silfverberg
  Node *root_node() { return &root; };  // returns the root node
  const Node *root_node() const { return &root; };  // returns the root node
  Node *new_node();                // memory alocation for a new node
  Arc *new_arc( Label l, Node *target ); // memory alocation for a new arc
  void add_string( char *s, bool extended=false );
  void add_path( vector<Label> ); // added by Erik Axelson
  void complete_alphabet();
  // ADDED
  set<Key> define_key_set();
  set<Key> *define_key_set_pointer();

  // added by Erik Axelson
  Node *create_node( vector<Node*> &node, long n );

  void keys_to_alphabet();
  void minimise_alphabet();
  void prune();  // remove unnecessary arcs

  // added by Erik Axelson
  void harmonize_node(Node *node, long vmark, KeyTable *old_table, KeyTable *new_table, Alphabet& alpha);
  void harmonize(KeyTable *old_table, KeyTable *new_table, Alphabet& alpha);

  int print_strings( FILE*, bool with_brackets=true ); //enumerate all strings

  bool analyze_string( char *s, FILE *file, bool with_brackets=true );
  bool generate_string( char *s, FILE *file, bool with_brackets=true );
  bool generate( FILE *file, bool separate=false );

  void clear();      // clears the transducer. The resulting transducer
                     // is like one created with Transducer()
  // copy duplicates an transducer
  // if called with a non-zero argument, upper and lower level are switched
  Transducer &copy( bool lswitch=false, const Alphabet *al=NULL );
  Transducer &switch_levels() { return copy( true ); };
  Transducer &splice( Label l, Transducer *a);
  Transducer &freely_insert( Label l );
  Transducer &replace_char( Character c, Character nc, bool ignore_epsilon_pairs=false );
  Transducer &replace_char( KeySet *ks, Character nc );
  Transducer &level( Level );
  Transducer &lower_level()   // creates an transducer for the "lower" language
    { return level(lower); };
  Transducer &upper_level()   // creates an transducer for the "upper" language
    { return level(upper); };
  Transducer &remove_epsilons();  // by Erik Axelson
  Transducer &determinise();  // creates a deterministic transducer
  Transducer &minimise( bool verbose=true ); // creates a minimised transducer
  void store( FILE*, bool alpha=true );       // stores the transducer in binary format  (ADDED: bool parameter)
  void store( ostream& out, bool alpha=false, bool prop=true );       //Added by Miikka Silfverberg
  void store_lowmem( FILE* );
  void store_lowmem( ostream& ); //Added by Miikka Silfverberg
  void read( FILE* );        // reads an transducer in binary format
  bool enumerate_paths( vector<Transducer*>& );
  bool enumerate_paths( vector<vector<Label> >&); // added by Erik Axelson

  Transducer &reverse();                  // reverse language
  Transducer &operator|( Transducer& );   // union, disjunction
  Transducer &operator+( Transducer& );   // concatenation
  Transducer &operator/( Transducer& );   // subtraction
  Transducer &operator&( Transducer& );   // intersection, conjunction
  Transducer &filter_epsilon_paths( Transducer & ); // HFST code
  Transducer &operator&&( Transducer& );  // shuffle 
  Transducer &operator||( Transducer& );  // composition
  Transducer &operator!();                // complement, negation
  Transducer &subtraction_negation();
  Transducer &kleene_star();
  bool operator==( Transducer& ); // minimises its arguments first

  bool is_cyclic();
  bool is_initial_cyclic_node(Node*);
  bool is_automaton();
  
  // Added by Erik Axelson
  bool is_deterministic(bool omit_root_node=false);
  bool is_minimised();

  bool is_infinitely_ambiguous(bool lower=true);
  bool is_empty();		 // For efficiency reasons, these functions
  bool generates_empty_string(); // are better called after minimisation

  // Added by Erik Axelson
  properties get_properties();
  properties calculate_properties();
  void set_properties(properties props);
  void traverse(Node *node, Character marker, bool reverse=false);

  // Added by Miikka Silfverberg
  /*void store( ostream &out );*/
  /*static void store_node( ostream &out, Node *node, NodeNumbering &index, 
    long vmark );
    static void store_arc_label( ostream &out, Arc *arc );*/
    
  /*void store_lowmem( ostream &out );*/
  /*  static void store_lowmem_node( ostream &out, Node *node, NodeNumbering &index,
      vector<unsigned int> &startpos);*/
  
  void add_as_transition(Node * node_in_another_transducer, 
			 Node * target_node_in_another_transducer,
			 Transducer * another_transducer );

  friend class NodeNumbering;
  friend class EdgeCount;
  friend class MakeCompactTransducer;
  friend ostream &operator<<(ostream&, Transducer&);
};


/*****************  class Node2Node  *******************************/
// By Erik Axelson

class Node2Node {

  struct hashf {
    size_t operator()(const Node *node) const {
      return (size_t)node; 
    }
  };
  struct equalf {
    int operator()(const Node *n1, const Node *n2) const {
      return (n1 == n2);
    }
  };

  typedef hash_map<Node*, Node*, hashf, equalf> NN;

 private:
  NN Eps2Rm;
  NN Rm2Eps;

 public:
  const Node *eps2rm( Node *node, Transducer *rm ) {
    NN::iterator it=Eps2Rm.find(node);
    if (it == Eps2Rm.end()) {
      Node *n = rm->new_node();
      Eps2Rm[node] = n;
      return n;
    }
    return it->second;
  };
  const Node *rm2eps( Node *node ) {
    NN::iterator it=Eps2Rm.find(node);
    return it->second;
  };
  Node2Node( Node *eps_root, Node *rm_root ) {
    Eps2Rm[eps_root] = rm_root;
    Rm2Eps[rm_root] = eps_root;
  };

};

#endif
