/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testSegRayCPA)                             */
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
  double rx = 0;    bool rx_set=false;
  double ry = 0;    bool ry_set=false;
  double ra = 0;    bool ra_set=false;

  double x1 = 0;    bool x1_set=false;
  double y1 = 0;    bool y1_set=false;
  double x2 = 0;    bool x2_set=false;
  double y2 = 0;    bool y2_set=false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "rx="))
      rx_set = setDoubleOnString(rx, argi.substr(3));
    else if(strBegins(argi, "ry="))
      ry_set = setDoubleOnString(ry, argi.substr(3));
    else if(strBegins(argi, "ra="))
      ra_set = setDoubleOnString(ra, argi.substr(3));

    else if(strBegins(argi, "x1="))
      x1_set = setDoubleOnString(x1, argi.substr(3));
    else if(strBegins(argi, "y1="))
      y1_set = setDoubleOnString(y1, argi.substr(3));
    else if(strBegins(argi, "x2="))
      x2_set = setDoubleOnString(x2, argi.substr(3));
    else if(strBegins(argi, "y2="))
      y2_set = setDoubleOnString(y2, argi.substr(3));

    else if((argi=="-h") || (argi=="--help")) {
      cout << "testSegRayCPA: test the segRayCPA() utility " << endl;
      cout << "Example:                                    " << endl;
      cout << "$ testSegRayCPA rx=5 ry=4 ra=90 x1=30 y1=34 x2=90 y2=34" << endl;
      cout << "cpa=30" << endl;
      return(0);
    }
    else if(strBegins(argi, "id="))
      argi = "just ignore id fields";
    else {
      cout << "Error: arg[" << argi << "] Exiting." << endl;
      return(1);
    }  
  }  
     
  if(!rx_set) return(cmdLineErr("rx is not set. Exiting."));
  if(!ry_set) return(cmdLineErr("ry is not set. Exiting."));
  if(!ra_set) return(cmdLineErr("ra is not set. Exiting."));
  if(!x1_set) return(cmdLineErr("x1 is not set. Exiting."));
  if(!y1_set) return(cmdLineErr("y1 is not set. Exiting."));
  if(!x2_set) return(cmdLineErr("x2 is not set. Exiting."));
  if(!y2_set) return(cmdLineErr("y2 is not set. Exiting."));

  double ix,iy;
  double cpa = segRayCPA(rx,ry,ra, x1,y1,x2,y2, ix,iy);

  cout << "cpa=" << doubleToStringX(cpa,2);
  return(0);
}
 
