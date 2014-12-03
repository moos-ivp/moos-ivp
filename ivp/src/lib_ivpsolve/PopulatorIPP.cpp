/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorIPP.cpp                                     */
/*    DATE: Nov 24th 2014                                        */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "PopulatorIPP.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"
#include "FileBuffer.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: populate

bool PopulatorIPP::populate(string filename)
{
  FILE *f = fopen(filename.c_str(), "r");
  if(!f) {
    if(m_verbose)
      cout << "Could not find File: " << filename << endl;
    return(false);
  }

  if(m_verbose)
    cout << "Successfully found File: " << filename << endl;
  fclose(f);

  if(m_ivp_problem)
    delete(m_ivp_problem);
  m_ivp_problem = new IvPProblem;

  vector<string> svector = fileBuffer(filename);
    
  for(unsigned int i=0; i<svector.size(); i++) {
    string line = stripBlankEnds(svector[i]);
    if((line.length()!=0) && ((line)[0]!='#')) {
      bool res = handleLine(line);
      if(!res) {
	cout << " Problem with line " << i+1;
	cout << " in the file: " << filename << endl;
	cout << line << endl;

	delete(m_ivp_problem);
	m_ivp_problem = 0;

	return(false);
      }
    }
  }

  if(m_verbose)
    cout << endl << "Done Populating from: " << filename << endl;
  return(true);
}


//-------------------------------------------------------------
// Procedure: handleLine

bool PopulatorIPP::handleLine(string line)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  line = stripComment(line, "#");
  line = stripBlankEnds(line);

  if(line.size() == 0)  // Either blank or comment line
    return(true);  
  
  string left  = biteStringX(line, '=');
  string right = line;
  
  if(left == "domain") {
    IvPDomain domain = stringToDomain(right);
    if(domain.size() > 0) {
      m_ivp_problem->setDomain(domain);
      return(true);
    }
  }
  if(left == "ipf") {
    if(m_verbose)
      cout << "." << flush;
    IvPFunction *ipf = StringToIvPFunction(right);
    if(ipf) {
      m_ivp_problem->addOF(ipf);
      return(true);
    }
  }
  if(left == "ipfs")
    return(true);

  return(false);
}
