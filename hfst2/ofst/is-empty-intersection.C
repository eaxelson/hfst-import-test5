#include "is-empty-intersection.h"

namespace HWFST {

  /* Coded by Miikka Silfverberg */

  struct NodePair {
    StateId first;
    StateId second; 
  };
  
  struct compare_NodePairs {
    bool operator() ( const NodePair &p1, const NodePair &p2 ) const {
      if ( p1.first == p2.first ) 
	return p1.second < p2.second;
      else 
	return p1.first < p2.first;
    }
  };
  
  struct SymPair {
    Label input;
    Label output;
  };

  struct compare_SymPairs {
    bool operator() (const SymPair &p1, const SymPair &p2) const {
      if (p1.input == p2.input) 
	return p1.output < p2.output;

      else
	return p1.input < p2.input;
    }

  };

  bool non_empty_intersection( fst::StdVectorFst * t1, StateId n1, 
			       fst::StdVectorFst * t2, StateId n2, 
			       set<NodePair,compare_NodePairs> &handled_nodes) {
    
    if ( (t1->Final( n1 ) == 0) and (t2->Final( n2 ) == 0) )
      return true;
    
    map<SymPair,StateId,compare_SymPairs> n1_labels;
    for ( fst::ArcIterator<fst::StdVectorFst> it(*t1,n1) ;
	  not it.Done();
	  it.Next() ) {
      const fst::StdArc arc = it.Value();
      SymPair s;
      s.input = arc.ilabel;
      s.output = arc.olabel;
      n1_labels[ s ] = arc.nextstate;
    }
 
    for ( fst::ArcIterator<fst::StdVectorFst> it(*t2,n2) ;
	  not it.Done();
	  it.Next() ) {      
      const fst::StdArc a = it.Value();
      SymPair s;
      s.input = a.ilabel;
      s.output = a.olabel;
      
      if ( n1_labels.find(s) == n1_labels.end() ) 
	continue;
      
      NodePair nodes;
      nodes.first = n1_labels[ s ];
      nodes.second = a.nextstate;
      
      if ( handled_nodes.find( nodes ) != handled_nodes.end() ) {
	continue;
      }
      
      handled_nodes.insert( nodes );
      
      if ( non_empty_intersection( t1, nodes.first, t2, nodes.second, handled_nodes ) )
	return true;
      
    }
    
    return false;
    
  };

  bool is_empty_intersection( fst::StdVectorFst *t1, fst::StdVectorFst *t2 ) {
    
    set<NodePair,compare_NodePairs> handled_nodes;

    NodePair root_nodes;
    root_nodes.first = t1->Start();
    root_nodes.second = t2->Start();

    handled_nodes.insert( root_nodes );

    return not non_empty_intersection( t1, t1->Start(), t2, t2->Start(), handled_nodes );
  }

}

