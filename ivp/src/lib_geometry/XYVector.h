/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYVector.h                                           */
/*    DATE: October 17th, 2010                                   */
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
 
#ifndef XY_VECTOR_HEADER
#define XY_VECTOR_HEADER

#include <string>
#include "XYObject.h"

class XYVector : public XYObject {
public:
  XYVector();
  XYVector(double x, double y, double mag=0, double ang=0);
  virtual ~XYVector() {}

  void  setPosition(double x, double y);

  void  setVectorXY(double xdot, double ydot);
  void  setVectorMA(double magnitude, double angle);

  void  mergeVectorXY(double xdot, double ydot);
  void  mergeVectorMA(double mag, double ang); 

  void  augMagnitude(double val);
  void  augAngle(double val);

  void  clear();

  void  setHeadSize(double v)  {m_head_size=v;}
  bool  head_size_set() const  {return(m_head_size >= 0);}

public:
  void   applySnap(double);
  void   shift_horz(double val) {m_x += val;}
  void   shift_vert(double val) {m_y += val;}

public:
  double    xpos() const     {return(m_x);}
  double    ypos() const     {return(m_y);}
  double    mag()  const     {return(m_mag);}
  double    ang()  const     {return(m_ang);}
  double    xdot() const     {return(m_xdot);}
  double    ydot() const     {return(m_ydot);}
  bool      valid() const    {return(m_valid);}
  double    headsize() const {return(m_head_size);}

  std::string get_spec(std::string s="") const;

 protected:  // Drawing hint
  double    m_head_size;

protected:
  double    m_x;
  double    m_y;

  // Two representations of the vector are kept in sync. The magnitude
  // of the vector in x,y directions (xdot, ydot), and the direction 
  // and magnitude of the vector (ang, mag). This is done to avoid 
  // repeated conversions on the same vector instance.
  double    m_ang;
  double    m_mag;
  double    m_xdot;
  double    m_ydot;

  // Vector is considered valid once the x,y position is set.
  bool      m_valid;
};

#endif










