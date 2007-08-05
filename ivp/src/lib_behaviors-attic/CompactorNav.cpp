/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CompactorNav.cpp (originally NBox.C)                 */
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

#include <math.h>
#include "IvPBox.h"
#include "BuildUtils.h"
#include "AngleUtils.h"
#include "CompactorNav.h"
#include "IvPDomain.h"

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

int    CompactorNav::p_osLAT  = 0;
int    CompactorNav::p_osLON  = 0;
double CompactorNav::dp_osLAT = 0.0;
double CompactorNav::dp_osLON = 0.0;
double CompactorNav::nmPerPix = 0.0;

//-------------------------------------------------------------
// Procedure: Constructor

CompactorNav::CompactorNav(IvPDomain gdomain)
{
  violation = false;
  pts_hit = 0;
  pts_verified = 0;
  pts_newmax = 0;
  maxpt_calls = 0;
}

//------------------------------------------------------
// Procedure: maxVal
//            okFlag = false: Box Empty --> Error.
//            okFlag = true:  Exact value returned;
//

double CompactorNav::maxVal(IvPBox *gbox, bool *okFlag)
{
  if(gbox->isPtBox()) 
    return(gbox->maxVal());

  IvPBox *bestPT = maxPt(gbox);
  if(bestPT) {
    *okFlag = true;
    double return_val = bestPT->maxVal();
    delete(bestPT);
    return(return_val);
  }
  else {
    *okFlag = false;
    return(0.0);
  }    
}


//---------------------------------------------------------------
// Procedure: maxPt
//   Returns:

IvPBox *CompactorNav::maxPt(const IvPBox *gbox)
{
  maxpt_calls++;
  
  double currVal, bestVal;
  double radCourse, newLon, newLat;
  int    dim = gbox->getDim();
  IvPBox* currPT = new IvPBox(dim);
  IvPBox* bestPT = 0;

  int crsStart, crsStop, crsStep=1;
  int spdStart, spdStop, spdStep=1;
  int tolStart, tolStop, tolStep=1;

  double crs_slope = gbox->wt(0);
  double spd_slope = gbox->wt(1);
  double tol_slope = gbox->wt(2);

  if(crs_slope <= 0) {
    crsStart = gbox->pt(0, 0);   
    crsStop  = gbox->pt(0, 1);
  }
  else {
    crsStart = gbox->pt(0, 1);   
    crsStop  = gbox->pt(0, 0);
    crsStep  = -1;
  }    

  if(spd_slope <= 0) {
    spdStart = gbox->pt(1, 0);   
    spdStop  = gbox->pt(1, 1);
  }
  else {
    spdStart = gbox->pt(1, 1);   
    spdStop  = gbox->pt(1, 0);
    spdStep  = -1;
  }
  
  if(tol_slope <= 0) {
    tolStart = gbox->pt(2, 0);   
    tolStop  = gbox->pt(2, 1);
  }
  else {
    tolStart = gbox->pt(2, 1);
    tolStop  = gbox->pt(2, 0);
    tolStep = -1;
  }

  double f_newLon, f_newLat;

  int temp_pts_verified = 0;

  for(int c=crsStart; c!=crsStop+crsStep; c += crsStep) {
    radCourse = degToRadians(c);
    currPT->setPTS(0, c, c);
    for(int s=spdStart; s!=spdStop+spdStep; s += spdStep) {
      currPT->setPTS(1, s, s);
      for(int t=tolStart; t!=tolStop+tolStep; t += tolStep) {
#if 1	
	newLon = ((sin(radCourse)*s*t)/(60.0 * nmPerPix)) + p_osLON;
	newLat = ((cos(radCourse)*s*t)/(60.0 * nmPerPix)) + p_osLAT;
#endif
#if 0
	newLon = sin(radCourse) * (double)s * (double)t;
	newLon = (newLon / (60.0 * nmPerPix)) + dp_osLON;
	newLat = cos(radCourse) * (double)s * (double)t;
	newLat = (newLat / (60.0 * nmPerPix)) + dp_osLAT;
#endif
	
	

	f_newLon = floor(newLon);
	f_newLat = floor(newLat);

	currPT->setPTS(2, t, t);
	currPT->setPTS(3, (int)newLon, (int)newLon);
	currPT->setPTS(4, (int)newLat, (int)newLat);

	if(newLon == f_newLon) {
	  currPT->setPTS(3, (int)(f_newLon), (int)(f_newLon));
	  currPT->setBDS(3, 1, 1);
	}
	else {
	  currPT->setPTS(3, (int)(f_newLon), (int)(ceil(newLon)));
	  currPT->setBDS(3, 0, 0);
	}
	if(newLat == f_newLat) {
	  currPT->setPTS(4, (int)(f_newLat), (int)(f_newLat));
	  currPT->setBDS(4, 1, 1);
	}
	else {
	  currPT->setPTS(4, (int)(f_newLat), (int)(ceil(newLat)));
	  currPT->setBDS(4, 0, 0);
	}

	pts_hit++;
	if(containedWithinBox(*currPT, *gbox)) {
	  currVal = gbox->ptVal(currPT);
	    if((!bestPT) || (currVal>bestVal)) {
	      if(!bestPT)
		bestPT = currPT->copy();
	      else
		bestPT->copy(currPT);
	      bestVal = currVal;

	      //--------------------------
	      //bestPT->setWT(bestVal);
	      //return(bestPT);
	      //--------------------------

	      pts_newmax++;
	      temp_pts_verified = 0;
	    }
	    else
	      temp_pts_verified++;
	}
      }
    }
  }
  pts_verified += temp_pts_verified;
  delete(currPT);
  if(bestPT) bestPT->setWT(bestVal);
  return(bestPT);
}

//---------------------------------------------------- compact
// Procedure: compact(Box*)

bool CompactorNav::compact(IvPBox *gbox)
{
  int   dim = gbox->getDim();

  violation = false;
  for(int i=0; i<dim; i++)
    if(gbox->pt(i, 0) > gbox->pt(i, 1))
      violation = true;

  bool moreToDo = true;
  while(moreToDo) {
    int res1  = !violation && compactCRS(gbox,0);
    int res2  = !violation && compactCRS(gbox,1);
    int res3  = !violation && compactSPD(gbox,0);
    int res4  = !violation && compactSPD(gbox,1);
    int res5  = !violation && compactTOL(gbox,0);
    int res6  = !violation && compactTOL(gbox,1);
    int res7  = !violation && compactLAT(gbox,0);
    int res8  = !violation && compactLAT(gbox,1);
    int res9  = !violation && compactLON(gbox,0);
    int res10 = !violation && compactLON(gbox,1);
    moreToDo  = (res1 || res2 || res3 || res4 || res5 ||
		 res6 || res7 || res8 || res9 || res10);
    //moreToDo = false;
  }

  // One final tweek: If we are looking at a point in the
  // the actual decision space, here course-speed-time, and
  // any of the other ranges are of size one, it means the
  // point lay somewhere in between, so make the bounds
  // exclusive.

  if((gbox->pt(0, 0) == gbox->pt(0, 1)) && 
     (gbox->pt(1, 0) == gbox->pt(1, 1)) &&
     (gbox->pt(2, 0) == gbox->pt(2, 1))) {
    if(gbox->pt(3, 1) - gbox->pt(3, 0) ==1) {
      gbox->bd(3, 1) = 0;
      gbox->bd(3, 0) = 0;
    }
    if((gbox->pt(4, 1) - gbox->pt(4, 0)) ==1 ) {
      gbox->bd(4, 1) = 0;
      gbox->bd(4, 0) = 0;
    }
  }

  return(violation);
}

//-------------------------------------------------- compactCRS()
// Procedure: compactCRS()
//   Returns: 1 if any compaction was performed.
//            0 if no compaction performed.
//     Notes: When maxmin is ONE,  we compact crsHigh.
//            When maxmin is ZERO, we compact crsLow.
//
//             12|   |5
//     Cases: 11 | 1 | 6       *Assumes pt is a point box.
//            -----------
//             4 | 0 | 2
//            -----------
//            10 | 3 | 7
//              9|   |8

bool CompactorNav::compactCRS(IvPBox *gbox, bool maxmin)
{
  if(violation) return(0);

  int crsLow=gbox->pt(0, 0);   int crsHigh=gbox->pt(0, 1);
  int lonLow=gbox->pt(3, 0);   int lonHigh=gbox->pt(3, 1);
  int latLow=gbox->pt(4, 0);   int latHigh=gbox->pt(4, 1);

  int kase = this->midBXCase(p_osLON,p_osLAT,lonLow,lonHigh,latLow,latHigh);
  if(kase==0) return(0);

  double cornerVal = -1.0;
  bool improvement = false;
  if(maxmin == 0) {
    switch(kase) {
    case 3:
      cornerVal = 0.0;
      break;
    case 1: case 11: case 12:
      cornerVal = relAng(p_osLON,p_osLAT,lonHigh,latHigh);
      break;
    case 2: case 5: case 6:
      cornerVal = relAng(p_osLON,p_osLAT,lonHigh,latLow);
      break;
    case 7: case 8:
      cornerVal = relAng(p_osLON,p_osLAT,lonLow,latLow);
      break;
    case 4: case 9: case 10:
      cornerVal = relAng(p_osLON,p_osLAT,lonLow,latHigh);
      break;
    }
    int intCornerVal = (int)ceil(cornerVal);
    if(intCornerVal > crsLow) {
      gbox->pt(0, 0) = intCornerVal;
      improvement = true;
    }
  }
  if(maxmin == 1) {
    switch(kase) {
    case 3:
      if(p_osLON == lonLow)
	cornerVal = relAng(p_osLON,p_osLAT,lonHigh,latLow);
      else cornerVal = 359.0;
      break;
    case 1: case 5: case 6:
      cornerVal = relAng(p_osLON,p_osLAT,lonLow,latHigh);
      break;
    case 2: case 7: case 8:
      cornerVal = relAng(p_osLON,p_osLAT,lonHigh,latHigh);
      break;
    case 9: case 10:
      cornerVal = relAng(p_osLON,p_osLAT,lonHigh,latLow);
      break;
    case 4: case 11: case 12:
      cornerVal = relAng(p_osLON,p_osLAT,lonLow,latLow);
      break;
    }
    int intCornerVal = (int)floor(cornerVal);
    if(intCornerVal < crsHigh) {
      gbox->pt(0, 1) = intCornerVal;
      improvement = true;
    }
  }

  if(gbox->pt(0, 0) > gbox->pt(0, 1)) 
    violation = false;
  return(improvement);
}

//------------------------------------------------- CompactSPD
// Procedure: compactSPD()
//   Returns: 1 if any compaction was performed.
//            0 if no compaction performed.
//     Notes: When maxmin is ONE,  we compact spdHigh.
//            When maxmin is ZERO, we compact spdLow.


bool CompactorNav::compactSPD(IvPBox *gbox, bool maxmin)
{
  if(violation) return(0);

  int spdLow=gbox->pt(1, 0);   int spdHigh=gbox->pt(1, 1);
  int tolLow=gbox->pt(2, 0);   int tolHigh=gbox->pt(2, 1);

  double dnewVal;
  int    inewVal;
  bool   improvement = false;

  if((maxmin==0) && (tolHigh > 0)) {        // If tolHigh is 0,
    dnewVal = minDist(gbox) / (tolHigh/60.0);   // no bounding done.
    inewVal = (int)ceil(dnewVal);
    if(inewVal > spdLow) {
      gbox->pt(1, 0) = inewVal;
      improvement = true;
    }
  }

  if((maxmin==1) && (tolLow > 0)) {        // If tolLow is 0,
    dnewVal = maxDist(gbox) / (tolLow/60.0);   // spdHigh could be
    inewVal = (int)floor(dnewVal);         // infinite, so no
    if(inewVal < spdHigh) {                // bounding is done.
      gbox->pt(1, 1) = inewVal;
      improvement = true;
    }
  }

  if(gbox->pt(1, 0) > gbox->pt(1, 1)) violation = true;
  return(improvement);
}

//-------------------------------------------------- compactTOL
// Procedure: compactTOL()
//   Returns: 1 if any compaction was performed.
//            0 if no compaction performed.
//     Notes: When maxmin is ONE,  we compact tolHigh.
//            When maxmin is ZERO, we compact tolLow.

bool CompactorNav::compactTOL(IvPBox *gbox, bool maxmin)
{
  if(violation) return(0);

  int spdLow=gbox->pt(1, 0);   int spdHigh=gbox->pt(1, 1);
  int tolLow=gbox->pt(2, 0);   int tolHigh=gbox->pt(2, 1);

  double dnewVal;
  int    inewVal;
  bool   improvement = false;
  if((maxmin==0) && (spdHigh > 0)) {         // If spdHigh is 0,
    dnewVal = 60.0 * minDist(gbox) / spdHigh;    // no bounding done.
    inewVal = (int)ceil(dnewVal);
    if(inewVal > tolLow) {
      gbox->pt(2, 0) = inewVal;
      improvement = true;
    }
  }

  if((maxmin==1) && (spdLow > 0)) {         // If spdLow is 0,
    dnewVal = 60.0 * maxDist(gbox) / spdLow;    // tolHigh could be
    inewVal = (int)floor(dnewVal);          // infinite, so no
    if(inewVal < tolHigh) {                 // bounding done.
      gbox->pt(2, 1) = inewVal;
      improvement = true;
    }
  }

  if(gbox->pt(2, 0) > gbox->pt(2, 1)) violation=true;
  return(improvement);
}

//------------------------------------------------ compactLAT()
// Procedure: compactLAT()
//   Returns: 1 if any compaction was performed.
//            0 if no compaction performed.
//     Notes: When maxmin is ONE,  we compact latHigh.
//            When maxmin is ZERO, we compact latLow.

bool CompactorNav::compactLAT(IvPBox *gbox, int maxmin)
{
  if(violation) return(false);

  int crsLow=gbox->pt(0, 0);   int crsHigh=gbox->pt(0, 1);
  int spdLow=gbox->pt(1, 0);   int spdHigh=gbox->pt(1, 1);
  int tolLow=gbox->pt(2, 0);   int tolHigh=gbox->pt(2, 1);
  int latLow=gbox->pt(4, 0);   int latHigh=gbox->pt(4, 1);

  double radcrsLow  = degToRadians(crsLow);
  double radcrsHigh = degToRadians(crsHigh);

  double latLL = ((cos(radcrsLow)  * spdLow  * tolLow) /
		  (60.0 * nmPerPix)) + p_osLAT;
  double latLH = ((cos(radcrsLow)  * spdHigh * tolHigh) /
		  (60.0 * nmPerPix)) + p_osLAT;
  double latHL = ((cos(radcrsHigh) * spdLow  * tolLow) /
		  (60.0 * nmPerPix)) + p_osLAT;
  double latHH = ((cos(radcrsHigh) * spdHigh * tolHigh) /
		  (60.0 * nmPerPix)) + p_osLAT;

  int  latVal;
  bool improvement = false;
  if(maxmin==0) {
    if((crsLow <= 180) && (180 <= crsHigh))
      latVal = (int)(p_osLAT-(spdHigh*tolHigh) / (60.0*nmPerPix));
    else {
      double latVal1 = min(latLL, latLH);
      double latVal2 = min(latHL, latHH);
      latVal  = (int)floor(min(latVal1, latVal2));
    }
    if(latVal > latLow) {
      gbox->pt(4, 0) = latVal;
      gbox->bd(4, 0) = 1;
      improvement = true;
    }
  }
  if(maxmin==1) {
    double latVal1 = max(latLL, latLH);
    double latVal2 = max(latHL, latHH);
    latVal  = (int)ceil(max(latVal1, latVal2));
    if(latVal < latHigh) {
      gbox->pt(4, 1) = latVal;
      gbox->bd(4, 1) = 1;
      improvement = true;
    }
  }
  if(gbox->pt(4, 0) > gbox->pt(4, 1)) violation = true;
  return(improvement);
}

//------------------------------------------------ compactLON()
// Procedure: compactLON()
//   Returns: 1 if any compaction was performed.
//            0 if no compaction performed.
//     Notes: When maxmin is ONE,  we compact lonHigh.
//            When maxmin is ZERO, we compact lonLow.
//      Note: The following are macros defined in CompactorNav.H. We do
//            this to save time creating temporary variables, and
//            to maintain lucid code:
//            #define crsLow  pts[0][0]  #define crsHigh pts[0][1]
//            #define spdLow  pts[1][0]  #define spdHigh pts[1][1]
//            #define tolLow  pts[2][0]  #define tolHigh pts[2][1]
//            #define lonLow  pts[3][0]  #define lonHigh pts[3][1]
//            #define latLow  pts[4][0]  #define latHigh pts[5][1]

bool CompactorNav::compactLON(IvPBox *gbox, int maxmin)
{
  if(violation) return(false);

  int crsLow=gbox->pt(0, 0);   int crsHigh=gbox->pt(0, 1);
  int spdLow=gbox->pt(1, 0);   int spdHigh=gbox->pt(1, 1);
  int tolLow=gbox->pt(2, 0);   int tolHigh=gbox->pt(2, 1);
  int lonLow=gbox->pt(3, 0);   int lonHigh=gbox->pt(3, 1);

  double radcrsLow  = degToRadians(crsLow);
  double radcrsHigh = degToRadians(crsHigh);

  double lonLL = ((sin(radcrsLow)  * spdLow  * tolLow) /
		  (60.0 * nmPerPix))  + p_osLON;
  double lonLH = ((sin(radcrsLow)  * spdHigh * tolHigh) /
		  (60.0 * nmPerPix)) + p_osLON;
  double lonHL = ((sin(radcrsHigh) * spdLow  * tolLow) /
		  (60.0 * nmPerPix))  + p_osLON;
  double lonHH = ((sin(radcrsHigh) * spdHigh * tolHigh) /
                  (60.0 * nmPerPix)) + p_osLON;

  int  lonVal;
  bool improvement = false;
  if(maxmin==0) {
    if((crsLow <= 270) && (270 <= crsHigh))
      lonVal = (int)(p_osLON-(spdHigh * tolHigh)/(60.0 * nmPerPix));
    else {
      double lonVal1 = min(lonLL, lonLH);
      double lonVal2 = min(lonHL, lonHH);
      lonVal  = (int)floor(min(lonVal1, lonVal2));
    }
    if(lonVal > lonLow) {
      gbox->pt(3, 0) = lonVal;
      gbox->bd(3, 0) = 1;
      improvement = true;
    }
  }

  if(maxmin==1) {
    if((crsLow <= 90) && (90 <= crsHigh))
      lonVal = p_osLON + (int)ceil((spdHigh * tolHigh) / (60.0 * nmPerPix));
    else {
      double lonVal1 = max(lonLL, lonLH);
      double lonVal2 = max(lonHL, lonHH);
      lonVal  = (int)ceil(max(lonVal1, lonVal2));
    }
    if(lonVal < lonHigh) {
      gbox->pt(3, 1) = lonVal;
      gbox->bd(3, 1) = 1;
      improvement = true;
    }
  }

  if(gbox->pt(3, 0) > gbox->pt(3, 1)) 
    violation = true;
  return(improvement);
}

//---------------------------------------------------- minDist()
// Procedure: minDist()
//   Returns:

double CompactorNav::minDist(IvPBox *gbox)
{

  int lonLow=gbox->pt(3, 0);   int lonHigh=gbox->pt(3, 1);
  int latLow=gbox->pt(4, 0);   int latHigh=gbox->pt(4, 1);

  int kase = this->midBXCase(p_osLON,p_osLAT,lonLow,lonHigh,latLow,latHigh);
  double pixDist = 0.0;
  switch(kase) {
  case 0:
    pixDist = 0.0;
  case 1:
    pixDist = p_osLAT-latHigh; break;
  case 2:
    pixDist = p_osLON-lonHigh; break;
  case 3:
    pixDist = latLow-p_osLAT; break;
  case 4:
    pixDist = lonLow-p_osLON;  break;
  case 5: case 6:
    pixDist = (int)hypot((double)(p_osLAT-latHigh), 
			 (double)(p_osLON-lonHigh));
    break;
  case 7: case 8:
    pixDist = (int)hypot((double)(p_osLAT-latLow),
			 (double)(p_osLON-lonHigh));
    break;
  case 9: case 10:
    pixDist = (int)hypot((double)(p_osLAT-latLow),
			 (double)(p_osLON-lonLow));
    break;
  case 11: case 12:
    pixDist = (int)hypot((double)(p_osLAT-latHigh),
			 (double)(p_osLON-lonLow));
    break;
  }
  return(pixDist * nmPerPix);
}

//---------------------------------------------------- maxDist()
// Procedure: maxDist()
//   Returns:
//                      Kase4   |    Kase1
//                            -----
//                      ------| 0 |-------
//                            -----
//                      Kase3   |    Kase2

double CompactorNav::maxDist(IvPBox *gbox)
{
  double pixDist = 0.0;

  int lonLow=gbox->pt(3, 0);   int lonHigh=gbox->pt(3, 1);
  int latLow=gbox->pt(4, 0);   int latHigh=gbox->pt(4, 1);

  //  if((p_osLAT <= latHigh) && (p_osLAT >= latLow) &&         // Kase 0
  //   (p_osLON <= lonHigh) && (p_osLON <= lonHigh)) return(0.0);

  if(p_osLON >= ((lonLow+lonHigh)/2.0))
    if(p_osLAT >= ((latLow+latHigh)/2.0))                 // Kase 1
      pixDist = hypot((double)(p_osLAT-latLow),
		      (double)(p_osLON-lonLow));
    else                                                // Kase 2
      pixDist = hypot((double)(p_osLAT-latHigh),
		      (double)(p_osLON-lonLow));
  else
    if(p_osLAT < ((latLow+latHigh)/2.0))                  // Kase 3
      pixDist = hypot((double)(p_osLAT-latHigh),
		      (double)(p_osLON-lonHigh));
    else                                                // Kase 4
      pixDist = hypot((double)(p_osLAT-latLow),
		      (double)(p_osLON-lonHigh));

  return(pixDist * nmPerPix);
}

//---------------------------------------------------------------
// Procedure: midBXCase
//   Purpose: o Determines the relative position of given point
//              to the given box. 
//
//             12|   |5
//     Cases: 11 | 1 | 6       *Assumes pt is a point box.
//            -----------
//             4 | 0 | 2
//            -----------
//            10 | 3 | 7
//              9|   |8

int CompactorNav::midBXCase(int ptx, int pty, int bxl, int bxh, int byl, int byh)
{
  if(ptx > bxh) {
    if(pty > byh) 
      if((ptx-bxh) < (pty-byh))  return(5);
      else	                 return(6);
    if(pty < byl) 
      if((ptx-bxh) > (byl-pty))  return(7);
      else	                 return(8);
    return(2);
  }
  if(ptx < bxl) {
    if(pty < byl) 
      if((bxl-ptx) < (byl-pty)) return(9);
      else                      return(10);
    if(pty > byh)
      if((bxl-ptx) > (pty-byh)) return(11);
      else                      return(12);
    return(4);
  }
  if(pty > byh) return(1);
  if(pty < byl) return(3);
  return(0);
}









