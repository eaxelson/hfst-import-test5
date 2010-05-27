/*******************************************************************/
/*                                                                 */
/*  FILE     expand.C                                              */
/*  MODULE   expand                                                */
/*  PROGRAM  HFST/SFST                                             */
/*  AUTHOR   The HFST project, University of Helsinki              */
/*                                                                 */
/*******************************************************************/


#include "fst.h"

namespace SFST 
{

  /* Expand a transition according to the previously unknown symbols in s. */
  void Transducer::expand_node( Node *origin, Label &l, Node *target, Transducer *a, hfst::StringSet &s )
{
  if ( l.lower_char() == 1 && l.upper_char() == 1 )     // cross product "?:?"
    {
      for (hfst::StringSet::iterator it1 = s.begin(); it1 != s.end(); it1++) 
	{
	  int inumber = alphabet.symbol2code(it1->c_str());
	  for (hfst::StringSet::iterator it2 = s.begin(); it2 != s.end(); it2++) 
	    {
	      int onumber = alphabet.symbol2code(it2->c_str());
	      if (inumber != onumber) {  
		// add transitions of type x:y (non-identity cross-product of symbols in s)
		origin->add_arc( Label(inumber, onumber), target, a );
	      }
	    }
	  // add transitions of type x:? and ?:x here
	  origin->add_arc( Label(inumber, 1), target, a );
	  origin->add_arc( Label(1, inumber), target, a );
	}
    }
  else if (l.lower_char() == 2 && l.upper_char() == 2 )  // identity "?:?"	     
    {
      for (hfst::StringSet::iterator it = s.begin(); it != s.end(); it++) 
	{
	  int number = alphabet.symbol2code(it->c_str());
	  // add transitions of type x:x
	  origin->add_arc( Label(number, number), target, a );
	}
    }
  else if (l.lower_char() == 1)  // "?:x"
    {
      for (hfst::StringSet::iterator it = s.begin(); it != s.end(); it++) 
	{
	  int number = alphabet.symbol2code(it->c_str());
	  origin->add_arc( Label(number, l.upper_char()), target, a );
	}
    }
  else if (l.upper_char() == 1)  // "x:?"
    {
      for (hfst::StringSet::iterator it = s.begin(); it != s.end(); it++) 
	{
	  int number = alphabet.symbol2code(it->c_str());
	  origin->add_arc( Label(l.lower_char(), number), target, a );
	}
    }  
  // add the original transition in all cases
  origin->add_arc( l, target, a );
  return;
}



/*******************************************************************/
/*                                                                 */
/*  HFST addition                                                  */
/*  Transducer::expand_nodes                                       */
/*                                                                 */
/*******************************************************************/

  Node *Transducer::expand_nodes( Node *node, Transducer *a, hfst::StringSet &s )
{
  if (!node->was_visited(vmark)) {
    
    node->set_forward(a->new_node());

    // define final nodes
    if (node->is_final())
      node->forward()->set_final(1);

    // iterate over all outgoing arcs of node
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *arc=p;
      Node *tn = expand_nodes( arc->target_node(), a, s);

      // Add a link to the new node
      Label l=arc->label();

      expand_node(node->forward(), l, tn, a, s);
    }
  }
  return node->forward();
}


/*******************************************************************/
/*                                                                 */
/*  HFST addition                                                  */
/*  Transducer::expand                                             */
/*                                                                 */
/*******************************************************************/

  /* Make a copy of this transducer where all transitions are expanded
     according to the previously unknown symbols listed in s. */
  Transducer &Transducer::expand(hfst::StringSet &s)

{
  Transducer *na = new Transducer();
  na->alphabet.copy(alphabet);

  na->deterministic = deterministic;
  na->minimised = minimised;
  na->root_node()->set_final(root_node()->is_final());
  incr_vmark();

  root_node()->set_forward(na->root_node());
  root_node()->was_visited(vmark);

  for( ArcsIter p(root_node()->arcs()); p; p++ ) {
    Arc *arc=p;
    Node *target_node=expand_nodes(arc->target_node(), na, s);
    Label l = arc->label();

    expand_node(na->root_node(), l, target_node, na, s);
  }
  return *na;
}


}

