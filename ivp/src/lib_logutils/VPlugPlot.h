/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  VPlugPlot() {}
  ~VPlugPlot() {}

  bool            addEvent(const std::string& var, 
			   const std::string& val, double time);
  void            setVehiName(std::string s) {m_vehi_name = s;}

  VPlug_GeoShapes getVPlugByIndex(unsigned int index) const;
  VPlug_GeoShapes getVPlugByTime(double gtime) const;
  std::string     getVehiName() const   {return(m_vehi_name);}
  unsigned int    size() const          {return(m_time.size());}
  double          getMinTime() const;
  double          getMaxTime() const;
  void            print() const;

  void            applySkew(double skew);

protected:
  std::string                  m_vehi_name;  // Name of the platform
  std::vector<double>          m_time;
  std::vector<VPlug_GeoShapes> m_vplugs;
};
#endif 









