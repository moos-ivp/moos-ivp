/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testDistPtsOnArc)                          */
/*    DATE: Nov 17th, 2018                                       */
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
  double x1 = 0;    bool x1_set=false;
  double y1 = 0;    bool y1_set=false;
  double x2 = 0;    bool x2_set=false;
  double y2 = 0;    bool y2_set=false;

  double lang=0;    bool lang_set=false;
  double rang=0;    bool rang_set=false;
  
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

    else if(strBegins(argi, "lang="))
      lang_set = setDoubleOnString(lang, argi.substr(5));
    else if(strBegins(argi, "rang="))
      rang_set = setDoubleOnString(rang, argi.substr(5));
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
  if(!x1_set) return(cmdLineErr("x1 is not set. Exiting."));
  if(!y1_set) return(cmdLineErr("y1 is not set. Exiting."));
  if(!x2_set) return(cmdLineErr("x2 is not set. Exiting."));
  if(!y2_set) return(cmdLineErr("y2 is not set. Exiting."));

  if(!lang_set) return(cmdLineErr("lang is not set. Exiting."));
  if(!rang_set) return(cmdLineErr("rang is not set. Exiting."));

  double dist = distSegToArc(x1,y1,x2,y2, cx,cy,cr, lang,rang);

  cout << "dist=" << doubleToStringX(dist,2);
  return(0);
}
