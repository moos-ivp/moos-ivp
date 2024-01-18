/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HintHolder.cpp                                       */
/*    DATE: Sep 30, 2022                                         */
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


//---------------------------------------------------------------
// Procedure: applyHints()
//      Note: For each attribute, if a prefix is given, first see
//            if there is an value associated with the prefix. If
//            not, check if the attribute has a general value. 

void applyHints(XYPoint& pt, const HintHolder& hints, string prefix)
{
  if(prefix != "")
    prefix += "_";

  if(hints.hasColor(prefix + "label_color"))
    pt.set_color("label",  hints.getColor(prefix + "label_color"));
  else if(hints.hasColor("label_color"))
    pt.set_color("label",  hints.getColor("label_color"));
  
  if(hints.hasColor(prefix + "vertex_color"))
    pt.set_color("vertex", hints.getColor(prefix + "vertex_color"));
  else if(hints.hasColor("vertex_color"))
    pt.set_color("vertex", hints.getColor("vertex_color"));

  if(hints.hasMeasure(prefix + "vertex_size"))
    pt.set_vertex_size(hints.getMeasure(prefix + "vertex_size"));
  else if(hints.hasMeasure("vertex_size"))
    pt.set_vertex_size(hints.getMeasure("vertex_size"));
}

//---------------------------------------------------------------
// Procedure: applyHints()
//      Note: For each attribute, if a prefix is given, first see
//            if there is an value associated with the prefix. If
//            not, check if the attribute has a general value. 

void applyHints(XYSegList& segl, const HintHolder& hints, string prefix)
{
  if(prefix != "")
    prefix += "_";
  
  if(hints.hasColor(prefix + "label_color"))
    segl.set_color("label",  hints.getColor(prefix + "label_color"));
  else if(hints.hasColor("label_color"))
    segl.set_color("label",  hints.getColor("label_color"));

  if(hints.hasColor(prefix + "vertex_color"))
    segl.set_color("vertex", hints.getColor(prefix+"vertex_color"));
  else if(hints.hasColor("vertex_color"))
    segl.set_color("vertex", hints.getColor("vertex_color"));

  if(hints.hasColor(prefix + "edge_color"))
    segl.set_color("edge", hints.getColor(prefix+"edge_color"));
  else if(hints.hasColor("edge_color"))
    segl.set_color("edge", hints.getColor("edge_color"));

  if(hints.hasMeasure(prefix + "edge_size"))
    segl.set_edge_size(hints.getMeasure(prefix + "edge_size"));
  else if(hints.hasMeasure("edge_size"))
    segl.set_edge_size(hints.getMeasure("edge_size"));

  if(hints.hasMeasure(prefix + "vertex_size"))
    segl.set_vertex_size(hints.getMeasure(prefix + "vertex_size"));
  else if(hints.hasMeasure("vertex_size"))
    segl.set_vertex_size(hints.getMeasure("vertex_size"));
}

  
//---------------------------------------------------------------
// Procedure: applyHints()
//      Note: For each attribute, if a prefix is given, first see
//            if there is an value associated with the prefix. If
//            not, check if the attribute has a general value. 

void applyHints(XYPolygon& poly, const HintHolder& hints, string prefix)
{
  if(prefix != "")
    prefix += "_";
  
  if(hints.hasColor(prefix + "label_color"))
    poly.set_color("label",  hints.getColor(prefix + "label_color"));
  else if(hints.hasColor("label_color"))
    poly.set_color("label",  hints.getColor("label_color"));

  if(hints.hasColor(prefix + "vertex_color"))
    poly.set_color("vertex", hints.getColor(prefix+"vertex_color"));
  else if(hints.hasColor("vertex_color"))
    poly.set_color("vertex", hints.getColor("vertex_color"));

  if(hints.hasColor(prefix + "edge_color"))
    poly.set_color("edge", hints.getColor(prefix+"edge_color"));
  else if(hints.hasColor("edge_color"))
    poly.set_color("edge", hints.getColor("edge_color"));

  if(hints.hasColor(prefix + "fill_color"))
    poly.set_color("fill", hints.getColor(prefix+"fill_color"));
  else if(hints.hasColor("fill_color"))
    poly.set_color("fill", hints.getColor("fill_color"));

  if(hints.hasMeasure(prefix + "edge_size"))
    poly.set_edge_size(hints.getMeasure(prefix + "edge_size"));
  else if(hints.hasMeasure("edge_size"))
    poly.set_edge_size(hints.getMeasure("edge_size"));

  if(hints.hasMeasure(prefix + "vertex_size"))
    poly.set_vertex_size(hints.getMeasure(prefix + "vertex_size"));
  else if(hints.hasMeasure("vertex_size"))
    poly.set_vertex_size(hints.getMeasure("vertex_size"));
}

