#include "hwfst-lookup-defs.h"
#include <climits>
#include <set>


/* 
   This module contains functions, which may be used for lookup
   of analyses corresponding to a string in a transducer. Strings
   are represented as KeyVectors. Written by Miikka Silfverberg
   in january 2009.

   A container for KeyVectors is the KeyVectorVector.
*/

namespace HWFST {

typedef std::set<StateId> StateSet;

KeyVectorVector * reverse( KeyVectorVector * key_strings ); 

KeyVector * reverse( KeyVector * key_string ); 

void add_prefix(Key k, KeyVectorVector * suffixes);

void add_to_continuations(KeyVectorVector * continuations,
			  KeyVectorVector * suffixes);

KeyVectorVector * 
find_all_continuations(StateId n, 
		       KeyVector::iterator input_position, 
		       KeyVector::iterator input_end_position,
		       Transducer &t,
		       bool preserve_epsilon);

KeyVector *
find_first_continuation(StateId n, 
			KeyVector::iterator input_position, 
			KeyVector::iterator input_end_position,
			Transducer &t,
			StateSet &states,
			bool &succeeded,
			bool preserve_epsilon);
 
KeyVector * find_first_output_string( Transducer &t, 
				      KeyVector * input );

KeyVectorVector * find_all_output_strings( Transducer &t, 
					   KeyVector * input );

Key get_next_maximal_prefix(Transducer &t,
			    StateId n,
			    KeyVector::iterator &input_position,
			    KeyVector::iterator input_end);

KeyVector * longest_prefix_tokenization(Transducer &t,
					KeyVector * input);
};
