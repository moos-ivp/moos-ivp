/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYObject.h                                           */
/*    DATE: July 18th, 2008                                      */
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
 
#ifndef XY_OBJECT_HEADER
#define XY_OBJECT_HEADER

#include <string>
#include "ColorParse.h"

class XYObject {
public:
  XYObject();
  virtual ~XYObject() {};

  virtual void clear();
  virtual bool valid() const  {return(true);};
  
  void   set_label(std::string str)     {m_label=str;};
  void   set_type(std::string str)      {m_type=str;};
  void   set_source(std::string str)    {m_source=str;};
  void   set_string(std::string str)    {m_string=str;};
  void   set_active(bool val)           {m_active=val;};
  void   set_time(double val)           {m_time=val;};
  void   set_label_color(const std::string&);

  bool        active()           const {return(m_active);};
  double      get_time()         const {return(m_time);};
  std::string get_label()        const {return(m_label);};
  std::string get_type()         const {return(m_type);};
  std::string get_source()       const {return(m_source);};
  std::string get_string()       const {return(m_string);};

  std::vector<double> get_label_color_v() const 
    {return(m_label_color_v);};

  std::string get_label_color_s() const 
    {return(m_label_color_s);};

protected:
  std::string  m_label;
  std::string  m_type;
  std::string  m_source;
  std::string  m_string;
  bool         m_active;
  double       m_time;

  std::vector<double> m_label_color_v;
  std::string         m_label_color_s;

};

#endif









