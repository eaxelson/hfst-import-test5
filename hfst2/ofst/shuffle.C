#include <iostream>
#include "shuffle.h"



namespace HWFST {

  typedef fst::StdArc::StateId StateId;
  fst::StdVectorFst *determinize_(fst::StdVectorFst *t);
  bool is_final(fst::StdVectorFst *t, StateId n);
  
  
  class PairMapping {
    // This class is used to map a node pair from two transducers
    // to a single node in another transducer
    
    typedef pair<StateId, StateId> NodePair;
    
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
    typedef hash_map<NodePair, StateId, hashf, equalf> PairMap;
    PairMap pm;
    
  public:
    typedef PairMap::iterator iterator;
    iterator begin() { return pm.begin(); };
    iterator end() { return pm.end(); };
    iterator find( StateId n1, StateId n2 )
    { return pm.find( NodePair(n1,n2) ); };
    StateId &operator[]( NodePair p ) { return pm.operator[](p); };
    
  };
  
  
  void shuffle_nodes( fst::StdVectorFst *t1, StateId n1,
		      fst::StdVectorFst *t2, StateId n2,
		      fst::StdVectorFst *result, StateId node, 
		      PairMapping &map ) {
    
    // fprintf(stderr, "\nshuffle_nodes: StateId n1: %hu  StateId n2: %hu  StateId node: %hu\n", n1, n2, node);

    // if both input nodes are final, so is the new one
    if (is_final(t1, n1) && is_final(t2, n2))
      result->SetFinal(node, 0);
    
    // iterate over all outgoing arcs of the first node
    for (fst::ArcIterator<fst::StdFst> aiter(*t1, n1); !aiter.Done(); aiter.Next()) {
      const fst::StdArc &arc = aiter.Value();
      StateId node1 = arc.nextstate;
      StateId node2 = n2;
      
      // Does the second node have an outgoing arc with the same label?
      if (1) {
	// Check whether this node pair has been encountered before
	PairMapping::iterator it=map.find(node1, node2);
	if (it == map.end()) {
	  // new node pair
	  // create a new node in the conjunction Transducer
	  StateId target_node = result->AddState();
	  // map the target node pair to the new node
	  map[pair<StateId,StateId>(node1,node2)] = target_node;
	  // add an arc to the new node
	  result->AddArc(node, fst::StdArc(arc.ilabel, arc.olabel, 0, target_node));  // WEIGHT?
	  // recursion
	  shuffle_nodes( t1, node1, t2, node2, result, target_node, map );
	}
	else {
	  // add an arc to the already existing target node 
	  result->AddArc(node, fst::StdArc(arc.ilabel, arc.olabel, 0, it->second));  // WEIGHT?
	}
      }
    }
    
    // iterate over all outgoing arcs of the second node
    for (fst::ArcIterator<fst::StdFst> aiter(*t2, n2); !aiter.Done(); aiter.Next()) {
      const fst::StdArc &arc = aiter.Value();
      StateId node2 = arc.nextstate;
      StateId node1 = n1;
      
      // Does the second node have an outgoing arc with the same label?
      if (1) {
	// Check whether this node pair has been encountered before
	PairMapping::iterator it=map.find(node1, node2);
	if (it == map.end()) {
	  // new node pair
	  // create a new node in the conjunction Transducer
	  StateId target_node = result->AddState();
	  // map the target node pair to the new node
	  map[pair<StateId,StateId>(node1,node2)] = target_node;
	  // add an arc to the new node
	  result->AddArc(node, fst::StdArc(arc.ilabel, arc.olabel, 0, target_node));  // WEIGHT?
	  // recursion
	  shuffle_nodes( t1, node1, t2, node2, result, target_node, map );
	}
	else {
	  // add an arc to the already existing target node 
	  result->AddArc(node, fst::StdArc(arc.ilabel, arc.olabel, 0, it->second));  // WEIGHT?
	}
      }
    }
  }
  
  
  //*************//
  //*  SHUFFLE  *//  
  //*************//
  
  fst::StdVectorFst *shuffle_( fst::StdVectorFst *t1, fst::StdVectorFst *t2  ) {
    
    /*StateId r1, r2;
    
    bool t1_deterministic=false;
    bool t2_deterministic=false;
    
    if (t1_deterministic)
      r1 = t1->Start();
    else {
      t1 = determinize_(t1);
      r1 = t1->Start();
    }
    
    if (t2_deterministic)
      r2 = t2->Start();
    else {
      t2 = determinize_(t2);
      r2 = t2->Start();
      }*/
    
    PairMapping map;
    
    fst::StdVectorFst *result = new fst::StdVectorFst;
    result->AddState();
    
    // recursively conjoin the two automata
    //shuffle_nodes( t1, r1, t2, r2, result, result->Start(), map);
    shuffle_nodes( t1, (unsigned)0, t2, (unsigned)0, result, (unsigned)0, map);
    
    // fprintf(stderr, "\n Returning the result.\n\n");
    return result;
  }
  
}

