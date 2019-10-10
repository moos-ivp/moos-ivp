/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testBearingMinMaxToPoly)                   */
/*    DATE: Sep 6th, 2019                                        */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}

int main(int argc, char** argv) 
{
  double px = 0;    bool px_set=false;
  double py = 0;    bool py_set=false;
  XYPolygon poly;   bool poly_set = false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "px="))
      px_set = setDoubleOnString(px, argi.substr(3));
    else if(strBegins(argi, "py="))
      py_set = setDoubleOnString(py, argi.substr(3));

    else if(strBegins(argi, "poly=")) {
      poly = string2Poly(argi.substr(5));
      if(poly.is_convex())
	poly_set = true;
    }
      
    else if((argi=="-h") || (argi=="--help")) {
      cout << "Usage:                                     " << endl;
      cout << "  testBearingMinMaxToPoly                  " << endl;
      cout << "Synopsis:                                  " << endl;
      cout << "  test the bearingMinMaxToPoly() utility   " << endl;
      cout << "Example:                                   " << endl;
      cout << "  $ testBearingMinMaxToPoly px=15 py=-40 \\" << endl;
      cout << "    poly=250,-50:275,-50:275,-25:250,-25   " << endl;
      cout << "bmin=96.58,bmax=97.28                      " << endl;
      return(0);
    }
    else if(strBegins(argi, "id="))
      argi = "just ignore id fields";
    else {
      cout << "Error: arg[" << argi << "] Exiting." << endl;
      return(1);
    }  
  }  
     
  if(!px_set)
    return(cmdLineErr("px is not set. Exiting."));
  if(!py_set)
    return(cmdLineErr("py is not set. Exiting."));
  if(!poly_set)
    return(cmdLineErr("poly is not set. Exiting."));

  double bmin=-1;
  double bmax=-1;
  bearingMinMaxToPoly(px,py, poly, bmin,bmax);

  cout << "bmin="  << doubleToStringX(bmin,2);
  cout << ",bmax=" << doubleToStringX(bmax,2);
  return(0);
}
 
