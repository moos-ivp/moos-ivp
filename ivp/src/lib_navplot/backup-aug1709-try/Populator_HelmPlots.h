/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_HelmPlots.h                                */
/*    DATE: July 15th, 2009 (at GLINT'09)                        */
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

#ifndef POPULATOR_HELMPLOTS_HEADER
#define POPULATOR_HELMPLOTS_HEADER

#include <string>
#include <vector>
#include "HelmPlot.h"
#include "ALogEntry.h"

class Populator_HelmPlots 
{
public:
  Populator_HelmPlots() {};
  ~Populator_HelmPlots() {};

  bool     populateFromEntries(const std::vector<ALogEntry>&);
  HelmPlot getHelmPlot() {return(m_helm_plot);};

protected:
  HelmPlot m_helm_plot;
};
#endif 








