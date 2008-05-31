/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NavPlot.h                                            */
/*    DATE: June 1st, 2005                                       */
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

#ifndef GRID_PLOT_HEADER_791
#define GRID_PLOT_HEADER_791

#include <string>
#include <vector>
#include "XYGrid.h"

class GridPlot
{
public:
  GridPlot() {};
  ~GridPlot() {};

  // Initialization is with the GRID_CONFIG string
  bool   initialize(double gtime, const std::string& gstr); 
  // Updates are with the GRID_DELTA string
  bool   applyDelta(double gtime, const std::string& gstr);

  double getTimeByIndex(unsigned int) const;

  XYGrid getGridByIndex(unsigned int) const;
  XYGrid getGridByTime(double) const;
  
  double getMinTime() const;
  double getMaxTime() const;
  unsigned int size() const         {return(m_time.size());};

  std::string getGridLabel() const  {return(m_gridlabel);};

protected:
  int getIndexByTime(double) const;

protected:
  std::string             m_gridlabel;  // The Grid Label
  std::vector<double>     m_time;
  std::vector<XYGrid>     m_grids;
};
#endif 








