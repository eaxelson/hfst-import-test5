#include "InternalTransducer.h"

namespace hfst {
  namespace implementations {
    
    HfstInternalTransducer::HfstInternalTransducer(): alphabet(NULL) {}

    HfstInternalTransducer::~HfstInternalTransducer() { delete alphabet; }

    void HfstInternalTransducer::add_line(unsigned int final_state, float final_weight) {
      InternalTransducerLine line; 
      line.final_line=true; 
      line.origin=final_state; 
      line.weight=final_weight; 
      lines.push_back(line); 
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
      lines.push_back(line);
    }
    
    bool HfstInternalTransducer::has_no_lines() { 
      return (lines.size() == 0); } 

    std::vector<InternalTransducerLine> * HfstInternalTransducer::get_lines() { 
      return &lines; }

    void HfstInternalTransducer::print_number(FILE *file) {
      for (std::vector<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {
	if (it->final_line)
	  fprintf(file, "%i\t%f\n", it->origin, it->weight);
	else
	  fprintf(file, "%i\t%i\t%i\t%i\t%f\n", it->origin, it->target, 
		  it->isymbol, it->osymbol, it->weight);
      }
    }

    void HfstInternalTransducer::print_symbol(FILE *file) {
      for (std::vector<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {
	if (it->final_line)
	  fprintf(file, "%i\t%f\n", it->origin, it->weight);
	else
	  fprintf(file, "%i\t%i\t%s\t%s\t%f\n", it->origin, it->target, 
		  alphabet->code2symbol(it->isymbol), alphabet->code2symbol(it->osymbol),
		  it->weight);
      }
    }

    void HfstInternalTransducer::substitute(void (*func)(std::string &isymbol, std::string &osymbol)) 
    {
      for (unsigned int i=0; i<lines.size(); i++) {
	if (not lines[i].final_line) {
	  std::string istring(alphabet->code2symbol(lines[i].isymbol));
	  std::string ostring(alphabet->code2symbol(lines[i].osymbol));
	  func(istring, ostring);
	  lines[i].isymbol = alphabet->add_symbol(istring.c_str());
	  lines[i].osymbol = alphabet->add_symbol(ostring.c_str()); 
	}
      }
    }
    
    /* zero weights inserted, does not handle non-final states that lead nowhere */
    void HfstInternalTransducer::insert_freely(const StringPair &symbol_pair) 
    {
      HfstInternalTransducer new_transducer;
      std::set<unsigned int> visited_states;
      unsigned int in = alphabet->add_symbol(symbol_pair.first.c_str());
      unsigned int out = alphabet->add_symbol(symbol_pair.second.c_str());

      for (std::vector<InternalTransducerLine>::iterator it = lines.begin(); 
	   it != lines.end(); it++) {	
	if (visited_states.find(it->origin) != visited_states.end()) {
	  visited_states.insert(it->origin);
	  new_transducer.add_line(it->origin, it->origin, in, out, 0);
	}
	if (it->final_line)
	  new_transducer.add_line(it->origin, it->weight);
	else
	  new_transducer.add_line(it->origin, it->target, it->isymbol, it->osymbol, it->weight);
      }
      lines.clear(); // is this needed?
      lines=new_transducer.lines;
    }
    
  }
}
