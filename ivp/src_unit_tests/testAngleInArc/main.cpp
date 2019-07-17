/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testAngleInArc)                            */
/*    DATE: Nov 12th, 2018                                       */
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
  double lang=0;   bool lang_set=false;
  double rang=0;   bool rang_set=false;
  double qang=0;   bool qang_set=false;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "lang="))
      lang_set = setDoubleOnString(lang, argi.substr(5));
    else if(strBegins(argi, "rang="))
      rang_set = setDoubleOnString(rang, argi.substr(5));
    else if(strBegins(argi, "qang="))
      qang_set = setDoubleOnString(qang, argi.substr(5));
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
  
  if(!lang_set) return(cmdLineErr("lang is not set. Exiting."));
  if(!rang_set) return(cmdLineErr("rang is not set. Exiting."));
  if(!qang_set) return(cmdLineErr("query angle is not set. Exiting."));

  bool inarc = angleInArc(lang, rang, qang);

  cout << "inarc=" << boolToString(inarc) << endl;
  return(0);
}
