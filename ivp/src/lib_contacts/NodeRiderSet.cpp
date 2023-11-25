/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeRiderSet.cpp                                     */
/*    DATE: May 7th 2022                                         */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include "NodeRiderSet.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Procedure: addNodeRider()

bool NodeRiderSet::addNodeRider(string str)
{
  NodeRider rider;

  bool handled = true;
  
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if((param == "var") && !strContainsWhite(value))
      handled = handled && rider.setVar(value);
    else if(param == "policy")
      handled = handled && rider.setPolicyConfig(value);
    else if(param == "rfld")
      handled = handled && rider.setRiderFld(value);
    else
      handled = false;
  }

  handled = handled && addNodeRider(rider);

  return(handled);
}

//------------------------------------------------------------
// Procedure: addNodeRider()

bool NodeRiderSet::addNodeRider(NodeRider rider)
{
  for(unsigned int i=0; i<m_riders.size(); i++) {
    if(m_riders[i].getVar() == rider.getVar())
      return(false);
  }

  m_riders.push_back(rider);
  return(true);
}

//------------------------------------------------------------
// Procedure: updateRider()

bool NodeRiderSet::updateRider(string var, string val, double utc)
{
  bool ok = false;
  for(unsigned int i=0; i<m_riders.size(); i++) {
    if(m_riders[i].getVar() == var) 
      ok = m_riders[i].updateValue(val, utc);
  }
  return(ok);
}

//------------------------------------------------------------
// Procedure: getVars()

vector<string> NodeRiderSet::getVars() const
{
  vector<string> vars;

  for(unsigned int i=0; i<m_riders.size(); i++) 
    vars.push_back(m_riders[i].getVar());

  return(vars);
}

//------------------------------------------------------------
// Procedure: getRiderSpecs()

vector<string> NodeRiderSet::getRiderSpecs() const
{
  vector<string> specs;

  for(unsigned int i=0; i<m_riders.size(); i++) {
    string spec = m_riders[i].getSpec();
    if(spec != "")
      specs.push_back(m_riders[i].getSpec());
  }
  
  return(specs);
}

//---------------------------------------------------------------
// Procedure: getNodeRider()

NodeRider NodeRiderSet::getNodeRider(unsigned int i) const
{
  NodeRider null_rider;
  if(i >= m_riders.size())
    return(null_rider);
  
  return(m_riders[i]);
}

//---------------------------------------------------------------
// Procedure: getRiderReports()

string NodeRiderSet::getRiderReports(double curr_time)
{
  string reports;
  for(unsigned int i=0; i<m_riders.size(); i++) {
    string policy  = m_riders[i].getPolicy();
    string moosvar = m_riders[i].getVar();
    string currval = m_riders[i].getCurrVal();
    string rfield  = m_riders[i].getRiderFld();
    double freq    = m_riders[i].getFrequency();
    bool   fresh   = m_riders[i].isFresh();
    double elapsed = curr_time - m_riders[i].getLastUTC();
    string report;

    if(currval == "")
      continue;

    if(rfield == "")
      rfield = moosvar;
    
    // Wrap in braces if value contains comma to protect
    // node report parsing.
    if(strContains(currval, ','))
      currval = "{" + currval + "}";
    
    if(policy == "always") 
      report = rfield + "=" + currval;
    else if(policy == "updated") {
      if(fresh)
	report = rfield + "=" + currval;
      else if(freq > 0) {
	if(elapsed >= freq) {
	  report = moosvar + "=" + currval;
	  m_riders[i].setLastUTC(curr_time);
	}
      }
    }
    else if(policy == "freq") {
      if(elapsed >= freq) {
	report = rfield + "=" + currval;
	m_riders[i].setLastUTC(curr_time);
      }
    }      
    m_riders[i].setFresh(false);

    if(reports != "")
      reports += ",";
    reports += report;
  }
    
  return(reports);
}

