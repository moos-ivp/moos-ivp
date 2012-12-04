/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeMessage.h                                        */
/*    DATE: Jan 7th 2011                                         */
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

#ifndef UFLD_NODE_MESSAGE_HEADER
#define UFLD_NODE_MESSAGE_HEADER

#include <string>

class NodeMessage
{
 public:
  NodeMessage();
  ~NodeMessage() {};

  void setSourceNode(const std::string& s)  {m_src_node=s;};
  void setDestNode(const std::string& s)    {m_dest_node=s;};
  void setDestGroup(const std::string& s)   {m_dest_group=s;};
  void setVarName(const std::string& s)     {m_var_name=s;};
  void setStringVal(const std::string& s)   {m_string_val=s;};

  void setDoubleVal(double v) {m_double_val=v; m_double_val_set=true;};


  std::string getSourceNode() const  {return(m_src_node);};
  std::string getDestNode() const    {return(m_dest_node);};
  std::string getDestGroup() const   {return(m_dest_group);};
  std::string getVarName() const     {return(m_var_name);};
  std::string getStringVal() const   {return(m_string_val);};
  double      getDoubleVal() const   {return(m_double_val);};

  unsigned int length() const;
    
  bool valid() const;

  std::string getSpec() const;

 protected: 
  std::string  m_src_node;
  std::string  m_dest_node;
  std::string  m_dest_group;
  std::string  m_var_name;
  std::string  m_string_val;
  double       m_double_val;
  bool         m_double_val_set;
};

#endif 


