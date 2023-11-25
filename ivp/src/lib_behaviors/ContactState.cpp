/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactState.cpp                                     */
/*    DATE: Sep 7th 2020                                         */
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

#include "ContactState.h"
#include "MBUtils.h"
#include "MacroUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

ContactState::ContactState()
{
  m_cnutc = 0;
  m_update_ok = false;
  m_helm_iter = 0;

  m_range = 0;
  
  m_os_fore_of_cn = false;
  m_os_aft_of_cn  = false;
  m_os_port_of_cn = false;
  m_os_star_of_cn = false;

  m_cn_fore_of_os = false;
  m_cn_aft_of_os  = false;
  m_cn_port_of_os = false;
  m_cn_star_of_os = false;
  
  m_cn_spd_in_os_pos = 0;

  m_os_cn_rel_bng = 0;
  m_cn_os_rel_bng = 0;
  m_os_cn_abs_bng = 0;
  
  m_rate_of_closure = 0;
  m_bearing_rate    = 0;
  m_contact_rate    = 0;
  
  m_range_gamma   = 0;
  m_range_epsilon = 0;
  
 
  m_os_passes_cn = false;
  m_os_passes_cn_port = false;
  m_os_passes_cn_star = false;
  
  m_cn_passes_os = false;
  m_cn_passes_os_port = false;
  m_cn_passes_os_star = false;
  
  m_os_crosses_cn = false;
  m_os_crosses_cn_stern = false;
  m_os_crosses_cn_bow   = false;
  m_os_crosses_cn_bow_dist = 0;

  m_cn_crosses_os = false;
  m_cn_crosses_os_bow = false;
  m_cn_crosses_os_stern = false;
  m_cn_crosses_os_bow_dist = 0;

  m_os_curr_cpa_dist = 0;

  m_index = 0;
}


//-----------------------------------------------------------
// Procedure: cnMacroExpand()

string ContactState::cnMacroExpand(string sdata, string macro) const
{
  if(macro == "ROC")
    sdata = macroExpand(sdata, macro, doubleToStringX(m_rate_of_closure,1));
  else if(macro == "OS_CN_REL_BNG")
    sdata = macroExpand(sdata, macro, doubleToStringX(m_os_cn_rel_bng,1));
  else if(macro == "CN_OS_REL_BNG")
    sdata = macroExpand(sdata, macro, doubleToStringX(m_cn_os_rel_bng,1));
  else if(macro == "BNG_RATE")
    sdata = macroExpand(sdata, macro, doubleToStringX(m_bearing_rate,1));

  else if(macro == "OS_FORE_OF_CN")
    sdata = macroExpand(sdata, macro, boolToString(m_os_fore_of_cn));
  else if(macro == "OS_AFT_OF_CN")
    sdata = macroExpand(sdata, macro, boolToString(m_os_aft_of_cn));
  else if(macro == "OS_PORT_OF_CN")
    sdata = macroExpand(sdata, macro, boolToString(m_os_port_of_cn));
  else if(macro == "OS_STAR_OF_CN")
    sdata = macroExpand(sdata, macro, boolToString(m_os_star_of_cn));

  else if(macro == "CN_FORE_OF_OS")
    sdata = macroExpand(sdata, macro, boolToString(m_cn_fore_of_os));
  else if(macro == "CN_AFT_OF_OS")
    sdata = macroExpand(sdata, macro, boolToString(m_cn_aft_of_os));
  else if(macro == "CN_PORT_OF_OS")
    sdata = macroExpand(sdata, macro, boolToString(m_cn_port_of_os));
  else if(macro == "CN_STAR_OF_OS")
    sdata = macroExpand(sdata, macro, boolToString(m_cn_star_of_os));

  else if(macro == "CN_SPD_IN_OS_POS")
    sdata = macroExpand(sdata, macro, doubleToStringX(m_cn_spd_in_os_pos,1));

  return(sdata);
}
  

