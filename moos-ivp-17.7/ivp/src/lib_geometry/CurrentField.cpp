/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CurrentField.cpp                                     */
/*    DATE: Jan 4th, 2011                                        */
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

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "CurrentField.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "FileBuffer.h"
#include "XYFormatUtilsVector.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

CurrentField::CurrentField()
{
  m_radius     = 20;
  m_active_ix  = 0;
  m_field_name = "generic_cfield";
  m_active_vertex = false;
}

//-------------------------------------------------------------------
// Procedure: populate
//   Purpose: 

bool CurrentField::populate(string filename)
{
  unsigned int lines_ok  = 0;
  unsigned int lines_bad = 0;

  vector<string> lines = fileBuffer(filename);
  unsigned int i, vsize = lines.size();
  if(vsize == 0)
    return(false);

  for(i=0; i<vsize; i++) {
    string line = stripBlankEnds(lines[i]);
    bool ok = handleLine(line);
    
    if(!ok) {
      lines_bad++;
      cout << "Problem with line " << i << endl;
      cout << "  [" << line << "]" << endl;
    }
    else
      lines_ok++;
  }

  applyRenderHints();

  cout << "Done Populating Current Field." << endl;
  cout << "  OK Entries: " << lines_ok << endl;
  cout << "  Bad Entries: " << lines_bad << endl;

  return(true);
}

//-------------------------------------------------------------------
// Procedure: addVector
//   Purpose: 

void CurrentField::addVector(const XYVector& new_vector, bool marked)
{
  m_vectors.push_back(new_vector);
  m_vmarked.push_back(marked);
}

//-------------------------------------------------------------------
// Procedure: getLocalForce
//   Purpose: 

void CurrentField::getLocalForce(double x, double y, 
				 double& return_force_x, 
				 double& return_force_y) const
{
  double total_force_x = 0;
  double total_force_y = 0;
  unsigned int count = 0;

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    double xpos = m_vectors[i].xpos();
    double ypos = m_vectors[i].ypos();
    double dist = distPointToPoint(x, y, xpos, ypos);
    if(dist < m_radius) {
      count++;

      // radius = 10
      // dist = 9, pct = 0.1 --> 0.01
      // dist = 1, pct = 0.9 --> 0.81

      double pct = (1 - (dist / m_radius));
      pct = pct * pct;
      
      double xdot = pct * m_vectors[i].xdot();
      double ydot = pct * m_vectors[i].ydot();

      total_force_x += xdot;
      total_force_y += ydot;
    }
  }
  if(count == 0) {
    return_force_x = 0;
    return_force_y = 0;
    return;
  }
  
  return_force_x = total_force_x / (double)(count); 
  return_force_y = total_force_y / (double)(count); 
}

//-------------------------------------------------------------------
// Procedure: setRadius
//   Purpose: 

void CurrentField::setRadius(double radius)
{
  if(radius < 1)
    radius = 1;
  m_radius = radius;
}

//-------------------------------------------------------------------
// Procedure: initGeodesy
//   Purpose: 

bool CurrentField::initGeodesy(double datum_lat, double datum_lon)
{
  bool ok = m_geodesy.Initialise(datum_lat, datum_lon);
  return(ok);
}

//-------------------------------------------------------------------
// Procedure: 
//   Purpose: 

void CurrentField::print()
{
  cout << "Current Field [" << m_field_name << "]:" << endl;
  cout << "Total Entries: " << size() << endl;
  cout << "Radius:" << m_radius << endl;
  
  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) 
    cout << m_vectors[i].get_spec() << endl;
}

//-------------------------------------------------------------------
// Procedure: markVector
//   Purpose: Mark the vector at the given index if it is within range
//            Unmark all previously marked vectors.

void CurrentField::markVector(unsigned int ix)
{
  if(ix >= m_vmarked.size())
    return;
  m_vmarked[ix] = true;

  unsigned int i, vsize = m_vmarked.size();
  for(i=0; i<vsize; i++)
    if(i!=ix) 
      m_vmarked[i] = false;

  m_active_ix = ix;
  m_active_vertex = true;
}


//-------------------------------------------------------------------
// Procedure: markupVector
//   Purpose: Mark the vector at the given index if it is within range
//            Keep all other marked vectors marked.

void CurrentField::markupVector(unsigned int ix)
{
  if(ix >= m_vmarked.size())
    return;
  m_vmarked[ix] = true;
  m_active_ix = ix;
  m_active_vertex = true;

}

//-------------------------------------------------------------------
// Procedure: unmarkVector
//   Purpose: UnMark the vector at the given index if within range

bool CurrentField::unmarkVector(unsigned int ix)
{
  if(ix >= m_vmarked.size())
    return(false);

  bool changed = false;
  if(m_vmarked[ix])
    changed = true;
  m_vmarked[ix] = false;

  // If the unmarked vector was the "active" vector, then choose a 
  // new active vector solely based on the highest index number. Ideally
  // we would revert to the "last" active vector, but this is just
  // not implemented.
  m_active_vertex = false;
  if(m_active_ix == ix) {
    m_active_ix = 0;
    unsigned int i, vsize = m_vmarked.size();
    for(i=0; i<vsize; i++) {
      if(m_vmarked[i]) {
	m_active_vertex = true;
	m_active_ix = i;
      }
    }
  }
  return(changed);
}

//-------------------------------------------------------------------
// Procedure: deleteVector
//   Purpose: Delete the vector at the given index if within range

bool CurrentField::deleteVector(unsigned int ix)
{
  if(ix >= m_vmarked.size())
    return(false);

  vector<XYVector> new_vectors;
  vector<bool>     new_vmarked;

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    if(ix != i) {
      new_vectors.push_back(m_vectors[i]);
      new_vmarked.push_back(m_vmarked[i]);
    }
  }

  m_vectors = new_vectors;
  m_vmarked = new_vmarked;
  return(true);
}

//-------------------------------------------------------------------
// Procedure: isMarked
//   Purpose: 

bool CurrentField::isMarked(unsigned int ix)
{
  if(ix >= m_vmarked.size())
    return(false);
  return(m_vmarked[ix]);
}


//-------------------------------------------------------------------
// Procedure: deleteMarkedVectors

void CurrentField::deleteMarkedVectors()
{
  if(m_vmarked.size() == 0)
    return;
  
  vector<XYVector> new_vectors;
  vector<bool>     new_vmarked;

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    if(!m_vmarked[i]) {
      new_vectors.push_back(m_vectors[i]);
      new_vmarked.push_back(false);
    }
  }

  m_vectors = new_vectors;
  m_vmarked = new_vmarked;
}

//-------------------------------------------------------------------
// Procedure: modVector
//   Purpose: 

void CurrentField::modVector(unsigned int ix, string param, double value)
{
  if(param == "aug_mag")
    m_vectors[ix].augMagnitude(value);
  else if(param == "aug_ang")
    m_vectors[ix].augAngle(value);
  else if(param == "aug_x") 
    m_vectors[ix].shift_horz(value);
  else if(param == "aug_y")
    m_vectors[ix].shift_vert(value);
}

//-------------------------------------------------------------------
// Procedure: modMarkedVectors
//   Purpose: 

void CurrentField::modMarkedVectors(string param, double value)
{
  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    if(m_vmarked[i]) {
      modVector(i, param, value);
    }
  }   
}

//-------------------------------------------------------------------
// Procedure: unmarkAllVectors()
//   Purpose: Mark all vectors as "unmarked"
//   Returns: true if this action results in any a change in any of 
//            the markings, i.e., if any vector was previously marked.

bool CurrentField::unmarkAllVectors()
{
  bool changed = false;
  unsigned int i, vsize = m_vmarked.size();
  for(i=0; i<vsize; i++) {
    if(m_vmarked[i])
      changed = true;
    m_vmarked[i] = false;
  }
  return(changed);
}

//-------------------------------------------------------------------
// Procedure: markAllVectors()
//   Purpose: Mark all vectors as "marked"
//   Returns: true if this action results in any a change in any of the
//            markings, i.e., if any vector was previously unmarked.

bool CurrentField::markAllVectors()
{
  bool changed = false;
  unsigned int i, vsize = m_vmarked.size();
  for(i=0; i<vsize; i++) {
    if(!m_vmarked[i])
      changed = true;
    m_vmarked[i] = true;
  }
  return(changed);
}

//-------------------------------------------------------------------
// Procedure: applySnap

void CurrentField::applySnap(double snapval)
{
  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++)
    m_vectors[i].applySnap(snapval);
}

//-------------------------------------------------------------------
// Procedure: getters
//   Purpose: 

XYVector CurrentField::getVector(unsigned int ix) const
{
  XYVector null_vector;
  if(ix >= m_vectors.size())
    return(null_vector);
  return(m_vectors[ix]);
}

bool CurrentField::getVMarked(unsigned int ix) const
{
  if(ix >= m_vmarked.size())
    return(false);
  return(m_vmarked[ix]);
}

double CurrentField::getXPos(unsigned int ix) const
{
  if(ix >= m_vectors.size())
    return(0);
  return(m_vectors[ix].xpos());
}

double CurrentField::getYPos(unsigned int ix) const
{
  if(ix >= m_vectors.size())
    return(0);
  return(m_vectors[ix].ypos());
}

double CurrentField::getForce(unsigned int ix) const
{
  if(ix >= m_vectors.size())
    return(0);
  return(m_vectors[ix].mag());
}

double CurrentField::getDirection(unsigned int ix) const
{
  if(ix >= m_vectors.size())
    return(0);
  return(m_vectors[ix].ang());
}

bool CurrentField::hasActiveVertex() const
{
  return(m_active_vertex);
}

//-------------------------------------------------------------------
// Procedure: selectVector
//   Purpose: 

unsigned int CurrentField::selectVector(double x, double y, double& dist)
{
  if(m_vectors.size() == 0) {
    dist = -1;
    return(0);
  }

  double xdist = x - m_vectors[0].xpos();
  double ydist = y - m_vectors[0].ypos();
  double closest_dist = hypot(xdist, ydist);
  unsigned int closest_ix = 0;

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    xdist = x - m_vectors[i].xpos();
    ydist = y - m_vectors[i].ypos();
    double dist = hypot(xdist, ydist);
    if(dist < closest_dist) {
      closest_ix = i;
      closest_dist = dist;
    }
  }
  
  dist = closest_dist;
  return(closest_ix);
}


//-------------------------------------------------------------------
// Procedure: getVectors()
//   Purpose: 

vector<XYVector> CurrentField::getVectors()
{
  return(m_vectors);
}


//-------------------------------------------------------------------
// Procedure: getVectorsMarked()
//   Purpose: 

vector<XYVector> CurrentField::getVectorsMarked()
{
  vector<XYVector> rvector;

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    if(m_vmarked[i])
      rvector.push_back(m_vectors[i]);
  }
  return(rvector);
}


//-------------------------------------------------------------------
// Procedure: getVectorsUnMarked()
//   Purpose: 

vector<XYVector> CurrentField::getVectorsUnMarked()
{
  vector<XYVector> rvector;

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    if(!m_vmarked[i])
      rvector.push_back(m_vectors[i]);
  }
  return(rvector);
}


//-------------------------------------------------------------------
// Procedure: getListing()
//   Purpose: 

vector<string> CurrentField::getListing()
{
  vector<string> rvector;
  
  rvector.push_back("FieldName: " + m_field_name);
  rvector.push_back("Radius:" + doubleToStringX(m_radius));
  rvector.push_back("");
  
  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    string str = doubleToStringX(m_vectors[i].xpos(),3);
    str += ",";
    str += doubleToStringX(m_vectors[i].ypos(),3);
    str += ",";
    str += doubleToStringX(m_vectors[i].mag(),3);
    str += ",";
    str += doubleToStringX(m_vectors[i].ang(),2);
    rvector.push_back(str);
  }
  return(rvector);
}

//-------------------------------------------------------------------
// Procedure: handleLine
//   Purpose: 

bool CurrentField::handleLine(string line)
{
  if(line == "")
    return(true);
  if(strBegins(line, "//"))
    return(true);

  line = tolower(line);
  if(strBegins(line, "fieldname: ")) {
    string unused = biteString(line, ':');
    m_field_name = stripBlankEnds(line);
    return(true);
  }

  if(strBegins(line, "radius: ")) {
    string unused = biteString(line, ':');
    line = stripBlankEnds(line);
    if(!isNumber(line))
      return(false);
    double val = atof(line.c_str());
    m_radius = val;
    return(true);
  }
    
  if(strBegins(line, "render_hint: ")) {
    string unused = biteString(line, ':');
    m_render_hints.push_back(stripBlankEnds(line));
    return(true);
  }
    
  XYVector new_vector = string2Vector(line);
  if(new_vector.valid())
    addVector(new_vector);
  else
    return(false);

#if 0
  string xstr = biteString(line, ',');
  string ystr = biteString(line, ',');
  string fstr = biteString(line, ',');
  string dstr = stripBlankEnds(line);
  
  if((xstr=="")||(ystr=="")||(fstr=="")||(dstr==""))
    return(false);

  double xval = atof(xstr.c_str());
  double yval = atof(ystr.c_str());
  double fval = atof(fstr.c_str());
  double dval = atof(dstr.c_str());
  XYVector new_vector(xval, yval, fval, dval);
#endif

  addVector(new_vector);
  return(true);
}

//-------------------------------------------------------------------
// Procedure: applyRenderHints
//   Purpose: Apply all user-specified render hints to all vectors

void CurrentField::applyRenderHints()
{
  unsigned int i, vsize = m_render_hints.size();
  for(i=0; i<vsize; i++) {
    string param = stripBlankEnds(biteString(m_render_hints[i], '='));
    string value = stripBlankEnds(m_render_hints[i]);
    applyRenderHint(param, value);
  }
}


//-------------------------------------------------------------------
// Procedure: applyRenderHint
//   Purpose: Apply this render hint to all vectors. Note the hint is
//            not applied if already set in the individual vector line
//            specified. Individual hints override general hints.

void CurrentField::applyRenderHint(string param, string value)
{
  param = tolower(param);
  double dval = atof(value.c_str());

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    if(param == "vector_edge_color") {
      if(!m_vectors[i].color_set("edge") && isColor(value))
	m_vectors[i].set_color("edge", value);
    }
    if(param == "vector_vertex_color") {
      if(!m_vectors[i].color_set("vertex") && isColor(value))
	m_vectors[i].set_color("vertex", value);
    }
    if(param == "vector_vertex_size") {
      if(!m_vectors[i].vertex_size_set() && (dval >= 0))
	m_vectors[i].set_vertex_size(dval);
    }
    if(param == "vector_edge_width") {
      if(!m_vectors[i].edge_size_set() && (dval >= 0))
	m_vectors[i].set_edge_size(dval);
    }
    if(param == "vector_label_color") {
      if(!m_vectors[i].color_set("label") && isColor(value))
	m_vectors[i].set_color("label", value);
    }
    if(param == "vector_head_size") {
      if(!m_vectors[i].head_size_set())
	m_vectors[i].setHeadSize(dval);
    }
  }
}











