#include <vector>
#include <iostream>
#include "fst.h"

using namespace std;


bool lt( vector<Node*> * v1, vector<Node*> * v2);
void bins(vector<vector<Node*>*> * v, int &begin, int &end, vector<Node*> * target, bool &found);
int  binsearch(vector<vector<Node*>*> * v, vector<Node*> * target, bool &found);

