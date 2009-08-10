/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_GeoPlots.h                                 */
/*    DATE: August 9th, 2009                                     */
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

#ifndef POPULATOR_GEOPLOTS_HEADER
#define POPULATOR_GEOPLOTS_HEADER

#include <string>
#include <vector>
#include "GeoPlot.h"

class Populator_GeoPlots 
{
public:
  Populator_GeoPlots() {};
  ~Populator_GeoPlots() {};

  bool     populateRawInfoFromALog(std::string);
  bool     populateGeoPlotFromRawInfo();

  GeoPlot  getGeoPlot() {return(m_geo_plot);};

protected:
  GeoPlot  m_geo_plot;

  // Need timestamps with each geo event
  std::vector<std::string>  m_geo_event_var;
  std::vector<std::string>  m_geo_event_val;
  std::vector<double>       m_geo_event_time;
};
#endif 








