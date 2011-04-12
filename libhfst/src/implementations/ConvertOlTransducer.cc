//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "ConvertTransducerFormat.h"
#include "optimized-lookup/convert.h"
#include "HfstTransitionGraph.h"
#include "HfstTransducer.h"

#ifndef DEBUG_MAIN
namespace hfst { namespace implementations
{

  /* -----------------------------------------------------------

      Conversion functions between HfstBasicTransducer and 
      optimized lookup transducers 

      ---------------------------------------------------------- */

/* An auxiliary function. */
unsigned int hfst_ol_to_hfst_basic_add_state
(hfst_ol::Transducer * t, 
 HfstBasicTransducer * basic,
 hfst_ol::HfstOlToBasicStateMap & state_map,
 bool weighted,
 hfst_ol::TransitionTableIndex index,
                                             unsigned int state_number)
{
  unsigned int new_state = state_number;
  state_map[index] = new_state;
  
  if(hfst_ol::indexes_transition_index_table(index))
  {
    const hfst_ol::TransitionIndex& transition_index = t->get_index(index);
    
    if(transition_index.final())
    {
        basic->add_state(new_state);
        basic->set_final_weight(new_state,
                                weighted ?
                                dynamic_cast<const hfst_ol::TransitionWIndex&>
                                (transition_index).final_weight() :
                                0.0);
    }
  }
  else // indexes transition table
  {
    const hfst_ol::Transition& transition = t->get_transition(index);
    
    if(transition.final())
    {
        basic->add_state(new_state);
        basic->set_final_weight(new_state,
                                weighted ?
                                dynamic_cast<const hfst_ol::TransitionW&>
                                (transition).get_weight() :
                                0.0);
    }
  }
  return new_state;
}


  /* Create an HfstBasicTransducer equivalent to hfst_ol::Transducer \a t . */
  HfstBasicTransducer * ConversionFunctions::
  hfst_ol_to_hfst_basic_transducer(hfst_ol::Transducer * t)
  {
      HfstBasicTransducer * basic = new HfstBasicTransducer();
      bool weighted = t->get_header().probe_flag(hfst_ol::Weighted);
      const hfst_ol::SymbolTable& symbols 
        = t->get_alphabet().get_symbol_table();
      
      
      /* This contains indices to either (1) the start of a set of entries 
         in the transition index table, or (2) the boundary before a set 
         of entries in the transition table; in this case, the following 
         entries will all have the same input symbol. In either case 
         the index represents a state and may be final The will already be 
         an entry in state_map for each value in agenda */
      std::vector<hfst_ol::TransitionTableIndex> agenda;
      hfst_ol::HfstOlToBasicStateMap state_map;
      unsigned int state_number=0;
      
      hfst_ol_to_hfst_basic_add_state
        (t, basic, state_map, weighted, 0, state_number);
      agenda.push_back(0);
      while(!agenda.empty())
      {
          hfst_ol::TransitionTableIndex current_index = agenda.back();
          agenda.pop_back();
          
          unsigned int current_state = state_map[current_index];
          
          hfst_ol::TransitionTableIndexSet transitions 
            = t->get_transitions_from_state(current_index);
          for(hfst_ol::TransitionTableIndexSet::const_iterator it
                =transitions.begin();it!=transitions.end();it++)
          {
              const hfst_ol::Transition& transition = t->get_transition(*it);
              
              if(state_map.find(transition.get_target()) == state_map.end())
              {
                  state_number++;
                  hfst_ol_to_hfst_basic_add_state
                    (t, basic, state_map, weighted, 
                     transition.get_target(), state_number);
                  agenda.push_back(transition.get_target());
              }
              basic->add_transition
                (current_state,
                 HfstBasicTransition
                 (state_map[transition.get_target()],
                  symbols[transition.get_input_symbol()],
                  symbols[transition.get_output_symbol()],
                  weighted ? dynamic_cast<const hfst_ol::TransitionW&>
                  (transition).get_weight() : 0 ));
          }
      }
      
      return basic;
      
  }

  /* Create an hfst_ol::Transducer equivalent to HfstBasicTransducer \a t.
     \a weighted defined whether the created transducer is weighted. */
  hfst_ol::Transducer * ConversionFunctions::
  hfst_basic_transducer_to_hfst_ol
  (const HfstBasicTransducer * t, bool weighted, std::string options)
  {
      const float packing_aggression = 0.85;
      const int floor_jump_threshold = 4; // a packing aggression parameter
      bool quick = options == "quick";
      typedef std::set<std::string> StringSet;
      using hfst_ol::SymbolNumber;
      using hfst_ol::NO_SYMBOL_NUMBER;
      // The transition array is indexed starting from this constant
      const unsigned int TA_OFFSET = 2147483648u;

      // Symbols must be in the following order in an optimized-lookup
      // transducer:
      // 1) epsilon
      // 2) other input symbols
      // 3) symbols that aren't used as input symbols
      // Flag diacritics must be indexed as if they were symbol #0
      // (epsilon) and otherwise have a proper unique number,
      // but they can appear anywhere in the alphabet.
      
      // In this case we implement an optimisation where flag diacritics
      // appear at the end of the alphabet. This allows us to ignore
      // them for indexing purposes, which potentially makes the index
      // table smaller and faster to pack.

      // We also gather information about possible gaps in the state numbering,
      // because we want it to be contiguous from now on.

      StringSet * input_symbols = new StringSet();
      StringSet * flag_diacritics = new StringSet();
      StringSet * other_symbols = new StringSet();
      
      std::vector<hfst_ol::StatePlaceholder> state_placeholders;

      std::map<unsigned int, unsigned int> * relabeled_states =
	  new std::map<unsigned int, unsigned int>();
      unsigned int first_transition = 0;

      for (HfstBasicTransducer::const_iterator it = t->begin(); 
           it != t->end(); ++it) {
	  unsigned int state_number = state_placeholders.size();
	  if (state_number != it->first) {
	      relabeled_states->operator[](it->first) = state_number;
	  }
	  state_placeholders.push_back(hfst_ol::StatePlaceholder(
					   state_number,
					   t->is_final_state(it->first),
					   first_transition));
	  ++first_transition; // there's a padding entry between states
          for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
		   = it->second.begin();
               tr_it != it->second.end(); ++tr_it) {
	      ++first_transition;
              if (FdOperation::is_diacritic(tr_it->get_input_symbol())) {
                  flag_diacritics->insert(tr_it->get_input_symbol());
              } else {
                  input_symbols->insert(tr_it->get_input_symbol());
              }
              other_symbols->insert(tr_it->get_output_symbol());
          }
      }
      
      SymbolNumber seen_input_symbols = 1; // We always have epsilon
      hfst_ol::SymbolTable symbol_table;
      std::set<SymbolNumber> flag_symbols;

      std::map<std::string, SymbolNumber> * string_symbol_map =
      new std::map<std::string, SymbolNumber>();

      // 1) epsilon
      string_symbol_map->operator[](internal_epsilon) = symbol_table.size();
      symbol_table.push_back(internal_epsilon);

      // 2) input symbols
      for (std::set<std::string>::iterator it = input_symbols->begin();
           it != input_symbols->end(); ++it) {
          if (!is_epsilon(*it)) {
              string_symbol_map->operator[](*it) = symbol_table.size();
              symbol_table.push_back(*it);
              ++seen_input_symbols;
          }
      }

      // 3) Flag diacritics
      for (std::set<std::string>::iterator it = flag_diacritics->begin();
           it != flag_diacritics->end(); ++it) {
          if (!is_epsilon(*it)) {
              string_symbol_map->operator[](*it) = symbol_table.size();
              flag_symbols.insert(symbol_table.size());
              symbol_table.push_back(*it);
              // don't increment seen_input_symbols - we use it for
              // indexing
          }
      }

      // 4) non-input symbols
      for (std::set<std::string>::iterator it = other_symbols->begin();
           it != other_symbols->end(); ++it) {
          if (!is_epsilon(*it) and input_symbols->count(*it) == 0 and
              flag_diacritics->count(*it) == 0) {
              string_symbol_map->operator[](*it) = symbol_table.size();
              symbol_table.push_back(*it);
          }
      }

      delete input_symbols;
      delete flag_diacritics;
      delete other_symbols;

    // Do a second pass over the transitions, figuring out everything
    // about the states except starting indices

    for (HfstBasicTransducer::const_iterator it = t->begin(); 
         it != t->end(); ++it) {
        for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
               = it->second.begin();
             tr_it != it->second.end(); ++tr_it) {
	    unsigned int state_number = it->first;
	    if (relabeled_states->count(state_number) != 0) {
		state_number = relabeled_states->operator[](state_number);
	    }
            // check for previously unseen inputs
            if (state_placeholders[state_number].inputs.count(
                    string_symbol_map->operator[](
                                    tr_it->get_input_symbol())) == 0) {
                state_placeholders[state_number].inputs[
                    string_symbol_map->operator[](tr_it->get_input_symbol())] =
                    std::vector<hfst_ol::TransitionPlaceholder>();
            }
	    unsigned int target = tr_it->get_target_state();
	    if (relabeled_states->count(target) != 0) {
		target = relabeled_states->operator[](target);
	    }
            hfst_ol::TransitionPlaceholder trans(
                target,
                string_symbol_map->operator[](tr_it->get_output_symbol()),
                tr_it->get_weight());
            state_placeholders[state_number]
                .inputs[string_symbol_map->operator[](
		    tr_it->get_input_symbol())].push_back(trans);
        }
    }
    delete relabeled_states;
    delete string_symbol_map;

    // For determining the index table we first sort the states (excepting
    // the starting state) by number of different input symbols.
    if (state_placeholders.begin() != state_placeholders.end()) {
	sort(state_placeholders.begin() + 1, state_placeholders.end(),
	     hfst_ol::compare_states_by_input_size);
    }

    hfst_ol::IndexPlaceholders * used_indices =
        new hfst_ol::IndexPlaceholders();

    // Now we assign starting indices (or alternatively determine a state
    // doesn't need an entry in the TIA ("is simple"). The starting state has
    // index 0. Used indices are stored in a map (at the beginning, every
    // index below or equal to the alphabet size is available except index 0).
    // For every state (in the TIA) thereafter, we check each available
    // starting index to see if it fits.

    // The starting state is special because it will have a TIA entry even if
    // it's simple, so we deal with it every time.

    unsigned int first_available_index = 0;
    unsigned int previous_first_index = 0;
    unsigned int previous_successful_index = 0;
    int floor_stuck_counter = 0;
    for (std::vector<hfst_ol::StatePlaceholder>::iterator it =
             state_placeholders.begin();
         it != state_placeholders.end(); ++it) {
        if (it->is_simple(flag_symbols) and it->state_number != 0) {
            continue;
        }
        unsigned int i = first_available_index;

        // While this index is not suitable for a starting index, keep looking
	if (!quick) {
	    while (!used_indices->fits(*it, flag_symbols, i)) {
		++i;
	    }
	}
        it->start_index = i;
	previous_successful_index = i;
        // Once we've found a starting index, insert a finality marker and
	// mark all the used indices
	used_indices->operator[](i) =
	    std::pair<unsigned int, SymbolNumber>(
		it->state_number, NO_SYMBOL_NUMBER);
        for (std::map<SymbolNumber,
                 std::vector<hfst_ol::TransitionPlaceholder> >
                 ::iterator sym_it = it->inputs.begin();
             sym_it != it->inputs.end(); ++sym_it) {
            SymbolNumber index_offset = sym_it->first;
            if (flag_symbols.count(index_offset) != 0) {
                index_offset = 0;
            }
            used_indices->operator[](i + index_offset + 1) =
                std::pair<unsigned int, SymbolNumber>
                (it->state_number, index_offset);
        }
	if (quick) {
	    first_available_index = used_indices->rbegin()->first + 1;
	    continue;
	}
	while (used_indices->unsuitable(
		   first_available_index, seen_input_symbols,
		   packing_aggression)) {
	    ++first_available_index;
	}
	if (first_available_index == previous_first_index) {
	    if (floor_stuck_counter > floor_jump_threshold) {
		SymbolNumber index_offset = it->inputs.rbegin()->first;
		if (flag_symbols.count(index_offset) != 0) {
		    index_offset = 0;
		}
		first_available_index =
		    previous_successful_index + 1 + index_offset;
		while (used_indices->unsuitable(
			   first_available_index,
			   seen_input_symbols, packing_aggression)) {
		    ++first_available_index;
		}
		floor_stuck_counter = 0;
		previous_first_index = first_available_index;
	    } else {
		++floor_stuck_counter;
	    }
	} else {
	    previous_first_index = first_available_index;
	    floor_stuck_counter = 0;
	}
    }

    // Now resort by state number for the rest
    // (this could definitely be neater...)
    if (state_placeholders.begin() != state_placeholders.end()) {
	sort(state_placeholders.begin() + 1, state_placeholders.end(),
	     hfst_ol::compare_states_by_state_number);
    }

    // Now for each index entry we write its input symbol and target

    hfst_ol::TransducerTable<hfst_ol::TransitionWIndex> windex_table;
    
    unsigned int greatest_index = 0;
    if (used_indices->size() != 0) {
        greatest_index = used_indices->rbegin()->first;
    }

    for(unsigned int i = 0; i <= greatest_index; ++i) {
        if (used_indices->count(i) == 0) { // blank entries
            windex_table.append(hfst_ol::TransitionWIndex());
        } else if (used_indices->operator[](i).second ==
		   NO_SYMBOL_NUMBER) { // finality markers
	    if (state_placeholders[used_indices->operator[](i).first].final) {
		windex_table.append(
		    hfst_ol::TransitionWIndex::create_final(
			state_placeholders[
			    used_indices->operator[](i).first].final_weight));
	    } else {
		windex_table.append(hfst_ol::TransitionWIndex());
	    }
	} else { // actual entries
            unsigned int idx = used_indices->operator[](i).first;
            SymbolNumber sym = used_indices->operator[](i).second;
            windex_table.append(
		hfst_ol::TransitionWIndex(
		    sym,
		    state_placeholders[idx].first_transition +
		    state_placeholders[idx].symbol_offset(
			sym, flag_symbols) + TA_OFFSET));
        }
    }

    delete used_indices;

    for (unsigned int i = 0; i < seen_input_symbols; ++i) {
        windex_table.append(hfst_ol::TransitionWIndex()); // padding
    }

    // Now write the transition table

    hfst_ol::TransducerTable<hfst_ol::TransitionW> wtransition_table;

    hfst_ol::write_transitions_from_state_placeholders(
        wtransition_table,
        state_placeholders,
        flag_symbols);

    hfst_ol::TransducerAlphabet alphabet(symbol_table);
    hfst_ol::TransducerHeader header(seen_input_symbols,
                                     symbol_table.size(),
                                     windex_table.size(),
                                     wtransition_table.size(),
                                     weighted);
    return new hfst_ol::Transducer(header,
                                   alphabet,
                                   windex_table,
                                   wtransition_table);
  }



  }}
#endif // DEBUG_MAIN
