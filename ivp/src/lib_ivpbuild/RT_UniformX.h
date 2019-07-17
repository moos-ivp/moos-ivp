/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RT_Uniform.h                                         */
/*    DATE: Aug 26th 2005 (from OFR_AOF written long ago)        */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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

#ifndef RT_UNIFORM_X_HEADER
#define RT_UNIFORM_X_HEADER

#include "PDMap.h"
#include "PQueue.h"
#include "Regressor.h"

class Regressor;

class RT_UniformX {
public:
  RT_UniformX(Regressor *regressor)
  {m_regressor=regressor; m_verbose=false;}
  virtual ~RT_UniformX() {}

  void setVerbose() {m_verbose=true;}
  
public:
  void setPlateaus(std::vector<IvPBox> plateaus) {m_plateaus=plateaus;} 
  
  PDMap*  create(const IvPBox& unifbox, const IvPBox& gelbox);

 private:
  void handleOverlappingPlateaus();
  
  BoxSet *subtractPlateaus(BoxSet*);
  BoxSet *subtractPlateau(BoxSet*, const IvPBox&);
  
 protected:
  Regressor* m_regressor;

  std::vector<IvPBox> m_plateaus;

  bool m_verbose;
};

#endif

