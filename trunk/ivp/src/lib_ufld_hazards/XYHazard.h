/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazard.h                                           */
/*    DATE: Jan 28th, 2012                                       */
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

#ifndef XY_HAZARD_HEADER
#define XY_HAZARD_HEADER

#include <string>
#include <map>

class XYHazard
{
 public:
  XYHazard();
  virtual ~XYHazard() {}

  // Setters
  void    setX(double x)               {m_x=x;  m_x_set=true;}
  void    setY(double y)               {m_y=y;  m_y_set=true;}
  void    setResemblance(double v)     {m_hr=v; m_hr_set=true;}
  void    setLabel(std::string str)    {m_label=str;}
  void    setType(std::string str)     {m_type=str;}
  void    setSource(std::string str)   {m_source=str;}

  bool    setAspect(double);
  bool    setAspectRange(double, double);
  bool    setAspectRangeMin(double);
  bool    setAspectRangeMax(double);

  bool    setShape(std::string);
  bool    setWidth(std::string);
  bool    setColor(std::string);
  
  void    incCounter(unsigned int amt=1, std::string s="generic");
  void    setCounter(unsigned int, std::string s="generic");

  // Getters
  double  getX() const               {return(m_x);}
  double  getY() const               {return(m_y);}
  double  getWidth() const           {return(m_width);}
  double  getResemblance() const     {return(m_hr);}
  double  getAspect() const          {return(m_aspect);}
  double  getAspectRangeMin() const  {return(m_aspect_rng_min);}
  double  getAspectRangeMax() const  {return(m_aspect_rng_max);}

  bool    hasResemblance() const     {return(m_hr_set);}
  bool    hasAspect() const          {return(m_aspect_set);}
  bool    hasAspectRangeMin() const  {return(m_aspect_rng_min_set);}
  bool    hasAspectRangeMax() const  {return(m_aspect_rng_max_set);}
  bool    valid() const              {return(m_x_set && m_y_set);}

  std::string getLabel() const       {return(m_label);}
  std::string getType() const        {return(m_type);}
  std::string getSource() const      {return(m_source);}
  std::string getShape() const       {return(m_shape);}
  std::string getColor() const       {return(m_color);}

  bool    isSetX() const    {return(m_x_set);}
  bool    isSetY() const    {return(m_y_set);}
  bool    isSetHR() const   {return(m_hr_set);}
  bool    isSetType() const {return(m_type != "");}

  unsigned int getCounter(std::string s="generic");

  std::string getSpec(std::string noshow="") const;

 protected: // Key properties
  double       m_x;
  double       m_y;
  double       m_hr;
  double       m_aspect;
  double       m_aspect_rng_min;
  double       m_aspect_rng_max;
  
  bool         m_x_set;
  bool         m_y_set;
  bool         m_hr_set;
  bool         m_aspect_set;
  bool         m_aspect_rng_min_set;
  bool         m_aspect_rng_max_set;

  std::map<std::string, unsigned int> m_counters;

  std::string  m_label;
  std::string  m_source;
  std::string  m_type;

 protected: // Drawing hints
  double       m_width;
  std::string  m_shape;
  std::string  m_color;
};

#endif 








