/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Sep 2023                                             */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>
using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main()

int main(int argc, char *argv[])
{ 
  bool   ang1_set = false;
  double ang1_val = 0;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i]; 
    if((argi == "-h") || (argi == "--help"))
      showHelpAndExit();
    else if(isNumber(argi) && !ang1_set) {
      ang1_val = atof(argi.c_str());
      ang1_set = true;
    }
    else {
      cout << "Bad Arg:[" << argi << "]. Exiting." << endl;
      exit(1);
    }    
  }

  if(!ang1_set) {
    cout << "No input angle provided." << endl;
    exit(1);
  }

  double ang1= angle360(ang1_val);
  double ang2 = ang1;
  
  if(ang1 == 0)
    ang2 = 90;
  else if(ang1 == 90)
    ang2 = 0;
  else if(ang1 == 180)
    ang2 = 270;
  else if(ang1 == 270)
    ang2 = 180;
  else if((ang1 > 0) && (ang1 < 90))
    ang2 = 90 - ang1;
  else if((ang1 > 90) && (ang1 < 180)) {
    double a = ang1 - 90;
    ang2 = angle360(-a);
  }
  else if((ang1 > 180) && (ang1 < 270)) {
    double a = ang1 - 90;
    ang2 = angle360(-a);
  }
  else if(ang1 > 270)
    ang2 = 450 - ang1;
  
  string ans = doubleToStringX(ang2,6);
  cout << ans << endl;
  
  return(0);
}
 

//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{ 
  cout << "Usage:                                              " << endl;
  cout << "  nzero angle [OPTIONS]                             " << endl;
  cout << "                                                    " << endl;
  cout << "Synopsis:                                           " << endl;
  cout << "  Convert a heading given with the Zero-North origin" << endl;
  cout << "  and convert it to a heading in Zero-East origin.  " << endl;
  cout << "  For example:                                      " << endl;
  cout << "                                                    " << endl;
  cout << "             0 --> 90                               " << endl;
  cout << "            90 --> 0                                " << endl;
  cout << "       358(-2) --> 92                               " << endl;
  cout << "      270(-90) --> 180                              " << endl;
  cout << "           179 --> 271                              " << endl;
  cout << "            95 --> 355                              " << endl;
  cout << "                                                    " << endl;
  cout << "Options:                                            " << endl;
  cout << "  --help, -h                                        " << endl;
  cout << "    Display this help message                       " << endl;
  exit(0);
}

