/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 2016                                             */
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
#include <cmath>
#include <stdlib.h>
#include <iostream>
using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{ 
  double start_x = 0;
  double start_y = 0;
  double angle = 0;
  double dist  = 0;
  double scale = 2;
  string sep   = " ";
  
  if(argc < 5)
    showHelpAndExit();

  for(int i=0; i<argc; i++) {
    string argi = argv[i]; 
    if((argi == "-h") || (argi == "--help"))
      showHelpAndExit();
  }

  for(int i=1; i<argc; i++) {
    string argi = argv[i]; 
    bool handled = true;
    if(i==1)
      handled = setDoubleOnString(start_x, argi);
    else if(i==2)
      handled = setDoubleOnString(start_y, argi);
    else if(i==3)
      handled = setDoubleOnString(angle, argi);
    else if(i==4)
      handled = setDoubleOnString(dist, argi);
    else if((argi == "--pt") || (argi == "-pt"))
      sep = ",";
    else if((argi == "--ptf") || (argi == "-ptf"))
      sep = "full";
    else if((argi == "--p0") || (argi == "-p0"))
      scale = 0;
    else if((argi == "--p1") || (argi == "-p1"))
      scale = 1;
    else if((argi == "--p2") || (argi == "-p2"))
      scale = 2;
    else if((argi == "--p3") || (argi == "-p3"))
      scale = 3;
    else if((argi == "--p4") || (argi == "-p4"))
      scale = 4;
    else if((argi == "--p5") || (argi == "-p5"))
      scale = 5;
    else
      handled = false;

    if(!handled) {
      cout << "Bad Arg:[" << argi << "]. Exiting." << endl;
      exit(1);
    }    
  }
  double new_x, new_y;
  projectPoint(angle, dist, start_x, start_y, new_x, new_y);

  string res = doubleToStringX(new_x,scale);
  
  if(sep != "full")
    res += sep + doubleToStringX(new_y,scale);
  else
    res = "x=" + res + ",y=" + doubleToStringX(new_y,scale);

  cout << res << endl;
  
  return(0);
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{ 
  cout << "Usage:                                              " << endl;
  cout << "  projpt x y angle dist [OPTIONS]                   " << endl;
  cout << "                                                    " << endl;
  cout << "Synopsis:                                           " << endl;
  cout << "  Return the position given a starting point (x,y), " << endl;
  cout << "  a relative angle, in degrees, and distance.       " << endl;
  cout << "                                                    " << endl;
  cout << "Options:                                            " << endl;
  cout << "  --help, -h                                        " << endl;
  cout << "    Display this help message                       " << endl;
  cout << "  --p0, -p0                                         " << endl;
  cout << "    Coordinates with scale 0 (whole integer)        " << endl;
  cout << "  --p1, -p1                                         " << endl;
  cout << "    Coordinates with scale 1 (1 pt1 after decimal)  " << endl;
  cout << "  --p2, -p2  (default)                              " << endl;
  cout << "    Coordinates with scale 2 (2 pts after decimal)  " << endl;
  cout << "  --p3, -p3                                         " << endl;
  cout << "    Coordinates with scale 3 (3 pts after decimal)  " << endl;
  cout << "  --p4, -p4                                         " << endl;
  cout << "    Coordinates with scale 4 (4 pts after decimal)  " << endl;
  cout << "  --p5, -p5                                         " << endl;
  cout << "    Coordinates with scale 5 (5 pts after decimal)  " << endl;
  cout << "                                                    " << endl;
  cout << "  --pt, -pt                                         " << endl;
  cout << "    Output is xval,yval instead of xval yval        " << endl;
  cout << "                                                    " << endl;
  cout << "  --ptf, -ptf                                       " << endl;
  cout << "    Output is x=xval,y=yval instead of xval yval    " << endl;
  cout << "                                                    " << endl;
  cout << "See also:                                           " << endl;
  cout << "  range x1 y1 x2 y2                                 " << endl;
  cout << "  projpt x y angle dist                             " << endl;
  cout << "  relbng x1 y1 x2 y2                                " << endl;
  cout << "  distptline px py x1 y1 x2 y2                      " << endl;
  cout << "  sinang angle                                      " << endl;
  cout << "  cosang angle                                      " << endl;

  exit(0);
}

