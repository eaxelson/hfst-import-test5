#include "lookup.h"

namespace HFST {

KeyVectorVector * reverse( KeyVectorVector * key_strings ) {
  if ( key_strings == NULL )
    return NULL;

  KeyVectorVector * reversed_key_strings = new KeyVectorVector;

  for ( KeyVectorVector::iterator it = key_strings->begin();
	it != key_strings->end();
	++it ) {
    KeyVector * reversed_key_string = new KeyVector;
    for ( KeyVector::reverse_iterator rit = (*it)->rbegin();
	  rit != (*it)->rend();
	  ++rit ) {
      reversed_key_string->push_back(*rit);
    }
    reversed_key_strings->push_back(reversed_key_string);
    delete (*it);
  }
  delete key_strings;
  return reversed_key_strings;
}

  KeyVector * reverse( KeyVector * key_string ) {
    if ( key_string == NULL )
      return NULL;
    KeyVector * reversed_key_string = new KeyVector;
    for ( KeyVector::reverse_iterator rit = key_string->rbegin();
	  rit != key_string->rend();
	  ++rit ) {
      reversed_key_string->push_back(*rit);
    }
    delete key_string;
    return reversed_key_string;
  }
   
  void add_prefix(Key k, KeyVectorVector * suffixes) {

    if ( suffixes->empty() ) {
      suffixes->push_back(new KeyVector);
      suffixes->at(0)->push_back(k);
      return;
    }

    for (KeyVectorVector::iterator it = suffixes->begin();
	 it != suffixes->end();
	 ++it ) {
      (*it)->push_back(k);
    }
  }

  void add_to_continuations(KeyVectorVector * continuations,
			    KeyVectorVector * suffixes) {
    for ( KeyVectorVector::iterator it = suffixes->begin();
	  it != suffixes->end();
	  ++it ) {
      continuations->push_back(*it);
    }
    delete suffixes;
  }

  KeyVector * find_first_continuation(Node * n, 
				      KeyVector::iterator input_position, 
				      KeyVector::iterator input_end_position,
				      NodeSet &previous_nodes_at_zero_distance,
				      bool &succeeded,
				      bool preserve_epsilons=false) {
    previous_nodes_at_zero_distance.insert(n);
    KeyVector * result = NULL;
    if ( (input_position == input_end_position) ) {
      if ( n->is_final() ) {
	succeeded = true;
	return new KeyVector;
      }
      for( ArcsIter arcs(n->arcs());
	   arcs;
	   arcs++ ) {
	Arc a = *arcs;
	if ( a.label().lower_char() == 0 ) {
	  if (previous_nodes_at_zero_distance.find(a.target_node()) ==
	      previous_nodes_at_zero_distance.end())
	    {
	      previous_nodes_at_zero_distance.insert(a.target_node());
	      result = find_first_continuation(a.target_node(),
					       input_position,
					       input_end_position,
					       previous_nodes_at_zero_distance,
					       succeeded);
	      if ( succeeded ){
		if ( (a.label().upper_char() != 0) or
		     preserve_epsilons)
		  result->push_back(a.label().upper_char());
		return result;
	      }
	      previous_nodes_at_zero_distance.erase(a.target_node());
	    }
	}	  
      }
    }
    else
      {
	for ( ArcsIter arcs(n->arcs());
	      arcs;
	      arcs++ ) {
	  Arc a = *arcs;
	  if ( a.label().lower_char() == 0 ) {
	    if (previous_nodes_at_zero_distance.find(a.target_node()) ==
		previous_nodes_at_zero_distance.end())
	      {
		previous_nodes_at_zero_distance.insert(a.target_node());
		result = find_first_continuation(a.target_node(),
						 input_position,
						 input_end_position,
						 previous_nodes_at_zero_distance,
						 succeeded);
		if ( succeeded ) {
		  if ( (a.label().upper_char() != 0) or
		       preserve_epsilons)
		    result->push_back(a.label().upper_char());
		  return result;
		}
		previous_nodes_at_zero_distance.erase(a.target_node());
	  }
	  }
	  else if ( a.label().lower_char() == *input_position ) {
	    NodeSet new_node_set;
	    result = find_first_continuation(a.target_node(),
					     input_position+1,
					     input_end_position,
					 new_node_set,
					     succeeded);
	    if ( succeeded ) {
	      if ( (a.label().upper_char() != 0) or
		   preserve_epsilons)
		result->push_back(a.label().upper_char());
	      return result;
	    }
	  }
	}
      }
    return result;
  }


  KeyVectorVector * 
  find_all_continuations(Node * n, 
			 KeyVector::iterator input_position, 
			 KeyVector::iterator input_end_position,
			 KeySet * skip_symbols,
			 bool preserve_epsilons=false) {
    
    KeyVectorVector * continuations = NULL;
    
    if ( input_position == input_end_position ) {
      for ( ArcsIter arcs(n->arcs()); arcs; arcs++ ) {
	Arc a = *arcs;
	if ( (a.label().lower_char() == 0))
	  {
	    KeyVectorVector * suffixes = 
	      find_all_continuations(a.target_node(),input_position,
				     input_end_position,skip_symbols );
	    if ( suffixes == NULL )
	      continue;
	    else if ( continuations == NULL ) {
	      continuations = new KeyVectorVector;
	    }
	    add_prefix(a.label().upper_char(),suffixes);
	    add_to_continuations(continuations,
				 suffixes);
	  }
	else if ( (skip_symbols->find(a.label().lower_char()) != 
		   skip_symbols->end())) {
	  KeyVectorVector * suffixes = 
	    find_all_continuations(a.target_node(),input_position,
				   input_end_position,skip_symbols );
	  if ( suffixes == NULL )
	    continue;
	  else if ( continuations == NULL ) {
	    continuations = new KeyVectorVector;
	  }
	  if ((skip_symbols->find(a.label().upper_char()) != 
	       skip_symbols->end()))
	    {
	      add_prefix(a.label().lower_char(),suffixes);
	    }
	  else
	    {
	      add_prefix(a.label().upper_char(),suffixes);
	      add_prefix(a.label().lower_char(),suffixes);
	    }
	  add_to_continuations(continuations,
			       suffixes);
	}
      }
      if ( not n->is_final() )
	return continuations;
      else {
	if  ( continuations == NULL ) {
	  continuations = new KeyVectorVector;
	  continuations->push_back( new KeyVector );
	  return continuations;
	}
	else {
	  continuations->push_back( new KeyVector );
	  return continuations;
	}
      }
    }
    
    for ( ArcsIter arcs(n->arcs()); arcs; arcs++ ) {
      Arc a = *arcs;
      if ( a.label().lower_char() == *input_position) {
	KeyVectorVector * suffixes = 
	  find_all_continuations(a.target_node(),input_position+1,
				 input_end_position,skip_symbols );
	if ( suffixes == NULL )
	  continue;
	else if ( continuations == NULL ) {
	  continuations = new KeyVectorVector;
	}
	add_prefix(a.label().upper_char(),suffixes);
	add_to_continuations(continuations,
			     suffixes);
      }
  
      if ( (a.label().lower_char() == 0))
	{
	  KeyVectorVector * suffixes = 
	  find_all_continuations(a.target_node(),input_position,
				 input_end_position,skip_symbols );
	  if ( suffixes == NULL )
	    continue;
	  else if ( continuations == NULL ) {
	    continuations = new KeyVectorVector;
	  }
	  add_prefix(a.label().upper_char(),suffixes);
	  add_to_continuations(continuations,
			       suffixes);
	}
      else if ( (skip_symbols->find(a.label().lower_char()) != 
		 skip_symbols->end())) {
	KeyVectorVector * suffixes = 
	  find_all_continuations(a.target_node(),input_position,
				 input_end_position,skip_symbols );
	if ( suffixes == NULL )
	  continue;
	else if ( continuations == NULL ) {
	  continuations = new KeyVectorVector;
	}
	if ((skip_symbols->find(a.label().upper_char()) != 
	     skip_symbols->end()))
	  {
	    add_prefix(a.label().lower_char(),suffixes);
	  }
	else
	  {
	    add_prefix(a.label().upper_char(),suffixes);
	    add_prefix(a.label().lower_char(),suffixes);
	  }
	add_to_continuations(continuations,
			     suffixes);
      }
      
    }
    
    return continuations;
  }

  KeyVector * find_first_output_string( Transducer * t,
					KeyVector * input ) {
    if ( input->empty() ) 
      return new KeyVector;
    Node * start = t->root_node();
    KeyVector::iterator input_position = input->begin();
    KeyVector::iterator last_input_position = input->end();
    bool succeeded = false;
    NodeSet nodes;
    KeyVector * reversed_output =
      find_first_continuation(start, input_position, 
			      last_input_position,nodes,succeeded);
    return reverse(reversed_output);

  }

  KeyVectorVector * find_all_output_strings( Transducer * t,
					     KeyVector * input,
					     KeySet * skip_symbols) {
    Node * start = t->root_node();
    KeyVector::iterator input_position = input->begin();
    KeyVector::iterator last_input_position = input->end();
    KeyVectorVector * reversed_outputs = 
      find_all_continuations(start, input_position, 
			     last_input_position,
			     skip_symbols);
    KeyVectorVector * outputs = reverse(reversed_outputs);
    if (outputs == NULL)
      {
	return new KeyVectorVector;
      }
    else
      {
	return outputs;
      }
  }

inline Key get_token( Node * n ) {
  for(ArcsIter arcs(n->arcs());
      arcs;
      arcs++) {
    Arc a = *arcs;
    if ( a.label().lower_char() == 0 )
      return a.label().upper_char();
  }
  return USHRT_MAX;
}

inline Node * input_target( Node * n, Key input_key ) {
  return n->target_node(Label(input_key,0));
}

Key get_next_maximal_prefix(Node * n,
			    KeyVector::iterator &input_position,
			    KeyVector::iterator input_end) {
  
  if ( input_position == input_end )
    return get_token(n);

  Node * next_node = input_target(n,*input_position);
  
  if ( next_node == NULL )
    return get_token(n);
  
  ++input_position;
  
  Key next_token = get_next_maximal_prefix(next_node,
					   input_position,
					   input_end);

  if ( next_token == USHRT_MAX ) {
    --input_position;
    return get_token(n);
  }
  
  return next_token;
}
  
KeyVector * longest_prefix_tokenization(Transducer * t,
					KeyVector * input) {

  KeyVector * result = new KeyVector;

  KeyVector::iterator input_position = input->begin();

  while( input_position != input->end() ) {
    
    Key next_token_key =
      get_next_maximal_prefix(t->root_node(),
			      input_position,
			      input->end());

    if ( next_token_key == USHRT_MAX ) {
      delete result;
      return NULL;
    }

    result->push_back(next_token_key);
  }
  
  return result;
}

bool has_infinitely_many_continuations(Node * n,
				       bool output,
				       KeyVector * str,
				       unsigned int index, 
				       NodeMap &nodes)
{

  bool has_old_index = false;
  unsigned int old_index = 0;

  if (nodes.find(n) != nodes.end())
    {
      has_old_index = true;
      old_index = nodes[n];
      if (index == old_index)
	{
	  return true;
	}
    }
  nodes[n] = index;

  if (output)
    { 
      for(ArcsIter arcs(n->arcs());
	  arcs;
	  arcs++) 
	{
	  Arc a = *arcs;
	  if ((index < str->size()) and
	      (a.label().lower_char() == str->at(index)))
		{
		  Node * new_n = a.target_node();
		  if (has_infinitely_many_continuations(new_n,
							output,
							str,
							index+1, 
							nodes))
		    {
		      return true;
		    }
		}
	  else if (a.label().lower_char() == 0)
	    {
	      Node * new_n = a.target_node();
	      if (has_infinitely_many_continuations(new_n,
						    output,
						    str,
						    index, 
						    nodes))
		{
		  return true;
		}
	    }	  
	}
    }
  else
    {
      for(ArcsIter arcs(n->arcs());
	  arcs;
	  arcs++) 
	{
	  Arc a = *arcs;
	  if ((index < str->size()) and
	      (a.label().upper_char() == str->at(index)))
	    {
	      Node * new_n = a.target_node();
	      if (has_infinitely_many_continuations(new_n,
						    output,
						    str,
						    index+1, 
						    nodes))
		{
		  return true;
		}
	    }
	  else if (a.label().upper_char() == 0)
	    {
	      Node * new_n = a.target_node();
	      if (has_infinitely_many_continuations(new_n,
						    output,
						    str,
						    index, 
						    nodes))
		{
		  return true;
		}
	    }
	}
    }
  if (has_old_index)
    {
      nodes[n] = old_index;
    }
  else
    {
      nodes.erase(n);
    }
  return false;
}
  

bool has_infinitely_many_analyses(Transducer * t,
				  bool output,
				  KeyVector * str)
{
  assert(str != NULL);
  
  NodeMap nodes;
  return has_infinitely_many_continuations(t->root_node(),
					   output,
					   str,
					   0,
					   nodes);
  
}

};
