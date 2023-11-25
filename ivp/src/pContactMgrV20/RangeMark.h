/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RangeMark.h                                          */
/*    DATE: July 19th, 2020                                      */
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

#ifndef RANGE_MARK_HEADER
#define RANGE_MARK_HEADER

#include <string>

class RangeMark
{
 public:
  RangeMark() {m_range=0;}
  RangeMark(std::string, double);
  virtual ~RangeMark() {}

 public: // Setters
  void setRangeMark(std::string, double);

 public: // Getters
  double      getRange()   const {return(m_range);}
  std::string getContact() const {return(m_contact);}

 private:
  double      m_range;
  std::string m_contact;
};

bool operator< (const RangeMark& one, const RangeMark& two);
bool operator== (const RangeMark& one, const RangeMark& two);

#endif 

