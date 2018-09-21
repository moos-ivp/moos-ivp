/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYObject.h                                           */
/*    DATE: July 18th, 2008                                      */
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
 
#ifndef XY_OBJECT_HEADER
#define XY_OBJECT_HEADER

#include <string>
#include <map>
#include "ColorPack.h"

class XYObject {
public:
  XYObject();
  virtual ~XYObject() {}

  virtual void   clear();
  virtual bool   valid() const {return(true);}
  
  void   set_label(const std::string& str)  {m_label=str;}
  void   set_source(const std::string& str) {m_source=str;}
  void   set_type(const std::string& str)   {m_type=str;}
  void   set_msg(const std::string& str)    {m_msg=str;}
  void   set_active(bool val)               {m_active=val;}
  void   set_time(double val)               {m_time=val;m_time_set=true;}
  void   set_vertex_size(double val);
  void   set_edge_size(double val);
  void   set_transparency(double);

  void   set_type()   {} // deprecated
  void   set_source() {} // deprecated

  //-----Soon To Be Deprecated/Removed -----------------------------------      
  void     set_vertex_color(const std::string& s) {set_color("vertex", s);}
  void     set_edge_color(const std::string& s)   {set_color("edge", s);}
  void     set_label_color(const std::string& s)  {set_color("label", s);}
  //-----Soon To Be Deprecated/Removed ----------------------------------- 

  bool      color_set(const std::string& key) const;
  void      set_color(const std::string& key, const std::string& color);
  void      set_color(const std::string& key, const ColorPack& color);
  ColorPack get_color(const std::string& key) const;

  bool      active()        const {return(m_active);}
  double    get_time()      const {return(m_time);}
  bool      time_set()      const {return(m_time_set);}

  double    get_vertex_size() const  {return(m_vertex_size);}
  bool      vertex_size_set() const  {return(m_vertex_size>=0);}
  double    get_edge_size() const    {return(m_edge_size);}
  bool      edge_size_set() const    {return(m_edge_size>=0);}
  double    get_transparency() const {return(m_transparency);}
  bool      transparency_set() const {return(m_transparency_set);}

  std::string get_label()     const {return(m_label);}
  std::string get_msg()       const {return(m_msg);}
  std::string get_type()      const {return(m_type);}
  std::string get_source()    const {return(m_source);}
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
  bool         m_transparency_set;

  std::map<std::string, ColorPack> m_color_map;

  double       m_vertex_size;
  double       m_edge_size;
  double       m_transparency;
};

#endif









