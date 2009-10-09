#include "fst.h"
#include "alphabet.h"
#include "binSearch.h"
#include <vector>
#include <string>
#include "parse_flag_diacritics.h"
#include <set>

class CompInfo;
class LexInfo;
class RulesInfo;
class StateInfo;

using namespace std;

typedef std::set<Character> FlagDiacriticSet;

class CompInfo {

 private:
  NodeNumbering * lex_numbering;
  vector<StateInfo*> * lexicon;

  FlagDiacriticSet * known_flag_diacritics;
  bool there_are_no_flag_diacritics;

  Node * target( Node * lex_node,  vector<Node*> * rule_nodes,bool &existed_already);
  Label compose_arcs( Arc * lex_arc,  Arc * rule_arc);

 public:
  Transducer * result;
  Node * current_comp_node;
  bool final;
   Alphabet * lexicon_alphabet;
   Alphabet * rules_alphabet;
  Alphabet * composition_alphabet;
  int number_of_nodes;
  int failing_rule_index;

  CompInfo (NodeNumbering * num, vector<StateInfo*> * lex, Transducer * res, 
	    Alphabet * lex_alphabet,  Alphabet * rules_alphabet,   FlagDiacriticSet * flag_diacritics);
  ~CompInfo( void ); 
  void compose(LexInfo * Lex, RulesInfo * Rules,  Arc * lex_arc,  Arc * rule_arc);
  bool is_flag_diacritic(Character c)
  {
    return there_are_no_flag_diacritics or 
      (known_flag_diacritics->find(c) != known_flag_diacritics->end()); 
  }
};

class RulesInfo {

 public:
  vector<Node*> * nodes;
  Arcs * first_rule_arcs;
  bool finals;

  RulesInfo( vector<Node*> * rule_nodes, bool rules_final );
  vector<Node*> * targets( Arc * arc, bool &finals, int &failing_rule_index);
};
  
class LexInfo {

 public:
  Node * node;
   Arcs * arcs;
  bool final;

  LexInfo(Node * n, bool final);
  LexInfo(Node * n);
};

class StateInfo {

 public:
   vector<vector<Node*>*> * rules;
   vector<Node*> * comp_nodes;

  StateInfo( void );
  ~StateInfo( void );
  Node * has_comp_state( vector<Node*> * rule_nodes,bool &existed_already, Transducer * result);
};

void isect_compose( CompInfo * Comp, LexInfo * Lex, RulesInfo * Rules);
Alphabet *  make_alphabet( const Alphabet &sigma1, const Alphabet &sigma2 ); 
Transducer * intersecting_composition(  Transducer &lexicon, 
					vector<Transducer*> * rules,
					FlagDiacriticSet * known_flag_diacritics = NULL, 
					bool minimise = true, 
					vector<char*> * rule_names = NULL, 
					bool verbose = false);
