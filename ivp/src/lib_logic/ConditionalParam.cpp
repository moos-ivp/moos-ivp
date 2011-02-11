/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ConditionalParam.cpp                                 */
/*    DATE: October 2006                                         */
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

#include <iostream>
#include "ConditionalParam.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------ 
// Procedure: setFromString
//
//    "param = val [state = true]"


bool ConditionalParam::setFromString(const string& giv_str)
{
  string str;

  if(m_comment_header == "")
    str = giv_str;
  else
    str = stripComment(giv_str, m_comment_header);

  str = stripBlankEnds(str);

  int len = str.length();
  int pos = str.find_last_of('[');
  if(str[len-1] != ']')
    return(false);
  if(pos <= 0)
    return(false);

  string param_str, condi_str;

  param_str.assign(str, 0, pos-1);
  param_str = stripBlankEnds(param_str);

  // Assuming from above the '[' is at pos, and ']' at len-1
  // strip off the brackets here.
  condi_str.assign(str, pos+1, len-2);

  int eqpos = param_str.find('=');
  int plen  = param_str.length();
  
  // if '=' not present or in first or last position, ret false
  if((eqpos <= 0) || (eqpos == len-1))
    return(false);
  
  m_parameter.assign(param_str, 0, eqpos-1);
  m_parameter = stripBlankEnds(m_parameter);

  m_param_val.assign(param_str, eqpos+1, plen-1);
  m_param_val = stripBlankEnds(m_param_val);
    
  m_ok = true;
  return(true);
}


//------------------------------------------------------ 
// Procedure: setFromString
// 
//    "param", "val [state = true]"


bool ConditionalParam::setFromString(const string& param_str,
				     const string& value_str)
{
  string str;

  if(m_comment_header == "")
    str = value_str;
  else
    str = stripComment(value_str, m_comment_header);

  str = stripBlankEnds(str);

  int len = str.length();
  int pos = str.find_last_of('[');
  if(str[len-1] != ']')
    return(false);
  if(pos <= 0)
    return(false);


  m_parameter = stripBlankEnds(param_str);

  m_param_val.assign(str, 0, pos-1);
  m_param_val = stripBlankEnds(m_param_val);

  // Assuming from above the '[' is at pos, and ']' at len-1
  // strip off the brackets here.
  string condi_str;
  condi_str.assign(str, pos+1, len-2);

  m_condition.setCondition(condi_str);

    
  m_ok = true;
  return(true);
}




