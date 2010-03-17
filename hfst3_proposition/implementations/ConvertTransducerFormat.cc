#include "ConvertTransducerFormat.h"
#include <stdbool.h>
#include "foma/fomalib.h"

namespace hfst { namespace implementations
{
InternalTransducer * sfst_to_internal_format(SFST::Transducer * t)
{
  InternalTransducer * internal_transducer = new fst::StdVectorFst();
  StateId start_state = internal_transducer->AddState();
  internal_transducer->SetStart(start_state);

  SfstStateVector agenda;
  agenda.push_back(t->root_node());

  SfstToOfstStateMap state_map;
  state_map[t->root_node()] = start_state;

  if (t->root_node()->is_final())
    { internal_transducer->SetFinal(start_state,0); }

  while (not agenda.empty())
    {
      SFST::Node * current_node = agenda.back();
      agenda.pop_back();

      StateId current_state = state_map[current_node];

      for (SFST::ArcsIter it(current_node->arcs()); it; it++)
	{
	  SFST::Arc * arc = it;

	  if (state_map.find(arc->target_node()) == state_map.end())
	    {
	      state_map[arc->target_node()] = internal_transducer->AddState();
	      agenda.push_back(arc->target_node());

	      if (arc->target_node()->is_final())
		{
		  internal_transducer->SetFinal
		    (state_map[arc->target_node()],0);
		}
	    }

	  internal_transducer->AddArc
	    (current_state,
	     fst::StdArc(arc->label().lower_char(),
			 arc->label().upper_char(),
			 0,
			 state_map[arc->target_node()]));
	}
    }
  return internal_transducer;
}


InternalTransducer * foma_to_internal_format(struct fsm * t)
{
  InternalTransducer * internal_transducer = new fst::StdVectorFst();
  FomaToOfstStateMap state_map;
  bool start_state_found=false;
  StateId start_state_id=0;
  
  struct fsm_state *fsm;
  fsm = t->states;
  
  // For every line in foma transducer:
  for (int i=0; (fsm+i)->state_no != -1; i++) {


    // 1. Find the corresponding source state in internal transducer
    //    or create it, if not found.
    StateId source_state;
    if (state_map.find((fsm+i)->state_no) == state_map.end()) {
      source_state = internal_transducer->AddState();
      state_map[(fsm+i)->state_no] = source_state; 
    }
    else
      source_state = state_map[(fsm+i)->state_no];

    // 2. If the source state is an initial state in foma:
    if ((fsm+1)->start_state == 1) {
      // If the start state has not yet been encountered,
      if (not start_state_found) {
	// mark the source state as initial.
	internal_transducer->SetStart(source_state);
	start_state_id = source_state;
	start_state_found=true;
      }
      // If the start state is encountered again, 
      else if (source_state == start_state_id) {
	// do nothing.
      }
      // If there are several initial states in foma transducer,
      else {
	// throw an exception.
	throw TransducerHasMoreThanOneStartStateException();
      }
    }


    // 3. If there are transitions leaving from the state,
    if ((fsm+i)->target != -1) {  

      // find the corresponding source state in internal transducer
      // or create it, if not found, and
      StateId target_state;
      if (state_map.find((fsm+i)->target) == state_map.end()) {
	target_state = internal_transducer->AddState();
	state_map[(fsm+i)->target] = target_state; 
      }
      else
	target_state = state_map[(fsm+i)->target];
      
      // create the transition between source and target states. 
      internal_transducer->AddArc
	(source_state,
	 fst::StdArc((fsm+i)->in,
		     (fsm+i)->out,
		     0,
		     target_state));
    }


    // 4. If the source state is final in foma,
    if ((fsm+i)->final_state == 1) {
      // mark it as final in internal transducer.
      internal_transducer->SetFinal(source_state,0); 
    }

  }

  // If there was not an initial state in foma transducer,
  if (not start_state_found) {
    // throw an exception.
    throw TransducerHasNoStartStateException();
  }

  return internal_transducer;
}


InternalTransducer * 
tropical_ofst_to_internal_format(fst::StdVectorFst * t)
{ return new fst::StdVectorFst(*t); }

InternalTransducer * log_ofst_to_internal_format
(LogFst * t)
{ //return new fst::StdVectorFst(*dynamic_cast<fst::StdVectorFst*>(t)); }
  InternalTransducer * u = new InternalTransducer;
  fst::Cast<LogFst,InternalTransducer>(*t,u);
  return u;
}
SFST::Transducer *  internal_format_to_sfst
(InternalTransducer * internal_transducer)
{
  SFST::Transducer * t = new SFST::Transducer();
  SFST::Node * start_node = t->root_node();

  if (internal_transducer->Start() == fst::kNoStateId)
    { throw TransducerHasNoStartStateException(); }
  StateId start_state = internal_transducer->Start();

  OfstStateVector agenda;
  agenda.push_back(start_state);

  OfstToSfstStateMap state_map;
  state_map[start_state] = start_node;

  if (internal_transducer->Final(start_state) != fst::TropicalWeight::Zero())
    { start_node->set_final(1); }

  while (not agenda.empty())
    {
      StateId current_state = agenda.back();
      agenda.pop_back();

      SFST::Node * current_node = state_map[current_state];

      for (StdArcIterator it(*internal_transducer,current_state);
	   not it.Done();
	   it.Next())
	{
	  const fst::StdArc &arc = it.Value();

	  if (state_map.find(arc.nextstate) == state_map.end())
	    {
	      state_map[arc.nextstate] = t->new_node();
	      if (internal_transducer->Final(arc.nextstate) 
		  != fst::TropicalWeight::Zero())
		{ state_map[arc.nextstate]->set_final(1); }
	      agenda.push_back(arc.nextstate);
	    }

	  current_node->add_arc(SFST::Label(arc.ilabel,arc.olabel),
				state_map[arc.nextstate],
				t);
	}
    }
  return t;
}

// TODO
struct fsm * internal_format_to_foma
(InternalTransducer * internal_transducer)
{ 
  (void)internal_transducer;
  return NULL; 
}

fst::StdVectorFst * internal_format_to_openfst(InternalTransducer * t)
{ return new fst::StdVectorFst(*t); }


LogFst * internal_format_to_log_ofst(InternalTransducer * t)
{ LogFst * u = new LogFst;
  fst::Cast<InternalTransducer,LogFst>(*t,u);
  return u; }
} }
#ifdef DEBUG_CONVERT
/********************************
 *                              *
 *          TEST MAIN           *
 *                              *
 ********************************/
int main(void)
{
  hfst::KeyTable * key_table = hfst::create_key_table();
  hfst::TransducerHandle t;
  HWFST::TransducerHandle tw;
  if (hfst::read_format() == 0)
    {
      try
	{
	  t = hfst::read_transducer(std::cin,key_table);
	}
      catch (const char * p)
	{
	  std::cerr << "ERROR: " << p << std::endl;
	}
      tw = sfst_to_internal_format(t);
      HWFST::write_transducer(tw,key_table);
    }
  else
    {
      try
	{
	  tw = HWFST::read_transducer(std::cin,key_table);
	}
      catch (const char * p)
	{
	  std::cerr << "ERROR: " << p << std::endl;
	}
      t = internal_format_to_sfst(tw);
      hfst::write_transducer(t,key_table);
    }
}
#endif
