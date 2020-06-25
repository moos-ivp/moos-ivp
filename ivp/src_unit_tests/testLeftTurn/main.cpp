/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testLeftTurn)                              */
/*    DATE: Jun 5th, 2020                                        */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}

int main(int argc, char** argv) 
{
  double x1 = 0;    bool x1_set=false;
  double y1 = 0;    bool y1_set=false;
  double x2 = 0;    bool x2_set=false;
  double y2 = 0;    bool y2_set=false;
  double x3 = 0;    bool x3_set=false;
  double y3 = 0;    bool y3_set=false;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "x1="))
      x1_set = setDoubleOnString(x1, argi.substr(3));
    else if(strBegins(argi, "y1="))
      y1_set = setDoubleOnString(y1, argi.substr(3));
    else if(strBegins(argi, "x2="))
      x2_set = setDoubleOnString(x2, argi.substr(3));
    else if(strBegins(argi, "y2="))
      y2_set = setDoubleOnString(y2, argi.substr(3));
    else if(strBegins(argi, "x3="))
      x3_set = setDoubleOnString(x3, argi.substr(3));
    else if(strBegins(argi, "y3="))
      y3_set = setDoubleOnString(y3, argi.substr(3));
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
  
  if(!x1_set) return(cmdLineErr("x1 is not set. Exiting."));
  if(!y1_set) return(cmdLineErr("y1 is not set. Exiting."));
  if(!x2_set) return(cmdLineErr("x2 is not set. Exiting."));
  if(!y2_set) return(cmdLineErr("y2 is not set. Exiting."));
  if(!x3_set) return(cmdLineErr("x3 is not set. Exiting."));
  if(!y3_set) return(cmdLineErr("y3 is not set. Exiting."));

  //bool left_turn = threePointTurnLeft(x1,y1,x3,y3,x2,y2);
  bool left_turn = threePointTurnLeft(x1,y1,x2,y2,x3,y3);
  
  cout << "left_turn=" << boolToString(left_turn);
  return(0);
}
