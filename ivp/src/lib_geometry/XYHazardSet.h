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
  XYHazardSet() {};
  virtual ~XYHazardSet() {};

  // Setters
  void    setSource(std::string s)         {m_source=s;};
  void    addHazard(XYHazard hazard);
  void    addHazard(double x, double y, 
		    std::string type="", 
		    std::string label="");

  XYHazard  findHazardMatch(XYHazard, double dist=1);

  XYHazard  findHazardByPosition(double x, double y);
  XYHazard  findHazardByLabel(std::string label);

  // Getters
  XYHazard      getHazard(unsigned int);

  unsigned int  size() const         {return(m_hazards.size());};
  std::string   getSource() const    {return(m_source);};
  std::string   getSpec() const;

 protected: // Key properties
  std::vector<XYHazard>  m_hazards;
  std::string            m_source;
};

#endif 

