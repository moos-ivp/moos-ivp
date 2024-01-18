/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_Waypoint.h                                       */
/*    DATE: Sep 30th, 2022                                       */
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
 
#ifndef HINT_HOLDER_HEADER
#define HINT_HOLDER_HEADER

#include <string>
#include <map>
#include "XYPoint.h"
#include "XYSegList.h"
#include "XYPolygon.h"

class HintHolder {
public:
  HintHolder();
  ~HintHolder() {}

  bool setColor(std::string, std::string);
  bool setMeasure(std::string, double);
  bool setMeasure(std::string, std::string);

  bool setHints(std::string);
  bool setHint(std::string);
  bool setHint(std::string, std::string);
  
  std::string getColor(std::string key) const;
  double getMeasure(std::string key) const;

  bool hasColor(std::string key) const;
  bool hasMeasure(std::string key) const;

  std::string getSpec() const;
  
protected: // configuration parameters

  std::map<std::string, std::string> m_map_colors;
  std::map<std::string, double>      m_map_measures;
};

void applyHints(XYPoint&, const HintHolder&, std::string prefix="");
void applyHints(XYSegList&, const HintHolder&, std::string prefix="");
void applyHints(XYPolygon&, const HintHolder&, std::string prefix="");

#endif

