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

#ifndef RT_UNIFORM_HEADER
#define RT_UNIFORM_HEADER

#include "PDMap.h"
#include "PQueue.h"
#include "Regressor.h"

class Regressor;

class RT_Uniform {
public:
  RT_Uniform(Regressor *regressor) {m_regressor=regressor;}
  virtual ~RT_Uniform() {}
  
public: 
  PDMap*  create(const IvPBox*u, const IvPBox*);
  
 protected:
  Regressor* m_regressor;
};

#endif








