/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CompactorNav.h                                       */
/*    DATE: June 13, 2004                                        */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifndef COMPACTOR_NAV_HEADER
#define COMPACTOR_NAV_HEADER

#include "Compactor.h"
#include "IvPDomain.h"

class CompactorNav: public Compactor {
public:
  CompactorNav(IvPDomain);
  virtual ~CompactorNav() {};

public:
  bool    compact(IvPBox*);
  double  maxVal(IvPBox*, bool*);
  IvPBox* maxPt(const IvPBox*);
  
  int&   ptsHit()      {return(pts_hit);};
  int&   ptsVerified() {return(pts_verified);};
  int&   ptsNewMax()   {return(pts_newmax);};
  int&   maxptCalls()  {return(maxpt_calls);};

protected:
  bool   compactCRS(IvPBox*, bool);
  bool   compactSPD(IvPBox*, bool);
  bool   compactTOL(IvPBox*, bool);
  bool   compactLAT(IvPBox*, int);
  bool   compactLON(IvPBox*, int);
  double minDist(IvPBox*);
  double maxDist(IvPBox*);
  int    midBXCase(int, int, int, int, int, int);


private:
  bool   violation;
  int    pts_hit;
  int    pts_verified;
  int    pts_newmax;
  int    maxpt_calls;
  double crsDelta;
  double spdDelta;
  double tolDelta;
  double crsBase;
  double spdBase;
  double tolBase;


public: // static variables
  static int    p_osLAT;
  static int    p_osLON;
  static double dp_osLAT;
  static double dp_osLON;
  static double nmPerPix;
};
#endif









