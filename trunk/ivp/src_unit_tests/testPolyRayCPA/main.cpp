/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testPolyRayCPA)                            */
/*    DATE: Sep 5th, 2019                                        */
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
  double rx = 0;    bool rx_set=false;
  double ry = 0;    bool ry_set=false;
  double ra = 0;    bool ra_set=false;

  XYPolygon poly;
  bool poly_set = false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "rx="))
      rx_set = setDoubleOnString(rx, argi.substr(3));
    else if(strBegins(argi, "ry="))
      ry_set = setDoubleOnString(ry, argi.substr(3));
    else if(strBegins(argi, "ra="))
      ra_set = setDoubleOnString(ra, argi.substr(3));

    else if(strBegins(argi, "poly=")) {
      poly = string2Poly(argi.substr(5));
      if(poly.is_convex())
	poly_set = true;
    }
      
    else if((argi=="-h") || (argi=="--help")) {
      cout << "testPolyRayCPA: test the polyRayCPA() utility " << endl;
      cout << "Example:                                      " << endl;
      cout << "$ testPolyRayCPA rx=15 ry=5 ra=90 ";
      cout << "poly=0,0:10,0:10,10:0,10  " << endl;
      cout << "cpa=5" << endl;
      return(0);
    }
    else if(strBegins(argi, "id="))
      argi = "just ignore id fields";
    else {
      cout << "Error: arg[" << argi << "] Exiting." << endl;
      return(1);
    }  
  }  
     
  if(!rx_set)
    return(cmdLineErr("rx is not set. Exiting."));
  if(!ry_set)
    return(cmdLineErr("ry is not set. Exiting."));
  if(!ra_set)
    return(cmdLineErr("ra is not set. Exiting."));
  if(!poly_set)
    return(cmdLineErr("poly is not set. Exiting."));

  double rix,riy;
  double cpa = polyRayCPA(rx,ry,ra,poly,rix,riy);

  cout << "cpa=" << doubleToStringX(cpa,2);
  return(0);
}
 
