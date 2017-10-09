/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CommandItem.h                                        */
/*    DATE: July 2nd, 2016                                       */
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

#ifndef COMMAND_ITEM_HEADER
#define COMMAND_ITEM_HEADER

#include <set>
#include <string>
#include <vector>

class CommandItem
{
 public:
  CommandItem();
  virtual ~CommandItem() {}

  // Setters
  void setCmdLabel(std::string s)  {m_label=s;}

  bool setCmdPostStr(std::string var, std::string sval);
  bool setCmdPostDbl(std::string var, double dval);

  bool addCmdPostReceiver(std::string);
  void limitedVNames(std::set<std::string>);

  void setCmdColor(std::string s) {m_bcolor=s;}
  
  // Getters
  std::string getCmdLabel() const {return(m_label);}

  std::string getCmdPostVar() const {return(m_moosvar);}
  std::string getCmdPostStr() const {return(m_valstr);}
  double      getCmdPostDbl() const {return(m_valdbl);}
  std::string getCmdPostType() const {return(m_valtype);}
  std::string getCmdColor() const   {return(m_bcolor);}

  std::string getCmdReceiver(unsigned int) const;
  std::vector<std::string> getAllReceivers() const;

  // Analyzers
  unsigned int totalReceivers() const {return(m_receivers.size());}

  bool hasReceiver(std::string) const;
  
  void print() const;
  
 private:

  std::string  m_label;
  std::string  m_moosvar;
  std::string  m_valstr;
  double       m_valdbl;
  std::string  m_valtype;
  std::string  m_bcolor;  

  std::vector<std::string>  m_receivers;
};

#endif 




