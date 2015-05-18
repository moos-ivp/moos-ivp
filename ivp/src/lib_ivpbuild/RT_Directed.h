/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RT_Directed.h                                        */
/*    DATE: Mar 20th, 2007                                       */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef RT_FOCUS_HEADER
#define RT_FOCUS_HEADER

#include <vector>
#include "PDMap.h"
#include "PQueue.h"

class Regressor;

class RT_Directed {
public:
  RT_Directed(Regressor*);
  virtual ~RT_Directed() {}

public: 
  PDMap* create(PDMap*, const IvPBox&, const IvPBox&, PQueue&);

 protected:
  void   updatePQueue(PQueue&, const std::vector<int>& idx_map);

protected:
  Regressor* m_regressor;
};

#endif





