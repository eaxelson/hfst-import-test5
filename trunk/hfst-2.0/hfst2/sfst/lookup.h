#include <vector>
#include <cassert>
#include "fst.h"
#include <climits>
#include <map>
#include <set>

using std::vector;

typedef unsigned short Key;
typedef std::map<Node*,unsigned int> NodeMap;
typedef std::set<Node*> NodeSet;

/* This module contains functions, which may be used for lookup
   of analyses corresponding to a string in a transducer. Strings
   are represented as Key vectors. Written by Miikka Silfverberg
   in january 2009.
*/

namespace HFST {

KeyVectorVector * reverse( KeyVectorVector * key_strings ); 
KeyVector * reverse( KeyVector * key_string ); 
void add_prefix(Key k, KeyVectorVector * suffixes);
void add_to_continuations(KeyVectorVector * continuations,
			  KeyVectorVector * suffixes);
KeyVectorVector * 
find_all_continuations(Node * n, 
		       KeyVector::iterator input_position, 
		       KeyVector::iterator input_end_position,
		       bool preserve_epsilon);

KeyVector * 
find_first_continuation(Node * n, 
			KeyVector::iterator input_position, 
			KeyVector::iterator input_end_position,
			NodeSet &previous_nodes_at_zero_distance,
			bool &succeeded,
			bool preserve_epsilon);

 
KeyVector * find_first_output_string( Transducer * t, 
					KeyVector * input );

KeyVectorVector * find_all_output_strings( Transducer * t, 
					      KeyVector * input );

Key get_next_maximal_prefix(Node * n,
			    KeyVector::iterator &input_position,
			    KeyVector::iterator input_end);

KeyVector * longest_prefix_tokenization(Transducer * t,
					KeyVector * input);


bool has_infinitely_many_continuations(Node * n,
				       bool output,
				       KeyVector * str,
				       unsigned int index, 
				       NodeMap &nodes);

bool has_infinitely_many_analyses(Transducer * t,
				  bool output,
				  KeyVector * str);
};
