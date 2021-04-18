/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: VCheck.cpp                                           */
/*   DATE: Sep 19th, 2020                                       */
/****************************************************************/

#include <iostream>
#include <cstdlib>
#include <vector>
#include "MBUtils.h"
#include "VCheck.h"

using namespace std;

//---------------------------------------------------------
// Constructor

VCheck::VCheck()
{
  m_val_dbl = 0;
  m_tstamp  = 0;

  m_max_tdelta = 0;
  m_max_vdelta = 0;

  m_val_set = false;
  m_tstamp_set = false;
  m_delta_set = false;

  m_evaluated = false;
  m_satisfied = false;

  m_actual_delta = -1;
  m_actual_time  = -1;
  m_actual_val_dbl = 0;
}


//---------------------------------------------------------
// Procecure: setVarCheck()
//  Examples: spec = "var=NAV_X, dval=43, time=185, max_delta=3.4"
//            spec = "var=WPT_IX, sval=4, time=34, max_tdelta=5
//            spec = "var=MISSION, sval=done, time=334, max_tdelta=5

bool VCheck::setVarCheck(string spec)
{
  vector<string> svector = parseString(spec, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];

    if(param == "var")
      setVarName(value);
    else if(param == "sval")
      setValString(value);
    else if((param == "dval") && isNumber(value))
      setValDouble(atof(value.c_str()));

    else if(param == "val") {
      if(isNumber(value))
	setValDouble(atof(value.c_str()));
      setValString(value);
    }

    else if((param == "time") && isNumber(value))
      setTimeStamp(atof(value.c_str()));
    else if((param == "max_tdelta") && isNumber(value))
      setMaxTimeDelta(atof(value.c_str()));
    else if((param == "max_vdelta") && isNumber(value))
      setMaxValDelta(atof(value.c_str()));
    else
      return(false);
  }

  if(m_varname == "")
    return(false);
  
  if(!m_val_set || !m_tstamp_set || !m_delta_set)
    return(false);

  return(true);
}
  

//---------------------------------------------------------
// Procecure: print()

void VCheck::print() const
{
  cout << "-------------------------------------------------" << endl;
  cout << "Varname: " << m_varname << endl;
  cout << "  val_str: [" << m_val_str << "]" << endl;
  cout << "  val_dbl: " << m_val_dbl << endl;
  cout << "  tstamp: " << m_tstamp << endl;
  cout << "  max_tdelta: " << m_max_tdelta << endl;
  cout << "  max_vdelta: " << m_max_vdelta << endl;
  cout << "  val_set: " << boolToString(m_val_set) << endl;
  cout << "  tstamp_set: " << boolToString(m_tstamp_set) << endl;
  cout << "  delta_set: " << boolToString(m_delta_set) << endl;
  cout << "  evaluated: " << boolToString(m_evaluated) << endl;
  cout << "  satisfied: " << boolToString(m_satisfied) << endl;
  cout << "    actual_delta: " << doubleToStringX(m_actual_delta) << endl;
  cout << "    actual_time: " << doubleToStringX(m_actual_time) << endl;
  cout << "    actual_val_dbl: " << doubleToStringX(m_actual_val_dbl) << endl;
  cout << "    actual_val_str: " << m_actual_val_str << endl;
}
  

//---------------------------------------------------------
// Overload the < operator
bool operator< (const VCheck& vcheck1, const VCheck& vcheck2)
{
  return(vcheck1.getTimeStamp() > vcheck2.getTimeStamp());
}

//---------------------------------------------------------
// Overload the > operator
bool operator> (const VCheck& vcheck1, const VCheck& vcheck2)
{
  return(vcheck1.getTimeStamp() < vcheck2.getTimeStamp());
}






