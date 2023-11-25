/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ConditionalParam.h                                   */
/*    DATE: Apr 24th 2007                                        */
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

#ifndef CONDITIONAL_PARAM_HEADER
#define CONDITIONAL_PARAM_HEADER

#include <string>
#include "LogicCondition.h"

class ConditionalParam {
public:
  ConditionalParam() {m_ok=false; m_comment_header="//";}
  ~ConditionalParam() {}
  
  void setCommentHeader(const std::string& s) {m_comment_header=s;}
  

  bool setFromString(const std::string&);

  bool setFromString(const std::string&, const std::string&);

  std::string getParam()    {return(m_parameter);}
  std::string getParamVal() {return(m_param_val);}

  LogicCondition getCondition() {return(m_condition);}
  
  bool ok()                 {return(m_ok);}

protected:
  LogicCondition     m_condition;
  std::string        m_parameter;
  std::string        m_param_val;
  std::string        m_comment_header;

  bool               m_ok;
};

#endif




















