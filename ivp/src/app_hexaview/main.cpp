/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Oct 30, 2005                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <vector>
#include <iostream>
#include "HEXA_GUI.h"
#include "MBUtils.h"
#include "IO_GeomUtils.h"
#include "XYPolygon.h"
#include "XYGrid.h"

using namespace std;

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int i, j;
  Fl::add_idle(idleProc);

  HEXA_GUI* gui = new HEXA_GUI(900, 800, "OpRegion Viewer");

  //string tif_file = "Pianosa.tif";  // default
  string tif_file = "AerialMIT.tif";  // default
  
  vector<XYPolygon> all_polys;
  vector<XYGrid>    all_grids;

  for(i=1; i<argc; i++) {
    string argi  = argv[i];
    
    if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
    else {
      vector<XYGrid> gvector = readGridsFromFile(argv[i]);
      for(j=0; j<gvector.size(); j++) {
	XYGrid grid = gvector[j];
	all_grids.push_back(grid);
      }
      vector<XYPolygon> pvector = readPolysFromFile(argv[i]);
      for(j=0; j<pvector.size(); j++) {
	XYPolygon poly = pvector[j];
	all_polys.push_back(poly);
      }
    }
  }
  
  gui->readTiff(tif_file.c_str());

  for(i=0; i<all_polys.size(); i++)
    gui->addPoly(all_polys[i]);

  for(i=0; i<all_grids.size(); i++)
    gui->addGrid(all_grids[i]);
  
  gui->updateXY();

  return Fl::run();
}




