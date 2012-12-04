/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardFieldGenerator.cpp                             */
/*    DATE: Jan 27th, 2012                                       */
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
#include <cmath>
#include "HazardFieldGenerator.h"
#include "XYHazard.h"
#include "MBUtils.h"

using namespace std;

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
// Procedure: setExp
//      Note: The m_exp factor is used to alter the randomly generated
//            hazard_resemblance factor. This factor is generated as a
//            random number in the range of [0,1] to start. Then it is
//            raised to value of m_exp. A high m_exp factor means that
//            the expected value of the hazard_resemblance factor will
//            be closer to zero. 

void HazardFieldGenerator::setExp(string str)
{
  if(!isNumber(str))
    return;

  double dval = atof(str.c_str());
  m_exp = vclip(dval, 1, 10);
}
    

//---------------------------------------------------------
// Procedure: generateObjectSet

bool HazardFieldGenerator::generateObjectSet(unsigned int amt, string obj_type)
{
  for(unsigned int i=0; i<amt; i++) {
    m_count++;
    XYPoint point = m_field_generator.generatePoint();

    double vx = snapToStep(point.get_vx(), m_pt_step);
    double vy = snapToStep(point.get_vy(), m_pt_step);

    string label = generateRandomUniqueLabel();

    XYHazard hazard;
    hazard.setX(vx);
    hazard.setY(vy);
    hazard.setType(obj_type);
    hazard.setLabel(label);

    if(m_exp >= 1) {
      int int_hr = rand() % 1000;
      double pct = (double)(int_hr) / 1000;
      double hr  = pow(pct, m_exp);
      hazard.setResemblance(hr);
    }

    string msg = hazard.getSpec();
    cout << "hazard = " << msg << endl;
  }
  return(true);
}


