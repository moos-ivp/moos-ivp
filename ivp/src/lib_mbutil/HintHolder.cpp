/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HintHolder.cpp                                       */
/*    DATE: Sep 30, 2022                                         */
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

#include "HintHolder.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

HintHolder::HintHolder()
{
}

//-----------------------------------------------------------
// Procedure: setColor()

bool HintHolder::setColor(string key, string sval) 
{
  key = findReplace(key, "lcolor", "label_color");
  if(!isColor(sval))
    return(false);

  m_map_colors[key] = sval;
  return(true);
}

//-----------------------------------------------------------
// Procedure: setMeasure()

bool HintHolder::setMeasure(string key, double dval) 
{
  if(dval < 0)
    return(false);

  m_map_measures[key] = dval;
  return(true);
}

//-----------------------------------------------------------
// Procedure: setMeasure()

bool HintHolder::setMeasure(string key, string sval) 
{
  if(!isNumber(sval))
    return(false);

  setMeasure(key, atof(sval.c_str()));
  return(true);
}

//-----------------------------------------------------------
// Procedure: getColor()

string HintHolder::getColor(string key) const
{
  map<string,string>::const_iterator q=m_map_colors.find(key);
  if(q==m_map_colors.end())
    return("");
  else
    return(q->second);
}

//-----------------------------------------------------------
// Procedure: getMeasure()

double HintHolder::getMeasure(string key) const
{
  map<string,double>::const_iterator q=m_map_measures.find(key);
  if(q==m_map_measures.end())
    return(0);
  else
    return(q->second);
}

//-----------------------------------------------------------
// Procedure: hasColor()

bool HintHolder::hasColor(string key) const
{
  map<string,string>::const_iterator q=m_map_colors.find(key);

  return(q != m_map_colors.end());
}

//-----------------------------------------------------------
// Procedure: hasMeasure()

bool HintHolder::hasMeasure(string key) const
{
  map<string,double>::const_iterator q=m_map_measures.find(key);

  return(q != m_map_measures.end());
}

//-----------------------------------------------------------
// Procedure: setHints(string)
//   Example: edge_color=blue, vertex_size=3

bool HintHolder::setHints(string hints) 
{
  vector<string> svector = parseString(hints, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    bool ok = setHint(svector[i]);
    if(!ok)
      return(false);
  }

  return(true);
}

//-----------------------------------------------------------
// Procedure: setHint(string)
//   Example: edge_color=blue

bool HintHolder::setHint(string hint) 
{
  string param = tolower(biteStringX(hint, '='));
  string value = hint;
  return(setHint(param, value));
}

//-----------------------------------------------------------
// Procedure: setHint(string, string)

bool HintHolder::setHint(string param, string value) 
{
  if(strContains(param, "color"))
    return(setColor(param, value));

  return(setMeasure(param, value));
}


//-----------------------------------------------------------
// Procedure: getSpec()

string HintHolder::getSpec() const
{
  string spec;
  
  map<string,string>::const_iterator p;
  for(p=m_map_colors.begin(); p!=m_map_colors.end(); p++) {
    if(spec != "")
      spec += ",";
    spec += p->first + "=" + p->second;
  }

  map<string,double>::const_iterator q;
  for(q=m_map_measures.begin(); q!=m_map_measures.end(); q++) {
    if(spec != "")
      spec += ",";
    spec += q->first + "=" + doubleToStringX(q->second,5);
  }

  return(spec);
}
