/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_GridPlot.cpp                               */
/*    DATE: May 14th, 2008                                       */
/*****************************************************************/

#include <iostream>
#include <stdlib.h>
#include "Populator_GridPlot.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populate

void Populator_GridPlot::populate(string filestr)
{
  vector<string> lines = fileBuffer(filestr);

  vector<vector<string> > plines; // parsed lines

  unsigned int i;
  for(i=0; i<lines.size(); i++) {
    if(lines[i][0] != '%') {
      lines[i] = findReplace(lines[i], '\t', ' ');
      lines[i] = compactConsecutive(lines[i], ' ');
      lines[i] = stripBlankEnds(lines[i]);
      vector<string> svector = parseString(lines[i], ' ');
      plines.push_back(svector);
    }
  }

  int vsize = plines.size();  
  for(int i=0; i<vsize; i++) {
    if(plines[i][1] == "GRID_CONFIG") {
      double time_stamp = atof(plines[i][0].c_str());
      handleConfigEntry(time_stamp, plines[i][3]);
    }
    else if(plines[i][1] == "GRID_DELTA") {
      double time_stamp = atof(plines[i][0].c_str());
      handleDeltaEntry(time_stamp, plines[i][3]);
    }
    else if(plines[i][1] == "GRID_DELTA_LOCAL") {
      double time_stamp = atof(plines[i][0].c_str());
      handleDeltaEntry(time_stamp, plines[i][3]);
    }
  }

}

//---------------------------------------------------------------
// Procedure: handleConfigEntry

void Populator_GridPlot::handleConfigEntry(double g_time, 
					   const string& g_str)
{
  GridPlot new_gridplot;
  bool new_gridplot_ok = new_gridplot.initialize(g_time, g_str);
  if(new_gridplot_ok)
    m_gridplots.push_back(new_gridplot);
}


//---------------------------------------------------------------
// Procedure: handleDeltaEntry
// Example:
//            GRID_DELTA_LOCAL = "ALPHA@41,0,0.27223,0,0" 

void Populator_GridPlot::handleDeltaEntry(double g_time, 
					  const string& g_str)
{
  vector<string> svector = parseString(g_str, '@');
  if(svector.size() != 2)
    return;
  
  string grid_label = svector[0];

  int vsize = m_gridplots.size();
  for(int i=0; i<vsize; i++) {
    if(m_gridplots[i].getGridLabel() == grid_label) {
      bool ok = m_gridplots[i].applyDelta(g_time, g_str);
      if(!ok)
	cout << "Populator_GridPlot: failed applyDelta" << endl;
    }
    else
      cout  << "Populator_GridPlot Label mismatch: " <<
	grid_label << "  " << m_gridplots[i].getGridLabel() << endl;
  }
}

//---------------------------------------------------------------
// Procedure: getGridPlot

GridPlot Populator_GridPlot::getGridPlot(int ix)
{
  int vsize = m_gridplots.size();
  
  if((ix < 0) || (ix >= vsize)) {
    GridPlot null_gridplot;
    return(null_gridplot);
  }
  else
    return(m_gridplots[ix]);
  
}

//---------------------------------------------------------------
// Procedure: getGridPlotLabel

string Populator_GridPlot::getGridPlotLabel(int ix)
{
  int vsize = m_gridplots.size();
  
  if((ix < 0) || (ix >= vsize)) {
    return("");
  }
  else
    return(m_gridplots[ix].getGridLabel());
  
}

//---------------------------------------------------------------
// Procedure: print

void Populator_GridPlot::print()
{
  int vsize = m_gridplots.size();
  
  cout << "Number of GridPlots: " << vsize << endl;
  
  for(int i=0; i<vsize; i++) {
    cout << "GridPlot[" << i << "]: " << endl;
    cout << "  PlotSize: " << m_gridplots[i].size() << endl;
  }
}






