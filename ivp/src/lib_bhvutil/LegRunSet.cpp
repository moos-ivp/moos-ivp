/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: LegRunSet.cpp                                   */
/*    DATE: June 11th, 2023                                 */
/************************************************************/

#include <iterator>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "LegRunSet.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

LegRunSet::LegRunSet()
{
}

//---------------------------------------------------------
// Procedure: getLegIDs()

vector<string> LegRunSet::getLegIDs() const
{
  vector<string> leg_ids;
  
  map<string, LegRun>::const_iterator p;
  for(p=m_map_legruns.begin(); p!=m_map_legruns.end(); p++) 
    leg_ids.push_back(p->first);
    
  return(leg_ids);
}

//---------------------------------------------------------
// Procedure: getLegRuns()

vector<LegRun> LegRunSet::getLegRuns() const
{
  vector<LegRun> legruns;
  
  map<string, LegRun>::const_iterator p;
  for(p=m_map_legruns.begin(); p!=m_map_legruns.end(); p++) 
    legruns.push_back(p->second);
    
  return(legruns);
}

//---------------------------------------------------------
// procedure: getLegRun()

LegRun LegRunSet::getLegRun(string id) const
{
  LegRun null_legrun;

  map<string,LegRun>::const_iterator q=m_map_legruns.find(id);
  if(q==m_map_legruns.end())
    return(null_legrun);

  return(q->second);
}

//---------------------------------------------------------
// Procedure: setLegParams()
//  Examples: id=alpha, p1=4:4, p2=5:9
//            id=alpha, turn1_rad=30, turn2_rad=40
//            id=alpha, turn1_ext=10, turn2_ext=15

bool LegRunSet::setLegParams(string str)
{
  string id = tokStringParse(str, "id");

  if(id == "")
    string id = tokStringParse(str, "vname");

  if(id == "")
    return(false);

  m_map_legruns[id].setParams(str);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleLegRunFile()

bool LegRunSet::handleLegRunFile(string str, double curr_time,
				 string& warning)
{
  vector<string> lines = fileBuffer(str);
  if(lines.size() == 0) {
    warning = "File not found, or empty: " + str;
    return(false);
  }

  // Part 1: Parse all the lines
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    if(strBegins(line, "//"))
      continue;
    if(strBegins(line, "#"))
      continue;
    if(line == "")
      continue;
    
    bool ok = setLegParams(line);
    if(!ok) {
      warning = "Bad LegRun Line:[" + line + "]";
      return(false);
    }
  }
  m_legrun_file = str;

  return(true);
}

