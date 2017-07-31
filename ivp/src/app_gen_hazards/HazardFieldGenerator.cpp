/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardFieldGenerator.cpp                             */
/*    DATE: Jan 27th, 2012                                       */
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

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "HazardFieldGenerator.h"
#include "XYHazard.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

HazardFieldGenerator::HazardFieldGenerator()
{
  // By default hazard locations are rounded to the nearest meter
  m_pt_step = 1;

  // Determines how similar a benign object may be to a hazard. Higher
  // number means benign objects on average resemble hazards less.
  m_resemblance_exp = 0;

  // Determines the default base optimal aspect angle for the object
  m_aspect_base  = 0;

  // Determines how much the actual optimal aspect may vary from 
  // the base optimal aspect.
  m_aspect_range = 360;


  m_aspect_min_base  = 15;
  m_aspect_min_range = 0;
  m_aspect_max_base  = 15;
  m_aspect_max_range = 0;

  // Indicates whether or not aspect information is to be included with 
  // randomly generated objects. Set to true if *any* of the aspect 
  // parameters are set.
  m_aspect_inplay = false;
}


//---------------------------------------------------------
// Procedure: addObjectSet
//   Example: spec = "100,hazard"
//            spec = "90,false_alarm"

bool HazardFieldGenerator::addObjectSet(string spec)
{
  // Part 1: Get the various fields just as strings
  string str_amt  = biteStringX(spec, ',');
  string str_type = spec;

  // Part 2: Handle the amount
  if(!isNumber(str_amt))
    return(false);
  
  int amt = atoi(str_amt.c_str());
  if(amt < 0)
    amt = 0;

  m_obj_set_amt.push_back((unsigned int)(amt));

  // Part 3: Handle the type information
  m_obj_set_type.push_back(str_type);

  return(true);
}

//---------------------------------------------------------
// Procedure: generate

bool HazardFieldGenerator::generate()
{
  srand(time(NULL));

  bool ok = true;
  unsigned int i, isize = m_obj_set_amt.size();
  for(i=0; i<isize; i++) {
    unsigned int amt = m_obj_set_amt[i];
    string obj_type  = m_obj_set_type[i];
    ok = ok && generateObjectSet(amt, obj_type);
  }

  return(ok);
}


//---------------------------------------------------------
// Procedure: generateRandomUniqueLabel

string HazardFieldGenerator::generateRandomUniqueLabel()
{
  unsigned int tries = 0;
  unsigned int max_tries = 1000;
  
  string return_str = "label_error";

  int field = 100;
  if(m_rand_labels.size() >= 100)
    field = 1000;
  if(m_rand_labels.size() >= 1000)
    field = 10000;
  if(m_rand_labels.size() >= 10000)
    field = 100000;

  bool done = false;
  while(!done && (tries < max_tries)) {
    int    ival = rand() % field;
    string sval = intToString(ival);
    if(m_rand_labels.count(sval) == 0) {
      m_rand_labels.insert(sval);
      return_str = sval;
      done = true;
    }
    else
      tries++;
  }

  return(return_str);
}
    

//---------------------------------------------------------
// Procedure: setResemblanceExp
//      Note: The m_resemblance_exp factor is used to alter the randomly 
//            generated hazard_resemblance factor. This factor is generated 
//            as a random number in the range of [0,1] to start. Then it 
//            is raised to value of m_exp. A high m_exp factor means that
//            the expected value of the hazard_resemblance factor will be
//            closer to zero. 

bool HazardFieldGenerator::setResemblanceExp(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  m_resemblance_exp = vclip(dval, 0.01, 10);

  return(true);
}
    
//---------------------------------------------------------
// Procedure: setAspectBase

bool HazardFieldGenerator::setAspectBase(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  m_aspect_base = angle360(dval);

  m_aspect_inplay = true;
  return(true);
}
    
//---------------------------------------------------------
// Procedure: setAspectRange

bool HazardFieldGenerator::setAspectRange(string str)
{
  if(!isNumber(str))
    return(false);

  double dval    = atof(str.c_str());
  m_aspect_range = vclip(dval, 0, 360);

  m_aspect_inplay = true;
  return(true);
}
    
//---------------------------------------------------------
// Procedure: setAspectMinBase

bool HazardFieldGenerator::setAspectMinBase(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());
  if((dval < 0) || (dval > 90))
    return(false);
  
  m_aspect_min_base = dval;
  m_aspect_inplay = true;
  return(true);
}
    

//---------------------------------------------------------
// Procedure: setAspectMinRange

bool HazardFieldGenerator::setAspectMinRange(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());
  if((dval < 0) || (dval > 90))
    return(false);
  
  m_aspect_min_range = dval;
  m_aspect_inplay = true;
  return(true);
}
    

//---------------------------------------------------------
// Procedure: setAspectMaxBase

bool HazardFieldGenerator::setAspectMaxBase(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());
  if((dval < 0) || (dval > 90))
    return(false);
  
  m_aspect_max_base = dval;
  m_aspect_inplay = true;
  return(true);
}
    

//---------------------------------------------------------
// Procedure: setAspectMaxRange

bool HazardFieldGenerator::setAspectMaxRange(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());
  if((dval < 0) || (dval > 90))
    return(false);
  
  m_aspect_max_range = dval;
  m_aspect_inplay = true;
  return(true);
}
    

//---------------------------------------------------------
// Procedure: generateObjectSet

bool HazardFieldGenerator::generateObjectSet(unsigned int amt, string obj_type)
{
  unsigned int pcount = m_field_generator.polygonCount();
  for(unsigned int i=0; i<pcount; i++) {
    XYPolygon poly = m_field_generator.getPolygon(i);
    string spec = poly.get_spec();
    cout << "region = " << spec << endl;
  }

  for(unsigned int i=0; i<amt; i++) {
    XYPoint point = m_field_generator.generatePoint();

    double vx = snapToStep(point.get_vx(), m_pt_step);
    double vy = snapToStep(point.get_vy(), m_pt_step);

    string label = generateRandomUniqueLabel();

    XYHazard hazard;
    hazard.setX(vx);
    hazard.setY(vy);
    hazard.setType(obj_type);
    hazard.setLabel(label);

    if(m_resemblance_exp >= 0.01) {
      int int_hr = rand() % 1000;
      double pct = (double)(int_hr) / 1000;
      double hr  = pow(pct, m_resemblance_exp);
      hazard.setResemblance(hr);
    }

    if(m_aspect_inplay) {
      int    randint;
      double pct, delta, aspect, range_min, range_max;

      // First: calculate the aspect angle from base and random point in range
      randint = rand() % 10000;
      pct     = (double)(randint) / 10000.0;
      delta   = pct * m_aspect_range;

      aspect  = m_aspect_base + delta;
      aspect  = angle360(aspect);
      aspect  = snapToStep(aspect, 0.1);
      hazard.setAspect(aspect);
      
      // Second: calculate the range_min
      randint   = rand() % 10000;
      pct       = (double)(randint) / 10000.0;
      range_min = m_aspect_min_base + (pct * m_aspect_min_range);
      range_min = vclip(range_min, 0, 90);
      range_min = snapToStep(range_min, 0.1);

      // Third: calculate the range_max
      // Note that the rangemax value here is the amount *beyond* range_min
      randint   = rand() % 10000;
      pct       = (double)(randint) / 10000.0;
      range_max = range_min + (pct * m_aspect_max_range);
      range_max = snapToStep(range_max, 0.1);
      range_max = vclip(range_max, range_min, 90);


      bool ok = hazard.setAspectRange(range_min, range_max);
      if(!ok)
	cout << "Illegal aspect range: " << range_min << ", " << range_max << endl;
    }

    string msg = hazard.getSpec();
    cout << "hazard = " << msg << endl;
  }
  return(true);
}









