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

    string msg = hazard.getSpec();
    cout << "hazard = " << msg << endl;
  }
  return(true);
}

