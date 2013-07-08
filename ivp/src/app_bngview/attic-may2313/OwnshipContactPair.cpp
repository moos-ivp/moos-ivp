/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: OwnshipContactPair.cpp                               */
/*    DATE: May 21, 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "OwnshipContactPair.h"
#include "FileBuffer.h"

using namespace std;

// ----------------------------------------------------------
// Constructor

OwnshipContactPair::OwnshipContactPair()
{
  m_osx = 0;
  m_osy = 0;
  m_osv = 0; 
  m_osh = 0;

  m_cnx = 0;
  m_cny = 0;
  m_cnv = 0; 
  m_cnh = 0;
}


// ----------------------------------------------------------
// Procedure: initFromFile()

bool OwnshipContactPair::initFromFile(string filename) 
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
// Procedure: range()

double OwnshipContactPair::range() const
{
  return(distPointToPoint(m_osx, m_osy, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: os_cn_relbearing()

double OwnshipContactPair::os_cn_relbearing() const
{
  return(relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: cn_os_relbearing()

double OwnshipContactPair::cn_os_relbearing() const
{
  return(relBearing(m_cnx, m_cny, m_cnh, m_osx, m_osy));
}

// ----------------------------------------------------------
// Procedure: os_cn_absrelbearing()

double OwnshipContactPair::os_cn_absrelbearing() const
{
  return(absRelBearing(m_osx, m_osy, m_osh, m_cnx, m_cny));
}

// ----------------------------------------------------------
// Procedure: cn_os_absrelbearing()

double OwnshipContactPair::cn_os_absrelbearing() const
{
  return(absRelBearing(m_cnx, m_cny, m_cnh, m_osx, m_osy));
}

// ----------------------------------------------------------
// Procedure: total_absrelbearing()

double OwnshipContactPair::total_absrelbearing() const
{
  return(totAbsRelBearing(m_osx, m_osy, m_osh, m_cnx, m_cny, m_cnh));
}

