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

#include "LogWeightTransducer.h"

namespace hfst { namespace implementations
{

  bool openfst_log_use_hopcroft=false;

  void openfst_log_set_hopcroft(bool value) {
    openfst_log_use_hopcroft=value;
  }

  void initialize_symbol_tables(LogFst *t);

  LogWeightInputStream::LogWeightInputStream(void):
    i_stream(),input_stream(cin)
  {}
  LogWeightInputStream::LogWeightInputStream(const char * filename):
    filename(filename),i_stream(filename),input_stream(i_stream)
  {}

  char LogWeightInputStream::stream_get() {
    return (char) input_stream.get(); }

  void LogWeightInputStream::stream_unget(char c) {
    input_stream.putback(c); }

  StringSet LogWeightTransducer::get_string_set(LogFst *t)
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
  NumberNumberMap LogWeightTransducer::create_mapping(LogFst *t1, LogFst *t2)
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
  void LogWeightTransducer::recode_symbol_numbers(LogFst *t, NumberNumberMap &km) 
  {
    for (fst::StateIterator<LogFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	for (fst::MutableArcIterator<LogFst> aiter(t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
	    LogArc new_arc;
	    new_arc.ilabel = km[arc.ilabel];
	    new_arc.olabel = km[arc.olabel];
	    new_arc.weight = arc.weight;
	    new_arc.nextstate = arc.nextstate;
	    aiter.SetValue(new_arc);
	  }
      }
    return;
  }

  LogFst * LogWeightTransducer::set_final_weights(LogFst * t, float weight)
  {
    for (fst::StateIterator<LogFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if ( t->Final(s) != LogWeight::Zero() )
	  t->SetFinal(s, weight);
      }
    return t;
  }

  LogFst * LogWeightTransducer::push_weights(LogFst * t, bool to_initial_state)
  {
    assert (t->InputSymbols() != NULL);
    LogFst * retval = new LogFst();
    if (to_initial_state)
      fst::Push<LogArc, REWEIGHT_TO_INITIAL>(*t, retval, fst::kPushWeights);
    else
      fst::Push<LogArc, REWEIGHT_TO_FINAL>(*t, retval, fst::kPushWeights);
    retval->SetInputSymbols(t->InputSymbols());
    return retval;
  }

  LogFst * LogWeightTransducer::transform_weights(LogFst * t,float (*func)(float f))
  {
    for (fst::StateIterator<LogFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if ( t->Final(s) != LogWeight::Zero() )
	  t->SetFinal( s, func(t->Final(s).Value()) );
	for (fst::MutableArcIterator<LogFst> aiter(t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
	    LogArc new_arc;
	    new_arc.ilabel = arc.ilabel;
	    new_arc.olabel = arc.olabel;
	    new_arc.weight = func(arc.weight.Value());
	    new_arc.nextstate = arc.nextstate;
	    aiter.SetValue(new_arc);
	  }
      }
    return t;
  }

  void LogWeightTransducer::write_in_att_format(LogFst *t, FILE *ofile)
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
      
    for (fst::StateIterator<LogFst> siter(*t); 
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
	for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
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
	if (t->Final(s) != LogWeight::Zero())
	  fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	break;
	}
      }

    for (fst::StateIterator<LogFst> siter(*t); 
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
	  for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const LogArc &arc = aiter.Value();
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
	  if (t->Final(s) != LogWeight::Zero())
	    fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	}
      }
  }


  void LogWeightTransducer::write_in_att_format_number(LogFst *t, FILE *ofile)
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
      
    for (fst::StateIterator<LogFst> siter(*t); 
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
	for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
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
	if (t->Final(s) != LogWeight::Zero())
	  fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	break;
	}
      }

    for (fst::StateIterator<LogFst> siter(*t); 
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
	  for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const LogArc &arc = aiter.Value();
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
	  if (t->Final(s) != LogWeight::Zero())
	    fprintf(ofile, "%i\t%f\n", origin, t->Final(s).Value());
	}
      }
  }


  void LogWeightTransducer::write_in_att_format(LogFst *t, std::ostream &os)
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
      
    for (fst::StateIterator<LogFst> siter(*t); 
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
	for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
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
	if (t->Final(s) != LogWeight::Zero())
	  os << origin << "\t"
	     << t->Final(s).Value() << "\n";
	break;
	}
      }

    for (fst::StateIterator<LogFst> siter(*t); 
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
	  for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const LogArc &arc = aiter.Value();
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
	  if (t->Final(s) != LogWeight::Zero())
	    os << origin << "\t"
	       << t->Final(s).Value() << "\n";
	}
      }
  }


  void LogWeightTransducer::write_in_att_format_number(LogFst *t, std::ostream &os)
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
      
    for (fst::StateIterator<LogFst> siter(*t); 
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
	for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
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
	if (t->Final(s) != LogWeight::Zero())
	  os << origin << "\t"
	     << t->Final(s).Value() << "\n";
	break;
	}
      }

    for (fst::StateIterator<LogFst> siter(*t); 
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
	  for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	    {
	      const LogArc &arc = aiter.Value();
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
	  if (t->Final(s) != LogWeight::Zero())
	    os << origin << "\t"
	       << t->Final(s).Value() << "\n";
	}
      }
  }
  

  // AT&T format is handled here ------------------------------

  /* Maps state numbers in AT&T text format to state ids used by OpenFst transducers. */
  typedef std::map<int, StateId> StateMap;

  // FIX: this would be better in namespace LogWeightTransducer...
  /* A method used by function 'read_in_att_format'.
     Returns the state id of state number state_number and adds a new
     state to t if state_number is encountered for the first time and
     updates state_map accordingly. */
  StateId add_and_map_state(LogFst *t, int state_number, StateMap &state_map)
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
  LogFst * LogWeightTransducer::read_in_att_format(FILE * ifile)
  {
    LogFst *t = new LogFst();
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

	    t->AddArc(origin_state, LogArc(input_number, output_number, weight, target_state));
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
  LogFst * LogWeightTransducer::expand_arcs(LogFst * t, StringSet &unknown)
  {
    //fprintf(stderr, "LogWeightTransducer::expand_arcs...\n");
    LogFst * result = new LogFst();
    std::map<StateId,StateId> state_map;   // maps states of this to states of result

    // go through all states in this
    for (fst::StateIterator<LogFst> siter(*t); 
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
	if (t->Final(s) != LogWeight::Zero())
	  result->SetFinal(result_s, t->Final(s).Value());


	// go through all the arcs in this
	for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();

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
			  result->AddArc(result_s, LogArc(inumber, onumber, arc.weight, result_nextstate));
		      }
		    result->AddArc(result_s, LogArc(inumber, 1, arc.weight, result_nextstate));
		    result->AddArc(result_s, LogArc(1, inumber, arc.weight, result_nextstate));
		  }
	      }
	    else if (arc.ilabel == 2 &&   // identity "?:?"
		     arc.olabel == 2 )       
	      {
		//fprintf(stderr, "identity ?:?\n");
		for (StringSet::iterator it = unknown.begin(); it != unknown.end(); it++) 
		  {
		    int64 number = is->Find(*it);
		    result->AddArc(result_s, LogArc(number, number, arc.weight, result_nextstate));
		    //fprintf(stderr, "added transition %i:%i\n", (int)number, (int)number);
		  }
	      }
	    else if (arc.ilabel == 1)  // "?:x"
	      {
		//fprintf(stderr, "?:x\n");
		for (StringSet::iterator it = unknown.begin(); it != unknown.end(); it++) 
		  {
		    int64 number = is->Find(*it);
		    result->AddArc(result_s, LogArc(number, arc.olabel, arc.weight, result_nextstate));
		    //fprintf(stderr, "added transition %i:%i\n", (int)number, (int)arc.olabel);
		  }
	      }
	    else if (arc.olabel == 1)  // "x:?"
	      {
		//fprintf(stderr, "x:?\n");
		for (StringSet::iterator it = unknown.begin(); it != unknown.end(); it++) 
		  {
		    int64 number = is->Find(*it);
		    result->AddArc(result_s, LogArc(arc.ilabel, number, arc.weight, result_nextstate));
		    //fprintf(stderr, "added transition %i:%i\n", (int)arc.ilabel, (int)number);
		  }
	      }

	    // the original transition is copied in all cases
	    result->AddArc(result_s, LogArc(arc.ilabel, arc.olabel, arc.weight, result_nextstate));		
	    
	  }
      }

     result->SetInputSymbols(t->InputSymbols());
    return result;
  }

  std::pair<LogFst*, LogFst*> LogWeightTransducer::harmonize
  (LogFst *t1, LogFst *t2, bool unknown_symbols_in_use)
  {

    assert(unknown_symbols_in_use);

    // fprintf(stderr, "TWT::harmonize...\n");

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
    
    LogFst *harmonized_t1;
    LogFst *harmonized_t2;

    harmonized_t1 = expand_arcs(t1, unknown_t1);
    harmonized_t1->SetInputSymbols(t1->InputSymbols());
    delete t1;

    harmonized_t2 = expand_arcs(t2, unknown_t2);
    harmonized_t2->SetInputSymbols(t2->InputSymbols());
    delete t2;

    //fprintf(stderr, "TWT::harmonize: harmonized t1's and t2's input symbol tables now contain (FINAL):\n");
    //harmonized_t1->InputSymbols()->WriteText(std::cerr);
    //std::cerr << "--\n";
    //harmonized_t2->InputSymbols()->WriteText(std::cerr);
    //std::cerr << "\n";

    // fprintf(stderr, "...TWT::harmonize\n");

    return std::pair<LogFst*, LogFst*>(harmonized_t1, harmonized_t2);

  }


  /* Need to check if i_symbol_table and o_symbol_table are compatible! 
     That is to se that there isn't a name "x" s.t. the input symbol number
     of "x" is not the same as its output symbol number.
     Not done yet!!!! */
  /*  void LogWeightInputStream::populate_key_table
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

  /* Skip the identifier string "LOG_OFST_TYPE" */
  void LogWeightInputStream::skip_identifier_version_3_0(void)
  { input_stream.ignore(14); }

  void LogWeightInputStream::skip_hfst_header(void)
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
  
  void LogWeightInputStream::close(void)
  {
    if (filename != string())
      { i_stream.close(); }
  }

  bool LogWeightInputStream::is_eof(void) const
  {
    return input_stream.peek() == EOF;
  }
  bool LogWeightInputStream::is_bad(void) const
  {
    if (filename == string())
      { return std::cin.bad(); }
    else
      { return input_stream.bad(); }    
  }
  bool LogWeightInputStream::is_good(void) const
  {
    if(is_eof())
      return false;
    if (filename == string())
      { return std::cin.good(); }
    else
      { return input_stream.good(); }
  }
  
  bool LogWeightInputStream::is_fst(void) const
  {
    return is_fst(input_stream);
  }
  
  bool LogWeightInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    int c = getc(f);
    ungetc(c, f);
    return c == 0xd6;
  }
  
  bool LogWeightInputStream::is_fst(istream &s)
  {
    return s.good() && (s.peek() == 0xd6);
  }

  bool LogWeightInputStream::operator() (void) const
  { return is_good(); }

  void LogWeightInputStream::ignore(unsigned int n)
  { input_stream.ignore(n); }

  LogFst * LogWeightInputStream::read_transducer()
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    LogFst * t;
    FstHeader header;
    try 
      {
	if (filename == string())
	  {
	    header.Read(input_stream,"STDIN");			    
	    t = static_cast<LogFst*>
	      (LogFst::Read(input_stream,
				  FstReadOptions("STDIN",
						 &header)));
	  }
	else
	  {
	    header.Read(input_stream,filename);			    
	    t = static_cast<LogFst*>
	      (LogFst::Read(input_stream,
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
	LogFst * t_harmonized = NULL;  // FIX THIS
	
	  //LogWeightTransducer::harmonize(t,key_map);
	delete t;
	return t_harmonized;
#endif
      }
    catch (HfstInterfaceException e)
      { throw e; }
  }

  /*
  LogWeightState::LogWeightState(StateId state,
					   LogFst * t):
    state(state), t(t) {}

  LogWeightState::LogWeightState(const LogWeightState &s)
  { this->state = s.state; this->t = s.t; }

  LogWeight LogWeightState::get_final_weight(void) const
  { return t->Final(state); }
    
  bool LogWeightState::operator< 
  (const LogWeightState &another) const
  { return state < another.state; }
  
   bool LogWeightState::operator== 
  (const LogWeightState &another) const
   { return (t == another.t) and (state == another.state); }
  
  bool LogWeightState::operator!= 
  (const LogWeightState &another) const
  { return not (*this == another); }

  LogWeightState::const_iterator 
  LogWeightState::begin(void) const
  { return LogWeightState::const_iterator(state,t); }

  LogWeightState::const_iterator 
  LogWeightState::end(void) const
  { return LogWeightState::const_iterator(); }

  void LogWeightState::print(KeyTable &key_table, ostream &out,
				  LogWeightStateIndexer &indexer) const
  {
    for (LogWeightState::const_iterator it = begin(); it != end(); ++it)
      { 
	LogWeightTransition tr = *it;
	tr.print(key_table,out,indexer);
      }
    if (get_final_weight() != LogWeight::Zero())
      { out << state << "\t" << get_final_weight() << std::endl; }
  }

  HfstState LogWeightState::get_state_number(void)
  {
    return this->state;
  }

  HfstWeight LogWeightState::get_state_weight(void)
  {
    return (t->Final(state)).Value();
  }

  LogWeightStateIndexer::LogWeightStateIndexer(LogFst * t):
    t(t) {}

  unsigned int LogWeightStateIndexer::operator[]
  (const LogWeightState &state)
  { return state.state; }

  const LogWeightState LogWeightStateIndexer::operator[]
  (unsigned int number)
  { return LogWeightState(number,t); }

  */


  LogWeightStateIterator::LogWeightStateIterator(LogFst * t):
    iterator(new StateIterator<LogFst>(*t))
  {}

  LogWeightStateIterator::~LogWeightStateIterator(void)
  { delete iterator; }

  void LogWeightStateIterator::next(void)
  {
    iterator->Next();
  }

  bool LogWeightStateIterator::done(void)
  {
    return iterator->Done();
  }

  LogWeightState LogWeightStateIterator::value(void)
  {
    return iterator->Value();
  }



  LogWeightTransition::LogWeightTransition(const LogArc &arc, LogFst *t):
    arc(arc), t(t)
  {}

  LogWeightTransition::~LogWeightTransition(void)
  {}

  std::string LogWeightTransition::get_input_symbol(void) const
  {
    return t->InputSymbols()->Find(arc.ilabel);
  }

  std::string LogWeightTransition::get_output_symbol(void) const
  {
    return t->InputSymbols()->Find(arc.olabel);
  }

  LogWeightState LogWeightTransition::get_target_state(void) const
  {
    return arc.nextstate;
  }

  LogWeight LogWeightTransition::get_weight(void) const
  {
    return arc.weight;
  }



  LogWeightTransitionIterator::LogWeightTransitionIterator(LogFst *t, StateId state):
    arc_iterator(new ArcIterator<LogFst>(*t, state)),
    t(t)
  {}

  LogWeightTransitionIterator::~LogWeightTransitionIterator(void)
  {}

  void LogWeightTransitionIterator::next()
  {
    arc_iterator->Next();
  }

  bool LogWeightTransitionIterator::done()
  {
    return arc_iterator->Done();
  }

  LogWeightTransition LogWeightTransitionIterator::value()
  {
    return LogWeightTransition(arc_iterator->Value(), this->t);
  }



  /*
  LogWeightTransition::LogWeightTransition
  (const LogArc &arc,StateId source_state,LogFst * t):
    arc(arc), source_state(source_state), t(t) {}
  
  Key LogWeightTransition::get_input_key(void) const
  { return arc.ilabel; }

  Key LogWeightTransition::get_output_key(void) const
  { return arc.olabel; }

  LogWeightState LogWeightTransition::get_target_state(void) const
  { return LogWeightState(arc.nextstate,t); }

  LogWeightState LogWeightTransition::get_source_state(void) const
  { return LogWeightState(source_state,t); }

  LogWeight LogWeightTransition::get_weight(void) const
  { return arc.weight; }

  void LogWeightTransition::print
  (KeyTable &key_table, ostream &out, LogWeightStateIndexer &indexer) 
    const
  {
    (void)indexer;
    out << source_state << "\t"
	<< arc.nextstate << "\t"
	<< key_table[arc.ilabel] << "\t"
	<< key_table[arc.olabel] << "\t"
	<< arc.weight << std::endl;				       
  }

  LogWeightTransitionIterator::LogWeightTransitionIterator
  (StateId state,LogFst * t):
    arc_iterator(new ArcIterator<LogFst>(*t,state)), state(state), t(t)
  { end_iterator = arc_iterator->Done(); }

  LogWeightTransitionIterator::LogWeightTransitionIterator(void):
    arc_iterator(NULL),state(0), t(NULL), end_iterator(true)
  {}
  
  LogWeightTransitionIterator::~LogWeightTransitionIterator(void)
  { delete arc_iterator; }

  void LogWeightTransitionIterator::operator=  
  (const LogWeightTransitionIterator &another)
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
    arc_iterator = new ArcIterator<LogFst>(*(another.t),another.state);
    arc_iterator->Seek(another.arc_iterator->Position());
    t = another.t;
    end_iterator = false;
  }

  bool LogWeightTransitionIterator::operator== 
  (const LogWeightTransitionIterator &another)
  { if (end_iterator and another.end_iterator)
      { return true; }
    if (end_iterator or another.end_iterator)
      { return false; }
    return 
      (t == another.t) and
      (state == another.state) and
      (arc_iterator->Position() == another.arc_iterator->Position());
  }

  bool LogWeightTransitionIterator::operator!= 
  (const LogWeightTransitionIterator &another)
  { return not (*this == another); }

  const LogWeightTransition LogWeightTransitionIterator::operator* 
  (void)
  { return LogWeightTransition(arc_iterator->Value(),state,t); }

  void LogWeightTransitionIterator::operator++ (void)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }

  void LogWeightTransitionIterator::operator++ (int)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }
  */

  fst::SymbolTable LogWeightTransducer::create_symbol_table(std::string name) {
    fst::SymbolTable st(name);
    st.AddSymbol("@_EPSILON_SYMBOL_@", 0);
    st.AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
    st.AddSymbol("@_IDENTITY_SYMBOL_@", 2);
    return st;
  }
  
  void LogWeightTransducer::initialize_symbol_tables(LogFst *t) {
    SymbolTable st = create_symbol_table("");
    t->SetInputSymbols(&st);
    //t->SetOutputSymbols(st);
    return;
  }

  LogFst * LogWeightTransducer::create_empty_transducer(void)
  { 
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s = t->AddState();
    t->SetStart(s);
    return t;
  }

  LogFst * LogWeightTransducer::create_epsilon_transducer(void)
  { 
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s = t->AddState();
    t->SetStart(s);
    t->SetFinal(s,0);
    return t;
  }

  // could these be removed?
  /*  LogFst * LogWeightTransducer::define_transducer(Key k)
  {
    LogFst * t = new LogFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(k,k,0,s2));
    return t;
  }
  LogFst * LogWeightTransducer::define_transducer
  (const KeyPair &key_pair)
  {
    LogFst * t = new LogFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(key_pair.first,key_pair.second,0,s2));
    return t;
    }*/


  LogFst * LogWeightTransducer::define_transducer(unsigned int number)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(number,number,0,s2));
    return t;
  }
  LogFst * LogWeightTransducer::define_transducer
    (unsigned int inumber, unsigned int onumber)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(inumber,onumber,0,s2));
    return t;
  }


  LogFst * LogWeightTransducer::define_transducer(const std::string &symbol)
  {
    LogFst * t = new LogFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(st.AddSymbol(symbol),
			st.AddSymbol(symbol),0,s2));
    t->SetInputSymbols(&st);
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer
    (const std::string &isymbol, const std::string &osymbol)
  {
    LogFst * t = new LogFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(st.AddSymbol(isymbol),
			st.AddSymbol(osymbol),0,s2));
    t->SetInputSymbols(&st);
    return t;
  }

  bool LogWeightTransducer::are_equivalent(LogFst *a, LogFst *b) 
  {
    LogFst * mina = minimize(a);
    LogFst * minb = minimize(b);
    //write_in_att_format_number(a, stdout);
    //std::cerr << "--\n";
    //write_in_att_format_number(b, stdout);
    //std::cerr << "\n\n";
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enca(*mina, &encode_mapper);
    EncodeFst<LogArc> encb(*minb, &encode_mapper);
    LogFst A(enca);
    LogFst B(encb);
    return Equivalent(A, B);
  }
  
  bool LogWeightTransducer::is_cyclic(LogFst * t)
  {
    return t->Properties(kCyclic, true) & kCyclic;
  }

  /*LogFst * LogWeightTransducer::define_transducer
  (const KeyPairVector &kpv)
  {
    LogFst * t = new LogFst;
    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (KeyPairVector::const_iterator it = kpv.begin();
	 it != kpv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	t->AddArc(s1,LogArc(it->first,it->second,0,s2));
	s1 = s2;
      }
    t->SetFinal(s1,0);
    return t;
    }*/


  LogFst * LogWeightTransducer::define_transducer
  (const StringPairVector &spv)
  {
    LogFst * t = new LogFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (StringPairVector::const_iterator it = spv.begin();
	 it != spv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	t->AddArc(s1,LogArc(st.AddSymbol(it->first),st.AddSymbol(it->second),0,s2));
	s1 = s2;
      }
    t->SetFinal(s1,0);
    t->SetInputSymbols(&st);
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer
  (const StringPairSet &sps, bool cyclic)
  {
    LogFst * t = new LogFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    t->SetStart(s1);

    if (not sps.empty()) {
      StateId s2 = t->AddState();
      for (StringPairSet::const_iterator it = sps.begin();
	   it != sps.end();
	   ++it)
	{
	  t->AddArc(s1,LogArc(st.AddSymbol(it->first),st.AddSymbol(it->second),0,s2));
	}
      s1 = s2;
    }
    t->SetFinal(s1,0);
    t->SetInputSymbols(&st);
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer
  (const std::vector<StringPairSet> &spsv)
  {
    LogFst * t = new LogFst;
    SymbolTable st = create_symbol_table("");

    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (std::vector<StringPairSet>::const_iterator it = spsv.begin();
	 it != spsv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	for (StringPairSet::const_iterator it2 = (*it).begin(); it2 != (*it).end(); it2++ ) {
	  t->AddArc(s1,LogArc(st.AddSymbol(it2->first),st.AddSymbol(it2->second),0,s2));
	}
	s1 = s2;
      }
    t->SetFinal(s1,0);
    t->SetInputSymbols(&st);
    return t;
  }

  LogFst * 
  LogWeightTransducer::copy(LogFst * t)
  { return new LogFst(*t); }

  LogFst * 
  LogWeightTransducer::determinize(LogFst * t)
  {
    RmEpsilonFst<LogArc> rm(*t);
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enc(rm,
			  &encode_mapper);
    DeterminizeFst<LogArc> det(enc);
    DecodeFst<LogArc> dec(det,
			  encode_mapper);
    return new LogFst(dec);
  }
  
#ifdef FOO
  LogFst * LogWeightTransducer::minimize(LogFst * t) {
    fst::RmEpsilon(t);
    fst::EncodeMapper<fst::LogArc> mapper(0x0001,fst::EncodeType(1)); //
    
    fst::EncodeFst<fst::LogArc> TEncode(*t, &mapper);
    LogFst Encoded_T(TEncode);

    LogFst *Determinized_T = new LogFst();
    fst::Determinize(Encoded_T, Determinized_T);

    fst::Minimize(Determinized_T);

    fst::DecodeFst<fst::LogArc> D1(*Determinized_T, mapper);
    LogFst *DecodedT = new LogFst(D1);
    delete Determinized_T;
    return DecodedT;
  }
#endif

  LogFst * LogWeightTransducer::minimize
  (LogFst * t)
  {
    LogFst * determinized_t = determinize(t);

    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enc(*determinized_t,
			  &encode_mapper);
    LogFst fst_enc(enc);
    Minimize<LogArc>(&fst_enc);
    fst::RmEpsilon(&fst_enc);  // For some reason, Minimize creates extra epsilons!

    Decode<LogArc>(&fst_enc, encode_mapper);
    delete determinized_t;

    return new LogFst(fst_enc);
  }

  void print_att_number(LogFst *t, FILE * ofile) {
    fprintf(ofile, "initial state: %i\n", t->Start());
    for (fst::StateIterator<LogFst> siter(*t); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	if ( t->Final(s) != LogWeight::Zero() )
	  fprintf(ofile, "%i\t%f\n", s, t->Final(s).Value());
	for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
	    fprintf(ofile, "%i\t%i\t%i\t%i\t%f\n", s, arc.nextstate, arc.ilabel, arc.olabel, arc.weight.Value());
	  }
      }
  }

  void LogWeightTransducer::test_minimize(void)
  {
    LogFst * t = new LogFst();
    StateId initial = t->AddState();
    t->SetStart(initial);
    StateId state = t->AddState();
    t->SetFinal(state, 0.5);
    t->AddArc(initial, LogArc(1,1,0.5,state));
    t->AddArc(state, LogArc(2,2,0.5,initial));
 
    // print t before minimize
    print_att_number(t, stderr);

    // epsilon removal
    RmEpsilonFst<LogArc> t_rm_eps(*t);

    // encode mapping
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> t_rm_eps_enc(t_rm_eps,
				   &encode_mapper);

    // determinization
    DeterminizeFst<LogArc> t_DET(t_rm_eps_enc);

    // minimization
    DecodeFst<LogArc> dec(t_DET,
			  encode_mapper);
    LogFst t_det_std(t_DET);

    Minimize<LogArc>(&t_det_std);

    // print t after minimize
    print_att_number(&t_det_std, stderr);

    // decoding
    //Decode<LogArc>(&t_det, encode_mapper);  COMMENTED
  }

  /* For HfstMutableTransducer */

  StateId 
  LogWeightTransducer::add_state(LogFst *t)
  { 
    StateId s = t->AddState();
    if (s == 0)
      t->SetStart(s);
    return s;
  }

  void 
  LogWeightTransducer::set_final_weight(LogFst *t, StateId s, float w)
  {
    t->SetFinal(s, w);
    return;
  }

  //SymbolTable st = create_symbol_table("");
  //t->SetInputSymbols(&st);

  void 
  LogWeightTransducer::add_transition(LogFst *t, StateId source, std::string &isymbol, std::string &osymbol, float w, StateId target)
  {
    SymbolTable *st = t->InputSymbols()->Copy();
    /*if (t->InputSymbols() != t->OutputSymbols()) {
      fprintf(stderr, "ERROR:  LogWeightTransducer::add_transition:  input and output symbols are not the same\n"); 
      throw hfst::exceptions::ErrorException(); 
      }*/
    unsigned int ilabel = st->AddSymbol(isymbol);
    unsigned int olabel = st->AddSymbol(osymbol);
    t->AddArc(source, LogArc(ilabel, olabel, w, target));
    t->SetInputSymbols(st);
    delete st;
    return;
  }

  float 
  LogWeightTransducer::get_final_weight(LogFst *t, StateId s)
  {
    return t->Final(s).Value();
  }

  float 
  LogWeightTransducer::is_final(LogFst *t, StateId s)
  {
    return ( t->Final(s) != LogWeight::Zero() );
  }

  StateId
  LogWeightTransducer::get_initial_state(LogFst *t)
  {
    return t->Start();
  }

  LogFst * 
  LogWeightTransducer::remove_epsilons(LogFst * t)
  { return new LogFst(RmEpsilonFst<LogArc>(*t)); }

  LogFst * 
  LogWeightTransducer::n_best(LogFst * t,int n)
  { 
    if (n < 0)
      { throw ImpossibleTransducerPowerException(); }
    LogFst * n_best_fst = new LogFst(); 
    fst::ShortestPath(*t,n_best_fst,(size_t)n);
    return n_best_fst;
  }

  LogFst * 
  LogWeightTransducer::repeat_star(LogFst * t)
  { return new LogFst(ClosureFst<LogArc>(*t,CLOSURE_STAR)); }

  LogFst * 
  LogWeightTransducer::repeat_plus(LogFst * t)
  { return new LogFst(ClosureFst<LogArc>(*t,CLOSURE_PLUS)); }

  LogFst *
  LogWeightTransducer::repeat_n(LogFst * t,int n)
  {
    if (n <= 0)
      { return create_epsilon_transducer(); }

    LogFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      { Concat(repetition,*t); }
    repetition->SetInputSymbols(t->InputSymbols());
    return repetition;
  }

  LogFst *
  LogWeightTransducer::repeat_le_n(LogFst * t,int n)
  {
    if (n <= 0)
      { return create_epsilon_transducer(); }

    LogFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      {
	LogFst * optional_t = optionalize(t);
	Concat(repetition,*optional_t);
	delete optional_t;
      }
    repetition->SetInputSymbols(t->InputSymbols());
    return repetition;
  }

  LogFst * 
  LogWeightTransducer::optionalize(LogFst * t)
  {
    LogFst * eps = create_epsilon_transducer();
    Union(eps,*t);
    eps->SetInputSymbols(t->InputSymbols());
    return eps;
  }


  LogFst * 
  LogWeightTransducer::invert(LogFst * t)
  {
    assert (t->InputSymbols() != NULL);
    LogFst * inverse = copy(t);
    assert (inverse->InputSymbols() != NULL);
    Invert(inverse);
    inverse->SetInputSymbols(t->InputSymbols());
    assert (inverse->InputSymbols() != NULL);
    return inverse;
  }

  /* Makes valgrind angry... */
  LogFst * 
  LogWeightTransducer::reverse(LogFst * t)
  {
    LogFst * reversed = new LogFst;
    Reverse<LogArc,LogArc>(*t,reversed);
    reversed->SetInputSymbols(t->InputSymbols());
    return reversed;
  }

  LogFst * LogWeightTransducer::extract_input_language
  (LogFst * t)
  { LogFst * retval =  new LogFst(ProjectFst<LogArc>(*t,PROJECT_INPUT)); 
    retval->SetInputSymbols(t->InputSymbols());
    return retval; }

  LogFst * LogWeightTransducer::extract_output_language
  (LogFst * t)
  { LogFst * retval = new LogFst(ProjectFst<LogArc>(*t,PROJECT_OUTPUT)); 
    retval->SetInputSymbols(t->InputSymbols());
    return retval; }
  
  typedef std::pair<int,int> LabelPair;
  typedef std::vector<LabelPair> LabelPairVector;

  LogFst * LogWeightTransducer::insert_freely
  (LogFst * t, const StringPair &symbol_pair)
  {
    SymbolTable * st = t->InputSymbols()->Copy();
    assert(st != NULL);
    for (fst::StateIterator<LogFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      t->AddArc(state_id, fst::LogArc(st->AddSymbol(symbol_pair.first), st->AddSymbol(symbol_pair.second), 0, state_id));
    }
    t->SetInputSymbols(st);
    delete st;
    return t;
  }

  LogFst * LogWeightTransducer::substitute
  (LogFst *t, void (*func)(std::string &isymbol, std::string &osymbol) ) 
  {
    LogFst * tc = t->Copy();
    SymbolTable * st = tc->InputSymbols()->Copy();
    assert(st != NULL);

    for (fst::StateIterator<LogFst> siter(*tc); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	for (fst::MutableArcIterator<LogFst> aiter(tc,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value(); // current values
	    LogArc new_arc;                    // new values
	    
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

  LogFst * LogWeightTransducer::substitute
  (LogFst * t,unsigned int old_key,unsigned int new_key)
  {
    LabelPairVector v;
    v.push_back(LabelPair(old_key,new_key));
    RelabelFst<LogArc> t_subst(*t,v,v);
    return new LogFst(t_subst);
  }
  
  LogFst * LogWeightTransducer::substitute(LogFst * t,
						      pair<unsigned int, unsigned int> old_key_pair,
						      pair<unsigned int, unsigned int> new_key_pair)
  {
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enc(*t,&encode_mapper);

    LogArc old_pair_code = 
      encode_mapper(LogArc(old_key_pair.first,old_key_pair.second,0,0));
    LogArc new_pair_code =
      encode_mapper(LogArc(new_key_pair.first,new_key_pair.second,0,0));

    // First cast up, then cast down... For some reason dynamic_cast<LogFst*>
    // doesn't work although both EncodeFst<LogArc> and LogFst extend Fst<LogArc>. 
    // reinterpret_cast worked, but that is apparently unsafe...
    LogFst * subst = 
      substitute(static_cast<LogFst*>(static_cast<Fst<LogArc>*>(&enc)),
		 static_cast<unsigned int>(old_pair_code.ilabel),
		 static_cast<unsigned int>(new_pair_code.ilabel));

    DecodeFst<LogArc> dec(*subst,encode_mapper);
    delete subst;
    return new LogFst(dec);
  }

  /* It is not certain whether the transition iterator goes through all the transitions that are added
     during the substitution. In that case, this function should build a new transducer instead of
     modifying the original one. */
  LogFst * LogWeightTransducer::substitute(LogFst *t,
						      StringPair old_symbol_pair,
						      StringPairSet new_symbol_pair_set)
  {
    LogFst * tc = t->Copy();
    fst::SymbolTable * st = tc->InputSymbols()->Copy();
    assert(st != NULL);
    for (fst::StateIterator<LogFst> siter(*tc); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	for (fst::MutableArcIterator<LogFst> aiter(tc,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
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
		      LogArc new_arc;
		      new_arc.ilabel = st->AddSymbol(it->first);
		      new_arc.olabel = st->AddSymbol(it->second);
		      new_arc.weight = arc.weight.Value();
		      new_arc.nextstate = arc.nextstate;
		      aiter.SetValue(new_arc); 
		      first_substitution=false; }
		    else
		      tc->AddArc(s, LogArc(st->AddSymbol(it->first), 
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

  LogFst * LogWeightTransducer::substitute(LogFst *t,
						      std::string old_symbol,
						      std::string new_symbol)
  {
    assert(t->InputSymbols() != NULL);
    SymbolTable * st = t->InputSymbols()->Copy();
    LogFst * retval = substitute(t, st->AddSymbol(old_symbol), st->AddSymbol(new_symbol));
    retval->SetInputSymbols(st);
    delete st;
    return retval;
  }

  LogFst * LogWeightTransducer::substitute(LogFst *t,
						      StringPair old_symbol_pair,
						      StringPair new_symbol_pair)
  {
    assert(t->InputSymbols() != NULL);
    SymbolTable * st = t->InputSymbols()->Copy();
    pair<unsigned int, unsigned int> old_pair(st->AddSymbol(old_symbol_pair.first),
					      st->AddSymbol(old_symbol_pair.second));
    pair<unsigned int, unsigned int> new_pair(st->AddSymbol(new_symbol_pair.first),
					      st->AddSymbol(new_symbol_pair.second));
    LogFst * retval = substitute(t, old_pair, new_pair);
    retval->SetInputSymbols(st);
    delete st;
    return retval;
  }

  LogFst * LogWeightTransducer::substitute(LogFst *t,
						      const StringPair old_symbol_pair,
						      LogFst *transducer)
  {
    //write_in_att_format(t, stderr);
    //cerr << "--\n";
    //write_in_att_format(transducer, stderr);
    //cerr << "----";

    assert(t->InputSymbols() != NULL);
    SymbolTable * st = t->InputSymbols()->Copy();

    int states = t->NumStates();
    for( int i = 0; i < states; ++i ) {

      for (fst::MutableArcIterator<LogFst> it(t,i);
	   not it.Done();
	   it.Next()) {

	fst::LogArc arc = it.Value();

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
	  for (fst::StateIterator<LogFst> siter(*transducer); !siter.Done(); siter.Next()) {

	    StateId tr_state_id = siter.Value();

	    // final states in tr correspond in t to a non-final state which has
	    // an epsilon transition to original destination state of arc that
	    // is being replaced
	    if ( is_final( transducer, tr_state_id ) )
	      t->AddArc( tr_state_id + start_state,
			 fst::LogArc( 0,
				      0,
				      transducer->Final(tr_state_id),  // final weight is copied to the epsilon transition
				      destination_state
				      )
			 );  

	    for (fst::ArcIterator<LogFst> aiter(*transducer, tr_state_id); !aiter.Done(); aiter.Next()) {

	      const fst::LogArc &tr_arc = aiter.Value();

	      // adding arc from state 'tr_state_id+start_state' to state 'tr_arc.nextstate'
	      // copy arcs from tr to t
	      t->AddArc( tr_state_id + start_state, 
			 fst::LogArc( tr_arc.ilabel, 
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


  LogFst * LogWeightTransducer::compose(LogFst * t1,
			 LogFst * t2)
  {
    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols(t1->InputSymbols());
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols(t2->InputSymbols());

    ArcSort(t1, OLabelCompare<LogArc>());
    ArcSort(t2, ILabelCompare<LogArc>());

    ComposeFst<LogArc> compose(*t1,*t2);
    LogFst *result = new LogFst(compose); 
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  /*
  LogFst * LogWeightTransducer::compose_intersect
  (LogFst * t, Grammar * grammar)
  {
    //t->SetInputSymbols(NULL);
    //t->SetOutputSymbols(NULL);
    fst::ArcSort<LogArc,fst::OLabelCompare<LogArc> > 
      (t,OLabelCompare<LogArc>());    
    ComposeIntersectFst cif(t,*grammar);
    LogFst * result = cif();
    result->SetInputSymbols(NULL);
    result->SetOutputSymbols(NULL);
    result->SetInputSymbols(grammar->get_first_rule()->InputSymbols() );
    return result;
  }
  */

  LogFst * LogWeightTransducer::concatenate(LogFst * t1,
						       LogFst * t2)
  {
    ConcatFst<LogArc> concatenate(*t1,*t2);
    LogFst *result = new LogFst(concatenate); 
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  LogFst * LogWeightTransducer::disjunct(LogFst * t1,
						    LogFst * t2)
  {
    UnionFst<LogArc> disjunct(*t1,*t2);
    LogFst *result = new LogFst(disjunct);
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  void LogWeightTransducer::disjunct_as_tries(LogFst &t1,
					      const LogFst * t2)
  { HFST_IMPLEMENTATIONS::disjunct_as_tries(t1,t1.Start(),t2,t2->Start()); }

  LogFst * LogWeightTransducer::intersect(LogFst * t1,
			   LogFst * t2)
  {
    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols(t1->InputSymbols());
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols(t2->InputSymbols());

    ArcSort(t1, OLabelCompare<LogArc>());
    ArcSort(t2, ILabelCompare<LogArc>());

    RmEpsilonFst<LogArc> rm1(*t1);
    RmEpsilonFst<LogArc> rm2(*t2);
    EncodeMapper<LogArc> encoder(0x0001,ENCODE);
    EncodeFst<LogArc> enc1(rm1, &encoder);
    EncodeFst<LogArc> enc2(rm2, &encoder);
    DeterminizeFst<LogArc> det1(enc1);
    DeterminizeFst<LogArc> det2(enc2);

    IntersectFst<LogArc> intersect(det1,det2);
    LogFst *foo = new LogFst(intersect);
    DecodeFst<LogArc> decode(*foo, encoder);
    delete foo;
    LogFst *result = new LogFst(decode);
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  LogFst * LogWeightTransducer::subtract(LogFst * t1,
			  LogFst * t2)
  {
    bool DEBUG=true;

    if (DEBUG) printf("Log subtract...\n");

    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols(t1->InputSymbols());
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols(t2->InputSymbols());

    ArcSort(t1, OLabelCompare<LogArc>());
    ArcSort(t2, ILabelCompare<LogArc>());

    RmEpsilonFst<LogArc> rm1(*t1);
    RmEpsilonFst<LogArc> rm2(*t2);

    if (DEBUG) printf("  ..epsilons removed\n");

    EncodeMapper<LogArc> encoder(0x0003,ENCODE); // t2 must be unweighted
    EncodeFst<LogArc> enc1(rm1, &encoder);
    EncodeFst<LogArc> enc2(rm2, &encoder);
    DeterminizeFst<LogArc> det1(enc1);
    DeterminizeFst<LogArc> det2(enc2);

    if (DEBUG) printf("  ..determinized\n");

    LogFst *difference = new LogFst();
    Difference(det1, det2, difference);
    DecodeFst<LogArc> subtract(*difference, encoder);
    delete difference;

    if (DEBUG) printf("  ..subtracted\n");

    //DifferenceFst<LogArc> subtract(enc1,enc2);
    LogFst *result = new LogFst(subtract); 
    result->SetInputSymbols(t1->InputSymbols());
    return result;
  }

  LogFst * LogWeightTransducer::set_weight(LogFst * t,float f)
  {
    LogFst * t_copy = new LogFst(*t);
    for (fst::StateIterator<LogFst> iter(*t); 
	 not iter.Done(); iter.Next())
      {
	if (t_copy->Final(iter.Value()) != fst::LogWeight::Zero())
	  { t_copy->SetFinal(iter.Value(),f); }
      }
    return t_copy;
  }
  /*
  LogWeightTransducer::const_iterator 
  LogWeightTransducer::begin(LogFst * t)
  { return LogWeightStateIterator(t); }

  LogWeightTransducer::const_iterator 
  LogWeightTransducer::end(LogFst * t)
  { (void)t;
    return LogWeightStateIterator(); }

  void LogWeightTransducer::print
  (LogFst * t, KeyTable &key_table, ostream &out) 
  {
    LogWeightStateIndexer indexer(t);
    for (LogWeightTransducer::const_iterator it = begin(t);
	 it != end(t);
	 ++it)
      { 
	LogWeightState s = *it;
	s.print(key_table,out,indexer);
      }
  }
  */


#ifdef foo
  LogFst * LogWeightTransducer::harmonize
  (LogFst * t,KeyMap &key_map)
  {
    LabelPairVector v;
    for (KeyMap::iterator it = key_map.begin();
	 it != key_map.end();
	 ++it)
      {
	v.push_back(LabelPair(it->first,it->second));
      }
    RelabelFst<LogArc> t_subst(*t,v,v);
    return new LogFst(t_subst);
  }

  // TODO
  void expand_unknown(LogFst *t, KeyTable key_table, SymbolSet &expand_unknown,
		      SymbolPairSet &expand_non_identity, Symbol unknown_symbol)
  {
    /*
    Key unknown_key = key_table.get_key(unknown_symbol);
      for (fst::StateIterator<LogFst> iter(*t); 
	   not iter.Done(); iter.Next())
	{
	  StateId s = iter.Value();
	  for (fst::ArcIterator<LogFst> it(*t,s); !it.Done(); it.Next())
	    {
	      const LogArc &arc = it.Value();
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
  
  static bool extract_strings(LogFst * t, LogArc::StateId s,
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
      bool final = t->Final(s) != LogWeight::Zero();
      hfst::WeightedPath<float> path(&lbuffer[0],&ubuffer[0],weight_sum+(final?t->Final(s).Value():0));
      hfst::ExtractStringsCb::RetVal ret = callback(path, final);
      if(!ret.continueSearch || !ret.continuePath)
      {
        path_visitations[s]--;
        return ret.continueSearch;
      }
    }
    
    // sort arcs by number of visitations
    vector<const LogArc*> arcs;
    for(fst::ArcIterator<LogFst> it(*t,s); !it.Done(); it.Next())
    {
      const LogArc& a = it.Value();
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
      const LogArc &arc = *(arcs[i]);
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
  
  void LogWeightTransducer::extract_strings(LogFst * t, hfst::ExtractStringsCb& callback,
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
  
  FdTable<int64>* LogWeightTransducer::get_flag_diacritics(LogFst * t)
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

  void LogWeightTransducer::represent_empty_transducer_as_having_one_state(LogFst *t)
  {
    if (t->Start() == fst::kNoStateId || t->NumStates() == 0) {
      delete t;
      t = create_empty_transducer();
    }
    return;
  }



  LogWeightOutputStream::LogWeightOutputStream(void):
    filename(std::string()), output_stream(std::cout)
  {
    if (!output_stream)
      fprintf(stderr, "LogWeightOutputStream: ERROR: failbit set (3).\n");
  }

  LogWeightOutputStream::LogWeightOutputStream(const char * str):
    filename(str),o_stream(str,std::ios::out),output_stream(o_stream)
  {}

  /*
  void LogWeightOutputStream::write_3_0_library_header(std::ostream &out)
  {
    if (!out)
      fprintf(stderr, "LogWeightOutputStream: ERROR: failbit set (2).\n");
    out.write("HFST3",6);
    //out.put(0);
    out.write("LOG_OFST_TYPE",14);
    //out.put(0);
  }
  */

  void LogWeightOutputStream::write(const char &c)
  {
    output_stream.put(char(c));
  }

  void LogWeightOutputStream::write_transducer(LogFst * transducer) 
  { 
    if (!output_stream)
      fprintf(stderr, "LogWeightOutputStream: ERROR: failbit set (1).\n");
    transducer->Write(output_stream,FstWriteOptions()); }

  void LogWeightOutputStream::close(void) 
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
  LogWeightTransducer ofst;
  LogFst * t = ofst.create_empty_transducer();
  for (LogWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      LogWeightState s = *it;
    }
  delete t;
  t = ofst.create_epsilon_transducer();
  for (LogWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      LogWeightState s = *it;
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
  LogFst * tt;
  tt = ofst.copy(t);
  tt->AddArc(0,LogArc(0,0,0,1));
  ofst.print(tt,key_table,std::cerr);
  LogFst * tt_det = ofst.minimize(tt);
  ofst.print(tt_det,key_table,std::cerr);
  delete t;
  t = ofst.invert(tt);
  delete t;
  delete tt;
  delete tt_det;
  std::cerr << "Test substitution" << std::endl;
  LogFst * fst = ofst.define_transducer(KeyPair(key_table["a"],
						      key_table["b"]));
  std::cerr << "Print a:b" << std::endl;
  ofst.print(fst,key_table,std::cerr);
  key_table.add_symbol("c");
  LogFst * fst1 = ofst.substitute(fst,key_table["a"],key_table["c"]);
  std::cerr << "Print c:b" << std::endl;
  ofst.print(fst1,key_table,std::cerr);
  LogFst * fst2 = 
    ofst.substitute(fst,
		    KeyPair(key_table["a"],key_table["b"]),
		    KeyPair(key_table["c"],key_table["c"]));
  std::cerr << "Print c:c" << std::endl;
  ofst.print(fst2,key_table,std::cerr);
  delete fst;
  delete fst1;
  delete fst2;
  LogWeightInputStream input;
  LogFst * input_fst = input.read_transducer(key_table);
  ofst.print(input_fst,key_table,std::cerr);
  delete input_fst;
}
#endif
