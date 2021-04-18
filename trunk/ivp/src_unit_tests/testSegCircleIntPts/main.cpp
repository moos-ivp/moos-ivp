/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testSegCircleIntPts)                       */
/*    DATE: Nov 5th, 2018                                        */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}

int main(int argc, char** argv) 
{
  double cx = 0;    bool cx_set=false;
  double cy = 0;    bool cy_set=false;
  double cr = 0;    bool cr_set=false;
  double x1 = 0;    bool x1_set=false;
  double y1 = 0;    bool y1_set=false;
  double x2 = 0;    bool x2_set=false;
  double y2 = 0;    bool y2_set=false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "cx="))
      cx_set = setDoubleOnString(cx, argi.substr(3));
    else if(strBegins(argi, "cy="))
      cy_set = setDoubleOnString(cy, argi.substr(3));
    else if(strBegins(argi, "cr="))
      cr_set = setDoubleOnString(cr, argi.substr(3));
    else if(strBegins(argi, "x1="))
      x1_set = setDoubleOnString(x1, argi.substr(3));
    else if(strBegins(argi, "y1="))
      y1_set = setDoubleOnString(y1, argi.substr(3));
    else if(strBegins(argi, "x2="))
      x2_set = setDoubleOnString(x2, argi.substr(3));
    else if(strBegins(argi, "y2="))
      y2_set = setDoubleOnString(y2, argi.substr(3));
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
  if(!x1_set) return(cmdLineErr("x1 is not set. Exiting."));
  if(!y1_set) return(cmdLineErr("y1 is not set. Exiting."));
  if(!x2_set) return(cmdLineErr("x2 is not set. Exiting."));
  if(!y2_set) return(cmdLineErr("y2 is not set. Exiting."));

  double rx1,ry1,rx2,ry2;
  int num_pts = segCircleIntPts(x1,y1,x2,y2, cx,cy,cr,
				rx1,ry1,rx2,ry2);

  cout << "num_pts=" << num_pts;
  cout << ",rx1=" << rx1 << ", ry1=" << ry1;
  cout << ",rx2=" << rx2 << ", ry2=" << ry2 << endl;
  return(0);
}
