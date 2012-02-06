/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: HazardFieldGenerator.cpp                             */
/*   DATE: Jan 27th, 2012                                       */
/****************************************************************/

#include <iostream>
#include <cstdlib>
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
// Procedure: generateObjectSet

bool HazardFieldGenerator::generateObjectSet(unsigned int amt, string obj_type)
{
  for(unsigned int i=0; i<amt; i++) {
    
    XYPoint point = m_field_generator.generatePoint();

    XYHazard hazard;
    hazard.setX(point.get_vx());
    hazard.setY(point.get_vy());
    hazard.setType(obj_type);

    string msg = hazard.getSpec();
    cout << msg << endl;
  }
  return(true);
}

