/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorSpec.cpp                                     */
/*    DATE: Feb 11th, 2010                                       */
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

#include <iostream>
#include "BehaviorSpec.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: setBehaviorKind

BehaviorSpec::BehaviorSpec()
{
  m_templating = "disallowed";
  m_templating_enabled = false;
  m_spawns_made = 0;
  m_spawns_tried = 0;
}

//--------------------------------------------------------------------
// Procedure: setBehaviorKind

void BehaviorSpec::setBehaviorKind(string kind, unsigned int line_num)
{
  m_behavior_kind = kind;
  m_kind_line_num = line_num;
}

//--------------------------------------------------------------------
// Procedure: addBehaviorConfig
//      Note: For the most part we are just accepting the config line 
//            to be handled by something processing this behavior spec. 
//      Note: We note the line number from the original behavior file 
//            in case we need to later give hints as to where the spec 
//            went wrong if it fails to result in the instatiation of 
//            a behavior.

void BehaviorSpec::addBehaviorConfig(string config_line, 
				     unsigned int line_num)
{
  // We do want to examine "updates=" and "templating=" configuration
  // lines since they provide information about when and how this 
  // behavior spec is used as a template for spawning new behaviors.
  string cline = config_line;
  string left  = tolower(biteStringX(cline, '='));
  string right = cline;
  if((left == "updates") && !strContainsWhite(right))
    m_updates_var = right;
  if(left == "templating")
    setTemplatingType(right);
  if(left == "name")
    m_name_prefix = right;

  // The templating information applies only to the template and is not
  // a behavior configuration. Therefore dont add the templating line
  // to the behavior specs as a configuration parameter.
  if(left != "templating") {
    m_config_lines.push_back(config_line);
    m_config_line_num.push_back(line_num);
  }
}

//--------------------------------------------------------------------
// Procedure: checkForSpawningStrings
//      Note: Checks the info_buffer to see if new info has been posted
//            in this template's UPDATES variable that would indicate
//            that a spawning has been requested.
//      Note: Returns the full, raw update strings.
//            Actual spawning does not occur until after the caller 
//            determines that candidate names are unique to the helm.
//      Note: We can't assume that only one posting to the UPDATES var
//            occurs on any one iteration. Thus we check for multiple
//            postings in the info_buffer and return a vector of strings.

vector<string> BehaviorSpec::checkForSpawningStrings()
{
  vector<string> spawning_strings;
  if((!m_templating_enabled) || (m_updates_var == ""))
    return(spawning_strings);
  
  bool ok;
  vector<string> svector = m_info_buffer->sQueryDeltas(m_updates_var, ok);

  // If a given UPDATES string does not contain a "name=" component, we
  // know it's a non-spawning update, so filter it out.
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    if(strContains(svector[i], "name"))
      spawning_strings.push_back(svector[i]);
  }

  return(spawning_strings);
}

//--------------------------------------------------------------------
// Procedure: setTemplatingType()

bool BehaviorSpec::setTemplatingType(string str)
{
  if((str == "clone") || (str == "spawn")) {
    m_templating = str;
    m_templating_enabled = true;
    return(true);
  }
  if(str == "disallow") {
    m_templating = str;
    m_templating_enabled = false;
    return(true);
  }
  return(false);
}

//--------------------------------------------------------------------
// Procedure: clear()

void BehaviorSpec::clear()
{
  m_behavior_kind = "";
  m_updates_var   = "";
  m_kind_line_num = 0;
  m_spawns_tried  = 0;
  m_spawns_made   = 0;
  m_templating    = "disallowed";
  m_templating_enabled = false;
  m_config_line_num.clear();
  m_config_lines.clear();
}


//--------------------------------------------------------------------
// Procedure: print()

void BehaviorSpec::print() const
{
  unsigned int i, vsize = m_config_lines.size();
  
  cout << "BehaviorSpec: Kind=[" << m_behavior_kind << "]";
  cout << " (on line " << m_kind_line_num << ")" << endl;
  cout << "  # of lines: " << vsize << endl;
  for(i=0; i<vsize; i++) {
    cout << "[" << i << "]: [" << m_config_lines[i] << "]" << endl;
  }
}

//--------------------------------------------------------------------
// Procedure: getConfigLine()

string BehaviorSpec::getConfigLine(unsigned int ix) const
{
  if(ix < m_config_lines.size())
    return(m_config_lines[ix]);
  else
    return("");
}

//--------------------------------------------------------------------
// Procedure: getConfigLineNum()

unsigned int BehaviorSpec::getConfigLineNum(unsigned int ix) const
{
  if(ix < m_config_line_num.size())
    return(m_config_line_num[ix]);
  else
    return(0);
}











