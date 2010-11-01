/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VState.cpp                                           */
/*    DATE: Oct 25th 2004                                        */
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "CurrentField.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "FileBuffer.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

CurrentField::CurrentField()
{
  reset();
}

//-------------------------------------------------------------------
// Procedure: reset
//   Purpose: 

void CurrentField::reset()
{
  m_field_name = "generic_cfield";
  m_radius = 20;

  m_vectors.clear();
}

//-------------------------------------------------------------------
// Procedure: populate
//   Purpose: 

bool CurrentField::populate(string filename)
{
  cout << "Populating Current Field: " << filename << endl;

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
  cout << "Done Populating Current Field." << endl;
  cout << "  OK Entries: " << lines_ok << endl;
  cout << "  Bad Entries: " << lines_bad << endl;

  return(true);
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

  addVector(new_vector);
  return(true);
}


//-------------------------------------------------------------------
// Procedure: addVector
//   Purpose: 

void CurrentField::addVector(const XYVector& new_vector)
{
  m_vectors.push_back(new_vector);
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
// Procedure: 
//   Purpose: 

void CurrentField::print()
{
  cout << "Current Field [" << m_field_name << "]:" << endl;
  cout << "Total Entries: " << size() << endl;
  cout << "Radius:" << m_radius << endl;

  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    m_vectors[i].print();
  }
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









