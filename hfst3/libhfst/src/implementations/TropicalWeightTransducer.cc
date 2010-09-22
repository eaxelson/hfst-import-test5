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

#include "TropicalWeightTransducer.h"

namespace hfst { namespace implementations
{
  bool openfst_tropical_use_hopcroft=false;

  void openfst_tropical_set_hopcroft(bool value) {
    openfst_tropical_use_hopcroft=value;
  }

  void initialize_symbol_tables(StdVectorFst *t);

  TropicalWeightInputStream::TropicalWeightInputStream(void):
    i_stream(),input_stream(cin)
  {}
  TropicalWeightInputStream::TropicalWeightInputStream(const char * filename):
    filename(filename),i_stream(filename),input_stream(i_stream)
  {}

  StringSet TropicalWeightTransducer::get_string_set(StdVectorFst *t)
  {
    assert(t->InputSymbols() != NULL);
    StringSet s;
    for ( fst::SymbolTableIterator it = fst::SymbolTableIterator(*(t->InputSymbols()));
	  not it.Done(); it.Next() ) {
      s.insert( std::string(it.Symbol()) );
    }
    return s;
  }

  /* Find the number-to-number mappings needed to be performed to t1 so that it will follow 
     the same symbol-to-number encoding as t2.
     @pre t2's symbol table must contain all symbols in t1's symbol table. 
  */
  NumberNumberMap TropicalWeightTransducer::create_mapping(fst::StdVectorFst *t1, fst::StdVectorFst *t2)
  {
    NumberNumberMap km;
    // find the number-to-number mappings for transducer t1
    for ( fst::SymbolTableIterator it = fst::SymbolTableIterator(*(t1->InputSymbols()));
	  not it.Done(); it.Next() ) {    
      km [ (unsigned int)it.Value() ] = (unsigned int) t2->InputSymbols()->Find( it.Symbol() );
    }
    return km;
  }

  /* Recode the symbol numbers in this transducer as indicated in KeyMap km. */
  void TropicalWeightTransducer::recode_symbol_numbers(StdVectorFst *t, NumberNumberMap &km) 
  {
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	for (fst::MutableArcIterator<StdVectorFst> aiter(t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    StdArc new_arc;
	    new_arc.ilabel = km[arc.ilabel];
	    new_arc.olabel = km[arc.olabel];
	    new_arc.weight = arc.weight;
	    new_arc.nextstate = arc.nextstate;
	    aiter.SetValue(new_arc);
	  }
      }
    return;
  }

  StdVectorFst * TropicalWeightTransducer::set_final_weights(StdVectorFst * t, float weight)
  {
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if ( t->Final(s) != TropicalWeight::Zero() )
	  t->SetFinal(s, weight);
      }
    return t;
  }

  StdVectorFst * TropicalWeightTransducer::push_weights(StdVectorFst * t, bool to_initial_state)
  {
    assert (t->InputSymbols() != NULL);
    fst::StdVectorFst * retval = new fst::StdVectorFst();
    if (to_initial_state)
      fst::Push<StdArc, REWEIGHT_TO_INITIAL>(*t, retval, fst::kPushWeights);
    else
      fst::Push<StdArc, REWEIGHT_TO_FINAL>(*t, retval, fst::kPushWeights);
    retval->SetInputSymbols(t->InputSymbols());
    return retval;
  }

  StdVectorFst * TropicalWeightTransducer::transform_weights(StdVectorFst * t,float (*func)(float f))
  {
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if ( t->Final(s) != TropicalWeight::Zero() )
	  t->SetFinal( s, func(t->Final(s).Value()) );
	for (fst::MutableArcIterator<StdVectorFst> aiter(t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    StdArc new_arc;
	    new_arc.ilabel = arc.ilabel;
	    new_arc.olabel = arc.olabel;
	    new_arc.weight = func(arc.weight.Value());
	    new_arc.nextstate = arc.nextstate;
	    aiter.SetValue(new_arc);
	  }
      }
    return t;
  }

  void TropicalWeightTransducer::write_in_att_format(StdVectorFst *t, FILE *ofile)
  {

    const SymbolTable *sym = t->InputSymbols();

    // this takes care that initial state is always printed as number zero
    // and state number zero (if it is not initial) is printed as another number
    // (basically as the number of the initial state in that case, i.e.
    // the numbers of initial state and state number zero are swapped)
    StateId zero_print=0;
    StateId initial_state = t->Start();
    if (initial_state != 0) {
      zero_print = initial_state;
    }
      
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	if (s == initial_state) {
	int origin;  // how origin state is printed, see the first comment
	if (s == 0)
	  origin = zero_print;
	else if (s == initial_state)
	  origin = 0;
	else
	  origin = (int)s;
	for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    int target;  // how target state is printed, see the first comment
	    if (arc.nextstate == 0)
	      target = zero_print;
	    else if (arc.nextstate == initial_state)
	      target = 0;
	    else
	      target = (int)arc.nextstate;
	    fprintf(ofile, "%i\t%i\t%s\t%s\t%f\n", origin, target,
		    sym->Find(arc.ilabel).c_str(), sym->Find(arc.olabel).c_str(),
		    arc.weight.Value());
	  }
	if (t->Final(s) != TropicalWeight::Zero())
	  fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	break;
	}
      }

    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if (s != initial_state) {
	  int origin;  // how origin state is printed, see the first comment
	  if (s == 0)
	    origin = zero_print;
	  else if (s == initial_state)
	    origin = 0;
	  else
	    origin = (int)s;
	  for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const StdArc &arc = aiter.Value();
	      int target;  // how target state is printed, see the first comment
	      if (arc.nextstate == 0)
		target = zero_print;
	      else if (arc.nextstate == initial_state)
		target = 0;
	      else
		target = (int)arc.nextstate;
	      fprintf(ofile, "%i\t%i\t%s\t%s\t%f\n", origin, target,
		      sym->Find(arc.ilabel).c_str(), sym->Find(arc.olabel).c_str(),
		      arc.weight.Value());
	    }
	  if (t->Final(s) != TropicalWeight::Zero())
	    fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	}
      }
  }


  void TropicalWeightTransducer::write_in_att_format_number(StdVectorFst *t, FILE *ofile)
  {

    // this takes care that initial state is always printed as number zero
    // and state number zero (if it is not initial) is printed as another number
    // (basically as the number of the initial state in that case, i.e.
    // the numbers of initial state and state number zero are swapped)
    StateId zero_print=0;
    StateId initial_state = t->Start();
    if (initial_state != 0) {
      zero_print = initial_state;
    }
      
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	if (s == initial_state) {
	int origin;  // how origin state is printed, see the first comment
	if (s == 0)
	  origin = zero_print;
	else if (s == initial_state)
	  origin = 0;
	else
	  origin = (int)s;
	for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    int target;  // how target state is printed, see the first comment
	    if (arc.nextstate == 0)
	      target = zero_print;
	    else if (arc.nextstate == initial_state)
	      target = 0;
	    else
	      target = (int)arc.nextstate;
	    fprintf(ofile, "%i\t%i\t\\%i\t\\%i\t%f\n", origin, target,
		    arc.ilabel, arc.olabel,
		    arc.weight.Value());
	  }
	if (t->Final(s) != TropicalWeight::Zero())
	  fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	break;
	}
      }

    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if (s != initial_state) {
	  int origin;  // how origin state is printed, see the first comment
	  if (s == 0)
	    origin = zero_print;
	  else if (s == initial_state)
	    origin = 0;
	  else
	    origin = (int)s;
	  for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const StdArc &arc = aiter.Value();
	      int target;  // how target state is printed, see the first comment
	      if (arc.nextstate == 0)
		target = zero_print;
	      else if (arc.nextstate == initial_state)
		target = 0;
	      else
		target = (int)arc.nextstate;
	      fprintf(ofile, "%i\t%i\t\\%i\t\\%i\t%f\n", origin, target,
		      arc.ilabel, arc.olabel,
		      arc.weight.Value());
	    }
	  if (t->Final(s) != TropicalWeight::Zero())
	    fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	}
      }
  }


  void TropicalWeightTransducer::write_in_att_format(StdVectorFst *t, std::ostream &os)
  {

    const SymbolTable *sym = t->InputSymbols();

    // this takes care that initial state is always printed as number zero
    // and state number zero (if it is not initial) is printed as another number
    // (basically as the number of the initial state in that case, i.e.
    // the numbers of initial state and state number zero are swapped)
    StateId zero_print=0;
    StateId initial_state = t->Start();
    if (initial_state != 0) {
      zero_print = initial_state;
    }
      
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	if (s == initial_state) {
	int origin;  // how origin state is printed, see the first comment
	if (s == 0)
	  origin = zero_print;
	else if (s == initial_state)
	  origin = 0;
	else
	  origin = (int)s;
	for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    int target;  // how target state is printed, see the first comment
	    if (arc.nextstate == 0)
	      target = zero_print;
	    else if (arc.nextstate == initial_state)
	      target = 0;
	    else
	      target = (int)arc.nextstate;
	    os << origin << "\t" 
	       << target << "\t" 
	       << sym->Find(arc.ilabel).c_str() << "\t"
	       << sym->Find(arc.olabel).c_str() << "\t" 
	       << arc.weight.Value() << "\n";
	  }
	if (t->Final(s) != TropicalWeight::Zero())
	  os << origin << "\t"
	     << t->Final(s).Value() << "\n";
	break;
	}
      }

    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if (s != initial_state) {
	  int origin;  // how origin state is printed, see the first comment
	  if (s == 0)
	    origin = zero_print;
	  else if (s == initial_state)
	    origin = 0;
	  else
	    origin = (int)s;
	  for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const StdArc &arc = aiter.Value();
	      int target;  // how target state is printed, see the first comment
	      if (arc.nextstate == 0)
		target = zero_print;
	      else if (arc.nextstate == initial_state)
		target = 0;
	      else
		target = (int)arc.nextstate;
	      os << origin << "\t" 
		 << target << "\t"
		 << sym->Find(arc.ilabel).c_str() << "\t"
		 << sym->Find(arc.olabel).c_str() << "\t"
		 << arc.weight.Value() << "\n";
	    }
	  if (t->Final(s) != TropicalWeight::Zero())
	    os << origin << "\t"
	       << t->Final(s).Value() << "\n";
	}
      }
  }


  void TropicalWeightTransducer::write_in_att_format_number(StdVectorFst *t, std::ostream &os)
  {

    const SymbolTable* sym = t->InputSymbols();

    // this takes care that initial state is always printed as number zero
    // and state number zero (if it is not initial) is printed as another number
    // (basically as the number of the initial state in that case, i.e.
    // the numbers of initial state and state number zero are swapped)
    StateId zero_print=0;
    StateId initial_state = t->Start();
    if (initial_state != 0) {
      zero_print = initial_state;
    }
      
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	if (s == initial_state) {
	int origin;  // how origin state is printed, see the first comment
	if (s == 0)
	  origin = zero_print;
	else if (s == initial_state)
	  origin = 0;
	else
	  origin = (int)s;
	for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    int target;  // how target state is printed, see the first comment
	    if (arc.nextstate == 0)
	      target = zero_print;
	    else if (arc.nextstate == initial_state)
	      target = 0;
	    else
	      target = (int)arc.nextstate;
	    os << origin << "\t" 
	       << target << "\t" 
	       << "\\" << arc.ilabel << "\t"
	       << "\\" << arc.olabel << "\t" 
	       << arc.weight.Value() << "\n";
	  }
	if (t->Final(s) != TropicalWeight::Zero())
	  os << origin << "\t"
	     << t->Final(s).Value() << "\n";
	break;
	}
      }

    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if (s != initial_state) {
	  int origin;  // how origin state is printed, see the first comment
	  if (s == 0)
	    origin = zero_print;
	  else if (s == initial_state)
	    origin = 0;
	  else
	    origin = (int)s;
	  for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const StdArc &arc = aiter.Value();
	      int target;  // how target state is printed, see the first comment
	      if (arc.nextstate == 0)
		target = zero_print;
	      else if (arc.nextstate == initial_state)
		target = 0;
	      else
		target = (int)arc.nextstate;
	      os << origin << "\t" 
		 << target << "\t"
		 << "\\" << arc.ilabel << "\t"
		 << "\\" << arc.olabel << "\t"
		 << arc.weight.Value() << "\n";
	    }
	  if (t->Final(s) != TropicalWeight::Zero())
	    os << origin << "\t"
	       << t->Final(s).Value() << "\n";
	}
      }
  }
  

  // AT&T format is handled here ------------------------------

  /* Maps state numbers in AT&T text format to state ids used by OpenFst transducers. */
  typedef std::map<int, StateId> StateMap;

  // FIX: this would be better in namespace TropicalWeightTransducer...
  /* A method used by function 'read_in_att_format'.
     Returns the state id of state number state_number and adds a new
     state to t if state_number is encountered for the first time and
     updates state_map accordingly. */
  StateId add_and_map_state(StdVectorFst *t, int state_number, StateMap &state_map)
  {
    StateMap::iterator it = state_map.find(state_number);
    if (it == state_map.end()) {
      StateId retval  = t->AddState();
      state_map.insert( std::pair<int, StateId>(state_number, retval) );
      return retval;
    }
    else
      return it->second;
  }

  // FIX: atof and atoi are not necessarily portable...
  /* Reads a description of a transducer in AT&T text format and returns a corresponding
     binary transducer. 
     @note The initial state must be numbered as zero. */
  StdVectorFst * TropicalWeightTransducer::read_in_att_format(FILE * ifile)
  {
    StdVectorFst *t = new StdVectorFst();
    SymbolTable st = create_symbol_table("");

    char line [255];
    StateMap state_map;

    // Add initial state that is numbered as zero.
    StateId initial_state = add_and_map_state(t, 0, state_map);
    t->SetStart(initial_state);


    while ( fgets(line, 255, ifile) != NULL ) 
      {
	if (*line == '-') // transducer separator
	  return t;
	//printf("read line: %s", line);
	char a1 [100]; char a2 [100]; char a3 [100]; char a4 [100]; char a5 [100];
	int n = sscanf(line, "%s\t%s\t%s\t%s\t%s", a1, a2, a3, a4, a5);
	//printf("number of arguments: (%i)\n", n);

	// set value of weight
	float weight = 0;
	if (n == 2)
	  weight = atof(a2);
	if (n == 5)
	  weight = atof(a5);

	if (n == 1 || n == 2)  // final state line
	  {
	    int final_number = atoi(a1);
	    StateId final_state = add_and_map_state(t, final_number, state_map);
	    t->SetFinal(final_state, weight);
	    //printf("...added final state %i with weight %f\n", final_state, weight);
	  }

	else if (n == 4 || n == 5)  // transition line
	  {
	    int origin_number = atoi(a1);
	    int target_number = atoi(a2);
	    StateId origin_state = add_and_map_state(t, origin_number, state_map);
	    StateId target_state = add_and_map_state(t, target_number, state_map);

	    int input_number = st.AddSymbol(std::string(a3));
	    int output_number = st.AddSymbol(std::string(a4));

	    t->AddArc(origin_state, StdArc(input_number, output_number, weight, target_state));
	    //printf("...added transition from state %i to state %i with input number %i and output number
	    //%i and weight %f\n", origin_state, target_state, input_number, output_number, weight);
	  }

	else  // line could not be parsed
	  {
	    printf("ERROR: in AT&T file: line: \"%s\"\n", line);
	    throw NotValidAttFormatException();
	  }

      }

    t->SetInputSymbols(&st);
    return t;
  }

  /* 
     Create a copy of this transducer where all transitions of type "?:?", "?:x" and "x:?"
     are expanded according to the StringSymbolSet 'unknown' that lists all symbols previously
     unknown to this transducer.
  */
  StdVectorFst * TropicalWeightTransducer::expand_arcs(StdVectorFst * t, StringSet &unknown)
  {
    //fprintf(stderr, "TropicalWeightTransducer::expand_arcs...\n");
    StdVectorFst * result = new StdVectorFst();
    std::map<StateId,StateId> state_map;   // maps states of this to states of result

    // go through all states in this
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	// create new state in result, if needed
	StateId s = siter.Value();
	StateId result_s;
	{
	map<StateId,StateId>::const_iterator it = state_map.find(s); 
	if ( it == state_map.end() )
	      {
		result_s = result->AddState();
		state_map[s] = result_s;
	      }
	    else 
	      result_s = it->second;
	}

	// make the new state initial, if needed
	if (t->Start() == s)
	  result->SetStart(result_s);

	// make the new state final, if needed
	if (t->Final(s) != TropicalWeight::Zero())
	  result->SetFinal(result_s, t->Final(s).Value());


	// go through all the arcs in this
	for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();

	    // find the corresponding target state in result or, if not found, create a new state
	    StateId result_nextstate;
	    map<StateId,StateId>::const_iterator it = state_map.find(arc.nextstate); 
	    if ( it == state_map.end() )
	      {
		result_nextstate = result->AddState();
		state_map[arc.nextstate] = result_nextstate;
	      }
	    else 
	      result_nextstate = it->second;

	    // expand the transitions, if needed
	    
	    const fst::SymbolTable *is = t->InputSymbols(); 

	    //fprintf(stderr, "ilabel: %i,    olabel: %i\n", arc.ilabel, arc.olabel);

	    if ( arc.ilabel == 1 &&       // cross-product "?:?"
		 arc.olabel == 1 )
	      {
		for (StringSet::iterator it1 = unknown.begin(); it1 != unknown.end(); it1++) 
		  {
		    //fprintf(stderr, "cross-product ?:?\n");
		    int64 inumber = is->Find(*it1);
		    for (StringSet::iterator it2 = unknown.begin(); it2 != unknown.end(); it2++) 
		      {
			int64 onumber = is->Find(*it2);
			if (inumber != onumber)
			  result->AddArc(result_s, StdArc(inumber, onumber, arc.weight, result_nextstate));
		      }
		    result->AddArc(result_s, StdArc(inumber, 1, arc.weight, result_nextstate));
		    result->AddArc(result_s, StdArc(1, inumber, arc.weight, result_nextstate));
		  }
	      }
	    else if (arc.ilabel == 2 &&   // identity "?:?"
		     arc.olabel == 2 )       
	      {
		//fprintf(stderr, "identity ?:?\n");
		for (StringSet::iterator it = unknown.begin(); it != unknown.end(); it++) 
		  {
		    int64 number = is->Find(*it);
		    result->AddArc(result_s, StdArc(number, number, arc.weight, result_nextstate));
		    //fprintf(stderr, "added transition %i:%i\n", (int)number, (int)number);
		  }
	      }
	    else if (arc.ilabel == 1)  // "?:x"
	      {
		//fprintf(stderr, "?:x\n");
		for (StringSet::iterator it = unknown.begin(); it != unknown.end(); it++) 
		  {
		    int64 number = is->Find(*it);
		    result->AddArc(result_s, StdArc(number, arc.olabel, arc.weight, result_nextstate));
		    //fprintf(stderr, "added transition %i:%i\n", (int)number, (int)arc.olabel);
		  }
	      }
	    else if (arc.olabel == 1)  // "x:?"
	      {
		//fprintf(stderr, "x:?\n");
		for (StringSet::iterator it = unknown.begin(); it != unknown.end(); it++) 
		  {
		    int64 number = is->Find(*it);
		    result->AddArc(result_s, StdArc(arc.ilabel, number, arc.weight, result_nextstate));
		    //fprintf(stderr, "added transition %i:%i\n", (int)arc.ilabel, (int)number);
		  }
	      }

	    // the original transition is copied in all cases
	    result->AddArc(result_s, StdArc(arc.ilabel, arc.olabel, arc.weight, result_nextstate));		
	    
	  }
      }

     result->SetInputSymbols(t->InputSymbols());
    return result;
  }

  std::pair<StdVectorFst*, StdVectorFst*> TropicalWeightTransducer::harmonize
  (StdVectorFst *t1, StdVectorFst *t2)
  {

    //fprintf(stderr, "TWT::harmonize...\n");

    // 1. Calculate the set of unknown symbols for transducers t1 and t2.

    StringSet unknown_t1;    // symbols known to another but not this
    StringSet unknown_t2;    // and vice versa
    StringSet t1_symbols = get_string_set(t1);
    StringSet t2_symbols = get_string_set(t2);
    collect_unknown_sets(t1_symbols, unknown_t1,
			 t2_symbols, unknown_t2);
    
    // 2. Add new symbols from transducer t1 to the symbol table of transducer t2...

    SymbolTable * st2 = t2->InputSymbols()->Copy();
    for ( StringSet::const_iterator it = unknown_t2.begin();
	  it != unknown_t2.end(); it++ ) {
	st2->AddSymbol(*it);
	//fprintf(stderr, "added %s to the set of symbols unknown to t2\n", (*it).c_str());
    }
    t2->SetInputSymbols(st2);
    delete st2;

    // ...calculate the number mappings needed in harmonization...
    NumberNumberMap km = create_mapping(t1, t2);

    // ... replace the symbol table of t1 with a copy of t2's symbol table
    //delete t1->InputSymbols(); this causes problems...
    t1->SetInputSymbols(t2->InputSymbols());

    // ...and recode the symbol numbers of transducer t1 so that
    //    it follows the new symbol table.
    recode_symbol_numbers(t1, km);

    // 3. Calculate the set of symbol pairs to which a non-identity "?:?"
    //    transition is expanded for both transducers.
    
    fst::StdVectorFst *harmonized_t1;
    fst::StdVectorFst *harmonized_t2;

    harmonized_t1 = expand_arcs(t1, unknown_t1);
    harmonized_t1->SetInputSymbols(t1->InputSymbols());

    harmonized_t2 = expand_arcs(t2, unknown_t2);
    harmonized_t2->SetInputSymbols(t2->InputSymbols());

    /*
    fprintf(stderr, "TWT::harmonize: harmonized t1's and t2's input symbol tables now contain (FINAL):\n");
    harmonized_t1->InputSymbols()->WriteText(std::cerr);
    std::cerr << "--\n";
    harmonized_t2->InputSymbols()->WriteText(std::cerr);
    std::cerr << "\n";

    fprintf(stderr, "...TWT::harmonize\n");*/

    return std::pair<StdVectorFst*, StdVectorFst*>(harmonized_t1, harmonized_t2);

  }


  /* Need to check if i_symbol_table and o_symbol_table are compatible! 
     That is to se that there isn't a name "x" s.t. the input symbol number
     of "x" is not the same as its output symbol number.
     Not done yet!!!! */
  /*  void TropicalWeightInputStream::populate_key_table
  (KeyTable &key_table,
   const SymbolTable * i_symbol_table,
   const SymbolTable * o_symbol_table,
   KeyMap &key_map)
  {
    KeyTable transducer_key_table;
    for (unsigned int i = 1; i < i_symbol_table->AvailableKey(); ++i)
      {
	std::string str = i_symbol_table->Find(i);
	const char * string = str.c_str(); 
	if (string != NULL)
	  { transducer_key_table.add_symbol(string); }
	else
	  { 	    
	    ostringstream oss(ostringstream::out);
	    oss << "@EMPTY@" << i;
	    transducer_key_table.add_symbol(oss.str().c_str());
	  }
      }
    for (unsigned int i = 1; i < o_symbol_table->AvailableKey(); ++i)
      {
	std::string str = o_symbol_table->Find(i);
	const char * string = str.c_str(); 
	if (string != NULL)
	  { transducer_key_table.add_symbol(string); }
	else
	  { 	    
	    ostringstream oss(ostringstream::out);
	    oss << "@EMPTY@" << i;
	    transducer_key_table.add_symbol(oss.str().c_str());
	  }
      }
    try
      { transducer_key_table.harmonize(key_map,key_table); }
    catch (const char * p)
      { throw p; }
      }*/

  /* Skip the identifier string "TROPICAL_OFST_TYPE" */
  void TropicalWeightInputStream::skip_identifier_version_3_0(void)
  { input_stream.ignore(19); }

  void TropicalWeightInputStream::skip_hfst_header(void)
  {
    input_stream.ignore(6);
    //char c;
    //i_stream.get(c);
    //switch (c)
    //{
    //case 0:
    skip_identifier_version_3_0();
    //break;
    //default:
    //assert(false);
    //}
  }
  
  void TropicalWeightInputStream::open(void) {}
  void TropicalWeightInputStream::close(void)
  {
    if (filename != string())
      { i_stream.close(); }
  }
  bool TropicalWeightInputStream::is_open(void) const
  { 
    if (filename != string())
      { return i_stream.is_open(); }
    return true;
  }
  bool TropicalWeightInputStream::is_eof(void) const
  {
    return input_stream.peek() == EOF;
  }
  bool TropicalWeightInputStream::is_bad(void) const
  {
    if (filename == string())
      { return std::cin.bad(); }
    else
      { return input_stream.bad(); }    
  }
  bool TropicalWeightInputStream::is_good(void) const
  {
    if(is_eof())
      return false;
    if (filename == string())
      { return std::cin.good(); }
    else
      { return input_stream.good(); }
  }
  
  bool TropicalWeightInputStream::is_fst(void) const
  {
    return is_fst(input_stream);
  }
  
  bool TropicalWeightInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    int c = getc(f);
    ungetc(c, f);
    return c == 0xd6;
  }
  
  bool TropicalWeightInputStream::is_fst(istream &s)
  {
    return s.good() && (s.peek() == 0xd6);
  }

  bool TropicalWeightInputStream::operator() (void) const
  { return is_good(); }

  StdVectorFst * TropicalWeightInputStream::read_transducer(bool has_header)
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    StdVectorFst * t;
    FstHeader header;
    try 
      {
	if (has_header)
	  skip_hfst_header();
	if (filename == string())
	  {
	    header.Read(input_stream,"STDIN");			    
	    t = static_cast<StdVectorFst*>
	      (StdVectorFst::Read(input_stream,
				  FstReadOptions("STDIN",
						 &header)));
	  }
	else
	  {
	    header.Read(input_stream,filename);			    
	    t = static_cast<StdVectorFst*>
	      (StdVectorFst::Read(input_stream,
				  FstReadOptions(filename,
						 &header)));
	  }
	if (t == NULL)
	  { throw TransducerHasWrongTypeException(); }
      }
    catch (TransducerHasWrongTypeException e)
      { throw e; }

    try
      {
	//const SymbolTable * isymbols = t->InputSymbols();
	//const SymbolTable * osymbols = t->OutputSymbols();
	return t;
#ifdef FOO
	if ((isymbols == NULL) and (osymbols == NULL))
	  { return t; }
	KeyMap key_map;
	if (isymbols != NULL)
	  {
	    populate_key_table(key_table,
			       isymbols,
			       osymbols,
			       key_map);
	  }
	StdVectorFst * t_harmonized = NULL;  // FIX THIS
	
	  //TropicalWeightTransducer::harmonize(t,key_map);
	delete t;
	return t_harmonized;
#endif
      }
    catch (HfstInterfaceException e)
      { throw e; }
  }

  /*
  TropicalWeightState::TropicalWeightState(StateId state,
					   StdVectorFst * t):
    state(state), t(t) {}

  TropicalWeightState::TropicalWeightState(const TropicalWeightState &s)
  { this->state = s.state; this->t = s.t; }

  TropicalWeight TropicalWeightState::get_final_weight(void) const
  { return t->Final(state); }
    
  bool TropicalWeightState::operator< 
  (const TropicalWeightState &another) const
  { return state < another.state; }
  
   bool TropicalWeightState::operator== 
  (const TropicalWeightState &another) const
   { return (t == another.t) and (state == another.state); }
  
  bool TropicalWeightState::operator!= 
  (const TropicalWeightState &another) const
  { return not (*this == another); }

  TropicalWeightState::const_iterator 
  TropicalWeightState::begin(void) const
  { return TropicalWeightState::const_iterator(state,t); }

  TropicalWeightState::const_iterator 
  TropicalWeightState::end(void) const
  { return TropicalWeightState::const_iterator(); }

  void TropicalWeightState::print(KeyTable &key_table, ostream &out,
				  TropicalWeightStateIndexer &indexer) const
  {
    for (TropicalWeightState::const_iterator it = begin(); it != end(); ++it)
      { 
	TropicalWeightTransition tr = *it;
	tr.print(key_table,out,indexer);
      }
    if (get_final_weight() != TropicalWeight::Zero())
      { out << state << "\t" << get_final_weight() << std::endl; }
  }

  HfstState TropicalWeightState::get_state_number(void)
  {
    return this->state;
  }

  HfstWeight TropicalWeightState::get_state_weight(void)
  {
    return (t->Final(state)).Value();
  }

  TropicalWeightStateIndexer::TropicalWeightStateIndexer(StdVectorFst * t):
    t(t) {}

  unsigned int TropicalWeightStateIndexer::operator[]
  (const TropicalWeightState &state)
  { return state.state; }

  const TropicalWeightState TropicalWeightStateIndexer::operator[]
  (unsigned int number)
  { return TropicalWeightState(number,t); }

  */


  TropicalWeightStateIterator::TropicalWeightStateIterator(StdVectorFst * t):
    iterator(new StateIterator<StdVectorFst>(*t))
  {}

  TropicalWeightStateIterator::~TropicalWeightStateIterator(void)
  { delete iterator; }

  void TropicalWeightStateIterator::next(void)
  {
    iterator->Next();
  }

  bool TropicalWeightStateIterator::done(void)
  {
    return iterator->Done();
  }

  TropicalWeightState TropicalWeightStateIterator::value(void)
  {
    return iterator->Value();
  }



  TropicalWeightTransition::TropicalWeightTransition(const StdArc &arc, StdVectorFst *t):
    arc(arc), t(t)
  {}

  TropicalWeightTransition::~TropicalWeightTransition(void)
  {}

  std::string TropicalWeightTransition::get_input_symbol(void) const
  {
    return t->InputSymbols()->Find(arc.ilabel);
  }

  std::string TropicalWeightTransition::get_output_symbol(void) const
  {
    return t->InputSymbols()->Find(arc.olabel);
  }

  TropicalWeightState TropicalWeightTransition::get_target_state(void) const
  {
    return arc.nextstate;
  }

  TropicalWeight TropicalWeightTransition::get_weight(void) const
  {
    return arc.weight;
  }



  TropicalWeightTransitionIterator::TropicalWeightTransitionIterator(StdVectorFst *t, StateId state):
    arc_iterator(new ArcIterator<StdVectorFst>(*t, state)),
    t(t)
  {}

  TropicalWeightTransitionIterator::~TropicalWeightTransitionIterator(void)
  {}

  void TropicalWeightTransitionIterator::next()
  {
    arc_iterator->Next();
  }

  bool TropicalWeightTransitionIterator::done()
  {
    return arc_iterator->Done();
  }

  TropicalWeightTransition TropicalWeightTransitionIterator::value()
  {
    return TropicalWeightTransition(arc_iterator->Value(), this->t);
  }



  /*
  TropicalWeightTransition::TropicalWeightTransition
  (const StdArc &arc,StateId source_state,StdVectorFst * t):
    arc(arc), source_state(source_state), t(t) {}
  
  Key TropicalWeightTransition::get_input_key(void) const
  { return arc.ilabel; }

  Key TropicalWeightTransition::get_output_key(void) const
  { return arc.olabel; }

  TropicalWeightState TropicalWeightTransition::get_target_state(void) const
  { return TropicalWeightState(arc.nextstate,t); }

  TropicalWeightState TropicalWeightTransition::get_source_state(void) const
  { return TropicalWeightState(source_state,t); }

  TropicalWeight TropicalWeightTransition::get_weight(void) const
  { return arc.weight; }

  void TropicalWeightTransition::print
  (KeyTable &key_table, ostream &out, TropicalWeightStateIndexer &indexer) 
    const
  {
    (void)indexer;
    out << source_state << "\t"
	<< arc.nextstate << "\t"
	<< key_table[arc.ilabel] << "\t"
	<< key_table[arc.olabel] << "\t"
	<< arc.weight << std::endl;				       
  }

  TropicalWeightTransitionIterator::TropicalWeightTransitionIterator
  (StateId state,StdVectorFst * t):
    arc_iterator(new ArcIterator<StdVectorFst>(*t,state)), state(state), t(t)
  { end_iterator = arc_iterator->Done(); }

  TropicalWeightTransitionIterator::TropicalWeightTransitionIterator(void):
    arc_iterator(NULL),state(0), t(NULL), end_iterator(true)
  {}
  
  TropicalWeightTransitionIterator::~TropicalWeightTransitionIterator(void)
  { delete arc_iterator; }

  void TropicalWeightTransitionIterator::operator=  
  (const TropicalWeightTransitionIterator &another)
  {
    if (this == &another) { return; }
    delete arc_iterator;
    if (another.end_iterator)
      {
	end_iterator = true;
	arc_iterator = NULL;
	state = 0;
	t = NULL;
	return;
      }
    arc_iterator = new ArcIterator<StdVectorFst>(*(another.t),another.state);
    arc_iterator->Seek(another.arc_iterator->Position());
    t = another.t;
    end_iterator = false;
  }

  bool TropicalWeightTransitionIterator::operator== 
  (const TropicalWeightTransitionIterator &another)
  { if (end_iterator and another.end_iterator)
      { return true; }
    if (end_iterator or another.end_iterator)
      { return false; }
    return 
      (t == another.t) and
      (state == another.state) and
      (arc_iterator->Position() == another.arc_iterator->Position());
  }

  bool TropicalWeightTransitionIterator::operator!= 
  (const TropicalWeightTransitionIterator &another)
  { return not (*this == another); }

  const TropicalWeightTransition TropicalWeightTransitionIterator::operator* 
  (void)
  { return TropicalWeightTransition(arc_iterator->Value(),state,t); }

  void TropicalWeightTransitionIterator::operator++ (void)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }

  void TropicalWeightTransitionIterator::operator++ (int)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }
  */

  fst::SymbolTable TropicalWeightTransducer::create_symbol_table(std::string name) {
    fst::SymbolTable st(name);
    st.AddSymbol("@_EPSILON_SYMBOL_@", 0);
    st.AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
    st.AddSymbol("@_IDENTITY_SYMBOL_@", 2);
    return st;
  }
  
  void TropicalWeightTransducer::initialize_symbol_tables(StdVectorFst *t) {
    SymbolTable st = create_symbol_table("");
    t->SetInputSymbols(&st);
    //t->SetOutputSymbols(st);
    return;
  }

  StdVectorFst * TropicalWeightTransducer::create_empty_transducer(void)
  { 
    StdVectorFst * t = new StdVectorFst;
    initialize_symbol_tables(t);
    StateId s = t->AddState();
    t->SetStart(s);
    return t;
  }

  StdVectorFst * TropicalWeightTransducer::create_epsilon_transducer(void)
  { 
    StdVectorFst * t = new StdVectorFst;
    initialize_symbol_tables(t);
    StateId s = t->AddState();
    t->SetStart(s);
    t->SetFinal(s,0);
    return t;
  }

  // could these be removed?
  /*  StdVectorFst * TropicalWeightTransducer::define_transducer(Key k)
  {
    StdVectorFst * t = new StdVectorFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(k,k,0,s2));
    return t;
  }
  StdVectorFst * TropicalWeightTransducer::define_transducer
  (const KeyPair &key_pair)
  {
    StdVectorFst * t = new StdVectorFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(key_pair.first,key_pair.second,0,s2));
    return t;
    }*/


  StdVectorFst * TropicalWeightTransducer::define_transducer(unsigned int number)
  {
    StdVectorFst * t = new StdVectorFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(number,number,0,s2));
    return t;
  }
  StdVectorFst * TropicalWeightTransducer::define_transducer
    (unsigned int inumber, unsigned int onumber)
  {
    StdVectorFst * t = new StdVectorFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(inumber,onumber,0,s2));
    return t;
  }


  StdVectorFst * TropicalWeightTransducer::define_transducer(const std::string &symbol)
  {
    StdVectorFst * t = new StdVectorFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(st.AddSymbol(symbol),
			st.AddSymbol(symbol),0,s2));
    t->SetInputSymbols(&st);
    return t;
  }

  StdVectorFst * TropicalWeightTransducer::define_transducer
    (const std::string &isymbol, const std::string &osymbol)
  {
    StdVectorFst * t = new StdVectorFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(st.AddSymbol(isymbol),
			st.AddSymbol(osymbol),0,s2));
    t->SetInputSymbols(&st);
    return t;
  }

  bool TropicalWeightTransducer::are_equivalent(StdVectorFst *a, StdVectorFst *b) 
  {
    StdVectorFst * mina = minimize(a);
    StdVectorFst * minb = minimize(b);
    //write_in_att_format_number(a, stdout);
    //std::cerr << "--\n";
    //write_in_att_format_number(b, stdout);
    //std::cerr << "\n\n";
    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> enca(*mina, &encode_mapper);
    EncodeFst<StdArc> encb(*minb, &encode_mapper);
    StdVectorFst A(enca);
    StdVectorFst B(encb);
    return Equivalent(A, B);
  }
  
  bool TropicalWeightTransducer::is_cyclic(StdVectorFst * t)
  {
    return t->Properties(kCyclic, true) & kCyclic;
  }

  /*StdVectorFst * TropicalWeightTransducer::define_transducer
  (const KeyPairVector &kpv)
  {
    StdVectorFst * t = new StdVectorFst;
    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (KeyPairVector::const_iterator it = kpv.begin();
	 it != kpv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	t->AddArc(s1,StdArc(it->first,it->second,0,s2));
	s1 = s2;
      }
    t->SetFinal(s1,0);
    return t;
    }*/


  StdVectorFst * TropicalWeightTransducer::define_transducer
  (const StringPairVector &spv)
  {
    StdVectorFst * t = new StdVectorFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (StringPairVector::const_iterator it = spv.begin();
	 it != spv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	t->AddArc(s1,StdArc(st.AddSymbol(it->first),st.AddSymbol(it->second),0,s2));
	s1 = s2;
      }
    t->SetFinal(s1,0);
    t->SetInputSymbols(&st);
    return t;
  }

  StdVectorFst * TropicalWeightTransducer::define_transducer
  (const StringPairSet &sps)
  {
    StdVectorFst * t = new StdVectorFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    t->SetStart(s1);

    if (not sps.empty()) {
      StateId s2 = t->AddState();
      for (StringPairSet::const_iterator it = sps.begin();
	   it != sps.end();
	   ++it)
	{
	  t->AddArc(s1,StdArc(st.AddSymbol(it->first),st.AddSymbol(it->second),0,s2));
	}
      s1 = s2;
    }
    t->SetFinal(s1,0);
    t->SetInputSymbols(&st);
    return t;
  }

  StdVectorFst * 
  TropicalWeightTransducer::copy(StdVectorFst * t)
  { return new StdVectorFst(*t); }

  StdVectorFst * 
  TropicalWeightTransducer::determinize(StdVectorFst * t)
  {
    RmEpsilonFst<StdArc> rm(*t);
    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> enc(rm,
			  &encode_mapper);
    DeterminizeFst<StdArc> det(enc);
    DecodeFst<StdArc> dec(det,
			  encode_mapper);
    return new StdVectorFst(dec);
  }
  
#ifdef FOO
  fst::StdVectorFst * TropicalWeightTransducer::minimize(StdVectorFst * t) {
    fst::RmEpsilon(t);
    fst::EncodeMapper<fst::StdArc> mapper(0x0001,fst::EncodeType(1)); //
    
    fst::EncodeFst<fst::StdArc> TEncode(*t, &mapper);
    fst::StdVectorFst Encoded_T(TEncode);

    fst::StdVectorFst *Determinized_T = new fst::StdVectorFst();
    fst::Determinize(Encoded_T, Determinized_T);

    fst::Minimize(Determinized_T);

    fst::DecodeFst<fst::StdArc> D1(*Determinized_T, mapper);
    fst::StdVectorFst *DecodedT = new fst::StdVectorFst(D1);
    delete Determinized_T;
    return DecodedT;
  }
#endif

  StdVectorFst * TropicalWeightTransducer::minimize
  (StdVectorFst * t)
  {
    StdVectorFst * determinized_t = determinize(t);

    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> enc(*determinized_t,
			  &encode_mapper);
    StdVectorFst fst_enc(enc);
    Minimize<StdArc>(&fst_enc);
    fst::RmEpsilon(&fst_enc);  // For some reason, Minimize creates extra epsilons!

    Decode<StdArc>(&fst_enc, encode_mapper);
    delete determinized_t;

    return new StdVectorFst(fst_enc);
  }

  void print_att_number(StdVectorFst *t, FILE * ofile) {
    fprintf(ofile, "initial state: %i\n", t->Start());
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	if ( t->Final(s) != TropicalWeight::Zero() )
	  fprintf(ofile, "%i\t%f\n", s, t->Final(s).Value());
	for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    fprintf(ofile, "%i\t%i\t%i\t%i\t%f\n", s, arc.nextstate, arc.ilabel, arc.olabel, arc.weight.Value());
	  }
      }
  }

  void TropicalWeightTransducer::test_minimize(void)
  {
    StdVectorFst * t = new StdVectorFst();
    StateId initial = t->AddState();
    t->SetStart(initial);
    StateId state = t->AddState();
    t->SetFinal(state, 0.5);
    t->AddArc(initial, StdArc(1,1,0.5,state));
    t->AddArc(state, StdArc(2,2,0.5,initial));
 
    // print t before minimize
    print_att_number(t, stderr);

    // epsilon removal
    RmEpsilonFst<StdArc> t_rm_eps(*t);

    // encode mapping
    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> t_rm_eps_enc(t_rm_eps,
				   &encode_mapper);

    // determinization
    DeterminizeFst<StdArc> t_DET(t_rm_eps_enc);

    // minimization
    DecodeFst<StdArc> dec(t_DET,
			  encode_mapper);
    StdVectorFst t_det_std(t_DET);

    Minimize<StdArc>(&t_det_std);

    // print t after minimize
    print_att_number(&t_det_std, stderr);

    // decoding
    //Decode<StdArc>(&t_det, encode_mapper);  COMMENTED
  }

  /* For HfstMutableTransducer */

  StateId 
  TropicalWeightTransducer::add_state(StdVectorFst *t)
  { 
    StateId s = t->AddState();
    if (s == 0)
      t->SetStart(s);
    return s;
  }

  void 
  TropicalWeightTransducer::set_final_weight(StdVectorFst *t, StateId s, float w)
  {
    t->SetFinal(s, w);
    return;
  }

  //SymbolTable st = create_symbol_table("");
  //t->SetInputSymbols(&st);

  void 
  TropicalWeightTransducer::add_transition(StdVectorFst *t, StateId source, std::string &isymbol, std::string &osymbol, float w, StateId target)
  {
    SymbolTable *st = t->InputSymbols()->Copy();
    /*if (t->InputSymbols() != t->OutputSymbols()) {
      fprintf(stderr, "ERROR:  TropicalWeightTransducer::add_transition:  input and output symbols are not the same\n"); 
      throw hfst::exceptions::ErrorException(); 
      }*/
    unsigned int ilabel = st->AddSymbol(isymbol);
    unsigned int olabel = st->AddSymbol(osymbol);
    t->AddArc(source, StdArc(ilabel, olabel, w, target));
    t->SetInputSymbols(st);
    delete st;
    return;
  }

  float 
  TropicalWeightTransducer::get_final_weight(StdVectorFst *t, StateId s)
  {
    return t->Final(s).Value();
  }

  float 
  TropicalWeightTransducer::is_final(StdVectorFst *t, StateId s)
  {
    return ( t->Final(s) != TropicalWeight::Zero() );
  }

  StateId
  TropicalWeightTransducer::get_initial_state(StdVectorFst *t)
  {
    return t->Start();
  }

  StdVectorFst * 
  TropicalWeightTransducer::remove_epsilons(StdVectorFst * t)
  { return new StdVectorFst(RmEpsilonFst<StdArc>(*t)); }

  StdVectorFst * 
  TropicalWeightTransducer::n_best(StdVectorFst * t,int n)
  { 
    if (n < 0)
      { throw ImpossibleTransducerPowerException(); }
    StdVectorFst * n_best_fst = new StdVectorFst(); 
    fst::ShortestPath(*t,n_best_fst,(size_t)n);
    return n_best_fst;
  }

  StdVectorFst * 
  TropicalWeightTransducer::repeat_star(StdVectorFst * t)
  { return new StdVectorFst(ClosureFst<StdArc>(*t,CLOSURE_STAR)); }

  StdVectorFst * 
  TropicalWeightTransducer::repeat_plus(StdVectorFst * t)
  { return new StdVectorFst(ClosureFst<StdArc>(*t,CLOSURE_PLUS)); }

  StdVectorFst *
  TropicalWeightTransducer::repeat_n(StdVectorFst * t,int n)
  {
    if (n <= 0)
      { return create_epsilon_transducer(); }

    StdVectorFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      { Concat(repetition,*t); }
    repetition->SetInputSymbols(t->InputSymbols());
    return repetition;
  }

  StdVectorFst *
  TropicalWeightTransducer::repeat_le_n(StdVectorFst * t,int n)
  {
    if (n <= 0)
      { return create_epsilon_transducer(); }

    StdVectorFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      {
	StdVectorFst * optional_t = optionalize(t);
	Concat(repetition,*optional_t);
	delete optional_t;
      }
    repetition->SetInputSymbols(t->InputSymbols());
    return repetition;
  }

  StdVectorFst * 
  TropicalWeightTransducer::optionalize(StdVectorFst * t)
  {
    StdVectorFst * eps = create_epsilon_transducer();
    Union(eps,*t);
    eps->SetInputSymbols(t->InputSymbols());
    return eps;
  }


  StdVectorFst * 
  TropicalWeightTransducer::invert(StdVectorFst * t)
  {
    assert (t->InputSymbols() != NULL);
    StdVectorFst * inverse = copy(t);
    assert (inverse->InputSymbols() != NULL);
    Invert(inverse);
    inverse->SetInputSymbols(t->InputSymbols());
    assert (inverse->InputSymbols() != NULL);
    return inverse;
  }

  /* Makes valgrind angry... */
  StdVectorFst * 
  TropicalWeightTransducer::reverse(StdVectorFst * t)
  {
    StdVectorFst * reversed = new StdVectorFst;
    Reverse<StdArc,StdArc>(*t,reversed);
    reversed->SetInputSymbols(t->InputSymbols());
    return reversed;
  }

  StdVectorFst * TropicalWeightTransducer::extract_input_language
  (StdVectorFst * t)
  { StdVectorFst * retval =  new StdVectorFst(ProjectFst<StdArc>(*t,PROJECT_INPUT)); 
    retval->SetInputSymbols(t->InputSymbols());
    return retval; }

  StdVectorFst * TropicalWeightTransducer::extract_output_language
  (StdVectorFst * t)
  { StdVectorFst * retval = new StdVectorFst(ProjectFst<StdArc>(*t,PROJECT_OUTPUT)); 
    retval->SetInputSymbols(t->InputSymbols());
    return retval; }
  
  typedef std::pair<int,int> LabelPair;
  typedef std::vector<LabelPair> LabelPairVector;

  StdVectorFst * TropicalWeightTransducer::insert_freely
  (StdVectorFst * t, const StringPair &symbol_pair)
  {
    SymbolTable * st = t->InputSymbols()->Copy();
    assert(st != NULL);
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      t->AddArc(state_id, fst::StdArc(st->AddSymbol(symbol_pair.first), st->AddSymbol(symbol_pair.second), 0, state_id));
    }
    t->SetInputSymbols(st);
    delete st;
    return t;
  }

  StdVectorFst * TropicalWeightTransducer::substitute
  (StdVectorFst *t, void (*func)(std::string &isymbol, std::string &osymbol) ) 
  {
    fst::StdVectorFst * tc = t->Copy();
    SymbolTable * st = tc->InputSymbols()->Copy();
    assert(st != NULL);

    for (fst::StateIterator<StdVectorFst> siter(*tc); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	for (fst::MutableArcIterator<StdVectorFst> aiter(tc,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value(); // current values
	    StdArc new_arc;                    // new values
	    
	    std::string istring = st->Find(arc.ilabel);
	    std::string ostring = st->Find(arc.olabel);
	    func(istring,ostring);
	    new_arc.ilabel = st->AddSymbol(istring);
	    new_arc.olabel = st->AddSymbol(ostring);
	    // copy weight and next state as such
	    new_arc.weight = arc.weight.Value();
	    new_arc.nextstate = arc.nextstate;
	    aiter.SetValue(new_arc);
	  }
      }
    tc->SetInputSymbols(st);
    delete st;
    return tc;    
  }

  StdVectorFst * TropicalWeightTransducer::substitute
  (StdVectorFst * t,unsigned int old_key,unsigned int new_key)
  {
    LabelPairVector v;
    v.push_back(LabelPair(old_key,new_key));
    RelabelFst<StdArc> t_subst(*t,v,v);
    return new StdVectorFst(t_subst);
  }
  
  StdVectorFst * TropicalWeightTransducer::substitute(StdVectorFst * t,
						      pair<unsigned int, unsigned int> old_key_pair,
						      pair<unsigned int, unsigned int> new_key_pair)
  {
    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> enc(*t,&encode_mapper);

    StdArc old_pair_code = 
      encode_mapper(StdArc(old_key_pair.first,old_key_pair.second,0,0));
    StdArc new_pair_code =
      encode_mapper(StdArc(new_key_pair.first,new_key_pair.second,0,0));

    // First cast up, then cast down... For some reason dynamic_cast<StdVectorFst*>
    // doesn't work although both EncodeFst<StdArc> and StdVectorFst extend Fst<StdArc>. 
    // reinterpret_cast worked, but that is apparently unsafe...
    StdVectorFst * subst = 
      substitute(static_cast<StdVectorFst*>(static_cast<Fst<StdArc>*>(&enc)),
		 static_cast<unsigned int>(old_pair_code.ilabel),
		 static_cast<unsigned int>(new_pair_code.ilabel));

    DecodeFst<StdArc> dec(*subst,encode_mapper);
    delete subst;
    return new StdVectorFst(dec);
  }

  /* It is not certain whether the transition iterator goes through all the transitions that are added
     during the substitution. In that case, this function should build a new transducer instead of
     modifying the original one. */
  StdVectorFst * TropicalWeightTransducer::substitute(StdVectorFst *t,
						      StringPair old_symbol_pair,
						      StringPairSet new_symbol_pair_set)
  {
    fst::StdVectorFst * tc = t->Copy();
    fst::SymbolTable * st = tc->InputSymbols()->Copy();
    assert(st != NULL);
    for (fst::StateIterator<fst::StdVectorFst> siter(*tc); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	for (fst::MutableArcIterator<StdVectorFst> aiter(tc,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    if ( strcmp( st->Find(arc.ilabel).c_str(), 
			 old_symbol_pair.first.c_str() ) == 0 &&
		 strcmp( st->Find(arc.olabel).c_str(), 
			 old_symbol_pair.second.c_str() ) == 0 )
	      {
		bool first_substitution=true;
		for (StringPairSet::iterator it = new_symbol_pair_set.begin(); 
		     it != new_symbol_pair_set.end(); it++)
		  {
		    if (first_substitution) {
		      StdArc new_arc;
		      new_arc.ilabel = st->AddSymbol(it->first);
		      new_arc.olabel = st->AddSymbol(it->second);
		      new_arc.weight = arc.weight.Value();
		      new_arc.nextstate = arc.nextstate;
		      aiter.SetValue(new_arc); 
		      first_substitution=false; }
		    else
		      tc->AddArc(s, StdArc(st->AddSymbol(it->first), 
					   st->AddSymbol(it->second), 
					   arc.weight.Value(), 
					   arc.nextstate));
		  }
	      }
	  }
      }
    tc->SetInputSymbols(st);
    delete st;
  }

  StdVectorFst * TropicalWeightTransducer::substitute(StdVectorFst *t,
						      std::string old_symbol,
						      std::string new_symbol)
  {
    assert(t->InputSymbols() != NULL);
    SymbolTable * st = t->InputSymbols()->Copy();
    StdVectorFst * retval = substitute(t, st->AddSymbol(old_symbol), st->AddSymbol(new_symbol));
    retval->SetInputSymbols(st);
    delete st;
    return retval;
  }

  StdVectorFst * TropicalWeightTransducer::substitute(StdVectorFst *t,
						      StringPair old_symbol_pair,
						      StringPair new_symbol_pair)
  {
    assert(t->InputSymbols() != NULL);
    SymbolTable * st = t->InputSymbols()->Copy();
    pair<unsigned int, unsigned int> old_pair(st->AddSymbol(old_symbol_pair.first),
					      st->AddSymbol(old_symbol_pair.second));
    pair<unsigned int, unsigned int> new_pair(st->AddSymbol(new_symbol_pair.first),
					      st->AddSymbol(new_symbol_pair.second));
    StdVectorFst * retval = substitute(t, old_pair, new_pair);
    retval->SetInputSymbols(st);
    delete st;
    return retval;
  }

  StdVectorFst * TropicalWeightTransducer::substitute(StdVectorFst *t,
						      const StringPair old_symbol_pair,
						      StdVectorFst *transducer)
  {
    //write_in_att_format(t, stderr);
    //cerr << "--\n";
    //write_in_att_format(transducer, stderr);
    //cerr << "----";

    assert(t->InputSymbols() != NULL);
    SymbolTable * st = t->InputSymbols()->Copy();

    int states = t->NumStates();
    for( int i = 0; i < states; ++i ) {

      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {

	fst::StdArc arc = it.Value();

	// find arcs that must be replaced
	if ( arc.ilabel == st->AddSymbol(old_symbol_pair.first) && 
	     arc.olabel == st->AddSymbol(old_symbol_pair.second) ) 
	  {

	  StateId destination_state = arc.nextstate;
	  StateId start_state = t->AddState();

	  // change the label of the arc to epsilon and point the arc to a new state
	  arc.ilabel = 0;
	  arc.olabel = 0;
	  arc.nextstate = start_state;  
	  // weight remains the same
	  it.SetValue(arc);


	  // add rest of the states to transducer t
	  int states_to_add = transducer->NumStates();
	  for (int j=1; j<states_to_add; j++)
	    t->AddState();


	  // go through all states and arcs in replace transducer tr
	  for (fst::StateIterator<fst::StdFst> siter(*transducer); !siter.Done(); siter.Next()) {

	    StateId tr_state_id = siter.Value();

	    // final states in tr correspond in t to a non-final state which has
	    // an epsilon transition to original destination state of arc that
	    // is being replaced
	    if ( is_final( transducer, tr_state_id ) )
	      t->AddArc( tr_state_id + start_state,
			 fst::StdArc( 0,
				      0,
				      transducer->Final(tr_state_id),  // final weight is copied to the epsilon transition
				      destination_state
				      )
			 );  

	    for (fst::ArcIterator<fst::StdFst> aiter(*transducer, tr_state_id); !aiter.Done(); aiter.Next()) {

	      const fst::StdArc &tr_arc = aiter.Value();

	      // adding arc from state 'tr_state_id+start_state' to state 'tr_arc.nextstate'
	      // copy arcs from tr to t
	      t->AddArc( tr_state_id + start_state, 
			 fst::StdArc( tr_arc.ilabel, 
				      tr_arc.olabel, 
				      tr_arc.weight,  // weight remains the same 
				      tr_arc.nextstate + start_state 
				      ) 
			 );

	    }
	  }
	}
      }
    }

    //write_in_att_format(t, stderr);
    //cerr << "\n\n";

    t->SetInputSymbols(st);
    delete st;
    return t;
  }


  StdVectorFst * TropicalWeightTransducer::compose(StdVectorFst * t1,
			 StdVectorFst * t2)
  {
    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols(t1->InputSymbols());
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols(t2->InputSymbols());

    ArcSort(t1, OLabelCompare<StdArc>());
    ArcSort(t2, ILabelCompare<StdArc>());

    ComposeFst<StdArc> compose(*t1,*t2);
    StdVectorFst *result = new StdVectorFst(compose); 
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  StdVectorFst * TropicalWeightTransducer::compose_intersect
  (StdVectorFst * t, Grammar * grammar)
  {
    //t->SetInputSymbols(NULL);
    //t->SetOutputSymbols(NULL);
    fst::ArcSort<StdArc,fst::OLabelCompare<StdArc> > 
      (t,OLabelCompare<StdArc>());    
    ComposeIntersectFst cif(t,*grammar);
    StdVectorFst * result = cif();
    result->SetInputSymbols(NULL);
    result->SetOutputSymbols(NULL);
    result->SetInputSymbols(grammar->get_first_rule()->InputSymbols() );
    return result;
  }

  StdVectorFst * TropicalWeightTransducer::concatenate(StdVectorFst * t1,
						       StdVectorFst * t2)
  {
    ConcatFst<StdArc> concatenate(*t1,*t2);
    StdVectorFst *result = new StdVectorFst(concatenate); 
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  StdVectorFst * TropicalWeightTransducer::disjunct(StdVectorFst * t1,
						    StdVectorFst * t2)
  {
    UnionFst<StdArc> disjunct(*t1,*t2);
    StdVectorFst *result = new StdVectorFst(disjunct);
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  StdVectorFst * TropicalWeightTransducer::intersect(StdVectorFst * t1,
			   StdVectorFst * t2)
  {
    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols(t1->InputSymbols());
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols(t2->InputSymbols());

    ArcSort(t1, OLabelCompare<StdArc>());
    ArcSort(t2, ILabelCompare<StdArc>());

    RmEpsilonFst<StdArc> rm1(*t1);
    RmEpsilonFst<StdArc> rm2(*t2);
    EncodeMapper<StdArc> encoder(0x0001,ENCODE);
    EncodeFst<StdArc> enc1(rm1, &encoder);
    EncodeFst<StdArc> enc2(rm2, &encoder);
    DeterminizeFst<StdArc> det1(enc1);
    DeterminizeFst<StdArc> det2(enc2);

    IntersectFst<StdArc> intersect(det1,det2);
    StdVectorFst *foo = new StdVectorFst(intersect);
    DecodeFst<StdArc> decode(*foo, encoder);
    delete foo;
    StdVectorFst *result = new StdVectorFst(decode);
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  StdVectorFst * TropicalWeightTransducer::subtract(StdVectorFst * t1,
			  StdVectorFst * t2)
  {
    bool DEBUG=false;

    if (DEBUG) printf("Tropical subtract...\n");

    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols(t1->InputSymbols());
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols(t2->InputSymbols());

    ArcSort(t1, OLabelCompare<StdArc>());
    ArcSort(t2, ILabelCompare<StdArc>());

    RmEpsilonFst<StdArc> rm1(*t1);
    RmEpsilonFst<StdArc> rm2(*t2);

    if (DEBUG) printf("  ..epsilons removed\n");

    EncodeMapper<StdArc> encoder(0x0003,ENCODE); // t2 must be unweighted
    EncodeFst<StdArc> enc1(rm1, &encoder);
    EncodeFst<StdArc> enc2(rm2, &encoder);
    DeterminizeFst<StdArc> det1(enc1);
    DeterminizeFst<StdArc> det2(enc2);

    if (DEBUG) printf("  ..determinized\n");

    StdVectorFst *difference = new StdVectorFst();
    Difference(det1, det2, difference);
    DecodeFst<StdArc> subtract(*difference, encoder);
    delete difference;

    if (DEBUG) printf("  ..subtracted\n");

    //DifferenceFst<StdArc> subtract(enc1,enc2);
    StdVectorFst *result = new StdVectorFst(subtract); 
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  StdVectorFst * TropicalWeightTransducer::set_weight(StdVectorFst * t,float f)
  {
    StdVectorFst * t_copy = new StdVectorFst(*t);
    for (fst::StateIterator<StdVectorFst> iter(*t); 
	 not iter.Done(); iter.Next())
      {
	if (t_copy->Final(iter.Value()) != fst::TropicalWeight::Zero())
	  { t_copy->SetFinal(iter.Value(),f); }
      }
    return t_copy;
  }
  /*
  TropicalWeightTransducer::const_iterator 
  TropicalWeightTransducer::begin(StdVectorFst * t)
  { return TropicalWeightStateIterator(t); }

  TropicalWeightTransducer::const_iterator 
  TropicalWeightTransducer::end(StdVectorFst * t)
  { (void)t;
    return TropicalWeightStateIterator(); }

  void TropicalWeightTransducer::print
  (StdVectorFst * t, KeyTable &key_table, ostream &out) 
  {
    TropicalWeightStateIndexer indexer(t);
    for (TropicalWeightTransducer::const_iterator it = begin(t);
	 it != end(t);
	 ++it)
      { 
	TropicalWeightState s = *it;
	s.print(key_table,out,indexer);
      }
  }
  */


#ifdef foo
  StdVectorFst * TropicalWeightTransducer::harmonize
  (StdVectorFst * t,KeyMap &key_map)
  {
    LabelPairVector v;
    for (KeyMap::iterator it = key_map.begin();
	 it != key_map.end();
	 ++it)
      {
	v.push_back(LabelPair(it->first,it->second));
      }
    RelabelFst<StdArc> t_subst(*t,v,v);
    return new StdVectorFst(t_subst);
  }

  // TODO
  void expand_unknown(StdVectorFst *t, KeyTable key_table, SymbolSet &expand_unknown,
		      SymbolPairSet &expand_non_identity, Symbol unknown_symbol)
  {
    /*
    Key unknown_key = key_table.get_key(unknown_symbol);
      for (fst::StateIterator<StdVectorFst> iter(*t); 
	   not iter.Done(); iter.Next())
	{
	  StateId s = iter.Value();
	  for (fst::ArcIterator<StdVectorFst> it(*t,s); !it.Done(); it.Next())
	    {
	      const StdArc &arc = it.Value();
	      if (arc.ilabel == unknown_key && arc.olabel == unknown_key) {
		
	      }
		
	    }
	  
	    }*/
    (void)t;
    (void)key_table;
    (void)expand_unknown;
    (void)expand_non_identity;
    (void)unknown_symbol;
    return;
  }
#endif
  
  static bool extract_strings(StdVectorFst * t, StdArc::StateId s,
    std::map<StateId,unsigned short> all_visitations, std::map<StateId, unsigned short> path_visitations,
    std::vector<char>& lbuffer, int lpos, std::vector<char>& ubuffer, int upos, float weight_sum,
    hfst::ExtractStringsCb& callback, int cycles,
    std::vector<hfst::FdState<int64> >* fd_state_stack, bool filter_fd)
  { 
    if(cycles >= 0 && path_visitations[s] > cycles)
      return true;
    all_visitations[s]++;
    path_visitations[s]++;
    
    if(lpos > 0 && upos > 0)
    {
      lbuffer[lpos]=0;
      ubuffer[upos]=0;
      bool final = t->Final(s) != TropicalWeight::Zero();
      hfst::WeightedPath<float> path(&lbuffer[0],&ubuffer[0],weight_sum+(final?t->Final(s).Value():0));
      hfst::ExtractStringsCb::RetVal ret = callback(path, final);
      if(!ret.continueSearch || !ret.continuePath)
      {
        path_visitations[s]--;
        return ret.continueSearch;
      }
    }
    
    // sort arcs by number of visitations
    vector<const StdArc*> arcs;
    for(fst::ArcIterator<StdVectorFst> it(*t,s); !it.Done(); it.Next())
    {
      const StdArc& a = it.Value();
      size_t i;
      for( i=0; i<arcs.size(); i++ )
        if (all_visitations[a.nextstate] < all_visitations[arcs[i]->nextstate])
          break;
      arcs.push_back(NULL);
      for( size_t k=arcs.size()-1; k>i; k-- )
        arcs[k] = arcs[k-1];
      arcs[i] = &a;
    }
    
    bool res = true;
    for( size_t i=0; i<arcs.size() && res == true; i++ )
    {
      const StdArc &arc = *(arcs[i]);
      bool added_fd_state = false;
    
      if (fd_state_stack) {
        if(fd_state_stack->back().get_table().get_operation(arc.ilabel) != NULL) {
          fd_state_stack->push_back(fd_state_stack->back());
          if(fd_state_stack->back().apply_operation(arc.ilabel))
            added_fd_state = true;
          else {
            fd_state_stack->pop_back();
            continue; // don't follow the transition
          }
        }
      }
      
      int lp=lpos;
      int up=upos;
      
      if (arc.ilabel != 0 && (!filter_fd || fd_state_stack->back().get_table().get_operation(arc.ilabel)==NULL))
      {
        std::string str = t->InputSymbols()->Find(arc.ilabel);
        if(lpos+str.length() >= lbuffer.size())
          lbuffer.resize(lbuffer.size()*2, 0);
        strcpy(&lbuffer[lpos], str.c_str());
        lp += str.length();
      }
      if (arc.olabel != 0 && (!filter_fd || fd_state_stack->back().get_table().get_operation(arc.olabel)==NULL))
      {
        std::string str = t->InputSymbols()->Find(arc.olabel);
        if(upos+str.length() > ubuffer.size())
          ubuffer.resize(ubuffer.size()*2, 0);
        strcpy(&ubuffer[upos], str.c_str());
        up += str.length();
      }
      
      res = extract_strings(t, arc.nextstate, all_visitations, path_visitations,
          lbuffer,lp, ubuffer,up, weight_sum+arc.weight.Value(), callback, cycles, fd_state_stack, filter_fd);
      
      if(added_fd_state)
        fd_state_stack->pop_back();
    }
    
    path_visitations[s]--;
    return res;
  }
  
  static const int BUFFER_START_SIZE = 64;
  
  void TropicalWeightTransducer::extract_strings(StdVectorFst * t, hfst::ExtractStringsCb& callback,
      int cycles, FdTable<int64>* fd, bool filter_fd)
  {
    if (t->Start() == -1)
      return;
    
    vector<char> lbuffer(BUFFER_START_SIZE, 0);
    vector<char> ubuffer(BUFFER_START_SIZE, 0);
    map<StateId, unsigned short> all_visitations;
    map<StateId, unsigned short> path_visitations;
    std::vector<hfst::FdState<int64> >* fd_state_stack = (fd==NULL) ? NULL : new std::vector<hfst::FdState<int64> >(1, hfst::FdState<int64>(*fd));
    
    hfst::implementations::extract_strings(t,t->Start(),all_visitations,path_visitations,
            lbuffer,0,ubuffer,0,0.0f,callback,cycles,fd_state_stack,filter_fd);
  }
  
  FdTable<int64>* TropicalWeightTransducer::get_flag_diacritics(StdVectorFst * t)
  {
    FdTable<int64>* table = new FdTable<int64>();
    const fst::SymbolTable* symbols = t->InputSymbols();
    for(fst::SymbolTableIterator it=fst::SymbolTableIterator(*symbols); !it.Done(); it.Next())
    {
      if(FdOperation::is_diacritic(it.Symbol()))
        table->define_diacritic(it.Value(), it.Symbol());
    }
    return table;
  }

  void TropicalWeightTransducer::represent_empty_transducer_as_having_one_state(StdVectorFst *t)
  {
    if (t->Start() == fst::kNoStateId || t->NumStates() == 0) {
      delete t;
      t = create_empty_transducer();
    }
    return;
  }



  TropicalWeightOutputStream::TropicalWeightOutputStream(void):
    filename(std::string()), output_stream(std::cout)
  {
    if (!output_stream)
      fprintf(stderr, "TropicalWeightOutputStream: ERROR: failbit set (3).\n");
  }

  TropicalWeightOutputStream::TropicalWeightOutputStream(const char * str):
    filename(str),o_stream(str,std::ios::out),output_stream(o_stream)
  {}

  void TropicalWeightOutputStream::write_3_0_library_header(std::ostream &out)
  {
    if (!out)
      fprintf(stderr, "TropicalWeightOutputStream: ERROR: failbit set (2).\n");
    out.write("HFST3",6);
    //out.put(0);
    out.write("TROPICAL_OFST_TYPE",19);
    //out.put(0);
  }

  void TropicalWeightOutputStream::write_transducer(StdVectorFst * transducer) 
  { 
    if (!output_stream)
      fprintf(stderr, "TropicalWeightOutputStream: ERROR: failbit set (1).\n");
    write_3_0_library_header(output_stream);
    transducer->Write(output_stream,FstWriteOptions()); }

  void TropicalWeightOutputStream::open(void) {}

  void TropicalWeightOutputStream::close(void) 
  {
    if (filename != string())
      { o_stream.close(); }
  }
  }
}

  




#ifdef DEBUG_MAIN
using namespace hfst::implementations;
hfst::symbols::GlobalSymbolTable KeyTable::global_symbol_table;
int main(void) 
{
  TropicalWeightTransducer ofst;
  StdVectorFst * t = ofst.create_empty_transducer();
  for (TropicalWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      TropicalWeightState s = *it;
    }
  delete t;
  t = ofst.create_epsilon_transducer();
  for (TropicalWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      TropicalWeightState s = *it;
    }
  delete t;
  KeyTable key_table;
  key_table.add_symbol("a");
  key_table.add_symbol("b");
  t = ofst.define_transducer(key_table["a"]);
  ofst.print(t,key_table,std::cerr);
  delete t;
  t = ofst.define_transducer(KeyPair(key_table["a"],
				     key_table["b"]));
  ofst.print(t,key_table,std::cerr);
  StdVectorFst * tt;
  tt = ofst.copy(t);
  tt->AddArc(0,StdArc(0,0,0,1));
  ofst.print(tt,key_table,std::cerr);
  StdVectorFst * tt_det = ofst.minimize(tt);
  ofst.print(tt_det,key_table,std::cerr);
  delete t;
  t = ofst.invert(tt);
  delete t;
  delete tt;
  delete tt_det;
  std::cerr << "Test substitution" << std::endl;
  StdVectorFst * fst = ofst.define_transducer(KeyPair(key_table["a"],
						      key_table["b"]));
  std::cerr << "Print a:b" << std::endl;
  ofst.print(fst,key_table,std::cerr);
  key_table.add_symbol("c");
  StdVectorFst * fst1 = ofst.substitute(fst,key_table["a"],key_table["c"]);
  std::cerr << "Print c:b" << std::endl;
  ofst.print(fst1,key_table,std::cerr);
  StdVectorFst * fst2 = 
    ofst.substitute(fst,
		    KeyPair(key_table["a"],key_table["b"]),
		    KeyPair(key_table["c"],key_table["c"]));
  std::cerr << "Print c:c" << std::endl;
  ofst.print(fst2,key_table,std::cerr);
  delete fst;
  delete fst1;
  delete fst2;
  TropicalWeightInputStream input;
  StdVectorFst * input_fst = input.read_transducer(key_table);
  ofst.print(input_fst,key_table,std::cerr);
  delete input_fst;
}
#endif
