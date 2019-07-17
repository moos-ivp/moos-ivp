/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Contact.h                                        */
/*    DATE: May 17th, 2013 (Generalizing over existing classes)  */
/*    DATE: Nov 30th, 2017 (Revised handling of CPAEngine)       */
/*    DATE: Jun 6th,  2018 (More Revised handling of CPAEngine)  */
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
#include "AOF_Contact.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_Contact::AOF_Contact(IvPDomain gdomain) : AOF(gdomain)
{
  m_tol_set = false;
  m_osx_set = false;
  m_osy_set = false;
  m_cnx_set = false;
  m_cny_set = false;
  m_cnh_set = false;
  m_cnv_set = false;

  m_collision_distance = 0;
  m_all_clear_distance = 0;

  m_collision_distance_set = false;
  m_all_clear_distance_set = false;

  m_stat_bng_os_cn = 0;

  m_cpa_engine_initialized = false;
}

//----------------------------------------------------------------
// Procedure: setCPAEngine()
//      Note: Typically a CPAEngine is owned by the caller and is passed
//            along to this AOF after creation. The owner will have
//            populated the trig cache once, thereby avoiding the needless
//            recalculation of trig functions 

void AOF_Contact::setCPAEngine(const CPAEngine& engine)
{
  m_cpa_engine = engine;
  m_cpa_engine_initialized = true;
}

//----------------------------------------------------------------
// Procedure: setOwnshipParams

void AOF_Contact::setOwnshipParams(double osx, double osy)
{
  m_osx = osx;
  m_osy = osy;

  m_osx_set = true;
  m_osy_set = true;
}

//----------------------------------------------------------------
// Procedure: setContactParams

void AOF_Contact::setContactParams(double cnx, double cny,
				   double cnh, double cnv)
{
  m_cnx = cnx;
  m_cny = cny;
  m_cnh = cnh;
  m_cnv = cnv;

  m_cnx_set = true;
  m_cny_set = true;
  m_cnh_set = true;
  m_cnv_set = true;
}


//----------------------------------------------------------------
// Procedure: setParam

bool AOF_Contact::setParam(const string& param, double param_val)
{
  if(param == "osx") {
    m_osx = param_val;
    m_osx_set = true;
    return(true);
  }
  else if(param == "osy") {
    m_osy = param_val;
    m_osy_set = true;
    return(true);
  }
  else if(param == "cnx") {
    m_cnx = param_val;
    m_cnx_set = true;
    return(true);
  }
  else if(param == "cny") {
    m_cny = param_val;
    m_cny_set = true;
    return(true);
  }
  else if(param == "cnh") {
    m_cnh = param_val;
    m_cnh_set = true;
    return(true);
  }
  else if(param == "cnv") {
    m_cnv = param_val;
    m_cnv_set = true;
    return(true);
  }
  else if(param == "collision_distance") {
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
// Procedure: initialize

bool AOF_Contact::initialize()
{
  // Check that the domain is exactly over the two vars course and speed.

  if(!m_domain.hasDomain("course") || !m_domain.hasDomain("speed")) {
    postMsgAOF("no crs or spd in IvPDomain");
    return(false);
  }

  if(!m_osx_set || !m_osy_set || !m_cnx_set) {
    postMsgAOF("osx, osy, or cnx not set");
    return(false);
  }
  
  if(!m_cny_set || !m_cnh_set || !m_cnv_set) {
    postMsgAOF("cny, cnh, or cnv not set");
    return(false);
  }
  
  if(!m_collision_distance_set) {
    postMsgAOF("collision_distance not set");
    return(false);
  }

  if(!m_all_clear_distance_set) {
    postMsgAOF("all_clear_distance not set");
    return(false);
  }
  
  if(!m_tol_set) {
    postMsgAOF("tol not set");
    return(false);
  }
  
  if(m_collision_distance > m_all_clear_distance) {
    postMsgAOF("collision_dist > all_clear_dist");
    return(false);
  }

  m_stat_bng_os_cn = relAng(m_osx, m_osy, m_cnx, m_cny);

  // Initialization will be avoided if the user has already initialized
  // or if the CPAEngine was set already via the setCPAEngine() function.
  // For this reason, users should make sure that setCPAEngine() is called
  // first, before this function, to make sure that the CPAEngine is not
  // initialized twice. Not incorrect if so, but this is inefficient since
  // CPAEngine initialization involves the building of caches.
  if(!m_cpa_engine_initialized)
    m_cpa_engine.reset(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);

  return(true);
}


//----------------------------------------------------------------
// Procedure: getCNSpeedInOSPos()

double AOF_Contact::getCNSpeedInOSPos() const
{
  return(m_cpa_engine.getCNSpeedInOSPos());
}

//----------------------------------------------------------------
// Procedure: aftOfContact()

bool AOF_Contact::aftOfContact() const
{
  return(m_cpa_engine.aftOfContact());
}

//----------------------------------------------------------------
// Procedure: portOfContact()

bool AOF_Contact::portOfContact() const
{
  return(m_cpa_engine.portOfContact());
}

//----------------------------------------------------------------
// Procedure: getRangeGamma()

double AOF_Contact::getRangeGamma() const
{
  return(m_cpa_engine.getRangeGamma());
}
