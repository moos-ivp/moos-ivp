/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: January 4th, 2011                                    */
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

#include <iostream>
#include <vector>
#include "CVIEW_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "IO_GeomUtils.h"
#include "XYPolygon.h"
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
    showReleaseInfo("currview", "gpl");
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: currview [filename] [filename]..." << endl;
    return(0);
  }

  Fl::add_idle(idleProc);
  CVIEW_GUI* gui = new CVIEW_GUI(900, 800, "currview");

  if(!gui) {
    cout << "Unable to initialize the GUI object" << endl;
    return(1);
  }
    
  string tif_file = "Default.tif";  // default
  string cfd_file;

  vector<string>    all_poly_strings;

  for(i=1; i<argc; i++) {
    string argi  = tolower(argv[i]);

    if(strEnds(argi, ".cfd"))
      cfd_file = argi;
    else if(strEnds(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
    else if((argi == "mit") || (argi=="charles"))
      tif_file = "AerialMIT-1024.tif";
    else if((argi == "forrest-lake") || (argi=="fl"))
      tif_file = "forrest19.tif";
    else if((argi == "palmer_river") || (argi=="pr"))
      tif_file = "palmer_river_a.tif";
    else {
      vector<string> svector;
      svector = readEntriesFromFile(argi, "poly:polygon");
      for(j=0; j<svector.size(); j++)
	all_poly_strings.push_back(svector[j]);
    }
  }
 
  gui->pviewer->setParam("tiff_file", tif_file);
  if(cfd_file != "")
    gui->pviewer->initCurrentField(cfd_file);

  cout << "# of file polys: " << all_poly_strings.size() << endl;
  for(j=0; j<all_poly_strings.size(); j++)
    gui->pviewer->setParam("view_polygon", all_poly_strings[j]);

  gui->updateXY();

  return Fl::run();
}





