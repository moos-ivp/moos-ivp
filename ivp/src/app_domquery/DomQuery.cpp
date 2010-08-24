/*****************************************************************/
/*    NAME: Michael Benjamin, H. Schmidt, J. Leonard             */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: DomQuery.cpp                                         */
/*    DATE: March 10th, 2009                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "DomQuery.h"
#include "BuildUtils.h"
#include "BoxSet.h"

using namespace std;

//--------------------------------------------------------
// Constructor

DomQuery::DomQuery()
{
  m_infile  = "";
}

//--------------------------------------------------------
// Procedure: buildDomain

void DomQuery::buildDomain()
{
  vector<string> fvector = fileBuffer(m_infile);
  unsigned int i, vsize = fvector.size();
  if(vsize == 0)
    return;

  IvPDomain new_domain;
  
  bool ok = true;
  for(i=0; ((i<vsize) && ok); i++) {
    string line = stripBlankEnds(fvector[i]);
    cout << line << endl;
    bool var_set = false;
    bool low_set = false;
    bool hgh_set = false;
    bool pts_set = false;
    string var, low, hgh, pts;
    vector<string> svector = parseString(line, ',');
    unsigned int j, lsize = svector.size();
    for(j=0; j<lsize; j++) {
      svector[j] = stripBlankEnds(svector[j]);
      vector<string> jvector = parseString(svector[j], '=');
      if(jvector.size()==2) {
	string left  = tolower(stripBlankEnds(jvector[0]));
	string right = stripBlankEnds(jvector[1]);
	if((left == "variable") || (left == "var")) {
	  var_set = true;
	  var = right;
	}
	if(left == "low") {
	  low_set = true;
	   low= right;
	}
	if((left == "hgh") || (left == "high")) {
	  hgh_set = true;
	  hgh = right;
	}
	if((left == "pts") || (left == "points")) {
	  pts_set = true;
	  pts = right;
	}
      }
    }
    if(var_set && low_set && hgh_set && pts_set &&
       isNumber(low) && isNumber(hgh) && isNumber(pts)) {
      double dlow = atof(low.c_str());
      double dhgh = atof(hgh.c_str());
      double dpts = atof(pts.c_str());
      new_domain.addDomain(var, dlow, dhgh, dpts);
    }
  }
  new_domain.print();

  m_domain = new_domain;
}
  
//--------------------------------------------------------
// Procedure: requestPieceCount()

void DomQuery::requestPieceCount(int pieces)
{
  if(m_domain.size() == 0)
    cout << "Empty Domain - exiting now." << endl;

  IvPBox ubox = genUnifBox(m_domain, pieces);
  ubox.print();

  IvPBox universe = domainToBox(m_domain);
  BoxSet *boxset = makeUniformDistro(universe, ubox, 1);
  
  int bsize = boxset->size();
    
  cout << "Actual pieces created: " << bsize << endl;

}

//--------------------------------------------------------
// Procedure: verifyInfile()

bool DomQuery::verifyInfile()
{
  // First abort condition: Cannot read the input file.
  FILE *f = fopen(m_infile.c_str(), "r");
  if(f)
    fclose(f);
  else {
    cout << "Aborted: " << m_infile << " cannot be opened. " << endl;
    return(false);
  }
  return(true);
}

