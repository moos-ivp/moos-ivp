/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestSet.cpp                                          */
/*    DATE:                                                      */
/*****************************************************************/

#include <set>
#include <iostream>

using namespace std;

int testSet(int argc, char *argv[])
{
  set<string> sset;
  sset.insert("apple");
  sset.insert("banana");
  sset.insert("hello");
  sset.insert("apple");
  
  set<string>::const_iterator sp;
  for(sp=sset.begin(); sp!=sset.end(); sp++) {
    string str = *sp;
    cout << ">" << str << endl;
  }
  
  cout << "size: " << sset.size();
  
  set<string> nset(sset);
  for(sp=nset.begin(); sp!=nset.end(); sp++) {
    string str = *sp;
    cout << ">" << str << endl;
  }
  return(0);
}
