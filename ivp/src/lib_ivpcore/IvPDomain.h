/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPDomain.h                                          */
/*    DATE: May 29 2004 At Indigo cafe in Montreal               */
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

#ifndef IVPDOMAIN_HEADER
#define IVPDOMAIN_HEADER

#include <string>
#include <vector>
#include "IvPBox.h"

class IvPDomain {
public:
  IvPDomain() {}
  ~IvPDomain() {}

 public: bool operator==(const IvPDomain&) const;

public:
  bool   addDomain(const std::string&, double, double, unsigned int);
  bool   addDomain(const IvPDomain&, const std::string&);
  bool   hasDomain(const std::string&) const;
  bool   hasOnlyDomain(const std::string&, const std::string& s="") const;
  void   clear();
  void   print() const;
  int    getIndex(const std::string&) const;

  unsigned int size() const                 {return(m_dname.size());}
  double getVarLow(unsigned int i) const    {return(m_dlow[i]);}
  double getVarHigh(unsigned int i) const   {return(m_dhigh[i]);}
  double getVarDelta(unsigned int i) const  {return(m_ddelta[i]);}
  double getVarLow(const std::string& s) const;    
  double getVarHigh(const std::string& s) const;    
  double getVarDelta(const std::string& s) const;    
  double getTotalPts() const;

  // Return number of points in the domain for a given variable name.
  // If the variable name is unknown, just return zero.
  unsigned int getVarPoints(const std::string& str) const;
  unsigned int getVarPoints(unsigned int i) const;    

  // For the ith domain index, and j steps into the domain, return
  // the corresponding floating point value.
  bool getVal(unsigned int i, unsigned int j, double &val) const
    {
      unsigned int dsize = m_dlow.size();
      if((i<dsize) && (j<m_dpoints[i])) {
	val = m_dlow[i] + (m_ddelta[i] * j);
	return(true);
      }      return(false);
    };

  // A simplified version of getVal where no error is indicate
  // if the domain or index is out of range.
  double getVal(unsigned int d, unsigned int j) const
    {
      unsigned int dsize = m_dlow.size();
      if((d<dsize) && (j<m_dpoints[d]))
	return(m_dlow[d] + (m_ddelta[d] * j));
      return(0);
    }

  // For domain given by the varible name, and j steps into the 
  // domain, return the corresponding floating point value.
  bool getVal(const std::string str, unsigned int j, double &val) const
    {
      return(getVal(getIndex(str), j, val));
    }
      
  // Return the variable name of ith element of the domain
  std::string getVarName(unsigned int i) const
    {
      if(i>=m_dname.size())
	return("");
      return(m_dname[i]);
    }

  // Return next lowest value, if there is one
  double getNextLowerVal(unsigned int index, double val,
			 int snaptype, bool wrap=false) const;

  // Return next higher value, if there is one
  double getNextHigherVal(unsigned int index, double val,
			  int snaptype, bool wrap=false) const;

  // Return the discrete index into the domain given by a double
  // input value. Round up, down or closest, depending on snaptype.
  unsigned int getDiscreteVal(unsigned int index, 
			      double val, int snaptype) const;

  double getSnappedVal(unsigned int index,
		       double dval, int snaptype) const;
  
private:
  std::vector<std::string>  m_dname;
  std::vector<double>       m_dlow;
  std::vector<double>       m_dhigh;
  std::vector<double>       m_ddelta;
  std::vector<unsigned int> m_dpoints;
};

#endif










