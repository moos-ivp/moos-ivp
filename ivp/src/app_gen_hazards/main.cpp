/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: main.cpp                                             */
/*   DATE: Jan 27th 2012                                        */
/****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "HazardFieldGenerator.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  bool   help_requested = false;
  bool   vers_requested = false;

  HazardFieldGenerator generator;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      vers_requested = true;
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      help_requested = true;
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--objects=")) {
      string object_set = argi.substr(10);
      bool ok = generator.addObjectSet(object_set);
      if(!ok) 
	cout << "Invalid object set: " << object_set << endl;
    }
    else if(strBegins(argi, "--polygon=")) {
      string poly_str = argi.substr(10);
      bool ok = generator.addPolygon(poly_str);
      if(!ok) 
	cout << "Invalid polygon specified: " << poly_str << endl;
      else
	cout << "Proper polygon specified: " << poly_str << endl;
    }
  }
  
  if(vers_requested) {
    showReleaseInfo("gen_object_field", "gpl");
    return(0);
  }

  if(help_requested) {
    cout << "Usage: gen_object_field [OPTIONS]                      " << endl;
    cout << "                                                       " << endl;
    cout << "Options:                                               " << endl;
    cout << "  --help, -h                                           " << endl;
    cout << "      Display this help message.                       " << endl;
    cout << "  --version,-v                                         " << endl;
    cout << "      Display the release version of gen_object_field. " << endl;
    cout << "  --objects=<object_set>                               " << endl;
    cout << "      Specify an object set of the form:               " << endl;
    cout << "      \"amount,type\"                                  " << endl;
    cout << "  --polygon=<poly>                                     " << endl;
    cout << "      Specify a polygon region of the form:            " << endl;
    cout << "      \"0,0 : 50,0 : 50,50 : 0:50\"                    " << endl;
    return(0);
  }

  generator.generate();
  
  return(0);
}

