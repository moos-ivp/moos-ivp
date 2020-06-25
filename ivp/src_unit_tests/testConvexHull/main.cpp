/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testConvexHull)                            */
/*    DATE: Jun 5, 2020                                          */
/*****************************************************************/
           
#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "ConvexHullGenerator.h"
#include "XYFormatUtilsPoly.h"
#include "XYFormatUtilsSegl.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}
 
int main(int argc, char** argv) 
{
  string pts = "pts={107.5,-53.5:112,-43.8:112,-46.1:111.7,-49.3:108.3,-52.7:107.7,-53.3}";

  pts = "pts={108.9,-39.9:107.88,-38.88:112,-43:110.32,-41.32:109.39,-40.39:107.8,-38.8:108.02,-39.02:108.19,-39.19:110.16,-41.16:111.68,-42.68:108.9,-39.9:107.52,-38.52:108.9,-39.9:108.65,-39.65:108.61,-39.61:105.96,-38:107.35,-38.35:103.81,-38:102.14,-38:109.01,-40.01}";

  pts = "pts={96.02,-42.98:96.76,-42.05:98.56,-39.8:98.72,-39.6:99.92,-38.1:100.08,-38:107,-38}";
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    cout << "argi:[" << argi << "]" << endl;
    if(strBegins(argi, "pts="))
      pts = argi;
    else if((argi=="-h") || (argi=="--help")) {
      cout << "testConvexHull: test ConvexHullGenerator utility " << endl;
      cout << "Example:                                         " << endl;
      cout << "$ testConvexHull pts={107.5,-53.5:112,-43.8:112,-46.1:111.7,-49.3:108.3,-52.7:107.7,-53.3}" << endl;
      cout << "hull=6" << endl;
      return(0);
    }
    else if(strBegins(argi, "id="))
      argi = "just ignore id fields";
    else {
      cout << "Error: arg[" << argi << "] Exiting." << endl;
      return(1);
    }  
  }  
     
  if(pts == "")
    return(cmdLineErr("pts are not set. Exiting."));

  XYSegList segl = string2SegList(pts);
  if(segl.size() == 0) {
    cout << "Empty set of points, or bad point string" << endl;
    return(2);
  }
    
  
  ConvexHullGenerator generator;
  for(unsigned int i=0; i<segl.size(); i++) 
    generator.addPoint(segl.get_vx(i), segl.get_vy(i));

  XYPolygon hull_poly = generator.generateConvexHull();
  if(hull_poly.is_convex())
    cout << "hull=" << uintToString(hull_poly.size()) << endl;
  else
    cout << "hull=0" << endl;

  return(0);
}
 
