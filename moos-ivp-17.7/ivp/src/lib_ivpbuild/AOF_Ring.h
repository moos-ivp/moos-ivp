/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Ring.h                                           */
/*    DATE: Very old - modernized Jan 21 2006                    */
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
 
#ifndef AOF_NRING_HEADER
#define AOF_NRING_HEADER

#include <string>
#include "AOF.h"
#include "IvPDomain.h"

class AOF_Ring: public AOF {
public:
  AOF_Ring(IvPDomain);
  virtual ~AOF_Ring() {}

public: // virtuals defined
  double evalBox(const IvPBox *b) const;
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);

public:
  void    print() const;

  IvPBox  getLOC() const    {return(m_location);}
  bool    getRPK() const    {return(m_ring_peak);}
  double  getRNG() const    {return(m_range);}
  double  getBAS() const    {return(m_base);}

  unsigned int getRAD() const {return(radius);}
  unsigned int getEXP() const {return(exponent);}

  std::string latexSTR(int full=0) const;

protected:
  double  boxDist(const IvPBox&, const IvPBox&) const;
  bool    randomize();

private:
  unsigned int  radius;
  unsigned int  radLow;
  unsigned int  radHigh;
  unsigned int  exponent;
  unsigned int  expLow;
  unsigned int  expHigh;
  unsigned int  dim;
  unsigned int  gradient_type;

  IvPBox  m_location;
  bool    m_ring_peak;  // 1 if pts on ring are peaks (vs. valleys)
  double  m_base;
  double  m_range;
  double  plateau;
  double  gradient_dist;

  // double  maxDist;
};

#endif

















