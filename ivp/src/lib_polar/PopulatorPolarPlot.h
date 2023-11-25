/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PopulatorPolarPlot.h                                 */
/*    DATE: May 5th, 2010                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef POPULATOR_POLAR_PLOT_HEADER
#define POPULATOR_POLAR_PLOT_HEADER

#include <string>
#include "PolarPlot.h"

class PopulatorPolarPlot
{
 public:
  PopulatorPolarPlot();
  ~PopulatorPolarPlot() {}
  
  bool readFile(std::string filename);
  
  PolarPlot getPolarPlot() const {return(m_polar_plot);}

 protected:

  PolarPlot m_polar_plot;

};
#endif 






