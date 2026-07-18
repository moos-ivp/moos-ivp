/*****************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: LegRunSet.h                                     */
/*    DATE: June 11th, 2023                                 */
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

#ifndef LEG_RUN_SET_HEADER
#define LEG_RUN_SET_HEADER

#include <string>
#include <map>
#include "LegRun.h"

class LegRunSet
{
public:
   LegRunSet();
  ~LegRunSet() {};

public: // Leg Setters
  bool setLegParams(std::string);

  bool handleLegRunFile(std::string, double, std::string&);
  
public: // Getters
  
  std::string getLegRunFile() const {return(m_legrun_file);}

  LegRun getLegRun(std::string id) const;

  std::vector<std::string> getLegIDs() const;

  std::vector<LegRun> getLegRuns() const;
  
public: // Analyzers

  unsigned int size() const {return(m_map_legruns.size());}
  
private: 
  std::map<std::string, LegRun> m_map_legruns;
  
  std::string m_legrun_file;
};

#endif 
