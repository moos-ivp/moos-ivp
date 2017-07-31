/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogicBuffer.H                                        */
/*    DATE: Sep 19th 2010 On the flight to IPAM                  */
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

#ifndef LOGIC_BUFFER_HEADER
#define LOGIC_BUFFER_HEADER

#include <string>
#include <vector>
#include "InfoBuffer.h"
#include "LogicCondition.h"

class LogicBuffer {
public:
  LogicBuffer();
  ~LogicBuffer();

public:
  bool addNewCondition(const std::string&);

  bool updateInfoBuffer(const std::string& moosvar,
			const std::string& value);

  bool updateInfoBuffer(const std::string& moosvar,
			double value);

  bool checkConditions();

  unsigned int size() {return(m_logic_conditions.size());}

  std::vector<std::string> getAllVars();

protected:
  std::vector<LogicCondition> m_logic_conditions;
  
  InfoBuffer *m_info_buffer;
};
#endif









