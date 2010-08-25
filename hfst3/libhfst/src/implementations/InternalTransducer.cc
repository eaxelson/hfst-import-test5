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

  }
}
