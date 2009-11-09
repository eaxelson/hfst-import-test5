#include "binSearch.h"

bool lt( vector<Node*> * v1, vector<Node*> * v2 ) {

  vector<Node*>::iterator it;
  vector<Node*>::iterator jt = v2->begin();

  for( it = v1->begin(); it != v1->end(); ++it ) {
    if ( *it < *jt ) { return true; }
    if ( *jt < *it ) { return false; }
    ++jt;
  }
  return false;

}

void bins(vector<vector<Node*>*> * v, int &begin, int &end, vector<Node*> * target, bool &found) {

  int mean = (begin + end)/2;

  if ( lt( v->at(mean),target ) ) {
    begin = mean+1;
    return;
  }
  if ( lt( target,v->at(mean) ) ) {
    end = mean-1;
    return;
}

  found = true;
  begin = mean;
}

int binsearch(vector<vector<Node*>*> * v, vector<Node*> * target, bool &found) {

  found = false;
  int begin = 0;
  int end = v->size() - 1;
  while ( not ( end < begin ) ) {
    bins(v, begin, end, target, found);
    if ( found ) { return begin; }
  }

  return end+1;

}


