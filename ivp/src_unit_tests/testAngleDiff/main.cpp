/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testAngleDiff)                             */
/*    DATE: Nov 11th, 2018                                       */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}

int main(int argc, char** argv) 
{
  double ang1=0;    bool ang1_set=false;
  double ang2=0;    bool ang2_set=false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "ang1="))
      ang1_set = setDoubleOnString(ang1, argi.substr(5));
    else if(strBegins(argi, "ang2="))
      ang2_set = setDoubleOnString(ang2, argi.substr(5));
    else if((argi=="-h") || (argi=="--help")) {
      cout << "--help not supported. See main.cpp." << endl;
      return(0);
    }
    else if(strBegins(argi, "id="))
      argi = "just ignore id fields";
    else {
      cout << "Error: arg[" << argi << "] Exiting." << endl;
      return(1);
    }
  }
  
  if(!ang1_set) return(cmdLineErr("ang1 is not set. Exiting."));
  if(!ang2_set) return(cmdLineErr("ang2 is not set. Exiting."));

  double diff = angleDiff(ang1, ang2);

  cout << "diff=" << doubleToStringX(diff) << endl;
  return(0);
}
