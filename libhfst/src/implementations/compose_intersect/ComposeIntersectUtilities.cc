#include "ComposeIntersectUtilities.h"

#ifdef COMPOSE_INTERSECT_UTILITIES_TEST
#include <iostream>
#include <cassert>
#include <set>
typedef 
hfst::implementations::compose_intersect_utilities::SpaceSavingMap<int,int> 
IntSpaceSavingMap;

struct CmpInt
{ bool operator() (int i, int j) const { return i < j; } };
typedef 
hfst::implementations::compose_intersect_utilities::SpaceSavingSet<int,CmpInt> 
IntSpaceSavingSet;

template<> CmpInt IntSpaceSavingSet::comparator = CmpInt();

int main(void)
{
  IntSpaceSavingMap mmap;
  assert(mmap.size() == 0);
  assert(mmap.has_key(0) == false);

  mmap[0] = 1;
  assert(mmap.size() == 1);
  assert(mmap.has_key(0) == true);
  assert(mmap[0] == 1);

  mmap[0] = 1;
  assert(mmap.size() == 1);
  assert(mmap.has_key(0) == true);
  assert(mmap[0] == 1);

  mmap[0] = 2;
  assert(mmap.size() == 1);
  assert(mmap.has_key(0) == true);
  assert(mmap[0] == 2);
  
  mmap[1] = 3;
  assert(mmap.size() == 2);
  assert(mmap.has_key(0) == true);
  assert(mmap.has_key(1) == true);
  assert(mmap[0] == 2);
  assert(mmap[1] == 3);

  mmap.clear();
  assert(mmap.size() == 0);
  assert(mmap.has_key(0) == false);
  assert(mmap.has_key(1) == false);

  IntSpaceSavingMap::const_iterator it = mmap.begin();
  assert(it == mmap.end());

  mmap[0] = 1;
  it = mmap.begin();
  assert(it != mmap.end());
  assert((*it).first == 0 and (*it).second == 1);
  ++it;
  assert(it == mmap.end());

  mmap[2] = 3;
  it = mmap.begin();
  assert(it != mmap.end());
  assert((*it).first == 0 and (*it).second == 1);
  ++it;
  assert(it != mmap.end());
  assert((*it).first == 2 and (*it).second == 3);
  ++it;
  assert(it == mmap.end());

  IntSpaceSavingSet sset;
  assert(sset.size() == 0);
  assert(not sset.has_element(0));
  
  sset.insert(0);
  assert(sset.size() == 1);
  assert(sset.has_element(0));

  sset.insert(1);
  assert(sset.size() == 2);
  assert(sset.has_element(0));
  assert(sset.has_element(1));

  sset.insert(1);
  sset.insert(1);
  sset.insert(0);
  sset.insert(2);
  sset.insert(4);
  sset.insert(3);
  assert(sset.has_element(0));
  assert(sset.has_element(1));
  assert(sset.has_element(2));
  assert(sset.has_element(3));
  assert(sset.has_element(4));

  sset.clear();
  assert(sset.size() == 0);
  assert(not sset.has_element(0));
  sset.insert(0);
  sset.insert(1);
  assert(sset.has_element(1));
  assert(sset.has_element(0));

  IntSpaceSavingSet::const_iterator jt = sset.begin();
  assert(jt != sset.end());
  assert(*jt == 0);
  ++jt;
  assert(jt != sset.end());
  assert(*jt == 1);
  ++jt;
  assert(jt == sset.end());

  sset.clear();
  assert(sset.size() == 0);
  assert(not sset.has_element(0));
  sset.insert(1);
  sset.insert(0);
  assert(sset.has_element(0));
  assert(sset.has_element(1));
 
  jt = sset.begin();
  assert(jt != sset.end());
  assert(*jt == 0);
  ++jt;
  assert(jt != sset.end());
  assert(*jt == 1);
  ++jt;
  assert(jt == sset.end());

  std::vector<int> v;
  v.push_back(0);
  v.push_back(1);

  for (size_t i = 0; i != 100000; ++i)
    { IntSpaceSavingSet * p = new IntSpaceSavingSet(); p->insert(0); p->insert(1);}
}
#endif // COMPOSE_INTERSECT_UTILITIES_TEST
