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


    HfstInternalTransducer::HfstInternalTransducer(): alphabet(NULL) {}

    HfstInternalTransducer::~HfstInternalTransducer() { delete alphabet; }

    void HfstInternalTransducer::add_line(unsigned int final_state, float final_weight) {
      InternalTransducerLine line; 
      line.final_line=true; 
      line.origin=final_state; 
      line.weight=final_weight; 
      lines.insert(line); 
    }

    void HfstInternalTransducer::add_line(unsigned int origin_state, unsigned int target_state,
					  unsigned int isymbol, unsigned int osymbol,
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

    unsigned int HfstInternalTransducer::max_state_number() {
      unsigned int max=0;
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

    void HfstInternalTransducer::read_symbol(FILE *file) 
    {
      assert(alphabet != NULL);
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
	    add_line( atoi(a1), atoi(a2), alphabet->add_symbol(a3), alphabet->add_symbol(a4), weight );
	  
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

    void HfstInternalTransducer::substitute(const StringPair &sp, const StringPairSet &sps) 
    {
      assert(alphabet != NULL);
      HfstInternalTransducer new_transducer;
      unsigned int inumber = alphabet->symbol2code(sp.first.c_str());
      unsigned int onumber = alphabet->symbol2code(sp.second.c_str());

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
      unsigned int inumber = alphabet->symbol2code(sp.first.c_str());
      unsigned int onumber = alphabet->symbol2code(sp.second.c_str());
      unsigned int max = max_state_number();
      unsigned int max_tr = transducer.max_state_number();
      unsigned int n=0; // this variable is incremented every time a substitution is made

      for (std::set<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {	
	if (it->final_line)  // final lines are added as such
	  new_transducer.add_line(it->origin, it->weight);
	else {

	  // transitions that match sp are substituted with a copy of transducer
	  if ((it->isymbol == inumber) && (it->osymbol == onumber)) 
	    {
	      unsigned int offset = (max + 1) + n*(max_tr + 1);
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
      std::set<unsigned int> visited_states;
      unsigned int in = alphabet->add_symbol(symbol_pair.first.c_str());
      unsigned int out = alphabet->add_symbol(symbol_pair.second.c_str());

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
    
  }
}
