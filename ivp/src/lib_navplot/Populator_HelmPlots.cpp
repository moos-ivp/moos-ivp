/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_HelmPlots.cpp                              */
/*    DATE: July 15th, 2009                                      */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Populator_HelmPlots.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populate

bool Populator_HelmPlots::populateFromALog()
{
  int i, vsize = m_helm_summaries.size();
  
  m_helm_plot.set_vehi_name("unknown");
  m_helm_plot.set_vehi_type("unknown");
  m_helm_plot.set_vehi_length(10);
  handleNodeReports();

  for(i=0; i<vsize; i++) {
    m_helm_plot.add_entry(m_helm_summary_times[i], 
			  m_helm_summaries[i]);
  }


  return(true);
}

//---------------------------------------------------------------
// Procedure: setFileALog

bool Populator_HelmPlots::setFileALog(string filestr)
{
  FILE *f = fopen(filestr.c_str(), "r");

  if(!f)
    return(false);

  fclose(f);
  m_file = filestr;
  m_lines = fileBuffer(filestr);

  unsigned int i;
  for(i=0; i<m_lines.size(); i++) {
    m_lines[i] = findReplace(m_lines[i], '\t', ' ');
    m_lines[i] = compactConsecutive(m_lines[i], ' ');
    m_lines[i] = stripBlankEnds(m_lines[i]);
    
    string time = stripBlankEnds(biteString(m_lines[i],' '));
    string var  = stripBlankEnds(biteString(m_lines[i],' '));
    if(var == "IVPHELM_SUMMARY") {
      string source = stripBlankEnds(biteString(m_lines[i],' '));
      string value  = stripBlankEnds(m_lines[i]);
      if((time!="")&&(var!="")&&(source!="")&&(value!="")) {
	m_helm_summary_times.push_back(atof(time.c_str()));
	m_helm_summaries.push_back(value);
      }
    }
    else if(var == "NODE_REPORT_LOCAL") {
      string source = stripBlankEnds(biteString(m_lines[i],' '));
      string value  = stripBlankEnds(m_lines[i]);
      if((time!="")&&(var!="")&&(source!="")&&(value!="")) 
	m_node_reports.push_back(value);
    }
  }

  return(true);
}

//---------------------------------------------------------------
// Procedure: handleNodeReports

bool Populator_HelmPlots::handleNodeReports()
{
  string vtype, vname, vlen;

  unsigned int i, vsize = m_node_reports.size();
  for(i=0; i<vsize; i++) {
    vector<string> kvector = parseString(m_node_reports[i], ',');
    unsigned int k, ksize = kvector.size();
    for(k=0; k<ksize; k++) {
      string left  = stripBlankEnds(tolower(biteString(kvector[k], '=')));
      string right = stripBlankEnds(kvector[k]);
      
      if(left == "name") {
	if((vname == "") || (vname == right))
	  vname = right;
	else {
	  cout << "Inconsistent vehicle name from NODE_REPORT_LOCAL entries" << endl;
	  return(false);
	}
      }
      else if(left == "type") {
	if((vtype == "") || (vtype == right))
	  vtype = right;
	else {
	  cout << "Inconsistent vehicle type from NODE_REPORT_LOCAL entries" << endl;
	  return(false);
	}
      }
      else if(left == "length") {
	if((vlen == "") || (vlen == right))
	  vlen = right;
	else {
	  cout << "Inconsistent vehicle length from NODE_REPORT_LOCAL entries" << endl;
	  return(false);
	}
      }
    }
  }

  m_helm_plot.set_vehi_name(vname);
  m_helm_plot.set_vehi_type(vtype);
  m_helm_plot.set_vehi_length(atof(vlen.c_str()));

  return(true);
}


//---------------------------------------------------------------
// Procedure: handleHelmSummaries

bool Populator_HelmPlots::handleHelmSummaries()
{
  return(true);
}
