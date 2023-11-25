/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_VPlugPlots.h                               */
/*    DATE: August 9th, 2009                                     */
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

#ifndef POPULATOR_VPLUG_PLOTS_HEADER
#define POPULATOR_VPLUG_PLOTS_HEADER

#include <vector>
#include "VPlugPlot.h"
#include "ALogEntry.h"

class Populator_VPlugPlots 
{
public:
  Populator_VPlugPlots() {}
  ~Populator_VPlugPlots() {}

  void  setVQual(std::string s) {m_vqual=s;}
  void  setBinVal(double v)     {m_vplug_plot.setBinVal(v);}
  
  bool  populateFromEntries(const std::vector<ALogEntry>&);
  bool  populateFromEntry(const ALogEntry&);
  
  VPlugPlot getVPlugPlot() {return(m_vplug_plot);}

protected:
  VPlugPlot m_vplug_plot;

  std::string m_vqual;
};
#endif 

