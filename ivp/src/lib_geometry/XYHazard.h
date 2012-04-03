/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazard.h                                           */
/*    DATE: Jan 28th, 2012                                       */
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

#ifndef XY_HAZARD_HEADER
#define XY_HAZARD_HEADER

#include <string>

class XYHazard
{
 public:
  XYHazard();
  virtual ~XYHazard() {};

  // Setters
  void    setX(double x)               {m_x=x;  m_x_set=true;};
  void    setY(double y)               {m_y=y;  m_y_set=true;};
  void    setResemblance(double v)     {m_hr=v; m_hr_set=true;};
  void    setLabel(std::string label)  {m_label=label;};
  void    setType(std::string v)       {m_type=v;};
  void    setSource(std::string v)     {m_source=v;};

  bool    setShape(std::string);
  bool    setWidth(std::string);
  bool    setColor(std::string);
  
  // Getters
  double  getX() const               {return(m_x);};
  double  getY() const               {return(m_y);};
  double  getWidth() const           {return(m_width);};
  double  getResemblance() const     {return(m_hr);};

  bool    hasResemblance() const     {return(m_hr_set);};
  bool    valid() const              {return(m_x_set && m_y_set);};

  std::string getLabel() const       {return(m_label);};
  std::string getType() const        {return(m_type);};
  std::string getSource() const      {return(m_source);};
  std::string getShape() const       {return(m_shape);};
  std::string getColor() const       {return(m_color);};
  std::string getSpec(std::string noshow="") const;

 protected: // Key properties
  double       m_x;
  double       m_y;
  double       m_hr;
  bool         m_x_set;
  bool         m_y_set;
  bool         m_hr_set;
  std::string  m_label;
  std::string  m_source;
  std::string  m_type;

 protected: // Drawing hints
  double       m_width;
  std::string  m_shape;
  std::string  m_color;
};

#endif 

