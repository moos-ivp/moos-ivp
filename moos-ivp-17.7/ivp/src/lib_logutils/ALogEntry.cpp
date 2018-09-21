/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogEntry.cpp                                        */
/*    DATE: Aug 12th, 2009                                       */
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

#include "ALogEntry.h"
#include "MBUtils.h"

using namespace std;

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


//--------------------------------------------------------
// Procedure: tokenField

bool ALogEntry::tokenField(const string& field, double& value) const
{
  return(tokParse(m_sval, field, ',', '=', value));
}







