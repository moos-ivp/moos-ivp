/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorIPP.h                                       */
/*    DATE: Nov 24th 2014                                        */
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

#ifndef POPULATOR_IPP_HEADER
#define POPULATOR_IPP_HEADER

#include <string>
#include "IvPProblem.h"
 
class PopulatorIPP
{
public:
  PopulatorIPP() {m_ivp_problem=0; m_grid_override_size=0;}
  ~PopulatorIPP() {}
  
  bool populate(std::string filename, int alg=0);

  void setVerbose(bool v)          {m_verbose=v;}
  IvPProblem* getIvPProblem()      {return(m_ivp_problem);}

  void setGridOverrideSize(int v)  {m_grid_override_size=v;}
  
protected:
  bool handleLine(std::string);
  void overrideGrid(IvPFunction*);

  
protected:
  IvPProblem* m_ivp_problem;
  bool        m_verbose;

  int         m_grid_override_size;

};
#endif




