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
  m_field_name = "generic_cfield";
  m_radius = 20;
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
    if((line != "") && (!strBegins(line, "//"))) {
      string xstr = biteString(line, ',');
      string ystr = biteString(line, ',');
      string fstr = biteString(line, ',');
      string dstr = stripBlankEnds(line);
      
      if((xstr=="")||(ystr=="")||(fstr=="")||(dstr=="")) {
	lines_bad++;
	cout << "Problem with line " << i << endl;
	cout << "  [" << line << "]" << endl;
      }
      else {
	lines_ok++;
	double xval = atof(xstr.c_str());
	double yval = atof(ystr.c_str());
	double fval = atof(fstr.c_str());
	double dval = atof(dstr.c_str());
	addVector(xval, yval, fval, dval);
      }
    }
  }
  cout << "Done Populating Current Field." << endl;
  cout << "  OK Entries: " << lines_ok << endl;
  cout << "  Bad Entries: " << lines_bad << endl;

  return(true);
}

//-------------------------------------------------------------------
// Procedure: addVector
//   Purpose: 

void CurrentField::addVector(double x, double y, double f, double dir)
{
  m_xpos.push_back(x);
  m_ypos.push_back(y);
  m_force.push_back(f);
  m_direction.push_back(angle360(dir));
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

  unsigned int i, vsize = m_xpos.size();
  for(i=0; i<vsize; i++) {
    cout << "[" << i << "]: ";
    cout << m_xpos[i] << "," << m_ypos[i];
    cout << " Force=" << m_force[i];
    cout << ", Dir=" << m_direction[i] << endl;
  }
}

//-------------------------------------------------------------------
// Procedure: getters
//   Purpose: 

double CurrentField::getXPos(unsigned int ix)
{
  if(ix >= m_xpos.size())
    return(0);
  return(m_xpos[ix]);
}

double CurrentField::getYPos(unsigned int ix)
{
  if(ix >= m_ypos.size())
    return(0);
  return(m_ypos[ix]);
}

double CurrentField::getForce(unsigned int ix)
{
  if(ix >= m_force.size())
    return(0);
  return(m_force[ix]);
}

double CurrentField::getDirection(unsigned int ix)
{
  if(ix >= m_direction.size())
    return(0);
  return(m_direction[ix]);
}

//-------------------------------------------------------------------
// Procedure: getForce
//   Purpose: 

void CurrentField::getForce(double x, double y, 
			    double& return_force_x, 
			    double& return_force_y)
{
  double total_force_x = 0;
  double total_force_y = 0;
  unsigned int count = 0;

  unsigned int i, vsize = m_xpos.size();
  for(i=0; i<vsize; i++) {
    double dist = distPointToPoint(x, y, m_xpos[i], m_ypos[i]);
    if(dist < m_radius) {
      count++;

      double pct = dist / m_radius;
      pct = pct * pct;
      
      double ang = m_direction[i];
      double mag = m_force[i];
      double rads = headingToRadians(ang);
      
      double force_x = cos(rads) * mag;
      double force_y = sin(rads) * mag;
      
      total_force_x += force_x;
      total_force_y += force_y;
    }
  }
  return_force_x = total_force_x / (double)(count); 
  return_force_y = total_force_y / (double)(count); 
}









