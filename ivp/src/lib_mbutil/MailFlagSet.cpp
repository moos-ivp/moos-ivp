/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VarDataPair.cpp                                      */
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

#include <iostream>
#include <cstdlib>
#include "MailFlagSet.h"
#include "MBUtils.h"
#include "MacroUtils.h"

using namespace std;

//------------------------------------------------------------------
// Procedure: constructor

MailFlagSet::MailFlagSet()
{
  m_mail_total = 0;

  m_max_new_flags = 1000;
}


//------------------------------------------------------------------
// Procedure: addFlag()
//   Example: @REPORT UPDATE=completed
//            @ANY MAIL_UPDATE=$[CNT]
//            @DEPLOY=true MARK=$[TIME]
//            @DEPLOY=true MARK=$[UTC]
//
// Supported Macros: $[CNT], $[UTC], $[TIME], $[KEY]

bool MailFlagSet::addFlag(string flag_str)
{
  string key = biteStringX(flag_str, '#');

  if((key == "") || (key[0] != '@'))
    return(false);

  biteString(key, '@');
  if(key == "") 
    return(false);
  
  string moosvar = biteStringX(flag_str, '=');
  string moosval = flag_str;

  if((moosvar == "") || (moosval == ""))
    return(false);
  
  VarDataPair pair(moosvar, moosval, "auto");

  m_map_mail_flags[key].push_back(pair);

  return(true);
}

//------------------------------------------------------------------
// Procedure: handleMail()

bool MailFlagSet::handleMail(string key, double curr_time)
{
  // Check if there are any pairs/flags for this key in particular
  vector<VarDataPair> flags;
  if(m_map_mail_flags.count(key))
    flags = m_map_mail_flags[key];
  else if(m_map_mail_flags.count("ANY"))
    flags = m_map_mail_flags["ANY"];
  else
    return(false);

  // Update total mail received of any mail_flag MOOS Variable (key)
  m_mail_total++;
  m_map_mail_total[key]++;
  
  // Expand macros for flags before returning all flags
  unsigned int key_cnt = m_mail_total;
  if(m_map_mail_total.count(key))
    key_cnt = m_map_mail_total[key];

  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    if(pair.is_string()) {
      string sval = pair.get_sdata();

      sval = macroExpand(sval, "CNT", key_cnt);
      sval = macroExpand(sval, "MCNT", m_mail_total);
      sval = macroExpand(sval, "UTC", doubleToStringX(curr_time,2));
      
      flags[i].set_sdata(sval, true);
    }

    if(m_new_flags.size() < m_max_new_flags)
      m_new_flags.push_back(flags[i]);
  }
  return(true);
}

//------------------------------------------------------------------
// Procedure: getMailFlagKeys()

vector<string> MailFlagSet::getMailFlagKeys() const
{
  vector<string> keys;

  map<string, vector<VarDataPair> >::const_iterator p;
  for(p=m_map_mail_flags.begin(); p!=m_map_mail_flags.end(); p++)
    keys.push_back(p->first);

  return(keys);
}


//------------------------------------------------------------------
// Procedure: getNewFlags()

vector<VarDataPair> MailFlagSet::getNewFlags()
{
  vector<VarDataPair> new_flags = m_new_flags;

  m_new_flags.clear();

  return(new_flags);
}



