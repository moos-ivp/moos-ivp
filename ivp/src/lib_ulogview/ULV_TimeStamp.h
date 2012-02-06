/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULV_TimeStamp.h                                      */
/*    DATE: Sep 26th, 2011                                       */
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

#ifndef ULV_TIME_STAMP_HEADER
#define ULV_TIME_STAMP_HEADER

#include <string>

class ULV_TimeStamp
{
public:
  ULV_TimeStamp();
  ~ULV_TimeStamp() {};
  
  double        time() const      {return(m_utc_time);};
  std::string   vname() const     {return(m_vname);};
  unsigned int  iter() const      {return(m_helm_iteration);};

  void          setTime(double v)       {m_utc_time=v;};
  void          setVName(std::string s) {m_vname=s;};
  void          setIter(double v)       {m_helm_iteration=v;};

  std::string   getString() const;
  bool          setFromString(const std::string&);

  bool          valid() const {return(m_utc_time > 0);};

protected:
  double        m_utc_time;
  std::string   m_vname;
  unsigned int  m_helm_iteration;
};
#endif 

