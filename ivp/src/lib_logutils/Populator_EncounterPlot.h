/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_EncounterPlot.h                            */
/*    DATE: Jan 12th 2016                                        */
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

#ifndef POPULATOR_ENCOUNTER_PLOT_HEADER
#define POPULATOR_ENCOUNTER_PLOT_HEADER

#include <vector>
#include "EncounterPlot.h"
#include "ALogEntry.h"

class Populator_EncounterPlot 
{
public:
  Populator_EncounterPlot() {}
  ~Populator_EncounterPlot() {}
  
  bool     populateFromEntries(const std::vector<ALogEntry>&);
  EncounterPlot getEncounterPlot() {return(m_encounter_plot);}
  
 protected:
  EncounterPlot m_encounter_plot;
};

#endif 




