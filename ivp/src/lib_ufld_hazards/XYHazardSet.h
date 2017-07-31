/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazardSet.h                                        */
/*    DATE: Mar 12th, 2012                                       */
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

#ifndef XY_HAZARD_SET_HEADER
#define XY_HAZARD_SET_HEADER

#include <string>
#include <vector>
#include "XYHazard.h"
#include "XYPolygon.h"

class XYHazardSet
{
 public:
  XYHazardSet();
  virtual ~XYHazardSet() {}

  // Setters
  void  addHazard(const XYHazard& hazard);
  bool  setHazard(unsigned int, const XYHazard& hazard);
  bool  setRegion(const XYPolygon& poly) {m_search_region=poly; return(true);}

  void  setSource(std::string s)  {m_source=s;}
  void  setName(std::string s)    {m_name=s;}
  void  setXPath(double v)        {m_xpath=v; m_xpath_set=true;}
  void  setYPath(double v)        {m_ypath=v; m_ypath_set=true;}
  void  clear()                   {m_source=""; m_name=""; m_hazards.clear();}

  // Getters
  int       findHazard(double x, double y) const;
  int       findHazard(const std::string& label) const;
  bool      hasHazard(const std::string& label) const;
  XYHazard  getHazard(unsigned int) const;

  XYPolygon     getRegion() const    {return(m_search_region);}
  std::string   getSource() const    {return(m_source);}
  std::string   getName() const      {return(m_name);}
  double        getXPath() const     {return(m_xpath);}
  double        getYPath() const     {return(m_ypath);}

  bool          isValidLaydown() const;
  bool          isValidReport() const;
  bool          isXPathSet() const       {return(m_xpath_set);}
  bool          isYPathSet() const       {return(m_ypath_set);}

  unsigned int  size() const         {return(m_hazards.size());}
  unsigned int  getHazardCnt() const {return(m_hazard_count);}
  unsigned int  getBenignCnt() const {return(m_benign_count);}

  unsigned int  getHazardCnt(const XYPolygon&) const;

  unsigned int  findMinXPath(double path_width);
  std::string   getSpec(std::string noshow="") const;


 protected: // Key properties
  std::vector<XYHazard>  m_hazards;
  std::string   m_source;
  std::string   m_name;
  XYPolygon     m_search_region;
 
  double        m_xpath;
  double        m_ypath;
  bool          m_xpath_set;
  bool          m_ypath_set;

  unsigned int  m_hazard_count;
  unsigned int  m_benign_count;
};

#endif 




