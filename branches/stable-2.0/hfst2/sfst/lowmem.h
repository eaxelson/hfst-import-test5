/*******************************************************************/
/*                                                                 */
/*  FILE     lowmem.h                                              */
/*  MODULE   lowmem                                                */
/*  PROGRAM  SFST                                                  */
/*  AUTHOR   Helmut Schmid, IMS, University of Stuttgart           */
/*                                                                 */
/*  PURPOSE  finite state tools                                    */
/*                                                                 */
/*******************************************************************/

#include <stdio.h>

#include "alphabet.h"

#include <vector>
using std::vector;

using std::pair;


/*****************  class LMArc  ***********************************/

class LMArc {

 public:
  Label label;
  unsigned int tnodepos;

  LMArc( void ) {};
};


/*****************  class LMNode  **********************************/

class LMNode {

 public:
  bool finalp;
  unsigned short number_of_arcs;
  LMArc *arc;

  LMNode( long pos, FILE *lmafile ) {
    fseek(lmafile, pos, SEEK_SET);
    size_t foo;
    foo=fread(&finalp, sizeof(finalp), 1, lmafile);
    foo=fread(&number_of_arcs, sizeof(number_of_arcs), 1, lmafile);
    arc = new LMArc[number_of_arcs];
    for( int i=0; i<(int)number_of_arcs; i++ ) {
      Character lc,uc;
      unsigned int tpos;
      foo=fread(&lc, sizeof(lc), 1, lmafile);
      foo=fread(&uc, sizeof(uc), 1, lmafile);
      foo=fread(&tpos, sizeof(tpos), 1, lmafile);
      arc[i].label = Label(lc,uc);
      arc[i].tnodepos = tpos;
    }
  };

  ~LMNode() { delete[] arc; };
};


/*****************  class LowMemTransducer  *************************/

class LowMemTransducer {
    
protected:
  void analyze( const LMNode&, vector<Character> &input, size_t ipos,
		Analysis&, vector<Analysis>& );

public:
  bool simplest_only;
  FILE *lmafile;
  LMNode *rootnode;
  Alphabet alphabet;
  LowMemTransducer( FILE* );
  ~LowMemTransducer() { delete rootnode; };

  void analyze_string( char *string, vector<Analysis> &analyses );
};
