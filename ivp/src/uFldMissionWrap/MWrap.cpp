/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MWrap.cpp                                       */
/*    DATE: November 12th, 2026                             */
/************************************************************/

#include "MBUtils.h"
#include "MWrap.h"

#include <cstdlib>
#include <vector>

using namespace std;

//---------------------------------------------------------
// Constructor()

MWrap::MWrap()
{
  m_pass      = false;
  m_pass_set  = false;
  m_score     = 0;
  m_score_set = false;
  m_utc       = 0;
}

//---------------------------------------------------------
// Procedure: getSpec()

string MWrap::getSpec() const
{
  string spec;
  if(m_vname == "")
    spec = "vname=unknown";
  else
    spec = "vname=" + m_vname;

  if(m_pass_set)
    spec = ",pass=" + boolToString(m_pass);
  if(m_score_set)
    spec = ",score=" + doubleToStringX(m_score,4);
  if(m_utc != 0)
    spec = ",utc=" + doubleToStringX(m_utc,2);

  return(spec);
}

//---------------------------------------------------------
// Procedure: valid()

bool MWrap::valid() const
{
  if(m_vname == "")
    return(false);
  return(true);
}


//---------------------------------------------------------
// Procedure: stringToMWrap()

MWrap stringToMWrap(string str)
{
  MWrap mwrap;
  
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "vname")
      mwrap.setVName(value);
    else if(param == "pass") {
      if(tolower(value) == "true")
	mwrap.setPass(true);
    }
    else if(param == "score")
      mwrap.setScore(atof(value.c_str()));
    else if(param == "utc")
      mwrap.setUTC(atof(value.c_str()));
  }

  return(mwrap);
}
