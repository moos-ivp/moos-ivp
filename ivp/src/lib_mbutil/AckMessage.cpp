/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AckMessage.cpp                                       */
/*    DATE: Feb 17th 2022                                        */
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

#include "AckMessage.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

AckMessage::AckMessage(string src, string dest, string id)
{
  m_id = id;
  m_src_node = src;
  m_dest_node = dest;
}

//------------------------------------------------------------
// Procedure: setSourceNode()

void AckMessage::setSourceNode(string str)
{
  if(strContainsWhite(str))
    return;
  if(str == m_dest_node)
    return;
  m_src_node = str;
}

//------------------------------------------------------------
// Procedure: setDestNode()

void AckMessage::setDestNode(string str)
{
  if(strContainsWhite(str))
    return;
  if(str == m_src_node)
    return;
  m_dest_node = str;
}

//------------------------------------------------------------
// Procedure: getSpec()

string AckMessage::getSpec() const
{
  string str = "id=" + m_id;
  str += ", src=" + m_src_node + ", dest=" + m_dest_node;
  return(str);
}

//---------------------------------------------------------------
// Procedure: valid()
//      Note: Determines if all the required fields have been set

bool AckMessage::valid() const
{
  if((m_id == "") || (m_src_node == "")  || (m_dest_node == ""))
    return(false);
  
  return(true);
}


//---------------------------------------------------------
// Procedure: string2AckMessage()
//   Example: src=ike, dest=abe, id=abe_124

AckMessage string2AckMessage(const string& message_string)
{
  AckMessage empty_message;
  AckMessage new_message;

  string string_val;

  vector<string> svector = parseStringQ(message_string, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    
    if(param == "src")
      new_message.setSourceNode(value);
    else if(param == "dest")
      new_message.setDestNode(value);
    else if(param == "id")
      new_message.setMessageID(value);
  }

  if(!new_message.valid())
    return(empty_message);

  return(new_message);
}






