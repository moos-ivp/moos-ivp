/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WrapDetector.h                                       */
/*    DATE: Jan 9th, 2023                                        */
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
 
#ifndef WRAP_DETECTOR_HEADER
#define WRAP_DETECTOR_HEADER

#include <list>
#include <string>
#include "XYSegment.h"

class WrapDetector {
public:
  WrapDetector();
  virtual ~WrapDetector() {}

  void updatePosition(double osx, double osy);
  void clear();
  void reset() {clear();}
  
  void setMinLeg(double);
  void setMaxLegs(unsigned int); 
  
  unsigned int getWraps() const {return(m_wraps);}
  
 protected: // State vars
  std::list<XYSegment> m_os_legs;
  
  double m_osx;  
  double m_osy;  
  bool   m_empty;

  unsigned int m_wraps;

 protected: // Config vars
  double m_min_leg;
  double m_max_legs;  
};

#endif














