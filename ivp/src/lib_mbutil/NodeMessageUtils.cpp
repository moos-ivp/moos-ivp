/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeMessageUtils.cpp                                 */
/*    DATE: Jan 7th 2011                                         */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "NodeMessageUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: string2NodeMessage
//   Example: src_node=ike,dest_node=henry,var_name=FOO,string_val=bar
//            or
//            src_node=ike,dest_group=red,var_name=FOO,string_val=bar
//            or
//            src_node=ike,dest_group=red,var_name=FOO,double_val=3.4

NodeMessage string2NodeMessage(const string& message_string)
{
  NodeMessage empty_message;
  NodeMessage new_message;

  string string_val;
  bool   string_val_quoted = false;

  vector<string> svector = parseStringQ(message_string, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    
    if(param == "src_node")
      new_message.setSourceNode(value);
    else if(param == "dest_node")
      new_message.setDestNode(value);
    else if(param == "dest_group")
      new_message.setDestGroup(value);
    else if(param == "var_name")
      new_message.setVarName(value);
    else if(param == "string_val")
      string_val = value;
    else if(param == "color")
      new_message.setColor(value);
    else if((param == "string_val_quoted") && (value == "true"))
      string_val_quoted = true;
    else if(param == "double_val")
      new_message.setDoubleVal(atof(value.c_str()));
  }

  // If extra quotes were automatically added to the string val to hide
  // separators in the string, remove them now.
  
  if(isQuoted(string_val) && string_val_quoted)
    string_val = stripQuotes(string_val);
  
  new_message.setStringVal(string_val);

  if(!new_message.valid())
    return(empty_message);

  return(new_message);
}








