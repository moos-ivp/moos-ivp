/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Ring.h                                           */
/*    DATE: Very old - modernized Jan 21 2006                    */
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
 
#ifndef AOF_NRING_HEADER
#define AOF_NRING_HEADER

#include <string>
#include "AOF.h"
#include "IvPDomain.h"

class AOF_Ring: public AOF {
public:
  AOF_Ring(IvPDomain);
  virtual ~AOF_Ring() {};

public: // virtuals defined
  double evalBox(const IvPBox *b) const;
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);

public:
  void    print() const;

  IvPBox  getLOC() const    {return(location);};
  bool    getRPK() const    {return(ringPeak);};
  double  getRNG() const    {return(range);};
  double  getBAS() const    {return(base);};

  unsigned int getRAD() const {return(radius);};
  unsigned int getEXP() const {return(exponent);};

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

  IvPBox  location;
  bool    ringPeak;  // 1 if pts on ring are peaks (vs. valleys)
  double  base;
  double  range;
  double  maxDist;
  double  plateau;
  double  gradient_dist;
};

#endif









