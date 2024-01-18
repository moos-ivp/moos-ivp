/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp                                             */
/*    DATE: Nov 19th, 2023                                       */
/*****************************************************************/

#include "MBUtils.h"
#include "GeomUtils.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "FldProjector.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{ 
  double root_x  = 0;
  double root_y  = 0;
  double angle   = 0;
  double grid    = 10;
  double offset  = 0;
  unsigned int cells_x = 26;
  unsigned int cells_y = 26;
  
  for(int i=0; i<argc; i++) {
    string argi = argv[i]; 
    if((argi == "-h") || (argi == "--help"))
      showHelpAndExit();
  }

  string cmds;
  for(int i=1; i<argc; i++) {
    string argi = argv[i]; 
    cmds += argi + " ";      

    bool handled = true;
    if(strBegins(argi, "--x="))
      handled = setDoubleOnString(root_x, argi.substr(4));
    else if(strBegins(argi, "--y="))
      handled = setDoubleOnString(root_y, argi.substr(4));
    else if(strBegins(argi, "--ang="))
      handled = setDoubleOnString(angle, argi.substr(6));
    else if(strBegins(argi, "--grid="))
      handled = setDoubleOnString(grid, argi.substr(7));
    else if(strBegins(argi, "--offset="))
      handled = setDoubleOnString(offset, argi.substr(9));
    else if(strBegins(argi, "--cx="))
      handled = setUIntOnString(cells_x, argi.substr(5));
    else if(strBegins(argi, "--cy="))
      handled = setUIntOnString(cells_y, argi.substr(5));
    else if(argi == "--pav") {
      root_x = 0;
      root_y = 0;
      angle = 64.6356;
      grid = 10;
      cells_x = 26;
      cells_y = 26;
      offset = 5;
    }
    else
      handled = false;

    if(!handled) {
      cout << "Bad Arg:[" << argi << "]. Exiting." << endl;
      exit(1);
    }    
  }

  cout << "# $ projfield " + cmds << endl;
  
  FldProjector proj(root_x, root_y, angle);
  proj.setGrid(grid);
  proj.setCellsX(cells_x);
  proj.setCellsY(cells_y);
  proj.setOffset(offset);
  
  proj.buildProjection();
  proj.print();
  
  return(0);
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{ 
  cout << "Usage:                                              " << endl;
  cout << "  projfield [OPTIONS]                               " << endl;
  cout << "                                                    " << endl;
  cout << "Synopsis:                                           " << endl;
  cout << "  Calculates a field of coordinates with key vals   " << endl;
  cout << "  By default a 26x26 field with AA at the root and  " << endl;
  cout << "  ZZ at the far corner.                             " << endl;
  cout << "                                                    " << endl;
  cout << "Options:                                            " << endl;
  cout << "  --help, -h                                        " << endl;
  cout << "    Display this help message                       " << endl;
  cout << "  --x=<val>                                         " << endl;
  cout << "    X coordinate of root point, default is 0        " << endl;
  cout << "  --y=<val>                                         " << endl;
  cout << "    Y coordinate of root point, default is 0        " << endl;
  cout << "  --grid=<val>                                      " << endl;
  cout << "    Grid cell size, default is 10 meters            " << endl;
  cout << "  --rows=<val>                                      " << endl;
  cout << "    Number of rows, default is 26 (max)             " << endl;
  cout << "  --cols=<val>                                      " << endl;
  cout << "    Number of columns, default is 26 (max)          " << endl;
  cout << "  --pav                                             " << endl;
  cout << "    Convenience option: Set to MIT Pavilion coords  " << endl;
  cout << "                                                    " << endl;
  cout << "Example:                                            " << endl;
  cout << "  projfield --x=-122 --y=-63 --ang=64.6356          " << endl;
  cout << "  projfield --pav (same as above)                   " << endl;
  cout << "  projfield --x=-122 --y=-63 --grid=5 --rows=20     " << endl;
  cout << "                                                    " << endl;
  cout << "See also:                                           " << endl;
  cout << "  projpt x y angle dist                             " << endl;

  exit(0);
}
