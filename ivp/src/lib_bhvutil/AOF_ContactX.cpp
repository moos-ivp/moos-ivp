/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_Contact.h                                        */
/*    DATE: May 17th, 2013 (Generalizing over existing classes)  */
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
#include "AOF_ContactX.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

AOF_ContactX::AOF_ContactX(IvPDomain domain, CPXEngine* eng)
  : AOF(domain)
{
  m_cpa_engine = eng;
  
  m_crs_ix = domain.getIndex("course");
  m_spd_ix = domain.getIndex("speed");

  m_tol_set = false;
  m_collision_distance = 0;
  m_all_clear_distance = 0;

  m_collision_distance_set = false;
  m_all_clear_distance_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam()

bool AOF_ContactX::setParam(const string& param, double param_val)
{
  if(param == "collision_distance") {
    m_collision_distance = param_val;
    m_collision_distance_set = true;
    return(true);
  }
  else if(param == "all_clear_distance") {
    m_all_clear_distance = param_val;
    m_all_clear_distance_set = true;
    return(true);
  }
  else if(param == "tol") {
    m_tol = param_val;
    m_tol_set = true;
    return(true);
  }
  else {
    string msg = "bad param[" + param + "], value[";    
    msg += doubleToStringX(param_val) + "]";    
    postMsgAOF(msg);    
    return(false);
  }
}

//----------------------------------------------------------------
// Procedure: initialize()

bool AOF_ContactX::initialize()
{
  // Check that the domain is exactly over two vars course and speed.
  
  if(!m_domain.hasDomain("course") || !m_domain.hasDomain("speed")) 
    return(postMsgAOF("no crs or spd in IvPDomain"));
  
  if((m_crs_ix==-1) || (m_spd_ix==-1)) 
    return(postMsgAOF("crs_ix or spd_ix not set"));
  
  if(!m_cpa_engine) 
    return(postMsgAOF("null CPXEngine"));
  
  if(!m_collision_distance_set) 
    return(postMsgAOF("collision_distance not set"));
  
  if(!m_all_clear_distance_set) 
    return(postMsgAOF("all_clear_distance not set"));
  
  if(!m_tol_set) 
    return(postMsgAOF("tol not set"));
  
  if(m_collision_distance > m_all_clear_distance) 
    return(postMsgAOF("collision_dist > all_clear_dist"));
    
  return(true);
}



