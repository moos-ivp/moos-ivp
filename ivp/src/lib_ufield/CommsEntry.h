/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CommsEntry.h                                         */
/*    DATE: Aug 30th 2022                                        */
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

#ifndef COMMS_ENTRY_HEADER
#define COMMS_ENTRY_HEADER

#include <string>

class CommsEntry
{
 public:
  CommsEntry(double x=0, double y=0, double utc=0, unsigned int id=0);
  ~CommsEntry() {}

  void setX(double x)            {m_x=x;}
  void setY(double y)            {m_y=y;}
  void setXY(double x, double y) {m_x=x; m_y=y;}
  void setUTC(double utc)        {m_utc=utc;}
  void setID(unsigned int id)    {m_id=id;}

  double getX() const        {return(m_x);}
  double getY() const        {return(m_y);}
  double getUTC() const      {return(m_utc);}
  unsigned int getID() const {return(m_id);}

  std::string getSpec() const;
  
 protected:

  double m_x;
  double m_y;
  double m_utc;
  unsigned int m_id;    
};

CommsEntry stringToCommsEntry(std::string);

#endif 










