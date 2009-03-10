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
#include <string>
#include <iostream>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "DomQuery.h"

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
  int vsize = fvector.size();
  if(vsize == 0)
    return;

  IvPDomain new_domain;
  
  bool ok = true;
  for(int i=0; ((i<vsize) && ok); i++) {
    string line = fvector[i];
    line = stripBlankEnds(line);
    cout << line << endl;
  }

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

