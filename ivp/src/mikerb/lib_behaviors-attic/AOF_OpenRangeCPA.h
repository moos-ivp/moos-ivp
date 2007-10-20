/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_OpenRangeCPA.h                                   */
/*    DATE: May 11th 2005                                        */
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
 
#ifndef AOF_OPENRANGE_CPA_HEADER
#define AOF_OPENRANGE_CPA_HEADER

#include "AOF.h"

class IvPDomain;
class AOF_OpenRangeCPA: public AOF {
public:
  AOF_OpenRangeCPA(IvPDomain, double, double, 
		   double, double, double, double);
  ~AOF_OpenRangeCPA() {};

public:    
  double evalBox(const IvPBox*) const;   // virtual defined

public:
  void   setMinFlag(int f) {minFlag = f;};
  void   print() const;
  double getcnLAT() {return(cnLAT);};
  double getcnLON() {return(cnLON);};
  double getcnCRS() {return(cnCRS);};
  double getcnSPD() {return(cnSPD);};
  double getK0()    {return(statK0);};
  
protected:
  void   setStatic();
  int    getBoxCRS(Box *b) {return(b->pt(0,0));};
  int    getBoxSPD(Box *b) {return(b->pt(1,0));};
  int    getBoxTOL(Box *b) {return(b->pt(2,0));};
  double metric(double) const;
  double metric2(double, double) const;

protected:
  double cnLAT;   // Contact Lat position at time Tm.
  double cnLON;   // Contact Lon position at time Tm.
  double cnSPD;   // Contact Speed in kts.
  double cnCRS;   // Contact Course in degrees (0-359).
  double osLAT;   // Ownship Lat position at time Tm.
  double osLON;   // Ownship Lon position at time Tm.

  double statK2;  // Components of k2, k1, k0 that are 
  double statK1;  // static (independent of the values of
  double statK0;  // osCRS, osSPD, osTOL).

  double gamCN;   // cnCRS in radians. 
  double cgamCN;  // Cosine of  cnCRS.
  double sgamCN;  // Sine  of   cnCRS.
  bool   minFlag; // True eval returns CPA.

  double crsDelta;
  double crsBase;
  double spdDelta;
  double spdBase;
  double tolDelta;
  double tolBase;
};

#endif





