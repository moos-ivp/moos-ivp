/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
// Procedure: getSummary()

string ALogEntry::getSummary() const
{
  string summary;

  summary += doubleToString(m_timestamp) + "  ";
  summary += padString(m_varname, 20, false) + "  ";

  summary += "(" + m_node.substr(0,3) + ")";
  summary += padString(m_source, 25, false) + "  ";
  if(m_isnum)
    summary += doubleToStringX(m_dval);
  else
    summary += m_sval;

  return(summary);
}


//--------------------------------------------------------
// Procedure: tokenField

bool ALogEntry::tokenField(const string& field, double& value) const
{
  return(tokParse(m_sval, field, ',', '=', value));
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
     (one.getNode() == two.getNode()) &&
     (one.getDoubleVal() == two.getDoubleVal()) &&
     (one.isNumerical() == two.isNumerical())   &&
     (one.getRawLine() == two.getRawLine())   &&
     (one.getStatus() == two.getStatus()))
    return(true);
  return(false);
}

//--------------------------------------------------------
// Procedure: overload not-equals operator

bool operator!= (const ALogEntry& one, const ALogEntry& two)
{
  if(one == two)
    return(false);

  return(true);
}

