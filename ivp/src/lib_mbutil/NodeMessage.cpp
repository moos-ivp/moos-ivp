/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeMessage.cpp                                      */
/*    DATE: Jan 7th 2011                                         */
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

#include "NodeMessage.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//------------------------------------------------------------
// Constructor

NodeMessage::NodeMessage(string src, string dest, string var)
{
  m_src_node   = src;
  m_dest_node  = dest;
  m_var_name   = var;

  m_double_val     = 0; 
  m_double_val_set = false;
  m_ack_requested  = false;
}

//------------------------------------------------------------
// Procedure: setStringVal()

void NodeMessage::setStringVal(const string& s)
{
  if(!isQuoted(s) && strContains(s, ','))
    m_string_val = "\"" + s + "\"";
  else
    m_string_val = s;
}

//------------------------------------------------------------
// Procedure: setColor()

void NodeMessage::setColor(const string& color_str)
{
  if(isColor(color_str))
    m_color = color_str;
}


//------------------------------------------------------------
// Procedure: getSpec()

string NodeMessage::getSpec(bool include_ack_info) const
{
  string str;
  if((m_msg_id != "") && include_ack_info)
    str = augmentSpec(str, "ack_id=" + m_msg_id);
  if((m_ack_requested) && include_ack_info)
    str = augmentSpec(str, "ack=true");
  if(m_src_node != "")
    str = augmentSpec(str, "src_node=" + m_src_node);
  if(m_dest_node != "") 
    str = augmentSpec(str, "dest_node=" + m_dest_node);
  if(m_dest_group != "") 
    str = augmentSpec(str, "dest_group=" + m_dest_group);
  if(m_src_app != "") 
    str = augmentSpec(str, "src_app=" + m_src_app);
  if(m_src_bhv != "") 
    str = augmentSpec(str, "src_bhv=" + m_src_bhv);
  if(m_var_name != "") 
    str = augmentSpec(str, "var_name=" + m_var_name);
  if(m_color != "") 
    str = augmentSpec(str, "color=" + m_color);
  if(m_double_val_set) {
    string str_dval = doubleToStringX(m_double_val,6);
    str = augmentSpec(str, "double_val=" + str_dval);
  }

  if(m_string_val != "") {
    if(str != "")
      str += ",";

    if(!isQuoted(m_string_val) && strContains(m_string_val, ","))
      str += "string_val=\"" + m_string_val + "\",string_val_quoted=true";
    else
      str += "string_val=" + m_string_val;
  }
  
  return(str);
}

//---------------------------------------------------------------
// Procedure: valid()
//      Note: Determines if all the required fields have been set

bool NodeMessage::valid() const
{
  if(m_src_node == "")
    return(false);

  if((m_dest_node == "") && (m_dest_group == ""))
    return(false);

  if(m_var_name == "")
    return(false);

  if((m_string_val != "") && m_double_val_set)
    return(false);

  return(true);
}


//---------------------------------------------------------------
// Procedure: getLength()
//      Note: Length is max of:
//            - size of double (2)
//            - length of string contents (1 per char)

unsigned int NodeMessage::length() const
{
  unsigned int overall_msg_length = 2;
  
  unsigned int str_length = m_string_val.length();
  if(str_length > 2)
    overall_msg_length = str_length;

  return(overall_msg_length);
}

//---------------------------------------------------------------
// Procedure: getStringValX()
//      Note: A convenience function to also strip off quotes if 
//            they are part of the returned string value

string NodeMessage::getStringValX() const
{
  return(stripQuotes(m_string_val));
}

