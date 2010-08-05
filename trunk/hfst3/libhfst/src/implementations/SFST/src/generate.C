/*******************************************************************/
/*                                                                 */
/*  FILE     generate.C                                            */
/*  MODULE   generate                                              */
/*  PROGRAM  SFST                                                  */
/*  AUTHOR   Helmut Schmid, IMS, University of Stuttgart           */
/*                                                                 */
/*  PURPOSE  prints automata                                       */
/*                                                                 */
/*******************************************************************/

#include "fst.h"

// Hfst addition
namespace SFST 
{

using std::vector;

const int BUFFER_SIZE = 10000;


/*******************************************************************/
/*                                                                 */
/*  Transducer::generate1                                          */
/*                                                                 */
/*******************************************************************/

bool Transducer::generate1( Node *node, Node2Int &visitations, char *buffer,
			   int pos, char *buffer2, int pos2, FILE *file  )
{
  bool result = false;

  if (pos > BUFFER_SIZE - 100 || pos2 > BUFFER_SIZE - 100)
    return result;
  if (node->is_final()) {
    buffer[pos] = '\n';
    buffer[pos+1] = 0;
    if (buffer2) {
      buffer2[pos2] = '\t';
      buffer2[pos2+1] = 0;
      fputs(buffer2, file);
    } 
    fputs(buffer, file);
    result = true;
  }

  visitations[node]++;
  // sort arcs by number of visitations
  vector<Arc*> arc;
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *a=p;
    Node *n=a->target_node();
    size_t i;
    for( i=0; i<arc.size(); i++ )
      if (visitations[n] < visitations[arc[i]->target_node()])
	break;
    arc.push_back(NULL);
    for( size_t k=arc.size()-1; k>i; k-- )
      arc[k] = arc[k-1];
    arc[i] = a;
  }
  for( size_t i=0; i<arc.size(); i++ ) {
    int p = pos;
    int p2 = pos2;
    Label l = arc[i]->label();
    if (buffer2) {
      Character lc=l.lower_char();
      Character uc=l.upper_char();
      if (lc != Label::epsilon)
	alphabet.write_char(lc, buffer, &p );
      if (uc != Label::epsilon)
	alphabet.write_char(uc, buffer2, &p2 );
    } else
      alphabet.write_label(l, buffer, &p );
    result |= generate1( arc[i]->target_node(), visitations, 
			 buffer, p, buffer2, p2, file);
  }
  return result;
}


/*******************************************************************/
/*                                                                 */
/*  Transducer::generate                                           */
/*                                                                 */
/*******************************************************************/

bool Transducer::generate( FILE *file, bool separate  )

{
  bool result;
  char buffer[BUFFER_SIZE];
  Node2Int visitations;

  if (separate) {
    char buffer2[BUFFER_SIZE];
    result = generate1( root_node(), visitations, buffer, 0, buffer2, 0,file);
  }
  else
    result = generate1( root_node(), visitations, buffer, 0, NULL, 0, file );

  return result;
}


  // HFST ADDITION

/*******************************************************************/
/*                                                                 */
/*  Transducer::generate_hfst1                                     */
/*                                                                 */
/*******************************************************************/

bool Transducer::generate_hfst1( Node *node, Node2Int &visitations, char *buffer,
				 int pos, char *buffer2, int pos2, hfst::WeightedPaths<float>::Set &results,
				 std::vector<hfst::FdState<Character> >* fd_state_stack, bool filter_fd)
{
  bool result = false;

  if (pos > BUFFER_SIZE - 100 || pos2 > BUFFER_SIZE - 100)
    return result;
  if (node->is_final()) {
    //buffer[pos] = '\n';
    //buffer[pos+1] = 0;
    buffer[pos] = 0;
    if (buffer2) {
      //buffer2[pos2] = '\t';
      //buffer2[pos2+1] = 0;
      buffer[pos2] = 0;
      results.insert( 
	  hfst::WeightedPath<float>(
	  buffer,
	  buffer2,
	  0 ) );
	} 
    else
      results.insert( 
	  hfst::WeightedPath<float>(
	  buffer,
	  buffer,
	  0 ) );

    result = true;
  }

  visitations[node]++;
  // sort arcs by number of visitations
  vector<Arc*> arc;
  for( ArcsIter p(node->arcs()); p; p++ ) {
    Arc *a=p;
    Node *n=a->target_node();
    size_t i;
    for( i=0; i<arc.size(); i++ )
      if (visitations[n] < visitations[arc[i]->target_node()])
	break;
    arc.push_back(NULL);
    for( size_t k=arc.size()-1; k>i; k-- )
      arc[k] = arc[k-1];
    arc[i] = a;
  }
  for( size_t i=0; i<arc.size(); i++ ) {
    int p = pos;
    int p2 = pos2;
    Label l = arc[i]->label();
    bool added_fd_state = false;
    
    if (fd_state_stack) {
      if(fd_state_stack->back().get_table().get_operation(l.lower_char()) != NULL) {
        fd_state_stack->push_back(fd_state_stack->back());
        if(fd_state_stack->back().apply_operation(l.lower_char()))
          added_fd_state = true;
        else {
          fd_state_stack->pop_back();
          continue; // don't follow the transition
        }
      }
    }
    
    if (buffer2) {
      Character lc=l.lower_char();
      Character uc=l.upper_char();
      if (lc != Label::epsilon && (!filter_fd || fd_state_stack->back().get_table().get_operation(lc)==NULL))
	alphabet.write_char(lc, buffer, &p );
      if (uc != Label::epsilon && (!filter_fd || fd_state_stack->back().get_table().get_operation(uc)==NULL))
	alphabet.write_char(uc, buffer2, &p2 );
    } else {
      if(!filter_fd || fd_state_stack->back().get_table().get_operation(l.lower_char())==NULL)
        alphabet.write_label(l, buffer, &p );
    }
    result |= generate_hfst1( arc[i]->target_node(), visitations, 
			 buffer, p, buffer2, p2, results, fd_state_stack, filter_fd);
    if(added_fd_state)
      fd_state_stack->pop_back();
  }
  return result;
}


/*******************************************************************/
/*                                                                 */
/*  Transducer::generate_hfst                                      */
/*                                                                 */
/*******************************************************************/

bool Transducer::generate_hfst( hfst::WeightedPaths<float>::Set &results, hfst::FdTable<Character>* fd, bool filter_fd, bool separate)

{
  bool result;
  char buffer[BUFFER_SIZE];
  Node2Int visitations;
  std::vector<hfst::FdState<Character> >* fd_state_stack = (fd==NULL) ? NULL : new std::vector<hfst::FdState<Character> >(1, hfst::FdState<Character>(*fd));
  if (separate) {
    char buffer2[BUFFER_SIZE];
    result = generate_hfst1( root_node(), visitations, buffer, 0, buffer2, 0, results, fd_state_stack, filter_fd);
  }
  else
    result = generate_hfst1( root_node(), visitations, buffer, 0, NULL, 0, results, fd_state_stack, filter_fd);

  return result;
}
}
