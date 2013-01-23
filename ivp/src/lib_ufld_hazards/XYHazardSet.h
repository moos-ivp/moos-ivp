/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazardSet.h                                        */
/*    DATE: Mar 12th, 2012                                       */
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

#ifndef XY_HAZARD_SET_HEADER
#define XY_HAZARD_SET_HEADER

#include <string>
#include <vector>
#include "XYHazard.h"

class XYHazardSet
{
 public:
  XYHazardSet();
  virtual ~XYHazardSet() {};

  // Setters
  void  addHazard(const XYHazard& hazard);
  bool  setHazard(unsigned int, const XYHazard& hazard);

  void  setSource(std::string s)  {m_source=s;};
  void  setName(std::string s)    {m_name=s;};
  void  clear()                   {m_source=""; m_name=""; m_hazards.clear();};

  // Getters
  int      findHazard(double x, double y) const;
  int      findHazard(const std::string& label) const;
  bool     hasHazard(const std::string& label) const;

  XYHazard getHazard(unsigned int) const;

  bool     isValidLaydown() const;
  bool     isValidReport() const;

  unsigned int  size() const         {return(m_hazards.size());};
  unsigned int  getHazardCnt() const {return(m_hazard_count);};
  unsigned int  getBenignCnt() const {return(m_benign_count);};

  std::string   getSource() const    {return(m_source);};
  std::string   getName() const    {return(m_name);};
  std::string   getSpec(std::string noshow="") const;

 protected: // Key properties
  std::vector<XYHazard>  m_hazards;
  std::string            m_source;
  std::string            m_name;

  unsigned int  m_hazard_count;
  unsigned int  m_benign_count;
};

#endif 


