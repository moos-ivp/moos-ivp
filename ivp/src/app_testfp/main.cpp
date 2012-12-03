/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
using namespace std;

#include <cstdio>
#include "Foo.h"
#include "XYPatternBlock.h"
#include "XYFormatUtilsSegl.h"

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{ 
  // Foo foo;
  // foo.doit();

  double startx = 10;
  double starty = 20;

  string xpos = "0";
  string ypos = "0";
  double width = 180;
  double height = 60;
  double swath = 15;
  double degs = 45;

  if(argc >= 3) {
    xpos = argv[1];
    ypos = argv[2];
  }
  cout << "XPOS = " << xpos << ", YPOS=" << ypos << endl;

  XYPatternBlock pblock;
  pblock.setParam("id_point", (xpos + "," + ypos));
  pblock.setParam("block_width", width);
  pblock.setParam("block_length", height);
  pblock.setParam("swath_width", swath);
  pblock.setParam("angle", degs);

  pblock.buildCompositeSegList(startx, starty);
  
  XYSegList new_seglist = pblock.getCompositeSegList();  

  new_seglist.apply_snap(0.1);

  cout << "spec: " << new_seglist.get_spec() << endl;


  string spec = "format=lawnmower, label=foxtrot, x=64000, y=64000, ";
  spec += "height=60, width=180, lane_width=15, rows=north-south, degs=45";

  XYSegList segl2 = string2SegList(spec);
  cout << "spec2: " << segl2.get_spec() << endl;
  
  return 0;
}

