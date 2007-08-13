/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPA_Model.cpp                                        */
/*    DATE: Feb 19, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "CPA_Model.h"
#include "AOF_CutRangeCPA.h"
#include "AOF_Waypoint2D.h"
#include "AOF_AvoidCollisionRC.h"
#include "AOF_WaypointRateClosure.h"
#include "AOF_RC.h"
#include "OF_Reflector.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "AngleUtils.h"

using namespace std;

CPA_Model::CPA_Model()
{
  m_os_x        = 0;
  m_os_y        = 0;
  m_os_maxtol   = 60;  // seconds
  m_os_maxspd   = 5.0; // meters per second
  m_os_wantcpa  = 50;  // meters

  m_cnx.push_back(-100);
  m_cny.push_back(-100);
  m_cnspd.push_back(2.0);
  m_cncrs.push_back(90.0);

  m_cnx.push_back(200);
  m_cny.push_back(-100);
  m_cnspd.push_back(4.0);
  m_cncrs.push_back(30.0);

  m_ivp_domain.addDomain("course", 0, 359, 360);
  m_ivp_domain.addDomain("speed",  0, m_os_maxspd, 51);
}


// ----------------------------------------------------------
// Procedure: shift_os_maxspd
//   Purpose: 

void CPA_Model::shift_os_maxspd(double v)
{
  m_os_maxspd += v/10;
  if(m_os_maxspd < 0)
    m_os_maxspd = 0;

  IvPDomain new_ivp_domain;
  new_ivp_domain.addDomain("course", 0, 359, 360);
  new_ivp_domain.addDomain("speed",  0, m_os_maxspd, 51);
  
  m_ivp_domain = new_ivp_domain;
}

// ----------------------------------------------------------
// Procedure: shift_os_maxtol
//   Purpose: 

void CPA_Model::shift_os_maxtol(double v)
{
  m_os_maxtol += v;
  if(m_os_maxtol < 5)
    m_os_maxtol = 5;
}


// ----------------------------------------------------------
// Procedure: set_cnx
//   Purpose: 

void CPA_Model::set_cnx(double val, int index)
{
  if((index < 0) || (index >= m_cnx.size()))
    return;
  m_cnx[index] = val;
}

// ----------------------------------------------------------
// Procedure: set_cny
//   Purpose: 

void CPA_Model::set_cny(double val, int index)
{
  if((index < 0) || (index >= m_cny.size()))
    return;
  m_cny[index] = val;
}

// ----------------------------------------------------------
// Procedure: shift_cnx
//   Purpose: 

void CPA_Model::shift_cnx(double val, int index)
{
  if((index < 0) || (index >= m_cnx.size()))
    return;
  m_cnx[index] += val;
}

// ----------------------------------------------------------
// Procedure: shift_cny
//   Purpose: 

void CPA_Model::shift_cny(double val, int index)
{
  if((index < 0) || (index >= m_cny.size()))
    return;
  m_cny[index] += val;
}

// ----------------------------------------------------------
// Procedure: shift_cnspd
//   Purpose: 

void CPA_Model::shift_cnspd(double v, int index)
{
  if((index < 0) || (index >= m_cnspd.size()))
    return;
  m_cnspd[index] += v/10;
  if(m_cnspd[index] < 0)
    m_cnspd[index] = 0;
}

// ----------------------------------------------------------
// Procedure: shift_cncrs
//   Purpose: 

void CPA_Model::shift_cncrs(double v, int index)
{
  if((index < 0) || (index >= m_cncrs.size()))
    return;
  m_cncrs[index] += v;
  if(m_cncrs[index] < 0)
    m_cncrs[index] += 360;
  if(m_cncrs[index] >= 360)
    m_cncrs[index] -= 360;
}

// ----------------------------------------------------------
// Procedure: get_param
//   Purpose: 

double CPA_Model::get_param(const string& param, int index) const
{
  if(param == "osx")
    return(m_os_x);
  else if(param == "osy")
    return(m_os_y);
  else if(param == "os_maxtol")
    return(m_os_maxtol);
  else if(param == "os_maxspd")
    return(m_os_maxspd);
  else if(param == "wantcpa")
    return(m_os_wantcpa);

  else if(param == "cnx") {
    if((index < 0) || (index >= m_cnx.size()))
      return(0);
    else
      return(m_cnx[index]);
  }

  else if(param == "cny") {
    if((index < 0) || (index >= m_cny.size()))
      return(0);
    else
      return(m_cny[index]);
  }

  if(param == "cnspd") {
    if((index < 0) || (index >= m_cnspd.size()))
      return(0);
    else
      return(m_cnspd[index]);
  }

  if(param == "cncrs") {
    if((index < 0) || (index >= m_cncrs.size()))
      return(0);
    else
      return(m_cncrs[index]);
  }
}

// ----------------------------------------------------------
// Procedure: calc_cpa_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_cpa_ipf(int ix, int unif_units)
{
  if((ix < 0) || (ix >= this->size()))
    return(0);
  
  AOF_CutRangeCPA aof(m_ivp_domain);

  aof.setParam("cnlat", m_cny[ix]);
  aof.setParam("cnlon", m_cnx[ix]);
  aof.setParam("cncrs", m_cncrs[ix]);
  aof.setParam("cnspd", m_cnspd[ix]);
  aof.setParam("oslat", m_os_y);
  aof.setParam("oslon", m_os_x);
  aof.setParam("tol",   m_os_maxtol);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);

  if(unif_units <= 0) 
    unif_units = 1;

  IvPBox unifbox(2);
  unifbox.setPTS(0, unif_units, unif_units);
  unifbox.setPTS(1, unif_units, unif_units);
  reflector.createUniform(&unifbox, &unifbox);
  
  IvPFunction *ipf = reflector.extractOF(false);

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: calc_wpt_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_wpt_ipf(int ix, int unif_units)
{
  if((ix < 0) || (ix >= this->size()))
    return(0);

#if 0
  ZAIC_PEAK spd_zaic(m_ivp_domain, "speed");
  spd_zaic.setSummit(m_cnspd[ix]);
  spd_zaic.setBaseWidth(2.0);
  spd_zaic.setSummitDelta(80);
  spd_zaic.setPeakWidth(1.0);
  spd_zaic.setMinMaxUtil(0,50);
  IvPFunction *spd_of = spd_zaic.extractOF();
  
  double rel_ang_to_wpt = relAng(m_os_x, m_os_y, m_cnx[ix], m_cny[ix]);
  ZAIC_PEAK crs_zaic(m_ivp_domain, "course");
  crs_zaic.setSummit(rel_ang_to_wpt);
  crs_zaic.setBaseWidth(90.0);
  crs_zaic.setBaseWidth(90.0);
  crs_zaic.setValueWrap(true);
  IvPFunction *crs_of = crs_zaic.extractOF();
  crs_of->setPWT(200);

  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(crs_of, spd_of);
#endif

#if 0
  IvPBox unifbox(2);
  unifbox.setPTS(0, unif_units, unif_units);
  unifbox.setPTS(1, unif_units, unif_units);

  AOF_Waypoint2D aof_wpt(m_ivp_domain);
  aof_wpt.setParam("speed", 2.0);
  aof_wpt.setParam("oslat", m_os_y);
  aof_wpt.setParam("oslon", m_os_x);
  aof_wpt.setParam("ptlat", m_cnx[ix]);
  aof_wpt.setParam("ptlon", m_cny[ix]);
  aof_wpt.initialize();

  OF_Reflector reflector(&aof_wpt);
  reflector.createUniform(&unifbox);

  IvPFunction *ipf = reflector.extractOF();
#endif

#if 1
  IvPBox unifbox(2);
  unifbox.setPTS(0, unif_units, unif_units);
  unifbox.setPTS(1, unif_units, unif_units);

  bool ok = true;
  AOF_WaypointRateClosure aof_wpt(m_ivp_domain);
  ok = ok && aof_wpt.setParam("desired_speed", m_cnspd[ix]);
  ok = ok && aof_wpt.setParam("oslat", m_os_y);
  ok = ok && aof_wpt.setParam("oslon", m_os_x);
  ok = ok && aof_wpt.setParam("ptlat", m_cnx[ix]);
  ok = ok && aof_wpt.setParam("ptlon", m_cny[ix]);
  ok = ok && aof_wpt.initialize();

  if(!ok)
    return(0);

  OF_Reflector reflector(&aof_wpt);
  //reflector.createUniform(&unifbox);
  reflector.createUniform(2000, 8);
  reflector.createPriority(2000);

  IvPFunction *ipf = reflector.extractOF();
#endif

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: calc_avd_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_avd_ipf(int ix, int unif_units)
{
  if((ix < 0) || (ix >= this->size()))
    return(0);
  
  AOF_AvoidCollisionRC aof(m_ivp_domain);

  aof.setParam("cn_lat", m_cny[ix]);
  aof.setParam("cn_lon", m_cnx[ix]);
  aof.setParam("cn_crs", m_cncrs[ix]);
  aof.setParam("cn_spd", m_cnspd[ix]);
  aof.setParam("os_lat", m_os_y);
  aof.setParam("os_lon", m_os_x);
  aof.setParam("os_tol", m_os_maxtol);
  aof.setParam("collision_distance", 20);
  aof.setParam("all_clear_distance", 155);

  bool ok = aof.initialize();
  if(!ok) {
    cout << "Problem init AOF_AvoidCollisionRC" << endl;
    exit(0);
  }

  OF_Reflector reflector(&aof, 1);

  if(unif_units <= 0) 
    unif_units = 1;

  IvPBox unifbox(2);
  unifbox.setPTS(0, unif_units, unif_units);
  unifbox.setPTS(1, unif_units, unif_units);
  reflector.createUniform(&unifbox, &unifbox);
  
  IvPFunction *ipf = reflector.extractOF(false);

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: calc_roc_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_roc_ipf(int ix, int unif_units)
{
  if((ix < 0) || (ix >= this->size()))
    return(0);
  
  AOF_RC aof(m_ivp_domain);

  aof.setParam("cnx", m_cnx[ix]);
  aof.setParam("cny", m_cny[ix]);
  aof.setParam("cnh", m_cncrs[ix]);
  aof.setParam("cnv", m_cnspd[ix]);
  aof.setParam("osx", m_os_x);
  aof.setParam("osy", m_os_y);
   
  bool ok = aof.initialize();
  if(!ok) {
    cout << "Problem init AOF_RC" << endl;
    exit(0);
  }

  OF_Reflector reflector(&aof, 1);

  if(unif_units <= 0) 
    unif_units = 1;

  IvPBox unifbox(2);
  unifbox.setPTS(0, unif_units, unif_units);
  unifbox.setPTS(1, unif_units, unif_units);
  reflector.createUniform(&unifbox, &unifbox);
  
  IvPFunction *ipf = reflector.extractOF(false);

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: extrapolate
//   Purpose: 

vector<double> CPA_Model::extrapolate(double g_spd, double g_time, 
				      int g_cn_index)
{
  vector<double> rvector;
  if((g_cn_index < 0) || (g_cn_index >= this->size()))
    return(rvector);
 
#if 0 
  for(int i=0; i<g_time; i++) {
    IvPFunction *ipf = calc_ipf(g_cn_index, 3);
    if(!ipf)
      return(rvector);
    PDMap *pdmap = ipf->getPDMap();

    int    best_heading_ix;
    double best_heading_res;

    for(int j=0; j<360; j++) {
      IvPBox ebox(2);
      ebox.setPTS(0, j, j);
      ebox.setPTS(1, g_spd, g_spd);
      double value = pdmap->evalPoint(&ebox);
      if((j==0) || (value > best_heading_val)) {
	best_heading_ix = j;
	best_heading_res = value;
      }
    }



  return(ipf);
#endif
}
