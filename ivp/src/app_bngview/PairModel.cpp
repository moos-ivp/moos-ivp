/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PairModel.cpp                                        */
/*    DATE: May 21, 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
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

  m_pwt_inner_dist = 99;
  m_pwt_outer_dist = 111;
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
// Procedure: shift_osh

void PairModel::shift_osh(double v)
{
  m_osh = angle360(m_osh + v);
}

// ----------------------------------------------------------
// Procedure: shift_cnh

void PairModel::shift_cnh(double v)
{
  m_cnh = angle360(m_cnh + v);
}

// ----------------------------------------------------------
// Procedure: move_os

void PairModel::move_os(double seconds)
{
  double meters = m_osv * seconds;

  double new_osx, new_osy;
  projectPoint(m_osh, meters, m_osx, m_osy, new_osx, new_osy);
  m_osx = new_osx;
  m_osy = new_osy;
}

// ----------------------------------------------------------
// Procedure: move_cn

void PairModel::move_cn(double seconds)
{
  double meters = m_cnv * seconds;

  double new_cnx, new_cny;
  projectPoint(m_cnh, meters, m_cnx, m_cny, new_cnx, new_cny);
  m_cnx = new_cnx;
  m_cny = new_cny;
}

// ----------------------------------------------------------
// Procedure: alt_os_range
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
// Procedure: alt_os_bearing
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
// Procedure: alt_os_bearing
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
// Procedure: range()

double PairModel::range() const
{
  return(distPointToPoint(m_osx, m_osy, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: os_cn_bearing()

double PairModel::os_cn_bearing() const
{
  return(relAng(m_osx, m_osy, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: cn_os_bearing()

double PairModel::cn_os_bearing() const
{
  return(relAng(m_cnx, m_cny, m_osx, m_osy));
}



// ----------------------------------------------------------
// Procedure: os_cn_relbearing()

double PairModel::os_cn_relbearing() const
{
  return(relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: cn_os_relbearing()

double PairModel::cn_os_relbearing() const
{
  return(relBearing(m_cnx, m_cny, m_cnh, m_osx, m_osy));
}



// ----------------------------------------------------------
// Procedure: os_cn_absrelbearing()

double PairModel::os_cn_absrelbearing() const
{
  return(absRelBearing(m_osx, m_osy, m_osh, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: cn_os_absrelbearing()

double PairModel::cn_os_absrelbearing() const
{
  return(absRelBearing(m_cnx, m_cny, m_cnh, m_osx, m_osy));
}

// ----------------------------------------------------------
// Procedure: total_absrelbearing()

double PairModel::total_absrelbearing() const
{
  return(totAbsRelBearing(m_osx, m_osy, m_osh, m_cnx, m_cny, m_cnh));
}


// ----------------------------------------------------------
// Procedure: get_cpa()

double PairModel::get_cpa(double tol) const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);

  double cpa = cpa_engine.evalCPA(m_osh, m_osv, tol);

  return(cpa);
}

// ----------------------------------------------------------
// Procedure: os_crosses_cn_bow()

bool PairModel::os_crosses_cn_bow() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.crossesBow(m_osh, m_osv));
}

// ----------------------------------------------------------
// Procedure: os_crosses_cn_bow_dist()

double PairModel::os_crosses_cn_bow_dist() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);

  double xdist = 0;
  cpa_engine.crossesBowDist(m_osh, m_osv, xdist);
  return(xdist);
}

// ----------------------------------------------------------
// Procedure: os_crosses_cn_stern_dist()

double PairModel::os_crosses_cn_stern_dist() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);

  double xdist = 0;
  cpa_engine.crossesSternDist(m_osh, m_osv, xdist);

  return(xdist);
}

// ----------------------------------------------------------
// Procedure: os_crosses_cn_stern()

bool PairModel::os_crosses_cn_stern() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.crossesStern(m_osh, m_osv));
}


// ----------------------------------------------------------
// Procedure: os_crosses_cn_either()

bool PairModel::os_crosses_cn_either() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.crossesBowOrStern(m_osh, m_osv));
}

// ----------------------------------------------------------
// Procedure: os_spd_in_cn_hdg()

double PairModel::os_spd_in_cn_hdg() const
{
  return(speedInHeading(m_osh, m_osv, m_cnh));
}


// ----------------------------------------------------------
// Procedure: os_fore_of_cn()

bool PairModel::os_fore_of_cn() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.foreOfContact());
}

// ----------------------------------------------------------
// Procedure: os_aft_of_cn()

bool PairModel::os_aft_of_cn() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.aftOfContact());
}

// ----------------------------------------------------------
// Procedure: os_port_of_cn()

bool PairModel::os_port_of_cn() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.portOfContact());
}

// ----------------------------------------------------------
// Procedure: os_starboard_of_cn()

bool PairModel::os_starboard_of_cn() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.starboardOfContact());
}

// ----------------------------------------------------------
// Procedure: os_passes_cn()

bool PairModel::os_passes_cn() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.passesContact(m_osh, m_osv));
}

// ----------------------------------------------------------
// Procedure: os_passes_cn_port()

bool PairModel::os_passes_cn_port() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.passesContactPort(m_osh, m_osv));
}

// ----------------------------------------------------------
// Procedure: os_passes_cn_starboard()

bool PairModel::os_passes_cn_starboard() const
{
  CPAEngine cpa_engine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  return(cpa_engine.passesContactStarboard(m_osh, m_osv));
}


