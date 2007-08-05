/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_WPT1D.h                                          */
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
 
#ifndef AOF_WPT1D_HEADER
#define AOF_WPT1D_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_WPT1D: public AOF {
 public:
  AOF_WPT1D(IvPDomain, double, double, double, double);
  ~AOF_WPT1D() {};

public:    
  double evalBox(const IvPBox*) const;   // virtual defined

protected:
  double metric(double) const;

protected:
  double osLAT;   // Ownship Lat position at time Tm.
  double osLON;   // Ownship Lon position at time Tm.
  double ptLAT;   
  double ptLON;   

  double crsBase;
  double crsDelta;
};

#endif





