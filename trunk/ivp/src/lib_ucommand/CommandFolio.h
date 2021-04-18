/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CommandFolio.h                                       */
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

#ifndef COMMAND_FOLIO_HEADER
#define COMMAND_FOLIO_HEADER

#include <string>
#include <vector>
#include <set>
#include "CommandItem.h"

class CommandFolio
{
 public:
  CommandFolio();
  virtual ~CommandFolio() {}

  // Setters
  bool addCmdItem(CommandItem);
  void limitedVNames(std::set<std::string>);
  
  // Getters
  CommandItem getCmdItem(unsigned int) const;

  std::vector<CommandItem> getAllCmdItems() const;
  
  std::set<std::string> getLimitedVNames() const {return(m_set_limited_vnames);}

  bool hasLimitedVNames() const {return(m_limited_vnames);}

  std::string getCmdColor(std::string label) const;
  
  // Analyzers
  unsigned int size() const {return(m_cmd_items.size());}

  std::set<std::string> getAllReceivers() const;
  std::set<std::string> getAllLabels(std::string vname) const;

  void print() const;
  
 private:
  std::vector<CommandItem>  m_cmd_items;

  std::set<std::string> m_set_limited_vnames;

  bool m_limited_vnames;
};

#endif 




