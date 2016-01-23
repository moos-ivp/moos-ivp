/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorAvkTable.cpp                                */
/*    DATE: Mar 26th, 2015                                       */
/*****************************************************************/

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include "FileBuffer.h"
#include "PopulatorAvkTable.h"
#include <cstdlib>

using namespace std;

//-------------------------------------------------------------
// Procedure: setAvkFile()

bool PopulatorAvkTable::setAvkFile(string filename)
{
  FILE *f = fopen(filename.c_str(), "r");
  if(!f)
    return(false);
  fclose(f);
  
  m_avk_file = filename;
  return(true);
}

//-------------------------------------------------------------
// Procedure: populate
//
// Expected file format:
//
// spd_gap = 0.1
// spd_max = 3.0
// hdg_gap = 5
// spd=0.3,dh=45,coeffs=0.232:1.23:8.733
// spd=0.3,dh=50,coeffs=2.832:4.24:6.533
// spd=0.3,dh=55,coeffs=1.273:1.21:8.233
// ...
// spd=1.2,dh=10,coeffs=8.239:1.29:2.773
// spd=1.2,dh=10,coeffs=4.262:1.43:8.433


bool PopulatorAvkTable::populate()
{
  if(m_avk_file == "")
    return(false);
  
  vector<string> lines = fileBuffer(m_avk_file);

  unsigned int i, lines_amt = lines.size();
  if(lines_amt == 0) {
    cout << "  File not found - or empty file." << endl;
    return(false);
  }

  double hdg_gap = 0;
  double spd_gap = 0;
  double spd_max = 0;

  // ------------------------------------------------------------------
  // Part 1: Get the AVKTable configuration parameters and sanity check
  // ------------------------------------------------------------------

  for(i=0; i<lines_amt; i++) {
    string line  = stripBlankEnds(lines[i]);
    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    if(param == "hdg_gap")
      hdg_gap = atof(value.c_str());
    else if(param == "spd_gap")
      spd_gap = atof(value.c_str());
    else if(param == "spd_max")
      spd_max = atof(value.c_str());
  }
  
  if(hdg_gap <= 0) {
    if(m_verbose)
      cout << "AVKTable popul failed: hdg_gap must be pos #: " << hdg_gap << endl;
    return(false);
  }
  if(spd_gap <= 0) {
    if(m_verbose)
      cout << "AVKTable popul failed: spd_gap must be pos #: " << spd_gap << endl;
    return(false);
  }
  if(spd_max <= 0) {
    if(m_verbose)
      cout << "AVKTable popul failed: spd_max must be pos #: " << spd_max << endl;
    return(false);
  }

  // ------------------------------------------------------------------
  // Part 2: Configure the AVKTable instance 
  // ------------------------------------------------------------------

  bool ok = m_avk_table.configure(hdg_gap, spd_gap, spd_max);
  if(!ok) {
    if(m_verbose)
      cout << "AKTable could not be configured." << endl;
    return(false);
  }

  // ------------------------------------------------------------------
  // Part 3: Populate the AVK_Table cells
  // ------------------------------------------------------------------

  for(i=0; i<lines_amt; i++) {
    string line = stripBlankEnds(lines[i]);
    if(strBegins(line, "spd=")) {
      double spd  = 0;
      double dh   = 0;
      double xend = 0;
      double time = 0;
      double rotation = 0;
      vector<double> coeffs;

      vector<string> jvector = parseString(line, ',');
      for(unsigned int j=0; j<jvector.size(); j++) {
	string param = tolower(biteStringX(jvector[j], '='));
	string value = jvector[j];
	if(param == "spd") 
	  spd = atof(value.c_str());
	else if(param == "dh")
	  dh = atof(value.c_str());
	else if(param == "xend")
	  xend = atof(value.c_str());
	else if(param == "time")
	  time = atof(value.c_str());
	else if(param == "rot")
	  rotation = atof(value.c_str());
	else if(param == "coeffs") {
          vector<string> kvector = parseString(value, ':');
	  for(unsigned int k=0; k<kvector.size(); k++) {
	    double coeff = atof(kvector[k].c_str());
	    coeffs.push_back(coeff);
	  }
	}
      }

      bool ok1 = m_avk_table.setCellCoeffs(spd, dh, coeffs);
      bool ok2 = m_avk_table.setCellXend(spd, dh, xend);
      bool ok3 = m_avk_table.setCellTime(spd, dh, time);
      bool ok4 = m_avk_table.setCellRotation(spd, dh, rotation);

      if(!ok1 || !ok2 || !ok3 || !ok4) {
	cout << "Problem with line:" << lines[i] << endl; 
	cout << ok1 << ok2 << ok3 << ok4 << endl;
	cout << "Could not add Coefficients to AVKTable." << endl;
	return(false);
      }
    }
  }
  
  if(m_verbose)
    cout << "AVKTable successfully created!" << endl;

  return(true);
}
