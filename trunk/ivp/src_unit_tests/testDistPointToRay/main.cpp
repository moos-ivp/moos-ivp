 /****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testDistPointToRay)                        */
/*    DATE: Nov 19th, 2018                                       */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "ArcUtils.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}

int main(int argc, char** argv) 
{
  double cx = 0;    bool cx_set=false;
  double cy = 0;    bool cy_set=false;
  double cr = 0;    bool cr_set=false;
  double px = 0;    bool px_set=false;
  double py = 0;    bool py_set=false;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "cx="))
      cx_set = setDoubleOnString(cx, argi.substr(3));
    else if(strBegins(argi, "cy="))
      cy_set = setDoubleOnString(cy, argi.substr(3));
    else if(strBegins(argi, "cr="))
      cr_set = setDoubleOnString(cr, argi.substr(3));
    else if(strBegins(argi, "px="))
      px_set = setDoubleOnString(px, argi.substr(3));
    else if(strBegins(argi, "py="))
      py_set = setDoubleOnString(py, argi.substr(3));
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
  
  if(!cx_set) return(cmdLineErr("cx is not set. Exiting."));
  if(!cy_set) return(cmdLineErr("cy is not set. Exiting."));
  if(!cr_set) return(cmdLineErr("cr is not set. Exiting."));
  if(!px_set) return(cmdLineErr("px is not set. Exiting."));
  if(!py_set) return(cmdLineErr("py is not set. Exiting."));

  double ix, iy;
  double dist = distPointToRay(px,py, cx,cy,cr, ix,iy);

  cout << "dist=" << doubleToStringX(dist,2);
  cout << ", ix=" << doubleToStringX(ix,2);
  cout << ", iy=" << doubleToStringX(iy,2);

  return(0);
}
