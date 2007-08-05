/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestChompString.cpp                                  */
/*    DATE:                                                      */
/*****************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include "MBUtils.h"

using namespace std;

int testChompString(int argc, char *argv[])
{
  cout << "+" << flush;
  string test = "hello:    abc        ";
  vector<string> svector = chompString(test, ':');
  cout << "svector.size():" << svector.size() << endl;
  for(int i=0; i<svector.size(); i++) {
    cout << "svector[" << i << "]:  {" << svector[i] <<  "}"<< endl;
  }
  return(0);
}


