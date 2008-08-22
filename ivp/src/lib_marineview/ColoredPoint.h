/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ColoredPoint.h                                       */
/*    DATE: Nov 19th 2004 (In Bedford NH, Friday)                */
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

#ifndef COLORED_POINT_HEADER
#define COLORED_POINT_HEADER

#include <list>

class ColoredPoint
{
 public:
  ColoredPoint() 
    {m_valid=false;};
  ColoredPoint(double x, double y)
    {m_x=x; m_y=y; m_valid=true;};
  
  bool isValid()    {return(m_valid);};  
  void setInvalid() {m_valid = false;};
  void setValid()   {m_valid = true;};

  double  m_x;
  double  m_y;
  bool    m_valid;
};

//typedef std::vector<ColoredPoint> CPVector;
typedef std::list<ColoredPoint> CPList;

#endif 


