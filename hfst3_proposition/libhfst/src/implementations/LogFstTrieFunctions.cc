#include "LogFstTrieFunctions.h"

namespace HFST_IMPLEMENTATIONS
{
  int has_arc(LogFst &t,
	      LogArc::StateId sourcestate,			  
	      LogArc::Label ilabel, 
	      LogArc::Label olabel)
  {
    for (ArcIterator<LogFst> aiter(t,sourcestate);
	 !aiter.Done();
	 aiter.Next())
      {
	if ((aiter.Value().ilabel == ilabel) and 
	    (aiter.Value().olabel == olabel))
	  { return aiter.Position(); }
      }

    return -1;    
  }

  void disjunct_as_tries(LogFst &t1,
			 StateId t1_state,
			 const LogFst * t2,
			 StateId t2_state)
  {
    if (t2->Final(t2_state) != LogWeight::Zero())
      {
	t1.SetFinal(t1_state,
		    Plus(t1.Final(t1_state),
			 t2->Final(t2_state)));
      }
    for (ArcIterator<LogFst> aiter(*t2,t2_state);
	 !aiter.Done();
	 aiter.Next())
      {
	const LogArc &arc = aiter.Value();
	int arc_index = has_arc(t1,t1_state,arc.ilabel,arc.olabel);
	if (arc_index == -1)
	  {
	    LogArc::StateId new_state = t1.AddState();
	    t1.AddArc(t1_state,LogArc(arc.ilabel,
				      arc.olabel,
				      arc.weight,
				      new_state));
	    add_sub_trie(t1,new_state,t2,arc.nextstate);
	  }
	else
	  {
	    MutableArcIterator<LogFst> ajter(&t1,t1_state);
	    ajter.Seek(arc_index);
	    disjunct_as_tries(t1,ajter.Value().nextstate,t2,arc.nextstate);
	  }
      }
  }
  
  void add_sub_trie(LogFst &t1,
		  StateId t1_state,
		    const LogFst * t2,
		    StateId t2_state)
  {
    if (t2->Final(t2_state) != LogWeight::Zero())
      {
	t1.SetFinal(t1_state,
		    Plus(t1.Final(t1_state),
			 t2->Final(t2_state)));
      }
    for (ArcIterator<LogFst> aiter(*t2,t2_state);
	 !aiter.Done();
	 aiter.Next())
      {
	const LogArc &arc = aiter.Value();
	LogArc::StateId new_state = t1.AddState();
	t1.AddArc(t1_state,LogArc(arc.ilabel,
				  arc.olabel,
				  arc.weight,
				  new_state));
	add_sub_trie(t1,new_state,t2,arc.nextstate);
      }
  }
}
