/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestMap.cpp                                          */
/*    DATE:                                                      */
/*****************************************************************/

#include <map>
#include <iostream>

using namespace std;

int testMap(int argc, char *argv[])
{
  //------------------------------------------------
  map<string, map<string,double> > meta_map;
  map<string,double> smap;
  
  meta_map["M1"]["hello"] = 10.0;
  meta_map["M2"]["world"] = 11.0;
  meta_map["M3"]["apple"] = 12.0;
  meta_map["M4"]["pear"]  = 13.0;
  meta_map["M4"]["banana"]  = 14.0;
  meta_map["M1"]["rasberry"]  = 15.0;
  meta_map["M1"][""]  = 22.0;
  meta_map[""][""]  = 123.0;
  meta_map[""]["B"]  = 124.0;

  const map<string, map<string,double> > feta_map;

  map<string, map<string, double> >::const_iterator p1;
  map<string, double>::const_iterator p2;
  
  for(p1=meta_map.begin(); p1!=meta_map.end(); p1++) {
    cout << "Outer Map: " << p1->first << endl;
    map<string, double> inner_map = p1->second;
    for(p2=inner_map.begin(); p2!=inner_map.end(); p2++) {
      cout << "  [" << p2->first << "]: " << p2->second << endl;
    }
  } 

  cout << "Result: " << meta_map[""][""] << endl;
  
  return(0);
}
