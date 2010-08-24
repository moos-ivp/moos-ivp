/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_VZAIC.cpp                                  */
/*    DATE: May 5th, 2010                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "PopulatorVZAIC.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "BuildUtils.h"

using namespace std;

PopulatorVZAIC::PopulatorVZAIC()
{
  m_minutil = 0;
  m_maxutil = 100;
}

//-------------------------------------------------------------
// Procedure: buildZAIC()

ZAIC_Vector *PopulatorVZAIC::buildZAIC()
{
  if(m_domain.size() == 0) {
    cout << "Unable to build ZAIC - IvPDomain size is zero" << endl;
    return(0);
  }

  if(m_domain_vals.size() != m_range_vals.size()) {
    cout << "Unable to build ZAIC - domain/range vector mismatch" << endl;
    return(0);
  }

  cout << "Number of domain vals: " << m_domain_vals.size() << endl;
  cout << "Number of range vals:  " << m_range_vals.size() << endl;

  string varname = m_domain.getVarName(0);
  ZAIC_Vector *new_zaic = new ZAIC_Vector(m_domain, varname); 

  new_zaic->setDomainVals(m_domain_vals);
  new_zaic->setRangeVals(m_range_vals);

  return(new_zaic);
}

//-------------------------------------------------------------
// Procedure: readFile()

bool PopulatorVZAIC::readFile(string filename)
{
  cout << "Reading in File: " << filename << endl;

  vector<string> lines = fileBuffer(filename);
  unsigned int i, vsize = lines.size();
  if(vsize == 0) {
    cout << "  File not found - or empty file." << endl;
    return(false);
  }

  for(i=0; i<vsize; i++) {
    string line  = stripBlankEnds(lines[i]);
    if(!strBegins(line, "//")) {
      string left  = stripBlankEnds(tolower(biteString(lines[i], '=')));
      string right = stripBlankEnds(lines[i]);
      
      if(left == "ivpdomain") // format: "X,0,100,101 : Y,0,1,11"
	m_domain = stringToDomain(right);
      else if(left == "domain") {
	vector<string> jvector = parseString(right, ',');
	unsigned int j, jsize = jvector.size();
	for(j=0; j<jsize; j++) 
	  m_domain_vals.push_back(atof(jvector[j].c_str()));
      }
      else if(left == "range") {
	vector<string> jvector = parseString(right, ',');
	unsigned int j, jsize = jvector.size();
	for(j=0; j<jsize; j++) 
	  m_range_vals.push_back(atof(jvector[j].c_str()));
      }
      else if(left == "minutil")
	m_minutil = atof(right.c_str());
      else if(left == "maxutil")
	m_maxutil = atof(right.c_str());
    }
  }
  cout << "Done reading in file: " << filename << endl;
  return(true);
}



