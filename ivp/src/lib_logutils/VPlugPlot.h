/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VPlugPlot.h                                          */
/*    DATE: Aug 9th, 2009                                        */
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

#ifndef GEO_PLOT_HEADER
#define GEO_PLOT_HEADER

#include <string>
#include <vector>
#include <list>
#include "VPlug_GeoShapes.h"

class VPlugPlot
{
public:
  VPlugPlot();
  ~VPlugPlot() {}

  bool    addEvent(const std::string& var, 
		   const std::string& val, double time);
  void    setVehiName(std::string s) {m_vehi_name = s;}
  void    setBinVal(double);
  
  double  getMinTime() const;
  double  getMaxTime() const;

  void    applySkew(double skew);

  void    print() const;
  void    summary(std::string indent="") const;

  std::string  getVehiName() const   {return(m_vehi_name);}
  unsigned int size() const          {return(m_time.size());}

  VPlug_GeoShapes getVPlugByIndex(unsigned int index) const;
  VPlug_GeoShapes getVPlugByTime(double gtime) const;

  
protected: // config vars
  double m_binval;
  
protected: // state vars
  std::string                  m_vehi_name;  // Name of the platform
  std::vector<double>          m_time;
  std::vector<VPlug_GeoShapes> m_vplugs;

  unsigned int m_view_point_cnt;
  unsigned int m_view_polygon_cnt;
  unsigned int m_view_seglist_cnt;
  unsigned int m_view_seglr_cnt;
  unsigned int m_view_circle_cnt;
  unsigned int m_view_arrow_cnt;
  unsigned int m_grid_config_cnt;
  unsigned int m_grid_delta_cnt;
  unsigned int m_view_range_pulse_cnt;
  unsigned int m_view_comms_pulse_cnt;
  unsigned int m_view_marker_cnt;
  unsigned int m_view_textbox_cnt;
};
#endif 










