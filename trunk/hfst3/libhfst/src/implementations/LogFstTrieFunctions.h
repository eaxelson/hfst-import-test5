#ifndef _LOG_FST_TRIE_FUNCTIONS_
#define _LOG_FST_TRIE_FUNCTIONS_
#include <fst/fstlib.h>
#include <vector>

namespace HFST_IMPLEMENTATIONS
{
  using namespace fst;
  typedef ArcTpl<LogWeight>::StateId StateId;
  typedef ArcTpl<LogWeight> LogArc;
  typedef VectorFst<LogArc> LogFst;
  typedef std::vector<LogArc> LogArcVector;

  struct LogArcLessThan
  {
    bool operator() (const LogArc &arc1,const LogArc &arc2) const;
  };

  void sort_state_arcs(LogFst &t,
		       LogArc::StateId state);

  int binary_search(ArcIterator<LogFst> &aiter,
		    LogArcLessThan &compare,
		    size_t first_arc_num,
		    size_t last_arc_num,
		    LogArc::Label ilabel,
		    LogArc::Label olabel);

  int has_arc(LogFst &t,
	      LogArc::StateId sourcestate,			  
	      LogArc::Label ilabel, 
	      LogArc::Label olabel);
  
  void disjunct_as_tries(LogFst &t1,
			 StateId t1_state,
			 const LogFst * t2,
			 StateId t2_state);

  void add_sub_trie(LogFst &t1,
		    StateId t1_state,
		    const LogFst * t2,
		    StateId t2_state);
}

#endif
