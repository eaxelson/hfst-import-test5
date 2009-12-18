#include "ConvertTransducerFormat.h"

namespace HFST_IMPLEMENTATIONS
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

InternalTransducer * 
tropical_ofst_to_internal_format(fst::StdVectorFst * t)
{ return new fst::StdVectorFst(*t); }

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
fst::StdVectorFst * internal_format_to_openfst(InternalTransducer * t)
{ return new fst::StdVectorFst(*t); }
}
#ifdef DEBUG_CONVERT
/********************************
 *                              *
 *          TEST MAIN           *
 *                              *
 ********************************/
int main(void)
{
  HFST::KeyTable * key_table = HFST::create_key_table();
  HFST::TransducerHandle t;
  HWFST::TransducerHandle tw;
  if (HFST::read_format() == 0)
    {
      try
	{
	  t = HFST::read_transducer(std::cin,key_table);
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
      HFST::write_transducer(t,key_table);
    }
}
#endif
