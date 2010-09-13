#include "InternalTransducer.h"

namespace hfst {
  namespace implementations {
    
    bool InternalTransducerLine::operator<(const InternalTransducerLine &another) const 
    {
      if (this->final_line && another.final_line) {
	if ( this->origin < another.origin )
	  return true;
	return false;
      }
      if (this->final_line) {
	if (this->origin < another.origin)
	  return true;
	return false;
      }
      if (another.final_line) {
	if (this->origin <= another.origin)
	  return true;
	return false;
      }

      if ( this->origin < another.origin ) return true;
      if ( this->origin > another.origin ) return false;

      if ( this->target < another.target ) return true;
      if ( this->target > another.target ) return false;

      if ( this->isymbol < another.isymbol ) return true;
      if ( this->isymbol > another.isymbol ) return false;

      if ( this->osymbol < another.osymbol ) return true;
      if ( this->osymbol > another.osymbol ) return false;

      if ( this->weight < another.weight ) return true;
      if ( this->weight > another.weight ) return false;

      return false;
    }


    HfstInternalTransducer::HfstInternalTransducer(): alphabet(new HfstAlphabet()) {}

    HfstInternalTransducer::~HfstInternalTransducer() { delete alphabet; }

    void HfstInternalTransducer::add_final_state(HfstState s, float weight) {
      add_line(s, weight);
    }

    void HfstInternalTransducer::add_line(HfstState final_state, float final_weight) {
      InternalTransducerLine line; 
      line.final_line=true; 
      line.origin=final_state; 
      line.weight=final_weight; 
      lines.insert(line); 
      final_states.insert(std::pair<HfstState,float>(final_state,final_weight));
    }

    void HfstInternalTransducer::add_transition(HfstTransition &transition) {
      assert(alphabet != NULL);
      InternalTransducerLine line;
      line.final_line = false;
      line.origin = transition.source;
      line.target = transition.target;
      line.isymbol = alphabet->add_symbol(transition.isymbol.c_str());
      line.osymbol = alphabet->add_symbol(transition.osymbol.c_str());
      line.weight = transition.weight;
      lines.insert(line);
    }

    void HfstInternalTransducer::add_line(HfstState origin_state, HfstState target_state,
					  HfstState isymbol, HfstState osymbol,
					  float weight) {
      InternalTransducerLine line;
      line.final_line=false;
      line.origin=origin_state;
      line.target=target_state;
      line.isymbol=isymbol;
      line.osymbol=osymbol;
      line.weight=weight;
      lines.insert(line);
    }
    
    bool HfstInternalTransducer::has_no_lines() { 
      return (lines.size() == 0); } 
    
    std::set<InternalTransducerLine> * HfstInternalTransducer::get_lines() { 
      return &lines; }

    HfstState HfstInternalTransducer::max_state_number() {
      HfstState max=0;
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {
	if (it->origin > max)
	  max = it->origin;
	if ((not it->final_line) && it->target > max)
	  max = it->target;
      }
      return max;
    }

    void HfstInternalTransducer::read_number(FILE *file) 
    {
      char line [255];
      while ( fgets(line, 255, file) != NULL ) 
	{
	  if (*line == '-') // transducer separator
	    return;
	  char a1 [100]; char a2 [100]; char a3 [100]; char a4 [100]; char a5 [100];
	  int n = sscanf(line, "%s\t%s\t%s\t%s\t%s", a1, a2, a3, a4, a5);

	  // set value of weight
	  float weight = 0;
	  if (n == 2)
	    weight = atof(a2);
	  if (n == 5)
	    weight = atof(a5);

	  if (n == 1 || n == 2)  // final state line
	    add_line( atoi(a1), weight );

	  else if (n == 4 || n == 5)  // transition line
	    add_line( atoi(a1), atoi(a2), atoi(a3), atoi(a4), weight );
	  
	  else  // line could not be parsed
	    throw hfst::exceptions::NotValidAttFormatException();       
	}
    }

    void HfstInternalTransducer::read_symbol(FILE *file, const std::string &epsilon_symbol) 
    {
      assert(alphabet != NULL);
      char line [255];
      while ( fgets(line, 255, file) != NULL ) 
	{
	  if (*line == '-') // transducer separator line is "--"
	    return;
	  char a1 [100]; char a2 [100]; char a3 [100]; char a4 [100]; char a5 [100];
	  int n = sscanf(line, "%s\t%s\t%s\t%s\t%s", a1, a2, a3, a4, a5);

	  // set value of weight
	  float weight = 0;
	  if (n == 2)
	    weight = atof(a2);
	  if (n == 5)
	    weight = atof(a5);

	  if (n == 1 || n == 2)  // final state line
	    add_line( atoi(a1), weight );

	  else if (n == 4 || n == 5) { // transition line
	    unsigned int inputnumber, outputnumber;
	    if (epsilon_symbol.compare(std::string(a3)) == 0)
	      inputnumber=0;
	    else
	      inputnumber = alphabet->add_symbol(a3);

	    if (epsilon_symbol.compare(std::string(a4)) == 0)
	      outputnumber=0;
	    else
	      outputnumber = alphabet->add_symbol(a4);

	    add_line( atoi(a1), atoi(a2), inputnumber, outputnumber, weight );
	  }
	  
	  else  // line could not be parsed
	    throw hfst::exceptions::NotValidAttFormatException();       
	}
    }

    void HfstInternalTransducer::print_number(FILE *file) {
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {
	if (it->final_line)
	  fprintf(file, "%i\t%f\n", it->origin, it->weight);
	else
	  fprintf(file, "%i\t%i\t%i\t%i\t%f\n", it->origin, it->target, 
		  it->isymbol, it->osymbol, it->weight);
      }
    }

    void HfstInternalTransducer::print_symbol(FILE *file) {
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {
	if (it->final_line)
	  fprintf(file, "%i\t%f\n", it->origin, it->weight);
	else {
	  if (alphabet != NULL)
	    fprintf(file, "%i\t%i\t%s\t%s\t%f\n", it->origin, it->target, 
		    alphabet->code2symbol(it->isymbol), alphabet->code2symbol(it->osymbol),
		    it->weight);
	  else
	    fprintf(file, "%i\t%i\t\\%i\t\\%i\t%f\n", it->origin, it->target, 
		    it->isymbol, it->osymbol,
		    it->weight);
	    }
      }
    }

    void HfstInternalTransducer::print_number(std::ostream &os) {
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {
	if (it->final_line)
	  os << it->origin << "\t" << it->weight << "\n";
	else
	  os << it->origin << "\t" << it->target << "\t" 
	     << it->isymbol << "\t" << it->osymbol << "\t" 
	     << it->weight << "\n";
      }
    }

    void HfstInternalTransducer::print_symbol(std::ostream &os) {
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {
	if (it->final_line)
	  os << it->origin << "\t" << it->weight << "\n";
	else {
	  if (alphabet != NULL)
	    os << it->origin << "\t" << it->target << "\t" 
	       << alphabet->code2symbol(it->isymbol) << "\t"
	       << alphabet->code2symbol(it->osymbol) << "\t"
	       << it->weight << "\n";
	  else
	    os << it->origin << "\t" << it->target << "\t" 
	       << "\\" << it->isymbol << "\t" << "\\" << it->osymbol << "\t"
	       << it->weight << "\n";
	    }
      }
    }

    void HfstInternalTransducer::substitute(const StringPair &sp, const StringPairSet &sps) 
    {
      assert(alphabet != NULL);
      HfstInternalTransducer new_transducer;
      HfstState inumber = alphabet->symbol2code(sp.first.c_str());
      HfstState onumber = alphabet->symbol2code(sp.second.c_str());

      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {	
	if (it->final_line)  // final lines are added as such
	  new_transducer.add_line(it->origin, it->weight);
	else {
	  // transitions that match sp are substituted with all transitions in sps
	  if ((it->isymbol == inumber) && (it->osymbol == onumber)) {
	    for (StringPairSet::iterator it2 = sps.begin(); it2 != sps.end(); it2++ )
	      new_transducer.add_line(it->origin, it->target, 
				      alphabet->add_symbol(it2->first.c_str()), 
				      alphabet->add_symbol(it2->second.c_str()), 
				      it->weight);
	  }
	  else // transitions that do not match sp are added as such
	    new_transducer.add_line(it->origin, it->target, it->isymbol, it->osymbol, it->weight);
	}
      }

      lines.clear(); // is this needed?
      lines=new_transducer.lines;
    }

    void HfstInternalTransducer::substitute(void (*func)(std::string &isymbol, std::string &osymbol)) 
    {
      assert(alphabet != NULL);
      HfstInternalTransducer new_transducer;
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {	
	if (it->final_line)  // final lines are added as such
	  new_transducer.add_line(it->origin, it->weight);
	else {
	  std::string istring(alphabet->code2symbol(it->isymbol));
	  std::string ostring(alphabet->code2symbol(it->osymbol));
	  func(istring, ostring);
	  new_transducer.add_line(it->origin, it->target, 
				  alphabet->add_symbol(istring.c_str()),
				  alphabet->add_symbol(ostring.c_str()),
				  it->weight);
	}
      }
      lines.clear(); // is this needed?
      lines=new_transducer.lines;
    }
    
    void HfstInternalTransducer::substitute(const StringPair &old_pair, const StringPair &new_pair) 
    {
      assert(alphabet != NULL);
      HfstInternalTransducer new_transducer;
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) 
	{
	  if (it->final_line) { // final lines are added as such
	    new_transducer.add_line(it->origin, it->weight);
	    //printf("..added final line\n");
	  }
	  else {
	    std::string istring(alphabet->code2symbol(it->isymbol));
	    std::string ostring(alphabet->code2symbol(it->osymbol));
	    if( (istring.compare(old_pair.first) == 0) && (ostring.compare(old_pair.second) == 0) ) 
	      {
		istring = new_pair.first;
		ostring = new_pair.second;		
	      }
	    new_transducer.add_line(it->origin, it->target, 
				    alphabet->add_symbol(istring.c_str()),
				    alphabet->add_symbol(ostring.c_str()),
				    it->weight);
	    //printf("..added line\n");
	  }
	}
      lines.clear(); // is this needed?
      lines=new_transducer.lines;
    }      

    void HfstInternalTransducer::substitute(const std::string &old_symbol, const std::string &new_symbol,
					    bool input_side, bool output_side) 
    {
      assert(alphabet != NULL);
      HfstInternalTransducer new_transducer;
      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) 
	{
	  if (it->final_line)  // final lines are added as such
	    new_transducer.add_line(it->origin, it->weight);
	  else {
	    std::string istring(alphabet->code2symbol(it->isymbol));
	    std::string ostring(alphabet->code2symbol(it->osymbol));
	    
	    if (input_side) {
	      if (istring.compare(old_symbol) == 0)
		istring = new_symbol;
	    }
	    if (output_side) {
	      if (ostring.compare(old_symbol) == 0)
		ostring = new_symbol;
	    }
	    
	    new_transducer.add_line(it->origin, it->target, 
				    alphabet->add_symbol(istring.c_str()),
				    alphabet->add_symbol(ostring.c_str()),
				    it->weight);
	  }
	}
      lines.clear(); // is this needed?
      lines=new_transducer.lines;
    }      

    
    /* harmonization must be done before calling this function. */
    void HfstInternalTransducer::substitute(const StringPair &sp, HfstInternalTransducer &transducer) 
    {
      assert(alphabet != NULL);
      HfstInternalTransducer new_transducer;
      HfstState inumber = alphabet->symbol2code(sp.first.c_str());
      HfstState onumber = alphabet->symbol2code(sp.second.c_str());
      HfstState max = max_state_number();
      HfstState max_tr = transducer.max_state_number();
      HfstState n=0; // this variable is incremented every time a substitution is made

      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {	
	if (it->final_line)  // final lines are added as such
	  new_transducer.add_line(it->origin, it->weight);
	else {

	  // transitions that match sp are substituted with a copy of transducer
	  if ((it->isymbol == inumber) && (it->osymbol == onumber)) 
	    {
	      HfstState offset = (max + 1) + n*(max_tr + 1);
	      // epsilon transition to the initial state of the substituting transducer
	      new_transducer.add_line(it->origin, offset, 0, 0, it->weight);

	      std::set<InternalTransducerLine> * tr_lines = transducer.get_lines();
	      for (std::set<InternalTransducerLine>::iterator it2 = tr_lines->begin(); it2 != tr_lines->end(); it2++)
		{
		  if (it2->final_line)  // epsilon transition to the target state of the transition being substituted
		    new_transducer.add_line(offset + it2->origin, it->target, 0, 0, it2->weight);
		  else
		    new_transducer.add_line(offset + it2->origin, offset + it2->target, it2->isymbol, it2->osymbol, it2->weight);
		}
	      n++; // a substitution was done
	    }

	  else // transitions that do not match sp are added as such
	    new_transducer.add_line(it->origin, it->target, it->isymbol, it->osymbol, it->weight);
	}
      }

      lines.clear(); // is this needed?
      lines=new_transducer.lines;
    }

    /* zero weights inserted, does not handle non-final states that lead nowhere */
    void HfstInternalTransducer::insert_freely(const StringPair &symbol_pair) 
    {
      bool DEBUG=false;
      if (DEBUG) {
	printf("HfstInternalTransducer::insert_freely..\n");
	print_symbol(stderr);
	printf("--\n");
      }

      assert(alphabet != NULL);
      HfstInternalTransducer new_transducer;
      std::set<HfstState> visited_states;
      HfstState in = alphabet->add_symbol(symbol_pair.first.c_str());
      HfstState out = alphabet->add_symbol(symbol_pair.second.c_str());

      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {	
	if (visited_states.find(it->origin) == visited_states.end()) {
	  visited_states.insert(it->origin);
	  new_transducer.add_line(it->origin, it->origin, in, out, 0);
	  if (DEBUG)
	    printf("  ..inserted freely state number %i\n", it->origin);
	}
	if (it->final_line)
	  new_transducer.add_line(it->origin, it->weight);
	else
	  new_transducer.add_line(it->origin, it->target, it->isymbol, it->osymbol, it->weight);
      }
      lines.clear(); // is this needed?
      lines=new_transducer.lines;
      if (DEBUG)
	print_symbol(stderr);
    }
   
    bool HfstInternalTransducer::is_final_state(HfstState s) {
      for (std::set<std::pair<HfstState, float> >::iterator it = final_states.begin(); 
	   it != final_states.end(); it++) {	      
	if (it->first == s)
	  return true;
      }
      return false;
    }
    
    float HfstInternalTransducer::get_final_weight(HfstState s) {
      for (std::set<std::pair<HfstState, float> >::iterator it = final_states.begin(); 
	   it != final_states.end(); it++) {	      
	if (it->first == s)
	  return it->second;
      }
      throw hfst::exceptions::StateIsNotFinalException();
    }

    HfstInternalTransducer::HfstInternalTransducer( const HfstInternalTransducer &transducer)
    {
      HfstInternalTransducer * retval = new HfstInternalTransducer();
      lines = transducer.lines;
      final_states = transducer.final_states;
      HfstAlphabet * alpha = new HfstAlphabet(*(transducer.alphabet));
      alphabet = alpha;
    }

    HfstStateIterator::HfstStateIterator(const HfstInternalTransducer &transducer)
    {
      for (std::set<InternalTransducerLine>::iterator it1 = transducer.lines.begin(); 
	   it1 != transducer.lines.end(); it1++) {
	state_set.insert(it1->origin);
	state_set.insert(it1->target);
      }   
      this->it = state_set.begin();
    }

    HfstState HfstStateIterator::value() {
      return *it;
    }

    void HfstStateIterator::next() {
      it++;
    }

    bool HfstStateIterator::done() {
      return (it == state_set.end());
    }

    bool HfstTransition::operator<(const HfstTransition &another) const {
      if (this->source < another.source) return true;
      if (this->source > another.source) return false;

      if (this->target < another.target) return true;
      if (this->target > another.target) return false;
      
      if (this->isymbol.compare(another.isymbol) < 0) return true;
      if (this->isymbol.compare(another.isymbol) > 0) return false;

      if (this->osymbol.compare(another.osymbol) < 0) return true;
      if (this->osymbol.compare(another.osymbol) > 0) return false;

      if ( this->weight < another.weight ) return true;
      if ( this->weight > another.weight ) return false;

      return false;
    }

    HfstTransitionIterator::HfstTransitionIterator(const HfstInternalTransducer &transducer, HfstState s) 
    {
      assert(transducer.alphabet != NULL);
      for (std::set<InternalTransducerLine>::iterator it1 = transducer.lines.begin(); 
	   it1 != transducer.lines.end(); it1++) {
	if (it1->origin == s) {
	  HfstTransition transition;
	  transition.source = it1->origin;
	  transition.target = it1->target;
	  transition.isymbol = std::string(transducer.alphabet->code2symbol(it1->isymbol));
	  transition.osymbol = std::string(transducer.alphabet->code2symbol(it1->osymbol));
	  transition.weight = it1->weight;
	  transition_set.insert(transition);
	}   
      }
      this->it = transition_set.begin();
    }

    HfstTransition HfstTransitionIterator::value() {
      return *it;
    }

    void HfstTransitionIterator::next() {
      it++;
    }

    bool HfstTransitionIterator::done() {
      return (it == transition_set.end());
    }

  }
}
