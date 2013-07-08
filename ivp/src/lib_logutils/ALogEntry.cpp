/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogEntry.cpp                                        */
/*    DATE: Aug 12th, 2009                                       */
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

#include "ALogEntry.h"

//--------------------------------------------------------
// Procedure: set()

void ALogEntry::set(double timestamp, const std::string& varname, 
		    const std::string& source, 
		    const std::string& srcaux,
		    const std::string& sval)
{
  m_timestamp = timestamp;
  m_varname   = varname;
  m_source    = source;
  m_srcaux    = srcaux;
  m_sval      = sval;
  m_dval      = 0;
  m_isnum     = false;
}
  

//--------------------------------------------------------
// Procedure: set()

void ALogEntry::set(double timestamp, const std::string& varname, 
		    const std::string& source, 
		    const std::string& srcaux,
		    double dval)
{
  m_timestamp = timestamp;
  m_varname   = varname;
  m_source    = source;
  m_srcaux    = srcaux;
  m_sval      = "";
  m_dval      = dval;
  m_isnum     = true;
}


//--------------------------------------------------------
// Procedure: overloaded less than operator

bool operator< (const ALogEntry& one, const ALogEntry& two)
{
  if(one.time() < two.time())
    return(true);
  else
    return(false);
}

//--------------------------------------------------------
// Procedure: overload equals operator

bool operator== (const ALogEntry& one, const ALogEntry& two)
{
  if((one.time() == two.time()) &&
     (one.getVarName() == two.getVarName()) &&
     (one.getSource() == two.getSource())   &&
     (one.getSrcAux() == two.getSrcAux())   &&
     (one.getStringVal() == two.getStringVal()) &&
     (one.getDoubleVal() == two.getDoubleVal()) &&
     (one.isNumerical() == two.isNumerical())   &&
     (one.getRawLine() == two.getRawLine())   &&
     (one.getStatus() == two.getStatus()))
    return(true);
  return(false);
}
