/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_TPEAK.cpp                                       */
/*    DATE: June 15th 2006                                       */
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

#include <iostream>
#include <math.h>
#include "ZAIC_TPEAK.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_TPEAK::ZAIC_TPEAK(IvPDomain g_domain, const string& g_varname) 
{
  varname      = g_varname;
  ivp_domain   = subDomain(g_domain, varname);

  summit       = 0;
  base_width   = 0;
  peak_width   = 0;
  summit_delta = 0;

  domain_ix    = ivp_domain.getIndex(varname);
  domain_high  = ivp_domain.getVarHigh(domain_ix);
  domain_low   = ivp_domain.getVarLow(domain_ix);
  domain_pts   = ivp_domain.getVarPoints(domain_ix);
  domain_delta = ivp_domain.getVarDelta(domain_ix);

  dpt_low      = 0;
  dpt_baselow  = 0;
  dpt_platlow  = 0;
  dpt_cent     = 0;
  dpt_plathigh = 0;
  dpt_basehigh = 0;
  dpt_high     = 0;

  ipt_low      = 0;
  ipt_baselow  = 0;
  ipt_platlow  = 0;
  ipt_cent     = 0;
  ipt_plathigh = 0;
  ipt_basehigh = 0;
  ipt_high     = 0;
}

//-------------------------------------------------------------
// Procedure: setSummit

bool ZAIC_TPEAK::setSummit(double g_val)
{
  bool ok = ((g_val >= domain_low) && (g_val <= domain_high));
  if(ok)
    summit = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setBaseWidth

bool ZAIC_TPEAK::setBaseWidth(double g_val)
{
  bool ok = (g_val >= 0);
  if(ok)
    base_width = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setPeakWidth

bool ZAIC_TPEAK::setPeakWidth(double g_val)
{
  bool ok = (g_val >= 0);
  if(ok)
    peak_width = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setSummitDelta

bool ZAIC_TPEAK::setSummitDelta(double g_val)
{
  bool ok = ((g_val >= 0) && (g_val <= 100));
  if(ok)
    summit_delta = g_val;
  return(ok);
}

//-------------------------------------------------------------
// Procedure: setParam

bool ZAIC_TPEAK::setParam(const string& param, const string& g_val)
{
  string val = tolower(stripBlankEnds(g_val));
  if(param == "summit")
    return(setSummit(atof(g_val.c_str())));
  
  else if(param == "peak_width")
    return(setPeakWidth(atof(g_val.c_str())));

  else if(param == "base_width")
    return(setBaseWidth(atof(g_val.c_str())));

  else if(param == "summit_delta")
    return(setSummitDelta(atof(g_val.c_str())));

  else
    return(false);
}
  
//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_TPEAK::extractOF()
{
  if(domain_ix == -1)
    return(0);

  bool ok = setDPointLocations();
  if(!ok) 
    return(0);

  ok = setIPointLocations();
  if(!ok) return(0);

#if 0
  cout << "dpt_low:      " << dpt_low      << endl;
  cout << "dpt_baselow:  " << dpt_baselow  << endl;
  cout << "dpt_peaklow:  " << dpt_platlow  << endl;
  cout << "dpt_cent:     " << dpt_cent     << endl;
  cout << "dpt_peakhigh: " << dpt_plathigh << endl;
  cout << "dpt_basehigh: " << dpt_basehigh << endl;
  cout << "dpt_high:     " << dpt_high     << endl;

  cout << endl;

  cout << "ipt_low:      " << ipt_low      << endl;
  cout << "ipt_baselow:  " << ipt_baselow  << endl;
  cout << "ipt_peaklow:  " << ipt_platlow  << endl;
  cout << "ipt_cent:     " << ipt_cent     << endl;
  cout << "ipt_peakhigh: " << ipt_plathigh << endl;
  cout << "ipt_basehigh: " << ipt_basehigh << endl;
  cout << "ipt_high:     " << ipt_high     << endl;
#endif

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *of = new IvPFunction(pdmap);

  return(of);
}


//-------------------------------------------------------------
// Procedure: setDPointLocations()
//            Assumes that the center summit 
// 
//     |                 o-------o-------o                   |
//     |               /                   \                 |
//     |             /                       \               |
//     |           /                           \             |
//     |         /                               \           |
//     |       /                                   \         |
//     |     /                                       \       |
//     o----o------------o-------o-------o-------------o-----o
//     0    1            2       3       4             5     6

//     |                         o                           |
//     |                       /   \                         |
//     |                     /       \                       |
//     |                   /           \                     |
//     |                 /               \                   |
//     |               /                   \                 |
//     |             /                       \               |
//     o------------o------------o-------------o-------------o
//     0            1          2,3,4           5             6

//     |        o-------o-------o                            |
//     |      /                  \                           |
//     |    /                      \                         |
//     |  /                          \                       |
//     |/                              \                     |
//     |                                 \                   |
//     |                                   \                 |
//     o--------o-------o-------o------------o---------------o
//    0.1       2       3       4            5               6


bool ZAIC_TPEAK::setDPointLocations()
{
  dpt_low  = domain_low;
  dpt_high = domain_high;

  if((summit >= dpt_low) && (summit <= dpt_high))
    dpt_cent = summit;
  else 
    return(false);

  // Setting the Plateau/Peak point locations
  if(peak_width == 0) {
    dpt_platlow  = dpt_cent;
    dpt_plathigh = dpt_cent;
  }
  else {
    if((summit - peak_width) > dpt_low)
      dpt_platlow = (summit - peak_width);
    else
      dpt_platlow = dpt_low;

    if((summit + peak_width) < dpt_high)
      dpt_plathigh = (summit + peak_width);
    else
      dpt_plathigh = dpt_high;
  }

  // Setting the BASE point locations
  if(base_width == 0) {
    dpt_baselow  = dpt_platlow;
    dpt_basehigh = dpt_plathigh;
  }
  else {
    if((dpt_platlow - base_width) > dpt_low)
      dpt_baselow = dpt_platlow - base_width;
    else
      dpt_baselow = dpt_low;
    
    if(dpt_plathigh + base_width < dpt_high)
      dpt_basehigh = dpt_plathigh + base_width;
    else 
      dpt_basehigh = dpt_high;
  }
  return(true);
}
     
//-------------------------------------------------------------
// Procedure: setIPointLocations()
//            Now round to the nearest IvPDomain increment;

bool ZAIC_TPEAK::setIPointLocations()
{
  double d = domain_delta;

  ipt_low      = 0;

  ipt_baselow  = (int)((dpt_baselow+(d/2)) / d);
  if(ipt_baselow < ipt_low)        // rounding safety measure
    ipt_baselow = ipt_low;

  ipt_platlow  = (int)((dpt_platlow+(d/2)) / d);
  if(ipt_platlow < ipt_baselow)    // rounding safety measure
    ipt_platlow = ipt_baselow;

  ipt_cent     = (int)((dpt_cent/d)+0.5);
  if(ipt_cent < ipt_platlow)       // rounding safety measure
    ipt_cent = ipt_platlow;

  ipt_plathigh = (int)((dpt_plathigh/d)+0.5);
  if(ipt_plathigh < ipt_cent)      // rounding safety measure
    ipt_plathigh = ipt_cent;

  ipt_basehigh = (int)((dpt_basehigh/d)+0.5);
  if(ipt_basehigh < ipt_plathigh)  // rounding safety measure
    ipt_basehigh = ipt_plathigh;

  ipt_high     = domain_pts - 1;

  i_basewidth = (int)((base_width + (d/2)) / d);
  i_peakwidth = (int)((peak_width + (d/2)) / d);

  return(true);
}


//-------------------------------------------------------------
// Procedure: setPDMap()
//
//     |                 o-------o-------o                   |
//     |               /                   \                 |
//     |             /                       \               |
//     |           /                           \             |
//     |         /                               \           |
//     |       /                                   \         |
//     |     /                                       \       |
//     o----o-----------o--------o-------o-------------o-----o
//       P0     P1         P2    P3   P4       P5        P6  
//
//     |                         o                           |
//     |                       /   \                         |
//     |                     /       \                       |
//     |                   /           \                     |
//     |                 /     No P2     \                   |
//     |               /       No P4       \                 |
//     |             /                       \               |
//     o------------o------------o-------------o-------------o
//         P0             P1     P3     P5         P6        
//
//     |        o-------o-------o                            |
//     |      /                  \                           |
//     |    /                      \                         |
//     |  /                          \                       |
//     |/                              \                     |
//     |  No P0                          \                   |
//     |                                   \                 |
//     o--------o-------o-------o------------o---------------o
//        P1        P2  P3  P4       P5           P6
//                                                             
//
//     |------o------o                                       |
//     |               \                                     |
//     |                 \                                   |
//     |                   \                                 |
//     | No P0               \                               |
//     | No P1                 \                             |
//     |                         \                           |
//     o--------------------------o--------------------------o
//       P2   P3   P4       P5              P6               
// 
//     |                                   o-------o-------|
//     |                                 /                 |
//     |                               /                   |
//     |                             /                     |
//     |                           /                 No P5 |
//     |                         /                   No P5 |
//     |                       /                           |
//     o----------------------o------------o-------o-------o
//     0    P0                   P1          P2    P3  P4


PDMap *ZAIC_TPEAK::setPDMap()
{
  double summit_hgt = 100;
  double plat_hgt   = 100 - summit_delta;

  int i;
  int piece_count = 0;
  IvPBox *piece[7];
  for(i=0; i<7; i++)
    piece[i] = 0;

  // Handle piece0 if it exists
  if(ipt_low < ipt_baselow) {
    piece[0] = new IvPBox(1,1);
    piece[0]->setPTS(0, 0, ipt_baselow-1);
    piece[0]->wt(0) = 0.0;
    piece[0]->wt(1) = 0.0;
    piece_count++;
  }

  // Handle piece1 if it exists
  if(ipt_baselow < ipt_platlow) {
    piece[1] = new IvPBox(1,1);
    piece[1]->setPTS(0, ipt_baselow , ipt_platlow-1);

    double run    = i_basewidth;
    double slope  = plat_hgt / run;
    double intcpt = -1.0 * slope * (ipt_platlow - i_basewidth);

    piece[1]->wt(0) = slope;
    piece[1]->wt(1) = intcpt;
    piece_count++;
  }
    
  // Handle piece2 if it exists
  if(ipt_platlow < ipt_cent) {
    piece[2] = new IvPBox(1,1);
    piece[2]->setPTS(0, ipt_platlow , ipt_cent-1);

    // y = mx+b, b = y-mx
    double run    = i_peakwidth;
    double slope  = (summit_hgt - plat_hgt) / run;
    double intcpt = plat_hgt - (slope * (ipt_cent - i_peakwidth));

    piece[2]->wt(0) = slope;
    piece[2]->wt(1) = intcpt;
    piece_count++;
  }
    
  // Handle piece3
  piece[3] = new IvPBox(1,1);
  piece[3]->setPTS(0, ipt_cent, ipt_cent);
  piece[3]->wt(0) =   0.0;
  piece[3]->wt(1) = 100.0;
  piece_count++;
    
  // Handle piece4 if it exists
  if(ipt_plathigh > ipt_cent) {
    piece[4] = new IvPBox(1,1);
    piece[4]->setPTS(0, ipt_cent+1, ipt_plathigh);

    // y = mx+b, b = y-mx
    double run    = i_peakwidth;
    double slope  = (plat_hgt - summit_hgt) / run;
    double intcpt = plat_hgt - (slope * (ipt_cent + i_peakwidth));

    piece[4]->wt(0) = slope;
    piece[4]->wt(1) = intcpt;
    piece_count++;
  }
    
  // Handle piece5 if it exists
  if(ipt_basehigh > ipt_plathigh) {
    piece[5] = new IvPBox(1,1);
    piece[5]->setPTS(0, ipt_plathigh+1, ipt_basehigh);

    double run    = i_basewidth;
    double slope  = -plat_hgt / run;
    double intcpt = -1.0 * slope * (ipt_plathigh + i_basewidth);

    piece[5]->wt(0) = slope;
    piece[5]->wt(1) = intcpt;
    piece_count++;
  }
    
  // Handle piece6 if it exists
  if(ipt_high > ipt_basehigh) {
    piece[6] = new IvPBox(1,1);
    piece[6]->setPTS(0, ipt_basehigh+1, ipt_high);
    piece[6]->wt(0) = 0.0;
    piece[6]->wt(1) = 0.0;
    piece_count++;
  }
  
  PDMap *pdmap;
  pdmap = new PDMap(piece_count, ivp_domain, 1);

  // Add the new pieces to the PDMap
  int ix = 0;
  for(i=0; i<7; i++) {
    if(piece[i]) {
      pdmap->bx(ix) = piece[i];
      ix++;
    }
  }

#if 0 // For debugging
  for(j=0; j<7; j++) {
    cout << "[" << j << "]: ";
    if(piece[j])
      piece[j]->print();
    else
      cout << "NULL" << endl;
  }
#endif

  return(pdmap);
}






