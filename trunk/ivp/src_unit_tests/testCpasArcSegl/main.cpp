/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testCpasArcSegl)                           */
/*    DATE: Nov 23rd, 2018                                       */
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
  double cx = 0;      bool cx_set=false;
  double cy = 0;      bool cy_set=false;
  double cr = 0;      bool cr_set=false;
  double lang = 0;    bool lang_set=false;
  double rang = 0;    bool rang_set=false;
  double thresh = 0;  bool thresh_set=false; 
  XYSegList segl;     bool segl_set=false;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "cx="))
      cx_set = setDoubleOnString(cx, argi.substr(3));
    else if(strBegins(argi, "cy="))
      cy_set = setDoubleOnString(cy, argi.substr(3));
    else if(strBegins(argi, "cr="))
      cr_set = setDoubleOnString(cr, argi.substr(3));
    else if(strBegins(argi, "lang="))
      lang_set = setDoubleOnString(lang, argi.substr(5));
    else if(strBegins(argi, "rang="))
      rang_set = setDoubleOnString(rang, argi.substr(5));

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
   
  if(!cx_set) return(cmdLineErr("cx is not set. Exiting."));
  if(!cy_set) return(cmdLineErr("cy is not set. Exiting."));
  if(!cr_set) return(cmdLineErr("cr is not set. Exiting."));

  if(!thresh_set) return(cmdLineErr("thresh is not set. Exiting."));

  if(!segl_set) return(cmdLineErr("seglist is not set. Exiting."));

  if(!lang_set) return(cmdLineErr("lang is not set. Exiting."));
  if(!rang_set) return(cmdLineErr("rang is not set. Exiting."));

  vector<double> vcpa;
  vector<double> vdcpa;
  unsigned int pts = cpasArcSegl(cx,cy,cr,lang,rang,
				 segl, thresh,
				 true, vcpa, vdcpa);
 
  for(unsigned int i=0; i<pts; i++) {
    if(i != 0)
      cout << ",";   
    cout << "cpa"   << (i+1) << "=" << doubleToStringX(vcpa[i],2); 
    cout << ",dcpa" << (i+1) << "=" << doubleToStringX(vdcpa[i],2); 
  }
  return(0);
}
