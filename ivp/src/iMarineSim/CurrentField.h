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

class CurrentField
{
public:
  CurrentField();
  ~CurrentField() {};

  bool populate(std::string filename);

  void addVector(double x, double y, double force, double direction);

  void getForce(double x, double y, double& fx, double& fy);

  void setRadius(double radius);

  void print();

  unsigned int size() {return(m_xpos.size());};

  double getXPos(unsigned int);
  double getYPos(unsigned int);
  double getForce(unsigned int);
  double getDirection(unsigned int);

protected:
  std::vector<double> m_xpos;
  std::vector<double> m_ypos;
  std::vector<double> m_force;
  std::vector<double> m_direction;

  std::string m_field_name;
  double      m_radius;
};

#endif 
