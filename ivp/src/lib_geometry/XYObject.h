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
#include "ColorPack.h"

class XYObject {
public:
  XYObject();
  virtual ~XYObject() {};

  virtual void clear();
  virtual bool valid() const  {return(true);};
  
  void   set_label(const std::string& str)  {m_label=str;};
  void   set_type(const std::string& str)   {m_type=str;};
  void   set_source(const std::string& str) {m_source=str;};
  void   set_msg(const std::string& str)    {m_msg=str;};
  void   set_active(bool val)               {m_active=val;};
  void   set_time(double val)               {m_time=val;m_time_set=true;};
  void   set_vertex_size(double val);
  void   set_edge_size(double val);
  void   set_vertex_color(const std::string&);
  void   set_edge_color(const std::string&);
  void   set_label_color(const std::string&);

  bool        active()        const {return(m_active);};
  double      get_time()      const {return(m_time);};
  bool        time_set()      const {return(m_time_set);};
  std::string get_label()     const {return(m_label);};
  std::string get_msg()       const {return(m_msg);};
  std::string get_type()      const {return(m_type);};
  std::string get_source()    const {return(m_source);};

  ColorPack get_label_color() const  {return(m_label_color);};
  bool      label_color_set() const  {return(m_label_color.set());};
  ColorPack get_vertex_color() const {return(m_vertex_color);};
  bool      vertex_color_set() const {return(m_vertex_color.set());};
  ColorPack get_edge_color()  const  {return(m_edge_color);};
  bool      edge_color_set()  const  {return(m_edge_color.set());};

  double    get_vertex_size() const {return(m_vertex_size);};
  bool      vertex_size_set() const {return(m_vertex_size>=0);};
  double    get_edge_size() const   {return(m_edge_size);};
  bool      edge_size_set() const   {return(m_edge_size>=0);};

  std::string get_spec(std::string s="") const;

  bool set_param(const std::string&, const std::string&);

 protected:
  void aug_spec(std::string&, std::string) const;

protected:
  std::string  m_label;
  std::string  m_type;
  std::string  m_source;
  std::string  m_msg;
  bool         m_active;
  double       m_time;
  bool         m_time_set;

  ColorPack    m_label_color;
  ColorPack    m_vertex_color;
  ColorPack    m_edge_color;
  double       m_vertex_size;
  double       m_edge_size;
};

#endif
