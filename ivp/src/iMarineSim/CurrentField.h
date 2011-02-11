/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VState.h                                             */
/*    DATE: Oct 20th 2010                                        */
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

#ifndef CURRENT_FIELD_HEADER
#define CURRENT_FIELD_HEADER

#include <string>
#include <vector>
#include "XYVector.h"

class CurrentField
{
public:
  CurrentField();
  ~CurrentField() {};

  void reset();
  bool populate(std::string filename);
  void addVector(const XYVector&);
  void getLocalForce(double x, double y, double& fx, double& fy) const;
  void setRadius(double radius);
  void print();

  XYVector getVector(unsigned int) const;
  double   getXPos(unsigned int) const;
  double   getYPos(unsigned int) const;
  double   getForce(unsigned int) const;
  double   getDirection(unsigned int) const;

  unsigned int size()     {return(m_vectors.size());};
  std::string getName()   {return(m_field_name);};
  double getRadius()      {return(m_radius);};

 protected:
  bool   handleLine(std::string);

protected:
  std::vector<XYVector> m_vectors;
  std::string           m_field_name;
  double                m_radius;
};

#endif 
