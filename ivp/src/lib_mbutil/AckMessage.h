/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AckMessage.h                                         */
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

#ifndef ACK_MESSAGE_HEADER
#define ACK_MESSAGE_HEADER

#include <string>

class AckMessage
{
 public:
  AckMessage(std::string src="", std::string dest="", std::string id="");
  ~AckMessage() {}

  // Setters
  void setSourceNode(std::string);
  void setDestNode(std::string);
  void setMessageID(std::string id) {m_id=id;}

  std::string getSourceNode() const {return(m_src_node);}
  std::string getDestNode() const   {return(m_dest_node);}
  std::string getMessageID() const  {return(m_id);}
  
  bool valid() const;

  std::string getSpec() const;

 protected: 
  std::string  m_id;
  std::string  m_src_node;
  std::string  m_dest_node;
};

AckMessage string2AckMessage(const std::string&);

#endif 

