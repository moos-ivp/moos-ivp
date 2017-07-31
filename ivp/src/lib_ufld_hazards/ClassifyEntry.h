/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ClassifyEntry.h                                      */
/*    DATE: March 23rd, 2013                                     */
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

#ifndef UHZ_CLASSIFY_ENTRY
#define UHZ_CLASSIFY_ENTRY

#include "XYHazard.h"

class ClassifyEntry {
 public:
  ClassifyEntry();
  virtual ~ClassifyEntry() {}

 public: // Setters
  void setHazard(XYHazard hazard) {m_post_hazard=hazard;}
  void setTimeReq(double v)       {m_time_requested=v;}
  void setPriority(double v)      {m_priority=v;}
  void setAspectAffected(bool v)  {m_aspect_affected=v;}
    
 public: // Getters
  XYHazard getHazard() const         {return(m_post_hazard);}
  double   getTimeReq() const        {return(m_time_requested);}
  double   getPriority() const       {return(m_priority);}
  bool     getAspectAffected() const {return(m_aspect_affected);}

 protected: // Member variables
  double      m_time_requested;
  double      m_priority;
  XYHazard    m_post_hazard; // type is perceived, not actual
  bool        m_aspect_affected;
};

// Overload the < operator 
bool operator< (const ClassifyEntry& entry1, const ClassifyEntry& entry2);
bool operator> (const ClassifyEntry& entry1, const ClassifyEntry& entry2);

#endif 







