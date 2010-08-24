/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF.h                                                */
/*    DATE:                                                      */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef AOF_HEADER
#define AOF_HEADER

#include <vector>
#include <string>
#include "IvPBox.h"
#include "IvPDomain.h"
class AOF{
public:
  AOF(const IvPDomain& dom) {m_domain=dom;};
  virtual ~AOF() {};

  // Virtual function
  virtual double evalBox(const IvPBox*) const {return(0);};
  
  virtual double evalPoint(const std::vector<double>&) const
    {return(0);};
  
  // Virtual functions
  virtual bool  setParam(const std::string&, double) 
    {return(false);};
  virtual bool  setParam(const std::string&, const std::string&) 
    {return(false);};
  virtual bool  initialize() 
    {return(true);};

  double extract(const std::string& var, const IvPBox* pbox) const {
    int index = m_domain.getIndex(var);
    if(index == -1)
      return(0);
    return(m_domain.getVal(index, pbox->pt(index)));
  }

  double extract(const std::string& varname, 
		 const std::vector<double>& point) const {
    int index = m_domain.getIndex(varname);
    if((index == -1) || ((unsigned int)(index) >= point.size()))
      return(0);
    return(point[index]);
  }

  IvPDomain getDomain() const {return(m_domain);};

  int  getDim() const   {return(m_domain.size());};

protected:
  IvPDomain m_domain;
};
#endif










