/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorIPP.cpp                                     */
/*    DATE: Nov 24th 2014                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "PopulatorIPP.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"
#include "IvPProblem_v3.h"
#include "IvPProblem_v2.h"
#include "FileBuffer.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: populate

bool PopulatorIPP::populate(string filename, int alg)
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

  if(alg == 0)
    m_ivp_problem = new IvPProblem;
  else if((alg == 1) || (alg == 2)) {
    m_ivp_problem = new IvPProblem_v2;
    if(alg == 1)
      ((IvPProblem_v2*)(m_ivp_problem))->setFullTreeTraversal();
  }
  else if(alg == 3)
    m_ivp_problem = new IvPProblem_v3;

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
      if(m_grid_override_size != 0)
	overrideGrid(ipf);
      m_ivp_problem->addOF(ipf);
      return(true);
    }
  }
  if(left == "ipfs")
    return(true);

  return(false);
}


//-------------------------------------------------------------
// Procedure: overrideGrid()

void PopulatorIPP::overrideGrid(IvPFunction* ipf)
{
  if((m_grid_override_size <= 0) || !ipf)
    return;

  int dim = ipf->getDim();
  IvPBox gelbox(dim);
  
  for(int d=0; d<dim; d++) {
    // Subtract 1: To get a 2D grid cell size of 10, for example, 
    // we need a point box with both dimentions set to 9.
    int gelsz = m_grid_override_size - 1;
    int domsz = ipf->getPDMap()->getDomain().getVarPoints(d);
    if(gelsz > domsz)
      gelsz = domsz;
    
    gelbox.setPTS(d, gelsz, gelsz);
  }

  ipf->getPDMap()->setGelBox(gelbox);
  ipf->getPDMap()->updateGrid();
}
  




