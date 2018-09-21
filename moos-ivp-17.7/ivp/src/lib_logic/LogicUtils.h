/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogicUtils.h                                         */
/*    DATE: October 12th 2006                                    */
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

#ifndef LOGIC_UTILS_HEADER
#define LOGIC_UTILS_HEADER

#include <vector>
#include <string>
#include "LogicCondition.h"

std::vector<std::string> parseRelation(std::string);

std::string pruneParens(std::string);

bool legalParens(std::string);

bool globalParens(std::string);

bool globalNotParens(std::string);

bool isValidVariable(std::string);

bool isValidLiteral(std::string);

bool isValidRightVariable(std::string);

bool isConditionalParamString(const std::string&, 
			      const std::string& s="//");

bool strFieldMatch(const std::string&, 
		   const std::string&, char c=':');

std::vector<std::string> getUniqueVars(const std::vector<LogicCondition>&);

#endif
















