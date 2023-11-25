/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

  unsigned int size() const                  {return(m_dname.size());}
  double getVarLow(unsigned int ix) const    {return(m_dlow[ix]);}
  double getVarHigh(unsigned int ix) const   {return(m_dhigh[ix]);}
  double getVarDelta(unsigned int ix) const  {return(m_ddelta[ix]);}
  double getVarLow(const std::string& s) const;    
  double getVarHigh(const std::string& s) const;    
  double getVarDelta(const std::string& s) const;    
  double getTotalPts() const;

  // Return number of points in the domain for a given variable name.
  // If the variable name is unknown, just return zero.
  unsigned int getVarPoints(const std::string& str) const;
  unsigned int getVarPoints(unsigned int ix) const;    

  // For the ith domain index, and j steps into the domain, return
  // the corresponding floating point value.
  bool getVal(unsigned int ix, unsigned int j, double &val) const;

  // A simplified version of getVal where no error is indicated
  // if the domain or index is out of range.
  double getVal(unsigned int ix, unsigned int j) const;

  // For domain given by the varible name, and j steps into the 
  // domain, return the corresponding floating point value.
  bool getVal(const std::string str, unsigned int j, double &val) const;
      
  // Return the variable name of ith element of the domain
  std::string getVarName(unsigned int ix) const;

  // Return next lowest value, if there is one
  double getNextLowerVal(unsigned int ix, double val,
			 int snaptype, bool wrap=false) const;

  // Return next higher value, if there is one
  double getNextHigherVal(unsigned int ix, double val,
			  int snaptype, bool wrap=false) const;

  // Return the discrete index into the domain given by a double
  // input value. Round up, down or closest, depending on snaptype.
  unsigned int getDiscreteVal(unsigned int ix, 
			      double val, int snaptype) const;

  double getSnappedVal(unsigned int ix,
		       double dval, int snaptype) const;

  double getSnappedValCeil(unsigned int ix, double dval) const;
  double getSnappedValFloor(unsigned int ix, double dval) const;
  double getSnappedValProx(unsigned int ix, double dval) const;


  //==============================================================
  

  double coupleAux(unsigned int index, double& gval,
		   bool up, bool wrap) const;

  
  // Return given val if in domain, or next higher value, if there is one
  double getEqOrHigherVal(unsigned int index, double val,
			  int snaptype, bool wrap=false) const;

  // Return given val if in domain, or next lower value, if there is one
  double getEqOrLowerVal(unsigned int index, double val,
			 int snaptype, bool wrap=false) const;


  
private:
  std::vector<std::string>  m_dname;
  std::vector<double>       m_dlow;
  std::vector<double>       m_dhigh;
  std::vector<double>       m_ddelta;
  std::vector<unsigned int> m_dpoints;
};

#endif

