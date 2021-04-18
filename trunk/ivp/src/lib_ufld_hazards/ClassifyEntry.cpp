/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ClassifyEntry.cpp                                    */
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

#include "ClassifyEntry.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

ClassifyEntry::ClassifyEntry()
{
  m_time_requested  = 0;
  m_priority        = 0;
  m_aspect_affected = false;
}


// Overload the < operator
bool operator< (const ClassifyEntry& entry1, const ClassifyEntry& entry2)
{
  return(entry1.getPriority() > entry2.getPriority());
}

// Overload the > operator
bool operator> (const ClassifyEntry& entry1, const ClassifyEntry& entry2)
{
  return(entry1.getPriority() < entry2.getPriority());
}







