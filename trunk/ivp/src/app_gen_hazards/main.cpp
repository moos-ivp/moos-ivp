/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Jan 27th 2012                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

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

  string arg_summary = argv[0];

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    arg_summary += " " + argi;
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      vers_requested = true;
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      help_requested = true;
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--exp="))
      generator.setExp(argi.substr(6));
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
    cout << "Usage: gen_hazards [OPTIONS]                           " << endl;
    cout << "                                                       " << endl;
    cout << "Options:                                               " << endl;
    cout << "  --help, -h                                           " << endl;
    cout << "      Display this help message.                       " << endl;
    cout << "  --version,-v                                         " << endl;
    cout << "      Display the release version of gen_object_field. " << endl;
    cout << "  --objects=<object_set>                               " << endl;
    cout << "      Specify an object set of the form:               " << endl;
    cout << "      \"amount,type\"                                  " << endl;
    cout << "  --exp=<value>                                        " << endl;
    cout << "      hazard resemblance exponent [1,10]               " << endl;
    cout << "  --polygon=<poly>                                     " << endl;
    cout << "      Specify a polygon region of the form:            " << endl;
    cout << "      \"0,0 : 50,0 : 50,50 : 0,50\"                    " << endl;
    cout << "  gen_hazards --polygon=-150,-75:-150,-400:400,-400:400,-75 --objects=20,hazard --objects=20,benign " << endl;
    return(0);
  }

  cout << "// " << arg_summary << endl;

  generator.generate();
  
  return(0);
}


