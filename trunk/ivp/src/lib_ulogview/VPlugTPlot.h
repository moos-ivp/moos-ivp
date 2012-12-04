/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlugTPlot.h                                         */
/*    DATE: Aug0909, Sep2811                                     */
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

#ifndef GEO_PLOT_HEADER
#define GEO_PLOT_HEADER

#include <string>
#include <vector>
#include <list>
#include "VPlug_GeoShapes.h"
#include "TimePlot.h"

class VPlugTPlot : public TimePlot
{
public:
 VPlugTPlot() : TimePlot() {};
  ~VPlugTPlot() {};

  bool            addEvent(const std::string& var, 
			   const std::string& val, double time);

  VPlug_GeoShapes getVPlugByIndex(unsigned int index) const;
  VPlug_GeoShapes getVPlugByTime(double gtime) const;
  VPlug_GeoShapes getVPlugByTimeUTC(double gtime) const;

  unsigned int    size()  const        {return(m_time.size());};
  void            print() const;

protected:
  std::vector<VPlug_GeoShapes> m_vplugs;
};
#endif 


