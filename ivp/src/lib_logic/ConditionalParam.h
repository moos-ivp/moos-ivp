/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ConditionalParam.h                                   */
/*    DATE: Apr 24th 2007                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#ifndef CONDITIONAL_PARAM_HEADER
#define CONDITIONAL_PARAM_HEADER

#include <string>
#include "LogicCondition.h"

class ConditionalParam {
public:
  ConditionalParam() {m_ok=false; m_comment_header="//";};
  ~ConditionalParam() {};
  
  void setCommentHeader(const std::string& s) {m_comment_header=s;};
  

  bool setFromString(const std::string&);

  bool setFromString(const std::string&, const std::string&);

  std::string getParam()    {return(m_parameter);};
  std::string getParamVal() {return(m_param_val);};

  LogicCondition getCondition() {return(m_condition);};
  
  bool ok()                 {return(m_ok);};

protected:
  LogicCondition     m_condition;
  std::string        m_parameter;
  std::string        m_param_val;
  std::string        m_comment_header;

  bool               m_ok;
};

#endif










