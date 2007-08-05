/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestStringsort.cpp                                   */
/*    DATE:                                                      */
/*****************************************************************/

#include <vector>
#include <string>
#include <iostream>
#include "MBUtils.h"

using namespace std;

int testStringSort(int argc, char *argv[])
{
  vector<string> svector;

  svector.push_back("vanilla");
  svector.push_back("rasberry");
  svector.push_back("bananna");
  svector.push_back("eggplant");
  svector.push_back("fig");
  svector.push_back("grapefruit");
  svector.push_back("honeydew");
  svector.push_back("iguana");
  svector.push_back("nectarine");
  svector.push_back("apple");
  svector.push_back("tomato");
  svector.push_back("lime");
  svector.push_back("yam");
  svector.push_back("mango");
  svector.push_back("cherry");
  svector.push_back("orange");
  svector.push_back("zucchini");
  svector.push_back("quince");
  svector.push_back("strawberry");
  svector.push_back("date");
  svector.push_back("peach");
  svector.push_back("ungulate");
  svector.push_back("watermellon");
  svector.push_back("kiwi");
  svector.push_back("xiqua");
  svector.push_back("jujube");
  
  vector<string> nvector = sortStrings(svector);
  
  for(int i=0; i<nvector.size(); i++) {
    cout << nvector[i] << endl;
  }
  

  return(0);
}
