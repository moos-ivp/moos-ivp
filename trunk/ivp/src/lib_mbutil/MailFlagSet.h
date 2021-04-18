/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MailFlagSet.h                                        */
/*    DATE: Apr 7th 2021                                         */
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

#ifndef MAIL_FLAG_SET_HEADER
#define MAIL_FLAG_SET_HEADER

#include <string>
#include <vector>
#include <map>
#include "VarDataPair.h"

class MailFlagSet
{
public:
  MailFlagSet();

  virtual ~MailFlagSet() {};

  unsigned int size() const {return(m_map_mail_flags.size());}

  bool addFlag(std::string);
  
  bool handleMail(std::string key, double curr_time);

  std::vector<VarDataPair> getNewFlags();

  std::vector<std::string> getMailFlagKeys() const;

 protected: // config vars

  // Map keyed on Mail Key (MOOS Var Name)
  std::map<std::string, std::vector<VarDataPair> > m_map_mail_flags;

  unsigned int m_max_new_flags;
  
 protected: // state vars

  unsigned int m_mail_total;

  // Map keyed on Mail Key (MOOS Var Name)
  std::map<std::string, unsigned int> m_map_mail_total;

  std::vector<VarDataPair> m_new_flags;
  
};

#endif 









