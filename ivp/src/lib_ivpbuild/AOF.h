/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF.cpp                                              */
/*    DATE:                                                      */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef AOF_HEADER
#define AOF_HEADER

#include <vector>
#include <list>
#include <string>
#include "IvPBox.h"
#include "IvPDomain.h"
class AOF{
public:
  AOF(const IvPDomain& dom) {m_domain=dom;}
  virtual ~AOF() {}

  // Virtual functions
  virtual double evalBox(const IvPBox*) const {return(0);}
  virtual double evalBoxDebug(const IvPBox*, std::vector<std::string>&) const
  {return(0);}

  virtual double evalPoint(const std::vector<double>&) const {return(0);}
  virtual bool  initialize() {return(true);}
  virtual bool  setParam(const std::string&, double) {return(false);}
  virtual bool  setParam(const std::string&, const std::string&) 
  {return(false);}

  virtual double evalBox(const IvPBox*, 
			 std::vector<std::string>&, 
			 std::vector<std::string>&) {return(0);}

#if 0
  bool setParams(const std::string& s1, double v1, const std::string& s2, double v2)
  {
    return(setParam(s1,v1) && setParam(s2,v2));
  }
#endif

  double extract(const std::string& var, const IvPBox* pbox) const;
  double extract(const std::string& varname, 
		 const std::vector<double>& point) const;

  IvPDomain getDomain() const {return(m_domain);}

  int  getDim() const   {return(m_domain.size());}

protected:
  IvPDomain m_domain;
};
#endif
