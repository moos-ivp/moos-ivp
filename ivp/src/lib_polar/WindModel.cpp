/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WindModel.cpp                                        */
/*    DATE: Jan 24th 2022                                        */
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
#include <iostream>
#include "WindModel.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

WindModel::WindModel()
{
  m_set = false;

  m_wind_dir = 0;
  m_wind_spd = 0;

  m_arrow.set_param("fill_color", "green");
  m_arrow.set_param("fill_transparency", "0.2");
  m_arrow.set_label("windmod");
}

//----------------------------------------------------------------
// Procedure: setConditions()
//   Example: spd=2.4, dir=135
//   Example: spd=2.4, dir=135, x=34, y=89
//      Note: Eventually will generalize to models where wind speed
//            varies between locations. For now, it is universal.

bool WindModel::setConditions(string str)
{ 
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    double dval = atof(value.c_str());

    if((param == "spd") && isNumber(value))
      setWindSpd(dval);
    else if((param == "dir") && isNumber(value) && (dval >= 0))
      setWindDir(dval);
    else
      return(false);
  }

  m_set = true;
  return(true);
}

//----------------------------------------------------------------
// Procedure: setWindDir()

bool WindModel::setWindDir(double dval)
{ 
  m_wind_dir = angle360(dval);

  // NOTE: The wind direction is the direction it is coming FROM.
  // The arrow will point 180 degs + the wind direction.
  double arrow_angle = angle360(m_wind_dir + 180);
  m_arrow.setAngle(arrow_angle);

  m_set = true;
  return(true);
}

//----------------------------------------------------------------
// Procedure: setWindSpd()

bool WindModel::setWindSpd(double dval)
{ 
  if(dval < 0)
    return(false);

  m_wind_spd = dval;

  m_set = true;
  return(true);
}

//----------------------------------------------------------------
// Procedure: modWindDir()

bool WindModel::modWindDir(double dval)
{ 
  m_wind_dir = angle360(m_wind_dir + dval);

  // NOTE: The wind direction is the direction it is coming FROM.
  // The arrow will point 180 degs + the wind direction.
  double arrow_angle = angle360(m_wind_dir + 180);
  m_arrow.setAngle(arrow_angle);

  m_set = true;
  return(true);
}

//----------------------------------------------------------------
// Procedure: modWindSpd()

bool WindModel::modWindSpd(double dval)
{ 
  m_wind_spd += dval;
  if(m_wind_spd < 0)
    m_wind_spd = 0;
  
  m_set = true;
  return(true);
}

//----------------------------------------------------------------
// Procedure: getWindDir(x,y)
//   Purpose: A placeholder for when wind direction is not uniform

double WindModel::getWindDir(double x, double y) const
{
  return(m_wind_dir);
}


//----------------------------------------------------------------
// Procedure: getWindSpd(x,y)
//   Purpose: A placeholder for when wind speed is not uniform

double WindModel::getWindSpd(double x, double y) const
{
  return(m_wind_spd);
}


//----------------------------------------------------------------
// Procedure: getArrowSpec()

string WindModel::getArrowSpec() const
{
  return(m_arrow.get_spec());
}


//----------------------------------------------------------------
// Procedure: getArrowSpec()

string WindModel::getArrowSpec(double x, double y) const
{
  XYArrow arrow = m_arrow;
  arrow.setBaseCenter(x,y);
  
  return(arrow.get_spec());
}

//----------------------------------------------------------------
// Procedure: getMaxSpeed()

double WindModel::getMaxSpeed(PolarPlot pplot, double hdg) const
{
  double polar_pct = pplot.getPolarPct(hdg);

  double max_spd = polar_pct * getWindSpd();

  return(max_spd);
}

//----------------------------------------------------------------
// Procedure: getArrowSpec()

string WindModel::getArrowSpec(string params) const
{
  // Start with a copy of the arrow
  XYArrow arrow = m_arrow;

  // Apply all specs to the copy, but just return the specs of the
  // original arrow if any one of the given param/value pairs fail.
  vector<string> svector = parseString(params, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];

    bool ok = arrow.set_param(param, value);
    if(!ok)
      return(m_arrow.get_spec());
  }
  
  return(arrow.get_spec());
}


//----------------------------------------------------------------
// Procedure: getModelSpec()
//   Example: spd=2.4, dir=135
//   Example: spd=2.4, dir=135, x=34, y=89
//      Note: Eventually will generalize to models where wind speed
//            varies between locations. For now, it is universal.

string WindModel::getModelSpec() const
{
  string spec = "spd=" + doubleToStringX(m_wind_spd,3);
  spec += ", dir=" + doubleToStringX(m_wind_dir);

  return(spec);
}

//----------------------------------------------------------------
// Procedure: stringToWindModel()

WindModel stringToWindModel(string str)
{
  WindModel model;
  
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    double dval  = atof(value.c_str());
    
    if((param == "spd") && isNumber(value))
      model.setWindSpd(dval);
    else if((param == "dir") && isNumber(value))
      model.setWindDir(dval);
  }

  return(model);
}


