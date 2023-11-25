/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PolarPlot.cpp                                        */
/*    DATE: July 16th 2021                                       */
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

#include <cmath>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "PolarPlot.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PolarPlot::PolarPlot()
{
  // The max_value is the maximal radial value associated with
  // any heading value. The default, 100, can be thought of as
  // 100 per cent. A 100 percent value would mean that the boat
  // speed in that direction would be 100 of the wind speed. So
  // values greater than 100 are disallowed. Values less than
  // 100 are allowed but should be carefully considered.
  m_max_value = 100;

  // Keep track of this state. When accessing interpolated points
  // first see if the interpolation step has been done. If not,
  // do the interpolation. The interpolated points are cached.
  m_interpolated = false;

  // A wind angle of zero means the wind is coming from due North,
  // moving in the directin of due South
  m_wind_angle = 0;
}

//---------------------------------------------------------
// Procedure: setWindAngle()

void PolarPlot::setWindAngle(double angle)
{  
  angle = angle360(angle);
  if(angle == m_wind_angle)
    return;

  m_wind_angle = angle;
  
  // Any time the wind angle has been modified, the cache of
  // interpolated points should be considered stale, in need of
  // recalculation before accessing.
  m_interpolated = false;
}

//---------------------------------------------------------
// Procedure: modWindAngle()

void PolarPlot::modWindAngle(double degrees)
{  
  if(degrees == 0)
    return;
  
  m_wind_angle = angle360(m_wind_angle + degrees);
  
  // Any time the wind angle has been modified, the cache of
  // interpolated points should be considered stale, in need of
  // recalculation before accessing.
  m_interpolated = false;
}

//---------------------------------------------------------
// Procedure: addSetPoint()

bool PolarPlot::addSetPoint(double hdg, double val)
{
  if((hdg < 0) || (hdg > 180))
    return(false);

  if(m_setpts.count(hdg) != 0)
    return(false);
  
  m_setpts[hdg] = val;

  // Any time a set point has been modified, the cache of interpolated
  // points should be considered stale, in need of recalculation
  // before accessing.
  m_interpolated = false;
  return(true);
}

//---------------------------------------------------------
// Procedure: addSetPoints()
//   Example: 0,0: 20,40: 45,65: 90,80: 110,90: 135,83: 150,83: 165,60: 180,50

bool PolarPlot::addSetPoints(string str)
{
  if(str == "")
    return(false);

  // Hold all new set points and confirm before adding
  vector<double> hdgs;
  vector<double> vals;
  
  vector<string> svector = parseString(str, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string hdg = biteStringX(svector[i], ',');
    string val = svector[i];
    if(!isNumber(hdg) || !isNumber(val))
      return(false);
    double hdg_dval = atof(hdg.c_str());
    double val_dval = atof(val.c_str());

    if((hdg_dval < 0) || (hdg_dval > 180))
      return(false);
    if(m_setpts.count(hdg_dval) != 0)
      return(false);
    if(val_dval < 0)
      return(false);

    hdgs.push_back(hdg_dval);
    vals.push_back(val_dval);
  }   

  // Now that all hdg/val pairs are confirmed good, add them.
  for(unsigned int j=0; j<hdgs.size(); j++)
    addSetPoint(hdgs[j], vals[j]);

  return(true);
}

//---------------------------------------------------------
// Procedure: getSetHdgs()

vector<double> PolarPlot::getSetHdgs() const
{
  vector<double> hdg_vector;
  
  map<double, double>::const_iterator p;
  for(p=m_setpts.begin(); p!=m_setpts.end(); p++) {
    double hdg = p->first;
    hdg_vector.push_back(hdg);
  }
  
  return(hdg_vector);    
}

//---------------------------------------------------------
// Procedure: getSetHdgsRotated()
//   Purpose: Get the headings of the set points, but also rotate
//            each heading based on the wind angle. Typically used
//            only when getting setpts for rendering in an
//            engineering GUI.

vector<double> PolarPlot::getSetHdgsRotated() const
{
  vector<double> hdg_vector;
  
  map<double, double>::const_iterator p;
  for(p=m_setpts.begin(); p!=m_setpts.end(); p++) {
    double hdg = p->first;
    hdg = angle360(hdg + m_wind_angle);
    hdg_vector.push_back(hdg);
  }
  
  return(hdg_vector);    
}

//---------------------------------------------------------
// Procedure: getSetVals()

vector<double> PolarPlot::getSetVals() const
{
  vector<double> val_vector;
  
  map<double, double>::const_iterator p;
  for(p=m_setpts.begin(); p!=m_setpts.end(); p++)
    val_vector.push_back(p->second);

  return(val_vector);    
}

//---------------------------------------------------------
// Procedure: getAllHdgs()

vector<double> PolarPlot::getAllHdgs()
{
  if(!m_interpolated)
    interpLinear();

  vector<double> hdg_vector;
  
  map<int, double>::const_iterator p;
  for(p=m_allpts.begin(); p!=m_allpts.end(); p++) {
    double hdg = (double)(p->first);
    hdg_vector.push_back(hdg);
  }
  
  return(hdg_vector);    
}

//---------------------------------------------------------
// Procedure: getAllVals()

vector<double> PolarPlot::getAllVals()
{
  if(!m_interpolated)
    interpLinear();

  vector<double> val_vector;
  
  map<int, double>::const_iterator p;
  for(p=m_allpts.begin(); p!=m_allpts.end(); p++)
    val_vector.push_back(p->second);

  return(val_vector);    
}


//---------------------------------------------------------
// Procedure: interpLinear()

void PolarPlot::interpLinear()
{
  m_allpts.clear();
  for(int h=0; h<360; h++) {
    int rotated_hdg = (int)(angle360(h + m_wind_angle + 0.5));

    if(h <= 180) {
      double val = interpLinear((double)(h));
      m_allpts[rotated_hdg] = val;
    }
    else {
      double val = interpLinear((double)(360-h));
      m_allpts[rotated_hdg] = val;
    }
  }
  m_interpolated = true;
}

//---------------------------------------------------------
// Procedure: getPolarPct()

double PolarPlot::getPolarPct(double hdg)
{
  if(!m_interpolated)
    interpLinear();

  hdg = angle360(hdg);
  
  int int_hdg = (int)(hdg + 0.5);

  unsigned int val_count = m_allpts.count(int_hdg);
  cout << "val_count:" << val_count << endl;
  cout << "hdg:" << hdg << endl;
  cout << "int_hdg:" << int_hdg << endl;
  
  double val = m_allpts[int_hdg];

  cout << "val: " << val << endl;
  
  
  // Sanity check
  if(m_max_value <= 0)
    return(0);

  double pct = val / m_max_value;
  if(pct > 1)
    return(1);

  return(pct);  
}

//---------------------------------------------------------
// Procedure: modPoint()

void PolarPlot::modPoint(double hdg, double val)
{
  double closest_hdg = getClosestHdg(hdg, val);

  m_setpts.erase(closest_hdg);

  addSetPoint(hdg, val);
}

//---------------------------------------------------------
// Procedure: removePoint()

void PolarPlot::removePoint(double hdg, double val)
{
  double closest_hdg = getClosestHdg(hdg, val);

  m_setpts.erase(closest_hdg);
}

//---------------------------------------------------------
// Procedure: getSpec()

string PolarPlot::getSpec() const
{
  vector<double> hdg_vector = getSetHdgs();
  vector<double> val_vector = getSetVals();

  if(hdg_vector.size() != val_vector.size())
    return("");

  string str;
  for(unsigned int i=0; i<hdg_vector.size(); i++) {
    if(i!=0)
      str += ":";
    str += doubleToStringX(hdg_vector[i],1) + ",";
    str += doubleToStringX(val_vector[i],2);
  }
  
  return(str);
}

//---------------------------------------------------------
// Procedure: interpLinear()

double PolarPlot::interpLinear(double hdg)
{
  if((hdg < 0) || (hdg > 180))
    return(0);

  double hmin = 0;
  double vmin = 0;
  double hmax = 0;
  double vmax = 0;

  bool ok = getNeighbors(hdg, hmin, vmin, hmax, vmax);
  // Sanity checks
  if(!ok || (hmin > hmax))
    return(0);
  if((hdg < hmin) || (hdg > hmax))
    return(0);
  
  // If there is an exact match setpt, no interp required
  if(hmin == hmax)
    return(vmin);

  double hdelta = hmax - hmin;
  double vdelta = vmax - vmin;

  double pct_hdg = (hdg - hmin) / hdelta;

  double interp_val = vmin + (pct_hdg * vdelta);

  return(interp_val);
}

//---------------------------------------------------------
// Procedure: getNeighbors()

bool PolarPlot::getNeighbors(double query_hdg,
			     double& hmin, double& vmin,
			     double& hmax, double& vmax)
{
  hmin = 0;
  vmin = 0;
  hmax = 180;
  vmax = 0;

  if(m_setpts.size() == 0)
    return(false);

  map<double, double>::iterator p;
  for(p=m_setpts.begin(); p!=m_setpts.end(); p++) {
    double hdg = p->first;
    double val = p->second;
    if(hdg == query_hdg) {
      hmin = hdg;
      vmin = val;
      hmax = hdg;
      vmax = val;
      return(true);
    }
    if(hdg < query_hdg) {
      hmin = hdg;
      vmin = val;
    }
    if(hdg > query_hdg) {
      hmax = hdg;
      vmax = val;
      return(true);
    }
  }
  return(true);
}


//---------------------------------------------------------
// Procedure: getClosestHdg()

double PolarPlot::getClosestHdg(double hdg, double val)
{
  double closest_hdg = 0;
  double closest_gap = -1;
  
  map<double, double>::iterator p;
  for(p=m_setpts.begin(); p!=m_setpts.end(); p++) {
    double phdg = p->first;
    double pval = p->second;

    double gap = hypot(phdg-hdg, pval-val);
    if((closest_gap < 0) || (gap < closest_gap)) {
      closest_hdg = phdg;
      closest_gap = gap;
    }
  }

  return(closest_hdg);
}
  
    
//---------------------------------------------------------
// Procedure: stringToPolarPlot()

PolarPlot stringToPolarPlot(string str)
{
  PolarPlot polar_plot;
  polar_plot.addSetPoints(str);

  return(polar_plot);
}


