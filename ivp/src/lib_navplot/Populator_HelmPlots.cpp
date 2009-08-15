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

using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries

bool Populator_HelmPlots::populateFromEntries(const vector<ALogEntry>& entries)
{
  int i, vsize = entries.size();
  if(vsize == 0)
    return(false);
  
  for(i=0; i<vsize; i++) {
    string var = entries[i].getVarName();
    if(var == "IVPHELM_SUMMARY")
      m_helm_plot.add_entry(entries[i].getTimeStamp(), 
			    entries[i].getStringVal());
    else if((var == "NODE_REPORT_LOCAL") || (var == "AIS_REPORT_LOCAL")) {
      string source = entries[i].getSource();
      if((source == "pNodeReporter") || (source == "pTransponderAIS")) {
	m_node_reports.push_back(entries[i].getStringVal());
      }
    }
  }

  // Read throught the node reports and get vehicle name, type, length
  // Results stored in m_helm_plot instance. A "false" value is returned
  // if either (a) that info is not found, or (b) it is inconsistent.
  bool handled = handleNodeReports();
  if(!handled)
    return(false);

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
	  cout << "Inconsistent vehicle name from AIS/NODE_REPORT_LOCAL entries" << endl;
	  return(false);
	}
      }
      else if(left == "type") {
	if((vtype == "") || (vtype == right))
	  vtype = right;
	else {
	  cout << "Inconsistent vehicle type from AIS/NODE_REPORT_LOCAL entries" << endl;
	  return(false);
	}
      }
      else if(left == "length") {
	if((vlen == "") || (vlen == right))
	  vlen = right;
	else {
	  cout << "Inconsistent vehicle length from AIS/NODE_REPORT_LOCAL entries" << endl;
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

