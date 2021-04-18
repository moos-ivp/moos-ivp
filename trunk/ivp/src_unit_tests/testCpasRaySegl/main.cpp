/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testCpasRaySegl)                           */
/*    DATE: Nov 20th, 2018                                       */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "ArcUtils.h"
#include "XYFormatUtilsSegl.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}

int main(int argc, char** argv) 
{
  double rx = 0;       bool rx_set=false;
  double ry = 0;       bool ry_set=false;
  double ra = 0;       bool ra_set=false;
  double thresh = 0;   bool thresh_set=false; 
  XYSegList segl;      bool segl_set=false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "rx="))
      rx_set = setDoubleOnString(rx, argi.substr(3));
    else if(strBegins(argi, "ry="))
      ry_set = setDoubleOnString(ry, argi.substr(3));
    else if(strBegins(argi, "ra="))
      ra_set = setDoubleOnString(ra, argi.substr(3));
    else if(strBegins(argi, "thresh="))
      thresh_set = setDoubleOnString(thresh, argi.substr(7));

    else if(strBegins(argi, "segl=")) {
      segl = string2SegList(argi.substr(5));
      if(segl.size() != 0)
	segl_set = true;
    }
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
   
  if(!rx_set) return(cmdLineErr("rx is not set. Exiting."));
  if(!ry_set) return(cmdLineErr("ry is not set. Exiting."));
  if(!ra_set) return(cmdLineErr("ra is not set. Exiting."));

  if(!thresh_set) return(cmdLineErr("thresh is not set. Exiting."));

  if(!segl_set) return(cmdLineErr("seglist is not set. Exiting."));

  vector<double> vcpa;
  vector<double> vray;
  unsigned int pts = cpasRaySegl(rx,ry,ra, segl, thresh, vcpa, vray);
 
  for(unsigned int i=0; i<pts; i++) {
    if(i != 0)
      cout << ","; 
    cout << "cpa"  << uintToString(i+1) << "=" << doubleToStringX(vcpa[i],2); 
    cout << ",ray" << uintToString(i+1) << "=" << doubleToStringX(vray[i],2); 
  }
  return(0);
}
