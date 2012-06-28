/*MA****************************************************************/
/*                                                                 */
/*  FILE     make-compact.h                                        */
/*  MODULE   make-compact                                          */
/*  PROGRAM  SFST                                                  */
/*  AUTHOR   Helmut Schmid, IMS, University of Stuttgart           */
/*                                                                 */
/*ME****************************************************************/

#include "fst.h"
#include "compact.h"


class MakeCompactTransducer : CompactTransducer {
    
private:
  void count_arcs(Node *node, NodeNumbering &index, long vmark);
  void store_arcs(Node *node, NodeNumbering &index, long vmark);
  void store_finalp( FILE *file );
  void store_first_arcs( FILE *file );
  void store_target_nodes( FILE *file );
  void store_labels( FILE *file );

public:
  MakeCompactTransducer( Transducer &a, Level sort=upper );

  void sort( Level );
  void store( FILE *file );
};