/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 1st, 2005                                        */
/*****************************************************************/

#include <vector>
#include "GEO_GUI.h"
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
  int i;
  unsigned int j;

  Fl::add_idle(idleProc);

  GEO_GUI* gui = new GEO_GUI(900, 800, "geoview");

  string tif_file = "Default.tif";  // default
  
  vector<XYPolygon> all_polys;
  vector<XYGrid>    all_grids;
  vector<XYCircle>  all_circles;
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
      vector<XYHexagon> hvector = readHexagonsFromFile(argv[i]);
      for(j=0; j<hvector.size(); j++)
	all_hexagons.push_back(hvector[j]);
    }
  }
 
  gui->readTiff(tif_file);

  cout << "# of file polys: " << all_polys.size() << endl;
  for(j=0; j<all_polys.size(); j++)
    gui->addPoly(all_polys[j]);

  cout << "# of file grids: " << all_grids.size() << endl;
  for(j=0; j<all_grids.size(); j++)
    gui->addGrid(all_grids[j]);
  
  cout << "# of file circles: " << all_circles.size() << endl;
  for(j=0; j<all_circles.size(); j++)
    gui->addCircle(all_circles[j]);
  
  cout << "# of file hexagons: " << all_hexagons.size() << endl;
  for(j=0; j<all_hexagons.size(); j++)
    gui->addPoly(all_hexagons[j]);
  
  gui->updateXY();

  return Fl::run();
}

