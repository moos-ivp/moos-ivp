/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPA_Model.cpp                                        */
/*    DATE: Feb 19, 2007                                         */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "CPA_Model.h"
#include "AOF_CutRangeCPA.h"
#include "AOF_Waypoint2D.h"
#include "AOF_AvoidCollisionRC.h"
#include "AOF_WaypointRateClosure.h"
#include "AOF_SimpleWaypoint.h"
#include "AOF_RC.h"
#include "AOF_R16.h"
#include "OF_Reflector.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

// ----------------------------------------------------------
// Constructor

CPA_Model::CPA_Model()
{
  m_os_x        = 0;
  m_os_y        = 0;
  m_os_maxtol   = 60;  // seconds
  m_os_maxspd   = 5.0; // meters per second
  m_os_wantcpa  = 50;  // meters

  push_new_contact();
  
  m_ivp_domain.addDomain("course", 0, 359, 360);
  m_ivp_domain.addDomain("speed",  0, m_os_maxspd, 51);
}


// ----------------------------------------------------------
// Procedure: initFromFile
//   Purpose: 

void CPA_Model::push_new_contact()
{
  unsigned int rsize = m_cnx.size();
  m_cnx.push_back(100 + (rsize*25));
  m_cny.push_back(100 + (rsize*25));
  m_cnspd.push_back(2.0);
  m_cncrs.push_back(90.0);
}


// ----------------------------------------------------------
// Procedure: initFromFile
//   Purpose: 

bool CPA_Model::initFromFile(std::string filename)
{
  if(size() == 0)
    push_new_contact();

  cout << "Attempting to open cpaview config file: " << filename << endl;
  
  vector<string> svector = fileBuffer(filename);
  unsigned int ix, vsize = svector.size();

  cout << "Number of lines read: " << vsize << endl;

  for(ix=0; ix<vsize; ix++) {
    string line = stripBlankEnds(svector[ix]);
    string line_orig = line;
    if((line != "") && !strBegins(line, "//")) {
      string param = tolower(stripBlankEnds(biteString(line, '=')));
      string value = stripBlankEnds(line);
      double dval  = atof(value.c_str());
      unsigned int index = m_cnx.size()-1;
      
      cout << "param:[" << param << "]  value:[" << value << endl;

      if(param == "new_contact") 
	push_new_contact();
	
      else if((param == "cn_speed") || (param == "cn_spd")) {
	if(dval < 0)
	  cout << "Warning: Line " << ix << " contains neg spd" << endl;
	set_cnspd(dval, index);
      }
      else if((param == "cn_heading") || (param == "cn_hdg"))
	set_cncrs(angle360(dval), index);
      else if(param == "cn_x")
	set_cnx(dval, index);
      else if(param == "cn_y")
	set_cny(dval, index);
      else if(param == "cn_range_bearing") 
	set_cn_range_bearing(value, index);
      else if(param == "os_maxtol") {
	if(dval < 0)
	  cout << "Warning: Line " << ix << " contains neg maxtol" << endl;
	m_os_maxtol = dval;
      }
      else if(param == "os_maxspd") {
	if(dval <= 0)
	  cout << "Warning: Line " << ix << " nonpos max speed" << endl;
	m_os_maxspd = dval;
      }
      else {
	cout << "Unhandled line:" << ix << endl;
	cout << "> " << line_orig << endl;
      }
    }
  }
  return true;
}


// ----------------------------------------------------------
// procedure: shift_os_maxspd
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

void CPA_Model::set_cnx(double val, unsigned int index)
{
  if(index >= m_cnx.size())
    return;
  m_cnx[index] = val;
}

// ----------------------------------------------------------
// Procedure: set_cny
//   Purpose: 

void CPA_Model::set_cny(double val, unsigned int index)
{
  if(index >= m_cny.size())
    return;
  m_cny[index] = val;
}

// ----------------------------------------------------------
// Procedure: set_cnspd
//   Purpose: 

void CPA_Model::set_cnspd(double val, unsigned int index)
{
  if(index >= m_cnspd.size())
    return;
  m_cnspd[index] = val;
}

// ----------------------------------------------------------
// Procedure: set_cncrs
//   Purpose: 

void CPA_Model::set_cncrs(double val, unsigned int index)
{
  if(index >= m_cncrs.size())
    return;
  m_cncrs[index] = val;
}

// ----------------------------------------------------------
// Procedure: set_cnrng
//   Purpose: 

void CPA_Model::set_cnrng(double val, unsigned int index)
{
  if(index >= m_cncrs.size())
    return;
  
  double cnx = m_cnx[index];
  double cny = m_cny[index];

  double curr_angle = relAng(m_os_x, m_os_y, cnx, cny);
  double new_range = val;
  if(new_range < 0)
    new_range = 0;

  projectPoint(curr_angle, new_range, m_os_x, m_os_y, 
	       m_cnx[index], m_cny[index]);
}

// ----------------------------------------------------------
// Procedure: set_cnbng
//   Purpose: 

void CPA_Model::set_cnbng(double val, unsigned int index)
{
  if(index >= m_cncrs.size())
    return;
  
  double cnx = m_cnx[index];
  double cny = m_cny[index];

  double curr_range = distPointToPoint(m_os_x, m_os_y, cnx, cny);
  double new_angle  = angle360(val);

  projectPoint(new_angle, curr_range, m_os_x, m_os_y, 
	       m_cnx[index], m_cny[index]);
}

// ----------------------------------------------------------
// Procedure: shift_cnx
//   Purpose: 

void CPA_Model::shift_cnx(double val, unsigned int index)
{
  if(index >= m_cnx.size())
    return;
  m_cnx[index] += val;
}

// ----------------------------------------------------------
// Procedure: shift_cny
//   Purpose: 

void CPA_Model::shift_cny(double val, unsigned int index)
{
  if(index >= m_cny.size())
    return;
  m_cny[index] += val;
}

// ----------------------------------------------------------
// Procedure: shift_cnspd
//   Purpose: 

void CPA_Model::shift_cnspd(double val, unsigned int index)
{
  if(index >= m_cnspd.size())
    return;
  m_cnspd[index] += val/10;
  if(m_cnspd[index] < 0)
    m_cnspd[index] = 0;
}

// ----------------------------------------------------------
// Procedure: shift_cncrs
//   Purpose: 

void CPA_Model::shift_cncrs(double val, unsigned int index)
{
  if(index >= m_cncrs.size())
    return;
  m_cncrs[index] += val;
  if(m_cncrs[index] < 0)
    m_cncrs[index] += 360;
  if(m_cncrs[index] >= 360)
    m_cncrs[index] -= 360;
}

// ----------------------------------------------------------
// Procedure: shift_range
//   Purpose: 

void CPA_Model::shift_range(double val, unsigned int index)
{
  if(index >= m_cncrs.size())
    return;
  
  double cnx = m_cnx[index];
  double cny = m_cny[index];

  double curr_angle = relAng(m_os_x, m_os_y, cnx, cny);
  double curr_range = distPointToPoint(m_os_x, m_os_y, cnx, cny);

  double new_range = curr_range + val;
  if(new_range < 0)
    new_range = 0;

  projectPoint(curr_angle, new_range, m_os_x, m_os_y, 
	       m_cnx[index], m_cny[index]);
}

// ----------------------------------------------------------
// Procedure: set_cn_range_bearing
//   Purpose: 

void CPA_Model::set_cn_range_bearing(string str, unsigned int index)
{
  if(index >= m_cnx.size())
    return;

  string range = stripBlankEnds(biteString(str, ','));
  string bearing = stripBlankEnds(str);

  if(!isNumber(range) || !isNumber(bearing))
    return;

  double d_range = atof(range.c_str());
  double d_bearing = angle360(atof(bearing.c_str()));
  
  if(d_range < 0)
    return;

  double newx, newy;
  projectPoint(d_bearing, d_range, m_os_x, m_os_y, newx, newy);
  m_cnx[index] = newx;
  m_cny[index] = newy;
}

// ----------------------------------------------------------
// Procedure: get_param
//   Purpose: 

double CPA_Model::get_param(const string& param, 
			    unsigned int index) const
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


  if(index >= m_cnx.size())
    return(0);

  if(param == "cnx")
    return(m_cnx[index]);
  else if(param == "cny")
    return(m_cny[index]);
  else if(param == "cnspd")
    return(m_cnspd[index]);
  else if(param == "cncrs")
    return(m_cncrs[index]);
  else if(param == "cn_range") 
    return(distPointToPoint(m_os_x, m_os_y, m_cnx[index], m_cny[index]));
  else if(param == "cn_bearing") 
    return(relAng(m_os_x, m_os_y, m_cnx[index], m_cny[index]));
  
  return(0);
}

// ----------------------------------------------------------
// Procedure: calc_cpa_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_cpa_ipf(unsigned int ix, 
				     unsigned int unif_units)
{
  if(ix >= this->size())
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

  bool ok = true;
  string dim0_name = m_ivp_domain.getVarName(0);
  string dim1_name = m_ivp_domain.getVarName(1);

  // Example String: "discrete @ x:40,y:20"

  string unif_str = "discrete @ ";
  unif_str += dim0_name + ":" + intToString(unif_units) + ",";
  unif_str += dim1_name + ":" + intToString(unif_units);
  
  ok = ok && reflector.setParam("uniform_piece", unif_str);
  reflector.create();

  IvPFunction *ipf = reflector.extractOF(false);

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: calc_wpt_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_wpt_ipf(unsigned int ix, 
				     unsigned int unif_units)
{
  if(ix >= this->size())
    return(0);

#if 1
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

#if 0
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


  ok = true;
  string dim0_name = m_ivp_domain.getVarName(0);
  string dim1_name = m_ivp_domain.getVarName(1);

  // Example String: "discrete @ x:40,y:20"
  string unif_str = "discrete @ ";
  unif_str += dim0_name + ":" + intToString(unif_units) + ",";
  unif_str += dim1_name + ":" + intToString(unif_units);
  
  ok = ok && reflector.setParam("uniform_piece", unif_str);
  reflector.create();

  IvPFunction *ipf = reflector.extractOF();
#endif

  return(ipf);
}
// ----------------------------------------------------------
// Procedure: calc_spt_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_spt_ipf(unsigned int ix, 
				     unsigned int unif_units)
{
  if(ix >= this->size())
    return(0);

  IvPFunction *ivp_function;

  bool ok = true;
  AOF_SimpleWaypoint aof_wpt(m_ivp_domain);
  ok = ok && aof_wpt.setParam("desired_speed", 2.0);
  ok = ok && aof_wpt.setParam("osx", m_os_x);
  ok = ok && aof_wpt.setParam("osy", m_os_y);
  ok = ok && aof_wpt.setParam("ptx", m_cnx[ix]);
  ok = ok && aof_wpt.setParam("pty", m_cny[ix]);
  ok = ok && aof_wpt.initialize();
  if(ok) {
    OF_Reflector reflector(&aof_wpt);
    reflector.create(600, 500);
    ivp_function = reflector.extractIvPFunction();
  }

  return(ivp_function);
}

// ----------------------------------------------------------
// Procedure: calc_avd_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_avd_ipf(unsigned int ix, 
				     unsigned int unif_units)
{
  if(ix >= this->size())
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

  ok = true;
  string dim0_name = m_ivp_domain.getVarName(0);
  string dim1_name = m_ivp_domain.getVarName(1);

  // Example String: "discrete @ x:40,y:20"
  string unif_str = "discrete @ ";
  unif_str += dim0_name + ":" + intToString(unif_units) + ",";
  unif_str += dim1_name + ":" + intToString(unif_units);
  
  ok = ok && reflector.setParam("uniform_piece", unif_str);
  reflector.create();

  IvPFunction *ipf = reflector.extractOF(false);

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: calc_roc_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_roc_ipf(unsigned int ix, 
				     unsigned int unif_units)
{
  if(ix >= this->size())
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

  ok = true;
  string dim0_name = m_ivp_domain.getVarName(0);
  string dim1_name = m_ivp_domain.getVarName(1);

  // Example String: "discrete @ x:40,y:20"
  string unif_str = "discrete @ ";
  unif_str += dim0_name + ":" + intToString(unif_units) + ",";
  unif_str += dim1_name + ":" + intToString(unif_units);
  
  ok = ok && reflector.setParam("uniform_piece", unif_str);
  reflector.create();
  
  IvPFunction *ipf = reflector.extractOF(false);

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: calc_roc_ipf
//   Purpose: 

IvPFunction *CPA_Model::calc_r16_ipf(unsigned int ix, 
				     unsigned int unif_units)
{
  if(ix >= this->size())
    return(0);
  
  AOF_R16 aof(m_ivp_domain);

  aof.setParam("cnx", m_cnx[ix]);
  aof.setParam("cny", m_cny[ix]);
  aof.setParam("cnh", m_cncrs[ix]);
  aof.setParam("cnv", m_cnspd[ix]);
  aof.setParam("osx", m_os_x);
  aof.setParam("osy", m_os_y);
  aof.setParam("tol", 60);
   
  bool ok = aof.initialize();
  if(!ok) {
    cout << "Problem init AOF_R16" << endl;
    exit(0);
  }

  OF_Reflector reflector(&aof, 1);

  if(unif_units <= 0) 
    unif_units = 1;

  ok = true;
  string dim0_name = m_ivp_domain.getVarName(0);
  string dim1_name = m_ivp_domain.getVarName(1);

  // Example String: "discrete @ x:40,y:20"
  string unif_str = "discrete @ ";
  unif_str += dim0_name + ":" + intToString(unif_units) + ",";
  unif_str += dim1_name + ":" + intToString(unif_units);
  
  ok = ok && reflector.setParam("uniform_piece", unif_str);
  reflector.create();
  
  IvPFunction *ipf = reflector.extractOF(false);

  return(ipf);
}

// ----------------------------------------------------------
// Procedure: extrapolate
//   Purpose: 

vector<double> CPA_Model::extrapolate(double g_spd, double g_time, 
				      unsigned int g_cn_index)
{
  vector<double> rvector;
  if(g_cn_index >= this->size())
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
  return(rvector);
}

