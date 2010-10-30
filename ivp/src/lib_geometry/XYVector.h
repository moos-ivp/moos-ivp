/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYVector.h                                           */
/*    DATE: October 17th, 2010                                   */
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
 
#ifndef XY_VECTOR_HEADER
#define XY_VECTOR_HEADER

#include <string>
#include "XYObject.h"

class XYVector : public XYObject {
public:
  XYVector();
  XYVector(double x, double y, double mag, double ang);
  virtual ~XYVector() {};

  void setPosition(double x, double y)  {m_x=x; m_y=y;};

  void setVectorXY(double xdot, double ydot);
  void setVectorMA(double magnitude, double angle);

  void clear();
  
public:
  void   shift_horz(double val) {m_x += val;}
  void   shift_vert(double val) {m_y += val;}
  void   print() const;

public:
  double    xpos() const   {return(m_x);}
  double    ypos() const   {return(m_y);}
  double    mag()  const   {return(m_mag);};
  double    ang()  const   {return(m_ang);};
  double    xdot() const   {return(m_xdot);};
  double    ydot() const   {return(m_ydot);};


  std::string get_spec(std::string s="") const;

protected:
  double    m_x;
  double    m_y;
  double    m_ang;
  double    m_mag;
  double    m_xdot;
  double    m_ydot;
};

#endif
