/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 1st, 2005                                        */
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

#include <vector>
#include <iostream>
#include "POLY_GUI.h"
#include "MBUtils.h"
#include "IO_GeomUtils.h"
#include "XYPolygon.h"
#include "XYGrid.h"
#include "XYCircle.h"
#include "XYHexagon.h"

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

  POLY_GUI* gui = new POLY_GUI(900, 800, "OpRegion Viewer");

  string tif_file = "Default.tif";  // default
  
  vector<XYPolygon> all_polys;
  vector<XYGrid>    all_grids;
  vector<XYCircle>  all_circles;
  vector<XYArc>     all_arcs;
  vector<XYHexagon> all_hexagons;

  for(i=1; i<argc; i++) {
    string argi  = tolower(argv[i]);
    if((argi == "mit") || (argi=="charles"))
      tif_file = "AerialMIT-1024.tif";
    if((argi == "wmit") || (argi=="wireframe") || (argi=="wf"))
      tif_file = "WireFrameMIT-1024.tif";
    if((argi == "mb") || (argi=="monterey"))
      tif_file = "Monterey-2048.tif";
    if((argi == "mbd"))
      tif_file = "Monterey-2048-30-30-100.tif";
  }

  for(i=1; i<argc; i++) {
    string argi  = argv[i];
    
    if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
    else {
      vector<XYGrid> gvector = readGridsFromFile(argv[i]);
      for(j=0; j<gvector.size(); j++)
	all_grids.push_back(gvector[j]);
      vector<XYPolygon> pvector = readPolysFromFile(argv[i]);
      for(j=0; j<pvector.size(); j++)
	all_polys.push_back(pvector[j]);
      vector<XYCircle> cvector = readCirclesFromFile(argv[i]);
      for(j=0; j<cvector.size(); j++) 
	all_circles.push_back(cvector[j]);
      vector<XYArc> avector = readArcsFromFile(argv[i]);
      for(j=0; j<avector.size(); j++)
	all_arcs.push_back(avector[j]);
      vector<XYHexagon> hvector = readHexagonsFromFile(argv[i]);
      for(j=0; j<hvector.size(); j++)
	all_hexagons.push_back(hvector[j]);
    }
  }
 
  gui->readTiff(tif_file);

  for(i=0; i<all_polys.size(); i++)
    gui->addPoly(all_polys[i]);

  for(i=0; i<all_grids.size(); i++)
    gui->addGrid(all_grids[i]);
  
  for(i=0; i<all_circles.size(); i++)
    gui->addCircle(all_circles[i]);
  
  for(i=0; i<all_arcs.size(); i++)
    gui->addArc(all_arcs[i]);
  
  for(i=0; i<all_hexagons.size(); i++)
    gui->addPoly(all_hexagons[i]);
  
  gui->updateXY();

  return Fl::run();
}


