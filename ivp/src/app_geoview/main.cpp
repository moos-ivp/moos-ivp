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
#include "XYSegList.h"
#include "XYGrid.h"
#include "XYCircle.h"
#include "XYHexagon.h"
#include "XYBuildUtils.h"
#include "LMV_Utils.h"

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

  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("geoview");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: geoview [filename] [filename]..." << endl;
    return(0);
  }

  Fl::add_idle(idleProc);
  GEO_GUI* gui = new GEO_GUI(900, 800, "geoview");

  string tif_file = "Default.tif";  // default

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

  vector<string>    all_poly_strings;
  vector<string>    all_segl_strings;
  vector<string>    all_grid_strings;
  vector<XYCircle>  all_circles;
  vector<XYHexagon> all_hexagons;
  vector<string>    all_markers;
  vector<string>    all_opvertices;
  vector<string>    all_geodesy;

  for(i=1; i<argc; i++) {
    string argi  = argv[i];
    
    if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
    else {
      vector<string> svector;
      svector = readEntriesFromFile(argv[i], "poly:polygon");
      for(j=0; j<svector.size(); j++)
	all_poly_strings.push_back(svector[j]);

      svector = readEntriesFromFile(argv[i], "segl:seglist");
      for(j=0; j<svector.size(); j++)
	all_segl_strings.push_back(svector[j]);

      svector = readEntriesFromFile(argv[i], "grid:xygrid");
      for(j=0; j<svector.size(); j++)
	all_grid_strings.push_back(svector[j]);

      vector<XYCircle> cvector = readCirclesFromFile(argv[i]);
      for(j=0; j<cvector.size(); j++) 
	all_circles.push_back(cvector[j]);
      vector<XYHexagon> hvector = readHexagonsFromFile(argv[i]);
      for(j=0; j<hvector.size(); j++)
	all_hexagons.push_back(hvector[j]);
      vector<string> mvector = readEntriesFromFile(argv[i], "marker");
      for(j=0; j<mvector.size(); j++)
	all_markers.push_back(mvector[j]);
      vector<string> ovector = readEntriesFromFile(argv[i], "op_vertex");
      for(j=0; j<ovector.size(); j++)
	all_opvertices.push_back(ovector[j]);
      vector<string> dvector = readEntriesFromFile(argv[i], "geodesy");
      for(j=0; j<dvector.size(); j++)
	all_geodesy.push_back(dvector[j]);
    }
  }
 
  gui->pviewer->setParam("tiff_file", tif_file);

  cout << "# of file polys: " << all_poly_strings.size() << endl;
  for(j=0; j<all_poly_strings.size(); j++)
    gui->pviewer->setParam("polygon", all_poly_strings[j]);

  cout << "# of file seglists: " << all_segl_strings.size() << endl;
  for(j=0; j<all_segl_strings.size(); j++)
    gui->pviewer->setParam("seglist", all_segl_strings[j]);
  
  cout << "# of file grids: " << all_grid_strings.size() << endl;
  for(j=0; j<all_grid_strings.size(); j++)
    gui->pviewer->setParam("grid", all_grid_strings[j]);
  
  cout << "# of file circles: " << all_circles.size() << endl;
  for(j=0; j<all_circles.size(); j++)
    gui->pviewer->addCircle(all_circles[j]);
  
#if 0
  cout << "# of file hexagons: " << all_hexagons.size() << endl;
  for(j=0; j<all_hexagons.size(); j++)
    gui->pviewer->addPoly(all_hexagons[j]);
#endif  

  cout << "# of file marker entries: " << all_markers.size() << endl;
  for(j=0; j<all_markers.size(); j++)
    gui->pviewer->setParam("marker", all_markers[j]);

  cout << "# of file op entries: " << all_opvertices.size() << endl;
  for(j=0; j<all_opvertices.size(); j++)
    gui->pviewer->setParam("op_vertex", all_opvertices[j]);

  cout << "# of file geodesy entries: " << all_geodesy.size() << endl;
  for(j=0; j<all_geodesy.size(); j++)
    gui->pviewer->setParam("geodesy_init", all_geodesy[j]);

  gui->updateXY();

  return Fl::run();
}

