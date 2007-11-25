/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPAEngine.h                                          */
/*    DATE: May 12th 2005                                        */
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
 
#ifndef CPA_ENGINE_HEADER
#define CPA_ENGINE_HEADER

class IvPDomain;
class CPAEngine {
public:
  CPAEngine(double, double, double, double, double, double);
  ~CPAEngine() {};

public:    
  double evalCPA(double, double, double, double* calc_roc=0);
  double evalROC(double, double);
  bool   crossesBow(double, double);

  double minMaxROC(double, double, double&, double&);

  double getcnLAT() {return(cnLAT);};
  double getcnLON() {return(cnLON);};
  double getcnCRS() {return(cnCRS);};
  double getcnSPD() {return(cnSPD);};
  double getK0()    {return(statK0);};
  
protected:
  void   setStatic();
  double smallAngle(double, double);

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

  double statCLOW;  // Course range in which OS is able 
  double statCHGH;  // to cross the path of the contact.
  double statCRNG;  // Range between CLOW and CHGH;
  double statCNANG; // Angle from ownship to the contact.
  double statCNDIS; // Distance from ownship to the contact.

  double gamCN;   // cnCRS in radians. 
  double cgamCN;  // Cosine of  cnCRS.
  double sgamCN;  // Sine  of   cnCRS.
};

#endif










