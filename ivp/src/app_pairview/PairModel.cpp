/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PairModel.cpp                                        */
/*    DATE: Jan 9th, 2025                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "GeomUtils.h"
#include "CPAEngine.h"
#include "AngleUtils.h"
#include "PairModel.h"
#include "FileBuffer.h"

using namespace std;

// ----------------------------------------------------------
// Constructor

PairModel::PairModel()
{
  m_osx = 0;
  m_osy = 0;
  m_osv = 0; 
  m_osh = 0;

  m_cnx = 0;
  m_cny = 0;
  m_cnv = 0; 
  m_cnh = 0;

  m_curr_time = 0;
  m_delt_time = 0;
  
  m_alert_range  = 50;
  m_warning_time = 30;

  m_alerted = false;
  m_warned  = false;
  m_alerted_prev = false;
  m_warned_prev  = false;

  m_alert_time = 0;
  m_warn_time  = 0;
}

// ----------------------------------------------------------
// Procedure: swap()

void PairModel::swap()
{
  double prev_osx = m_osx;
  double prev_osy = m_osy;
  double prev_osv = m_osv;
  double prev_osh = m_osh;

  m_osx = m_cnx;
  m_osy = m_cny;
  m_osv = m_cnv;
  m_osh = m_cnh;

  m_cnx = prev_osx;
  m_cny = prev_osy;
  m_cnv = prev_osv;
  m_cnh = prev_osh;
}


// ----------------------------------------------------------
// Procedure: copy()

void PairModel::copy(PairModel pair_model, string which) 
{
  if((which == "both") || (which == "ownship")) {
    m_osx = pair_model.get_osx();
    m_osy = pair_model.get_osy();
    m_osv = pair_model.get_osv(); 
    m_osh = pair_model.get_osh();
  }

  if((which == "both") || (which == "contact")) {
    m_cnx = pair_model.get_cnx();
    m_cny = pair_model.get_cny();
    m_cnv = pair_model.get_cnv(); 
    m_cnh = pair_model.get_cnh();
  }

  m_alert_range  = pair_model.get_alert_range();
  m_warning_time = pair_model.get_warning_time();

  m_alerted = pair_model.get_alerted();
  m_warned  = pair_model.get_warned();
  m_alerted_prev = false;
  m_warned_prev  = false;

  m_alert_time = pair_model.get_alerted_time();
  m_warn_time  = pair_model.get_warned_time();
}


// ----------------------------------------------------------
// Procedure: initFromFile()

bool PairModel::initFromFile(string filename) 
{
  cout << "Attempting read config file: " << filename << endl;
  
  vector<string> svector = fileBuffer(filename);
  unsigned int ix, vsize = svector.size();

  cout << "Number of lines read: " << vsize << endl;

  for(ix=0; ix<vsize; ix++) {
    string orig = stripBlankEnds(svector[ix]);
    string line = orig;
    if((line != "") && !strBegins(line, "//")) {
      string param = tolower(biteStringX(line, '='));
      string value = line;
      double dval  = atof(value.c_str());
      
      cout << "param:[" << param << "]  value:[" << value << endl;

      if((param == "cnx") && isNumber(value))
	set_cnx(dval);
      else if((param == "cny")  && isNumber(value))
	set_cny(dval);
      else if((param == "cnh")  && isNumber(value))
	set_cnh(dval);
      else if((param == "cnv")  && isNumber(value))
	set_cnv(dval);
      
      else if((param == "osx")  && isNumber(value))
	set_osx(dval);
      else if((param == "osy")  && isNumber(value))
	set_osy(dval);
      else if((param == "osh")  && isNumber(value))
	set_osh(dval);
      else if((param == "osv")  && isNumber(value))
	set_osv(dval);

      else {
	cout << "Unhandled line:" << ix << endl;
	cout << "> " << orig << endl;
      }
    }
  }
  return(true);
}

// ----------------------------------------------------------
// Procedure: update()

void PairModel::update()
{
  double curr_range = range();

  // Part 1: Update the alerted state
  if(curr_range < m_alert_range) {
    if(!m_alerted_prev)
      m_alert_time = m_curr_time;
    m_alerted = true;
    m_alerted_prev = true;
  }
  else {
    m_alerted = false;
    m_alerted_prev = false;
    m_alert_time = 0;
  }

  // Part 3: Update the warning state
  if(curr_range < get_warning_range()) {
    if(!m_warned_prev)
      m_warn_time = m_curr_time;
    m_warned = true;
    m_warned_prev = true;
  }
  else {
    m_warned = false;
    m_warned_prev = false;
    m_warn_time = 0;
  }
    
  if(m_alerted && m_warned) 
    m_delt_time = m_alert_time - m_warn_time;
  else
    m_delt_time = 0; 
}

// ----------------------------------------------------------
// Procedure: shift_osh()

void PairModel::shift_osh(double v)
{
  m_osh = angle360(m_osh + v);
}

// ----------------------------------------------------------
// Procedure: shift_cnh()

void PairModel::shift_cnh(double v)
{
  m_cnh = angle360(m_cnh + v);
}

// ----------------------------------------------------------
// Procedure: move_os()

void PairModel::move_os(double seconds)
{
  double meters = m_osv * seconds;

  double new_osx, new_osy;
  projectPoint(m_osh, meters, m_osx, m_osy, new_osx, new_osy);
  m_osx = new_osx;
  m_osy = new_osy;
}

// ----------------------------------------------------------
// Procedure: move_cn()

void PairModel::move_cn(double seconds)
{
  double meters = m_cnv * seconds;
  
  double new_cnx, new_cny;
  projectPoint(m_cnh, meters, m_cnx, m_cny, new_cnx, new_cny);
  m_cnx = new_cnx;
  m_cny = new_cny;
}

// ----------------------------------------------------------
// Procedure: alt_os_range()
//   Purpose: Change the ownship x,y position by increasing or decreasing
//            the range to the contact. Holding ownship speed and heading
//            constant.

void PairModel::alt_os_range(double meters)
{
  double angle_cn_to_os = relAng(m_cnx, m_cny, m_osx, m_osy);

  double new_osx, new_osy;
  projectPoint(angle_cn_to_os, meters, m_osx, m_osy, new_osx, new_osy);
  m_osx = new_osx;
  m_osy = new_osy;
}

// ----------------------------------------------------------
// Procedure: alt_os_bearing()
//   Purpose: Change the ownship x,y position by increasing or decreasing
//            the bearing to the contact. Holding ownship speed and heading
//            constant.

void PairModel::alt_os_bearing(double degrees)
{
  double angle_cn_to_os = relAng(m_cnx, m_cny, m_osx, m_osy);
  double new_angle_cn_to_os = angle360(angle_cn_to_os + degrees);
  double range = distPointToPoint(m_osx, m_osy, m_cnx, m_cny);

  double new_osx, new_osy;
  projectPoint(new_angle_cn_to_os, range, m_cnx, m_cny, new_osx, new_osy);
  m_osx = new_osx;
  m_osy = new_osy;
}

// ----------------------------------------------------------
// Procedure: alt_os_bearing()
//   Purpose: Change the ownship x,y position by increasing or decreasing
//            the bearing to the contact. Holding ownship speed constant
//            but altering ownship heading to hold the relative bearing
//            constant.

void PairModel::alt_os_bearing_holdrb(double degrees)
{
  double beg_rel_bearing = relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny);
  
  double angle_cn_to_os = relAng(m_cnx, m_cny, m_osx, m_osy);
  double new_angle_cn_to_os = angle360(angle_cn_to_os + degrees);
  double range = distPointToPoint(m_osx, m_osy, m_cnx, m_cny);

  double new_osx, new_osy;
  projectPoint(new_angle_cn_to_os, range, m_cnx, m_cny, new_osx, new_osy);
  m_osx = new_osx;
  m_osy = new_osy;

  double bearing_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  
  double new_heading = angle360(bearing_os_to_cn - beg_rel_bearing);
  m_osh = new_heading;
}

// ----------------------------------------------------------
// Procedure: rate_of_closure()

double PairModel::rate_of_closure() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.evalROC(m_osh, m_osv));
}

// ----------------------------------------------------------
// Procedure: range_rate()

double PairModel::range_rate() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  double roc = cpa_engine.evalROC(m_osh, m_osv);
  return(-roc);
}

// ----------------------------------------------------------
// Procedure: range()

double PairModel::range() const
{
  return(distPointToPoint(m_osx, m_osy, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: get_cpa()

double PairModel::get_cpa(double tol) const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.evalCPA(m_osh, m_osv, tol));
}

// ----------------------------------------------------------
// Procedure: get_warning_range()

double PairModel::get_warning_range() const
{
  double roc = rate_of_closure();
  double delta = roc * m_warning_time;

  double warning_range = m_alert_range + delta;

  return(warning_range);
  
}

