/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYMarker.h                                           */
/*    DATE: May 12th, 2011                                       */
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
 
#ifndef XY_MARKER_HEADER
#define XY_MARKER_HEADER

#include "XYObject.h"
#include "ColorPack.h"

class XYMarker : public XYObject {
public:
  XYMarker();
  virtual ~XYMarker() {}

  // Setters
  bool    set_type(const std::string&);
  void    set_vx(double v)       {m_x=v;}
  void    set_vy(double v)       {m_y=v;}
  void    set_width(double v);
 
  // Getters
  double      get_vx() const     {return(m_x);}
  double      get_vy() const     {return(m_y);}
  double      get_width() const  {return(m_width);}
  std::string get_type() const   {return(m_type);}

  std::string get_spec(std::string s="") const;

protected:
  double       m_x;
  double       m_y;
  double       m_width;
  std::string  m_type;
};

#endif





