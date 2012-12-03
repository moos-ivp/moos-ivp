/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_VPlugTPlots.h                              */
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

#ifndef POPULATOR_VPLUG_TPLOTS_HEADER
#define POPULATOR_VPLUG_TPLOTS_HEADER

#include <vector>
#include "VPlugTPlot.h"
#include "ALogEntry.h"

class Populator_VPlugTPlots 
{
public:
  Populator_VPlugTPlots() {};
  ~Populator_VPlugTPlots() {};

  bool        populateFromEntries(const std::vector<ALogEntry>&);

  void        setLogStartUTC(double v)  {m_logstart_utc=v;};
  VPlugTPlot  getVPlugPlot()            {return(m_vplug_plot);};

protected:
  VPlugTPlot  m_vplug_plot;
  double      m_logstart_utc;

};
#endif 



