/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ColoredPoint.h                                       */
/*    DATE: Nov 19th 2004 (In Bedford NH, Friday)                */
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

#ifndef COLORED_POINT_HEADER
#define COLORED_POINT_HEADER

#include <list>

class ColoredPoint
{
 public:
  ColoredPoint() 
    {m_valid=false;}
  ColoredPoint(double x, double y)
    {m_x=x; m_y=y; m_valid=true;}
  
  bool isValid()    {return(m_valid);}  
  void setInvalid() {m_valid = false;}
  void setValid()   {m_valid = true;}

  double  m_x;
  double  m_y;
  bool    m_valid;
};

typedef std::list<ColoredPoint> CPList;

#endif 











