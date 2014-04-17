/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Contact.h                                        */
/*    DATE: May 17th, 2013 (Generalizing over existing classes)  */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include "AOF_Contact.h"

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
  m_pwt_inner_distance = 0;
  m_pwt_outer_distance = 0;

  m_collision_distance_set = false;
  m_all_clear_distance_set = false;
  m_pwt_inner_distance_set = false;
  m_pwt_outer_distance_set = false;
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

void AOF_Contact::setContactParams(double cnx, double cny, double cnh, double cnv)
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
  else if(param == "pwt_inner_distance") {
    m_pwt_inner_distance = param_val;
    m_pwt_inner_distance_set = true;
    return(true);
  }
  else if(param == "pwt_outer_distance") {
    m_pwt_outer_distance = param_val;
    m_pwt_outer_distance_set = true;
    return(true);
  }
  else if(param == "tol") {
    m_tol = param_val;
    m_tol_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_Contact::initialize()
{
  // Check that the domain is exactly over the two vars course and speed.

  if(!m_domain.hasDomain("course") || !m_domain.hasDomain("speed"))
    return(false);

  //if(m_domain.size() != 2)
  //  return(false);

  if(!m_osx_set || !m_osy_set || !m_cnx_set) 
    return(false);
  
  if(!m_cny_set || !m_cnh_set || !m_cnv_set) 
    return(false);
  
  if(!m_collision_distance_set || 
     !m_all_clear_distance_set || !m_tol_set) 
    return(false);
  
  if(m_collision_distance > m_all_clear_distance)
    return(false);
  
  m_cpa_engine = CPAEngine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);

  return(true);
}




