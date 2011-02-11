/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_RC.cpp                                           */
/*    DATE: Aug 5th, 2007                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "AOF_RC.h"
#include "AngleUtils.h"
#include "CPA_Utils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_RC::AOF_RC(IvPDomain gdomain) 
  : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_osx_set = false;
  m_osy_set = false;
  m_cnx_set = false;
  m_cny_set = false;
  m_cnh_set = false;
  m_cnv_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_RC::setParam(const string& param, double param_val)
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
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_RC::initialize()
{
  if((m_crs_ix==-1) || (m_spd_ix==-1))
    return(false);

  if(!m_osx_set || !m_osy_set || !m_cnx_set) 
    return(false);

  if(!m_cny_set || !m_cnh_set || !m_cnv_set) 
    return(false);

  return(true);
}


//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_RC::evalBox(const IvPBox *b) const
{
  double eval_crs, eval_spd, closing_spd;
  
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);
  
  closing_spd  = closingSpeed(m_osx, m_osy, eval_spd, eval_crs,
			      m_cnx, m_cny, m_cnv, m_cnh);

  cout << "spd: " << eval_spd << "  hdg: " << eval_crs;
  cout << "  roc: " << closing_spd << endl;
  return(closing_spd * 100);
}
